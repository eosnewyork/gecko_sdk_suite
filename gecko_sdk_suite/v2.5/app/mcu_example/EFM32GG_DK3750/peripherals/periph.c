/***************************************************************************//**
 * @file
 * @brief Peripheral Toggle Example, shows how to enable and disable
 *     on board peripherals on the EFM32GG_DK3750 development kit. Each
 *     peripheral has an on-board LED showing the peripherals that are enabled.
 *     The API is necessary to reduce back-powering mainly, and reusing pins
 *     for alternative functionality.
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
#include "em_cmu.h"
#include "bsp.h"
#include "bsp_trace.h"

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
  /* Initialize DK board register access */
  BSP_Init(BSP_INIT_DEFAULT);

  /* If first word of user data page is non-zero, enable Energy Profiler trace */
  BSP_TraceProfilerSetup();

  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
    while (1) ;
  }

  /* "Silly" loop that just enables peripheral access to the EFM32, and then
   * disables them again. Verify that DK LEDs light up when access is enabled */
  while (1) {
    /* Enable peripheral */
    BSP_PeripheralAccess(BSP_MICROSD, true); Delay(500);
    BSP_PeripheralAccess(BSP_I2C, true); Delay(500);
    BSP_PeripheralAccess(BSP_TOUCH, true); Delay(500);
    BSP_PeripheralAccess(BSP_I2S, true); Delay(500);
    BSP_PeripheralAccess(BSP_AUDIO_OUT, true); Delay(500);
    BSP_PeripheralAccess(BSP_AUDIO_IN, true); Delay(500);
    BSP_PeripheralAccess(BSP_ANALOG_DIFF, true); Delay(500);
    BSP_PeripheralAccess(BSP_ANALOG_SE, true); Delay(500);
    BSP_PeripheralAccess(BSP_ETH, true); Delay(500);
    BSP_PeripheralAccess(BSP_RS232_UART, true); Delay(500);
    BSP_PeripheralAccess(BSP_RS232_LEUART, true); Delay(500);
    BSP_PeripheralAccess(BSP_TRACE, true); Delay(500);
    /* Disable peripheral */
    BSP_PeripheralAccess(BSP_MICROSD, false); Delay(500);
    BSP_PeripheralAccess(BSP_I2C, false); Delay(500);
    BSP_PeripheralAccess(BSP_TOUCH, false); Delay(500);
    BSP_PeripheralAccess(BSP_I2S, false); Delay(500);
    BSP_PeripheralAccess(BSP_AUDIO_OUT, false); Delay(500);
    BSP_PeripheralAccess(BSP_AUDIO_IN, false); Delay(500);
    BSP_PeripheralAccess(BSP_ANALOG_DIFF, false); Delay(500);
    BSP_PeripheralAccess(BSP_ANALOG_SE, false); Delay(500);
    BSP_PeripheralAccess(BSP_ETH, false); Delay(500);
    BSP_PeripheralAccess(BSP_RS232_UART, false); Delay(500);
    BSP_PeripheralAccess(BSP_RS232_LEUART, false); Delay(500);
    BSP_PeripheralAccess(BSP_TRACE, false); Delay(500);
  }
}
