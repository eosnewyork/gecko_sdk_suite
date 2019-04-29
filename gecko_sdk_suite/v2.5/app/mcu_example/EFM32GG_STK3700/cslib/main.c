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
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "em_device.h"
#include "em_chip.h"
#include "cslib.h"
#include "device_init.h"
#include "comm_routines.h"

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  CHIP_Init();

  systemInit();

  CSLIB_commInit();

  CSLIB_initHardware();
  CSLIB_initLibrary();

  // Infinite loop
  while (1) {
    CSLIB_update();
    CSLIB_commUpdate();
    CSLIB_lowPowerUpdate();
  }
}
