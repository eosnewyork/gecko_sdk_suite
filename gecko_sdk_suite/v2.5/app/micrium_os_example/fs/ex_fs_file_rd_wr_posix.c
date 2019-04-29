/***************************************************************************//**
 * @file
 * @brief File System Example
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_FS_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <ex_description.h>

#include  <fs/include/fs_core_posix.h>

#include  "ex_fs_file_rd_wr_posix.h"
#include  "ex_fs_utils.h"
#include  "ex_fs.h"

#include  <common/include/rtos_utils.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               LOGGING
 *
 * Note(s) : (1) This example outputs information to the console via the function printf() via a macro
 *               called EX_TRACE(). This can be modified or disabled if printf() is not supported.
 *******************************************************************************************************/

#ifndef EX_TRACE
#define  EX_TRACE(...)              printf(__VA_ARGS__)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void Ex_FS_FileRdWr_PosixExec(FS_VOL_HANDLE vol_handle,
                                     CPU_CHAR      *p_file_name,
                                     CPU_INT32U    file_size,
                                     RTOS_ERR      *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                         Ex_FS_FileRdWr_Posix()
 *
 * @brief  Perform file read/write example with the Posix API.
 *******************************************************************************************************/
void Ex_FS_FileRdWr_Posix(void)
{
  FS_MEDIA_HANDLE media_handle;
  FS_VOL_HANDLE   vol_handle;
  RTOS_ERR        err;

  media_handle = FSMedia_Get(EX_CFG_FS_ACTIVE_MEDIA_NAME);      // Get media handle.
  APP_RTOS_ASSERT_CRITICAL(!FS_MEDIA_HANDLE_IS_NULL(media_handle),; );
  EX_TRACE("FS Example: File rd/wr Posix on '%s'...", EX_CFG_FS_ACTIVE_MEDIA_NAME);

  //                                                               Open device and volume.
  vol_handle = Ex_FS_VolOpen(media_handle,
                             "vol1",                            // Unique volume name.
                             &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  //                                                               Execute file read/write example.
  Ex_FS_FileRdWr_PosixExec(vol_handle,
                           "file-rdwr-posix.txt",
                           5133u,                               // File size.
                           &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  Ex_FS_VolClose(vol_handle, &err);                             // Close volume and device.
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("OK\n\r");
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       Ex_FS_FileRdWr_PosixExec()
 *
 * @brief  This example will do the following main steps:
 *             - (a) Write some known data to the specified file.
 *             - (b) Read data from the specified file.
 *             - (c) Verify data read.
 *
 * @param  vol_handle   Volume handle.
 *
 * @param  p_file_name  Pointer to file name.
 *
 * @param  file_size    File size.
 *
 * @param  p_err        Pointer to variable that will receive the return error code from this
 *                      function.
 *
 * @note   (1)  This example writes data to the file in one shot. It does NOT support writing a file
 *              in several chunks. If the file size is greater than the available memory segment
 *              size, an error is returned.
 *******************************************************************************************************/
static void Ex_FS_FileRdWr_PosixExec(FS_VOL_HANDLE vol_handle,
                                     CPU_CHAR      *p_file_name,
                                     CPU_INT32U    file_size,
                                     RTOS_ERR      *p_err)
{
  CPU_INT08U   *p_buf;
  CPU_CHAR     *p_file_path_buf;
  MEM_SEG_INFO seg_info;
  CPU_SIZE_T   seg_rem_size;
  CPU_SIZE_T   expected_path_len;
  FILE         *p_file;
  CPU_INT08U   pattern_start = 0;
  CPU_BOOLEAN  ok;
  int          result;
  RTOS_ERR     local_err;

  //                                                               Ensure enough space to allocate internal rsrc.
  seg_rem_size = Mem_SegRemSizeGet(&Ex_FS_MemSeg, sizeof(CPU_ALIGN), &seg_info, &local_err);
  APP_RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE),; );
  APP_RTOS_ASSERT_CRITICAL((seg_rem_size > (file_size + EX_FS_MAX_PATH_NAME_LEN)),; );

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  //                                                               ------------------ INITIALIZATION ------------------
  //                                                               Allocate internal buffer.
  p_buf = (CPU_INT08U *)Mem_SegAlloc("Ex - FS file posix buf",
                                     &Ex_FS_MemSeg,
                                     file_size,
                                     p_err);
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_FileRdWr_PosixExec(): Error allocating Tx/Rx buf w/ err %d", p_err->Code);
    return;
  }
  //                                                               Allocate buffer for file path.
  p_file_path_buf = (CPU_CHAR *)Mem_SegAlloc("Ex - FS file posix path buf",
                                             &Ex_FS_MemSeg,
                                             EX_FS_MAX_PATH_NAME_LEN,
                                             p_err);
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_FileRdWr_PosixExec(): Error allocating file path buf w/ err %d", p_err->Code);
    goto end_mem_seg;
  }

  //                                                               Get volume name associated to volume handle.
  Mem_Clr((void *)p_file_path_buf, EX_FS_MAX_PATH_NAME_LEN);
  FSVol_NameGet(vol_handle,
                p_file_path_buf,
                EX_FS_MAX_PATH_NAME_LEN,
                p_err);
  if (p_err->Code != RTOS_ERR_NONE) {
    EX_TRACE("Ex_FS_FileRdWr_PosixExec(): Error getting volume name w/ err %d", p_err->Code);
    goto end_mem_seg;
  }
  //                                                               Build full file path.
  expected_path_len = Str_Len(p_file_path_buf)
                      + Str_Len(p_file_name)
                      + 1u;
  APP_RTOS_ASSERT_CRITICAL((expected_path_len < EX_FS_MAX_PATH_NAME_LEN),; );
  Str_Cat(p_file_path_buf, "/");
  Str_Cat(p_file_path_buf, p_file_name);
  //                                                               Open file in read/write mode.
  p_file = fopen(p_file_path_buf,                               // Buffer containing full path to file.
                 "w+b");                                        // Create file accessed in write or read and truncate...
                                                                // ...file size to 0 bytes.
  if (p_file == DEF_NULL) {
    p_error("Ex_FS_FileRdWr_PosixExec(): Error opening file");
    p_err->Code = RTOS_ERR_FAIL;
    goto end_mem_seg;
  }
  //                                                               ---------------- EXAMPLE EXECUTION -----------------
  //                                                               File position to beginning of file.
  result = fseek(p_file,
                 0,                                             // Offset added to reference start position.
                 SEEK_SET);                                     // Reference start position: beginning of file.
  if (result != 0) {
    p_error("Ex_FS_FileRdWr_PosixExec(): Error positioning file");
    p_err->Code = RTOS_ERR_FAIL;
    goto end_file;
  }

  Ex_FS_BufFill(p_buf, file_size, pattern_start);               // Write buffer with known pattern.
                                                                // Write data to file.
  result = fwrite((void *)p_buf,
                  sizeof(CPU_INT08U),                           // Size in bytes of each data element to be written.
                  file_size,                                    // Number of data elements, each one with size of 1 byte
                  p_file);
  if (result == 0) {
    p_error("Ex_FS_FileRdWr_PosixExec(): Error writing file");
    p_err->Code = RTOS_ERR_FAIL;
    goto end_file;
  }
  //                                                               File position to beginning of file.
  result = fseek(p_file,
                 0,                                             // Offset added to reference start position.
                 SEEK_SET);                                     // Reference start position: beginning of file.
  if (result != 0) {
    p_error("Ex_FS_FileRdWr_PosixExec(): Error positioning file");
    p_err->Code = RTOS_ERR_FAIL;
    goto end_file;
  }

  //                                                               Read data from file.
  Mem_Clr((void *)p_buf, file_size);
  result = fread((void *)p_buf,
                 sizeof(CPU_INT08U),                            // Size, in bytes, of each data element to be read.
                 file_size,                                     // Number of data elements, each one with size of 1 byte
                 p_file);
  if (result == 0) {
    p_error("Ex_FS_FileRdWr_PosixExec(): Error reading file");
    p_err->Code = RTOS_ERR_FAIL;
    goto end_file;
  }
  //                                                               Verify data.
  ok = Ex_FS_BufValidate(p_buf, file_size, pattern_start);
  if (!ok) {
    EX_TRACE("Ex_FS_FileRdWr_PosixExec(): data read from file mismatch\r\n");
    p_err->Code = RTOS_ERR_FAIL;
  }
  //                                                               --------------------- CLOSING ----------------------
end_file:
  result = fclose(p_file);                                      // Close file.
  APP_RTOS_ASSERT_CRITICAL(result == 0,; );

end_mem_seg:
  Mem_SegClr(&Ex_FS_MemSeg, &local_err);                        // Clear memory segment for next example.
  APP_RTOS_ASSERT_CRITICAL(local_err.Code == RTOS_ERR_NONE,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
