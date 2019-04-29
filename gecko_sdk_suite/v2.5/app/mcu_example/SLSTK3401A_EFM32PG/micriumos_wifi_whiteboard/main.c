/***************************************************************************//**
 * @file
 * @brief MicOS + WiFi + Graphics LCD Demo
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"

#include <bsp_os.h>
#include "bspconfig.h"
#include "app.h"

#include <common/include/rtos_utils.h>
#include <kernel/include/os.h>
#include <kernel/include/os_trace.h>

/**************************************************************************//**
 * @brief Main entry point.
 *****************************************************************************/
int main(void)
{
  RTOS_ERR err;

  CPU_Init();
  BSP_SystemInit();

  // Override the kernel's default configuration to reduce data footprint.
  OS_ConfigureMsgPoolSize(16);

  // Initialize Micrium OS Kernel.
  OS_TRACE_INIT();
  OSInit(&err);

  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

  // Initialize the application tasks.
  APP_Init();

  // Start Micrium OS
  OSStart(&err);

  // We should never get here.
  while (1) {
    ;
  }
}
