/***************************************************************************//**
 * @file
 * @brief TFT printf example for EFM32GG990F1024 using EBI addressed map access
 *        See other examples for direct drive operation
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
#include "em_device.h"
#include "em_cmu.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "retargettft.h"

volatile uint32_t msTicks; /* counts 1ms timeTicks */

/* Local prototypes */
void Delay(uint32_t dlyTicks);

/***************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 ******************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;       /* increment counter necessary in Delay()*/
}

/***************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 ******************************************************************************/
void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  int toggleLED = 0;

  /* Use 48MHZ HFXO as core clock frequency */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  /* Initialize DK board register access */
  /* This demo currently only works in EBI mode */
  BSP_Init(BSP_INIT_DEFAULT);

  /* If first word of user data page is non-zero, enable Energy Profiler trace */
  BSP_TraceProfilerSetup();

  /* Setup SysTick Timer for 10 msec interrupts  */
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
    while (1) ;
  }

  /* Initialize the TFT stdio retarget module. */
  RETARGET_TftInit();

  /* Update TFT display forever */
  while (1) {
    /* Output text on TFT - using address mapped mode */
    printf("Hello, EFM32 Giant Gecko world\n");

    /* Toggle led after each TFT_displayUpdate iteration */
    if (toggleLED) {
      BSP_LedsSet(0x0000);
      toggleLED = 0;
    } else {
      BSP_LedsSet(0x000f);
      toggleLED = 1;
    }
    Delay(1000);
  }
}
