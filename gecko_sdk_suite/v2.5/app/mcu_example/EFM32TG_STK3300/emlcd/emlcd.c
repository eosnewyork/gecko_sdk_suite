/***************************************************************************//**
 * @file
 * @brief LCD controller and Energy Mode/RTC demo for EFM32TG_STK3300
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

#include <stdint.h>
#include <stdbool.h>
#include "em_device.h"
#include "em_chip.h"
#include "segmentlcd.h"
#include "rtcdriver.h"
#include "lcdtest.h"
#include "bsp_trace.h"

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* If first word of user data page is non-zero, enable Energy Profiler trace */
  BSP_TraceProfilerSetup();

  /* Enable LCD without voltage boost */
  SegmentLCD_Init(false);

  /* Run Energy Mode with LCD demo, see lcdtest.c */
  Test();

  /* Never going to reach this statement. Infinte loop in Test(). */
  return 0;
}
