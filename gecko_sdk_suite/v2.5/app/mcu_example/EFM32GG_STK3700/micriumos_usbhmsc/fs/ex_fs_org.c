/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/*
*********************************************************************************************************
*
*                                         FILE SYSTEM EXAMPLE
*
* File : ex_fs.c
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
T*********************************************************************************************************
*/

#define  EX_FS_MODULE

#include <rtos_description.h>
#ifdef  RTOS_MODULE_FS_AVAIL

#if  defined(RTOS_MODULE_FS_SCSI_AVAIL)                  || \
     ((FS_STORAGE_CFG_MEDIA_POLL_TASK_EN == DEF_ENABLED) && \
      (defined(RTOS_MODULE_FS_SD_CARD_AVAIL)             || \
       defined(RTOS_MODULE_FS_SD_SPI_AVAIL)))
#include  "ex_fs_media_poll.h"
#endif

#include  "ex_fs.h"
#include  "ex_fs_utils.h"

#include  <cpu_cfg.h>
#include  <cpu/include/cpu.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/rtos_err.h>
#include  <common/include/lib_def.h>
#include  <common/source/collections/slist_priv.h>
#include  <kernel/include/os.h>

#include  <fs/include/fs_core.h>
#include  <fs/include/fs_core_cache.h>
#include  <fs/include/fs_core_file.h>
#include  <fs/include/fs_core_partition.h>
#include  <fs/include/fs_fat.h>
#include  <fs/include/fs_storage.h>
#include  <fs/include/fs_ramdisk.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            LOCAL DEFINES
*
* Note(s) : (1) This configuration allows to low-level format NAND or NOR chip with FSMedia_LowFmt().
*
*               DEF_ENABLED     Low-level format enabled.
*               DEF_DISABLED    Low-level format disabled.
*
*               If you have a blank NAND or NOR chip, you must low-level format the memory chip prior to
*               accessing the media by the high-level file system API. You may want to low-level format
*               explicitly even if the NAND or NOR chip is already formatted. After a low-level format,
*               you must format the NAND or NOR chip with a high-level format. For instance, high-level
*               formatting as a FAT volume. High-level format is controlled by the configuration
*               EX_CFG_FS_MEDIA_HIGH_LEVEL_FMT_EN.
*               Low-level format does NOT apply to RAM, SD and SCSI devices.
*
*           (2) This configuration allows to high-level format any media (NAND, NOR, RAM Disk, SD) with
*               FS_FAT_Fmt().
*
*               DEF_ENABLED     High-level format enabled.
*               DEF_DISABLED    High-level format disabled.
*
*               When enabled, the high-level format will apply to the unique partition composing your
*               media. If a NAND or NOR chip have been low-level formatted, the high-level format is
*               mandatory.
*********************************************************************************************************
*********************************************************************************************************
*/

                                                                /* --------------- LOCAL CONFIGURATION ---------------- */
#define  EX_CFG_FS_MEDIA_LOW_LEVEL_FMT_EN       DEF_DISABLED    /* See Note #1.                                         */
#define  EX_CFG_FS_MEDIA_HIGH_LEVEL_FMT_EN      DEF_DISABLED    /* See Note #2.                                         */


#define  EX_FS_MEM_SEG_LEN                      (10u * 1024u)   /* Memory segment size dedicated to FS examples.        */

                                                                /* RAM Disk zone definition.                            */
#define  EX_FS_RAM_SEC_SIZE                     512u            /* RAM Disk sector size.                                */
#define  EX_FS_RAM_SEC_NBR                      96u             /* RAM Disk number of sectors.                          */


/*
*********************************************************************************************************
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*********************************************************************************************************
*/

typedef  struct  ex_fs_data {
    FS_CACHE  *CachePtr;
} EX_FS_DATA;


/*
*********************************************************************************************************
*                                               LOGGING
*
* Note(s) : (1) This example outputs information to the console via the function printf() via a macro
*               called EX_TRACE(). This can be modified or disabled if printf() is not supported.
*********************************************************************************************************
*/

#include  <stdio.h>
#define  EX_TRACE(...)              printf(__VA_ARGS__)


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/


static  CPU_INT08U   Ex_FS_MemSegData[EX_FS_MEM_SEG_LEN];       /* Memory segment available to FS examples.             */
static  EX_FS_DATA  *Ex_FS_DataPtr;                             /* Pointer to private data.                             */

                                                                /* ------------- RAM DISK ZONE DEFINITION ------------- */
static  CPU_INT08U       Ex_FS_RAM_Disk[EX_FS_RAM_SEC_SIZE * EX_FS_RAM_SEC_NBR];
static  FS_RAM_DISK_CFG  Ex_FS_RAM_Disk_Cfg = {
    .DiskPtr = Ex_FS_RAM_Disk,
    .LbCnt   = EX_FS_RAM_SEC_NBR,
    .LbSize  = EX_FS_RAM_SEC_SIZE
};


/*
*********************************************************************************************************
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

static  CPU_SIZE_T  Ex_FS_MediaLbSizeMaxGet(void);


/*
*********************************************************************************************************
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*********************************************************************************************************
*/

#if defined(RTOS_MODULE_FS_NAND_AVAIL) || \
    defined(RTOS_MODULE_FS_NOR_AVAIL)

#if (EX_CFG_FS_MEDIA_LOW_LEVEL_FMT_EN == DEF_ENABLED) && (EX_CFG_FS_MEDIA_HIGH_LEVEL_FMT_EN == DEF_DISABLED)
#error  "If NAND or NOR is low-level formatted, EX_CFG_FS_MEDIA_HIGH_LEVEL_FMT_EN must be DEF_ENABLED."
#endif
#endif

#if defined(RTOS_MODULE_FS_SCSI_AVAIL)
#if !defined(RTOS_MODULE_USB_HOST_AVAIL) && !defined(RTOS_MODULE_USB_HOST_MSC_AVAIL)
#error "RTOS_MODULE_USB_HOST_AVAIL and RTOS_MODULE_USB_HOST_MSC_AVAIL must be defined in rtos_description.h when SCSI is used."
#endif
#endif

/*
*********************************************************************************************************
*********************************************************************************************************
*                                          GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             Ex_FS_Init()
*
* Description : Initialize FS core and media drivers.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Note(s)     : (1) Connection and disconnection callbacks are used:
*                   - When the Media Poll task is enabled and SCSI and/or SD are/is available.
*                   - Each time SCSI is available. Indeed, SCSI can notify the application about a
*                     removable connection and disconnection without the Media Polling task.
*
*               (2) Low-level format is mandatory on blank NAND or NOR chip. You may want to explicitly
*                   low-level a NAND/NOR chip already formatted but all the media content will be
*                   deleted.
*                   See section 'LOCAL DEFINES' Note #1 in this file for more details.
*
*               (3) High-level format will delete all the media content.
*                   See section 'LOCAL DEFINES' Note #2 in this file for more details.
*
*********************************************************************************************************
*/

void  Ex_FS_Init (void)
{
    FS_CACHE_CFG       cache_cfg;
    FS_BLK_DEV_HANDLE  blk_dev_handle;
    RTOS_ERR           err;


                                                                /* ------------------- GENERAL INIT ------------------- */
#if  defined(RTOS_MODULE_FS_SCSI_AVAIL)                  || \
     ((FS_STORAGE_CFG_MEDIA_POLL_TASK_EN == DEF_ENABLED) && \
      (defined(RTOS_MODULE_FS_SD_CARD_AVAIL)             || \
       defined(RTOS_MODULE_FS_SD_SPI_AVAIL)))
                                                                /* Configure media callbacks (see Note #1).             */
    FSStorage_ConfigureMediaConnCallback(Ex_FS_MediaPollOnConn,
                                         Ex_FS_MediaPollOnDisconn);
#endif

    FSStorage_Init(&err);                                       /* Initialize Storage with default configuration.       */
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

    FSCore_Init(&err);                                          /* Initialize FS Core with default configuration.       */
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

                                                                /* Create memory segment available for all FS examples. */
    Mem_SegCreate("FS app mem seg",
                  &Ex_FS_MemSeg,
                  (CPU_ADDR)Ex_FS_MemSegData,
                   EX_FS_MEM_SEG_LEN,
                   LIB_MEM_CFG_HEAP_PADDING_ALIGN,
                  &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

                                                                /* Allocate example private data control.               */
    Ex_FS_DataPtr = (EX_FS_DATA *) Mem_SegAlloc("Ex - FS Data",
                                                DEF_NULL,
                                                sizeof(EX_FS_DATA),
                                                &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

    /* --------------------- RAM DISK --------------------- */
                                                                /* Add RAM Disk instance to file system.                */
    FS_RAM_Disk_Add("ram0",                                     /* String identifying this RAM Disk instance.           */
                    &Ex_FS_RAM_Disk_Cfg,                        /* Configuration describing RAM area.                   */
                    &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

                                                                /* ------------ LOW-LEVEL FORMAT IF NEEDED ------------ */
#if (EX_CFG_FS_MEDIA_LOW_LEVEL_FMT_EN == DEF_ENABLED)           /* Use this code block with care (see Note #2).         */
    {
        FS_MEDIA_HANDLE  media_handle;
        FS_MEDIA_TYPE    media_type;


        EX_TRACE("Ex_FS_Init(): Low-level %s format...", EX_CFG_FS_ACTIVE_MEDIA_NAME);

        media_handle = FSMedia_Get(EX_CFG_FS_ACTIVE_MEDIA_NAME);
        APP_RTOS_ASSERT_CRITICAL(!FS_MEDIA_HANDLE_IS_NULL(media_handle), ;);

        media_type = FSMedia_TypeGet(media_handle, &err);
        APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);
                                                                /* Only NAND or NOR can be low-level formatted.         */
        APP_RTOS_ASSERT_CRITICAL(((media_type == FS_MEDIA_TYPE_NAND) || (media_type == FS_MEDIA_TYPE_NOR)), ;);

        FSMedia_LowFmt(media_handle, &err);                     /* Low-format NAND or NOR.                              */
        APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);
        EX_TRACE("OK\r\n");
    }
#endif
                                                                /* ---------------------- CACHE ----------------------- */
                                                                /* Create cache instance shared by all media.           */
    cache_cfg.Align        = sizeof(CPU_ALIGN);
    cache_cfg.BlkMemSegPtr = DEF_NULL;
    cache_cfg.MaxLbSize    = Ex_FS_MediaLbSizeMaxGet();         /* Get logical block size of 1st media.                 */
    cache_cfg.MinLbSize    = 512u;
    cache_cfg.MinBlkCnt    = 4u;

    Ex_FS_DataPtr->CachePtr = FSCache_Create(&cache_cfg, &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

                                                                /* ----------------- FORMAT RAM DISK ------------------ */
                                                                /* Format RAM Disk as a FAT volume.                     */
    blk_dev_handle = FSBlkDev_Open(FSMedia_Get("ram0"), &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

                                                                /* Assign cache to this device.                         */
    FSCache_Assign(blk_dev_handle,
                   Ex_FS_DataPtr->CachePtr,                     /* Pointer to cache instance previously created.        */
                   &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

    FS_FAT_Fmt(blk_dev_handle,
               FS_PARTITION_NBR_VOID,                           /* Only 1 partition.                                    */
               DEF_NULL,                                        /* Format with default values.                          */
               &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

    FSBlkDev_Close(blk_dev_handle, &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);
                                                                /* -------------------- MEDIA POLL -------------------- */
#if  defined(RTOS_MODULE_FS_SCSI_AVAIL)                  || \
     ((FS_STORAGE_CFG_MEDIA_POLL_TASK_EN == DEF_ENABLED) && \
      (defined(RTOS_MODULE_FS_SD_CARD_AVAIL)             || \
       defined(RTOS_MODULE_FS_SD_SPI_AVAIL)))

    Ex_FS_MediaPollInit();                                      /* Create resources to illustrate media polling.        */
#endif

                                                                /* ----------- HIGH-LEVEL FORMAT IF NEEDED ------------ */
#if (EX_CFG_FS_MEDIA_HIGH_LEVEL_FMT_EN == DEF_ENABLED)          /* Use this code block with care (see Note #3).         */
    {
        FS_MEDIA_HANDLE  media_handle;


        EX_TRACE("Ex_FS_Init(): High-level %s format...", EX_CFG_FS_ACTIVE_MEDIA_NAME);

        media_handle   = FSMedia_Get(EX_CFG_FS_ACTIVE_MEDIA_NAME);
        APP_RTOS_ASSERT_CRITICAL(!FS_MEDIA_HANDLE_IS_NULL(media_handle), ;);

        blk_dev_handle = FSBlkDev_Open(media_handle, &err);
        APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);
                                                                /* Assign cache to this device.                         */
        FSCache_Assign(blk_dev_handle,
                       Ex_FS_DataPtr->CachePtr,                 /* Pointer to cache instance previously created.        */
                       &err);
        APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);
                                                                /* Format media as a FAT volume.                        */
        FS_FAT_Fmt(blk_dev_handle,
                   FS_PARTITION_NBR_VOID,                       /* Only 1 partition.                                    */
                   DEF_NULL,                                    /* Format with default values.                          */
                   &err);
        APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

        FSBlkDev_Close(blk_dev_handle, &err);
        APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);
        EX_TRACE("OK\r\n");
    }
#endif
}


/*
*********************************************************************************************************
*                                            Ex_FS_VolOpen()
*
* Description : Perform all required operations to open a volume.
*
* Argument(s) : media_handle    Handle to a media.
*
*               p_vol_name      Pointer to volume name.
*
*               p_err           Pointer to variable that will receive the return error code from this
*                               function.
*
* Return(s)   : Volume handle, if NO error(s).
*               Null handle,   otherwise.
*
* Note(s)     : none.
*
*********************************************************************************************************
*/

FS_VOL_HANDLE  Ex_FS_VolOpen (FS_MEDIA_HANDLE   media_handle,
                              CPU_CHAR         *p_vol_name,
                              RTOS_ERR         *p_err)
{
    FS_BLK_DEV_HANDLE  blk_dev_handle;
    FS_VOL_HANDLE      vol_handle;
    RTOS_ERR           err_local;


                                                                /* Open a device.                                       */
    blk_dev_handle = FSBlkDev_Open(media_handle, p_err);
    if (p_err->Code != RTOS_ERR_NONE) {
        EX_TRACE("Ex_FS_VolOpen(): error opening device w/ err %d.\r\n", p_err->Code);
        return (FSVol_NullHandle);
    }
                                                                /* Assign cache to this device.                         */
    FSCache_Assign(blk_dev_handle,
                   Ex_FS_DataPtr->CachePtr,                     /* Pointer to cache instance previously created.        */
                   p_err);
    if (p_err->Code != RTOS_ERR_NONE) {
        EX_TRACE("Ex_FS_VolOpen(): error assigning cache w/ err %d.\r\n", p_err->Code);
        goto end_dev;
    }
                                                                /* Open the volume.                                     */
    vol_handle = FSVol_Open(blk_dev_handle,
                            1u,                                 /* Partition #1.                                        */
                            p_vol_name,                         /* Unique volume name across all open volumes.          */
                            FS_VOL_OPT_DFLT,                    /* Default options: write allowed, auto sync disabled.  */
                            p_err);
    if (p_err->Code != RTOS_ERR_NONE) {
        EX_TRACE("Ex_FS_VolOpen(): error opening volume w/ err %d\r\n", p_err->Code);
        goto end_dev;
    }

    return (vol_handle);

end_dev:
    FSBlkDev_Close(blk_dev_handle, &err_local);                 /* Close the device.                                    */
    APP_RTOS_ASSERT_CRITICAL(err_local.Code == RTOS_ERR_NONE, FSVol_NullHandle);

    return (FSVol_NullHandle);
}


/*
*********************************************************************************************************
*                                           Ex_FS_VolClose()
*
* Description : Perform all required operations to close a volume.
*
* Argument(s) : vol_handle      Volume handle.
*
*               p_err           Pointer to variable that will receive the return error code from this
*                               function.
*
* Return(s)   : none.
*
* Note(s)     : none.
*
*********************************************************************************************************
*/

void  Ex_FS_VolClose (FS_VOL_HANDLE   vol_handle,
                      RTOS_ERR       *p_err)
{
    FS_BLK_DEV_HANDLE  blk_dev_handle;


                                                                /* Retrieve device handle from volume handle.           */
    blk_dev_handle = FSVol_BlkDevGet(vol_handle);
    if (FS_BLK_DEV_HANDLE_IS_NULL(blk_dev_handle)) {
        p_err->Code = RTOS_ERR_FAIL;
        return;
    }

    FSVol_Close(vol_handle, p_err);                             /* Close the volume first.                              */
    if (p_err->Code != RTOS_ERR_NONE) {
        return;
    }

    FSBlkDev_Close(blk_dev_handle, p_err);                      /* Close the device last.                               */
    if (p_err->Code != RTOS_ERR_NONE) {
        return;
    }
}


/*
*********************************************************************************************************
*                                            Ex_FS_BufFill()
*
* Description : Fill a buffer with a specified pattern start.
*
* Argument(s) : p_buf           Pointer to buffer.
*
*               buf_size        Buffer size.
*
*               pattern_start   Pattern start value.
*
* Return(s)   : none.
*
* Note(s)     : none.
*
*********************************************************************************************************
*/

void  Ex_FS_BufFill (CPU_INT08U  *p_buf,
                     CPU_SIZE_T   buf_size,
                     CPU_INT08U   pattern_start)
{
    CPU_SIZE_T  k;
    CPU_INT08U  pattern = pattern_start;

    for (k = 0u; k < buf_size; k++) {
        p_buf[k] = (pattern & 0xFF);
        pattern++;
    }
}


/*
*********************************************************************************************************
*                                          Ex_FS_BufValidate()
*
* Description : Validate buffer content with a specified pattern start.
*
* Argument(s) : p_buf           Pointer to buffer.
*
*               buf_size        Buffer size.
*
*               pattern_start   Pattern start value.
*
* Return(s)   : DEF_YES, if NO error(s).
*               DEF_NO,  otherwise
*
* Note(s)     : none.
*
*********************************************************************************************************
*/

CPU_BOOLEAN  Ex_FS_BufValidate (CPU_INT08U  *p_buf,
                                CPU_SIZE_T   buf_size,
                                CPU_INT08U   pattern_start)
{
    CPU_SIZE_T  k;
    CPU_INT08U  pattern = pattern_start;


    for (k = 0u; k < buf_size; k++) {
        if (p_buf[k] != (pattern & 0xFF)) {
            return (DEF_NO);
        }
        pattern++;
    }
    return (DEF_YES);
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                        Ex_FS_MediaLbSizeGet()
*
* Description : Get the logical block size of the active media.
*
* Argument(s) : none.
*
* Return(s)   : First media logical block size.
*
* Note(s)     : (1) If the error is RTOS_ERR_IO and the media is SD, it certainly means that the SD card
*                   is not inserted in its slot. To get the logical block size, the SD card must be
*                   inserted prior to initialize the FS examples.
*
*********************************************************************************************************
*/

static  CPU_SIZE_T  Ex_FS_MediaLbSizeMaxGet (void)
{
    RTOS_ERR           err;
    FS_MEDIA_HANDLE    media_handle;
    FS_BLK_DEV_HANDLE  blk_dev_handle;
    CPU_SIZE_T         lb_size;


    media_handle = FSMedia_Get(EX_CFG_FS_ACTIVE_MEDIA_NAME);    /* Get media handle.                                    */
    APP_RTOS_ASSERT_CRITICAL(!FS_MEDIA_HANDLE_IS_NULL(media_handle), 0u);

                                                                /* Open a device.                                       */
    blk_dev_handle = FSBlkDev_Open(media_handle, &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, 0u);    /* See Note #1.                                         */
                                                                /* Retrieve logical block size of open device.          */
    lb_size = FSBlkDev_LbSizeGet(blk_dev_handle, &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, 0u);
                                                                /* Close the device.                                    */
    FSBlkDev_Close(blk_dev_handle, &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, 0u);

    return (lb_size);
}

#endif  /* RTOS_MODULE_FS_AVAIL */
