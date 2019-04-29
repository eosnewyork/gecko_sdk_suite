/***************************************************************************//**
 * @file
 * @brief Simple LED Blink Demo
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
#include "em_cmu.h"
#include "em_emu.h"
#include "bsp.h"
#include "bsp_trace.h"

volatile uint32_t msTicks; // counts 1ms timeTicks

void Delay(uint32_t dlyTicks);

/***************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 ******************************************************************************/
void SysTick_Handler(void)
{
  msTicks++; // increment counter necessary in Delay()
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
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;

  // Chip errata
  CHIP_Init();

  // Increase VMCU to 3.3 V if powered by MCU USB port.
  EMU->R5VOUTLEVEL = 10U << _EMU_R5VOUTLEVEL_OUTLEVEL_SHIFT;

  EMU_DCDCInit(&dcdcInit);

  // If first word of user data page is non-zero, enable Energy Profiler trace
  BSP_TraceProfilerSetup();

  // Setup SysTick Timer for 1 msec interrupts
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
    while (1) ;
  }

  // Initialize LED driver
  BSP_LedsInit();
  // Turn on LED1
  BSP_LedToggle(1);

  // Infinite blink loop
  while (true) {
    Delay(1000);
    // Toggle LEDs
    BSP_LedToggle(0);
    BSP_LedToggle(1);
  }
}
