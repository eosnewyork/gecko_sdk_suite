/***************************************************************************//**
 * @file
 * @brief IO Core
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
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_IO_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <io/include/io.h>
#include  <rtos_description.h>

#if (defined(RTOS_MODULE_IO_SERIAL_SPI_AVAIL))
#include  <io/source/serial/spi/spi_priv.h>
#endif

#if (defined(RTOS_MODULE_IO_SD_AVAIL))
#include  <io/source/sd/sd_priv.h>
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               IO_Init()
 *
 * @brief    Initializes the IO modules.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
void IO_Init(RTOS_ERR *p_err)
{
#if (defined(RTOS_MODULE_IO_SERIAL_SPI_AVAIL))
  SPI_Init(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#endif

#if (defined(RTOS_MODULE_IO_SD_AVAIL))
  SD_Init(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#endif
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_IO_AVAIL))
