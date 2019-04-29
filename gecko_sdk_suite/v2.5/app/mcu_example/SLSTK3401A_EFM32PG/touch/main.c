/***************************************************************************//**
 * @file
 * @brief Capacitive touch example for SLSTK3401A
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

#include <stdio.h>
#include <stdlib.h>

#include "em_device.h"
#include "em_chip.h"

#include "capsense.h"
#include "display.h"
#include "retargettextdisplay.h"

static volatile uint32_t msTicks; /* counts 1ms timeTicks */

/***************************************************************************//**
 * @brief
 *   SysTick_Handler. Interrupt Service Routine for system tick counter
 ******************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;
}

/***************************************************************************//**
 * @brief
 *   Delays number of msTick Systicks (typically 1 ms)
 *
 * @param dlyTicks
 *   Number of ticks (ms) to delay
 ******************************************************************************/
static void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}

/***************************************************************************//**
 * @brief
 *   Main function
 ******************************************************************************/
int main(void)
{
  int i;

  /* Chip errata */
  CHIP_Init();

  /* Setup SysTick timer for 1 msec interrupts  */
  if (SysTick_Config(SystemCoreClockGet() / 1000)) {
    while (1) ;
  }

  /* Initialize the display module. */
  DISPLAY_Init();

  /* Retarget stdio to the display. */
  RETARGET_TextDisplayInit();

  /* Start capacitive sense buttons */
  CAPSENSE_Init();

  i = 0;
  printf("\n\n\n  %3d", i);

  while (1) {
    Delay(100);
    CAPSENSE_Sense();

    if (CAPSENSE_getPressed(BUTTON1_CHANNEL)
        && !CAPSENSE_getPressed(BUTTON0_CHANNEL)) {
      i++;
      printf("\r  %3d", i);
    } else if (CAPSENSE_getPressed(BUTTON0_CHANNEL)
               && !CAPSENSE_getPressed(BUTTON1_CHANNEL)) {
      if (i > 0) {
        i--;
      }
      printf("\r  %3d", i);
    }
  }
}
