/***************************************************************************//**
 * @file
 * @brief Simple GPIO interrupt dispatcher Demo for EFM32TG_STK3300
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
#include "em_gpio.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "gpiointerrupt.h"

/***************************************************************************//**
 * @brief  Gpio callback
 * @param  pin - pin which triggered interrupt
 ******************************************************************************/
void gpioCallback(uint8_t pin)
{
  if (pin == 8) {
    BSP_LedClear(0);
  } else if (pin == 11) {
    BSP_LedSet(0);
  }
}

/***************************************************************************//**
 * @brief  Gpio setup. Setup button pins to trigger falling edge interrupts.
 *  Register callbacks for that interrupts.
 ******************************************************************************/
void gpioSetup(void)
{
  /* Enable GPIO in CMU */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Initialize GPIO interrupt dispatcher */
  GPIOINT_Init();

  /* Configure PD8 and PB11 as input */
  GPIO_PinModeSet(gpioPortD, 8, gpioModeInput, 0);
  GPIO_PinModeSet(gpioPortB, 11, gpioModeInput, 0);

  /* Register callbacks before setting up and enabling pin interrupt. */
  GPIOINT_CallbackRegister(8, gpioCallback);
  GPIOINT_CallbackRegister(11, gpioCallback);

  /* Set falling edge interrupt for both ports */
  GPIO_IntConfig(gpioPortD, 8, false, true, true);
  GPIO_IntConfig(gpioPortB, 11, false, true, true);
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* If first word of user data page is non-zero, enable Energy Profiler trace */
  BSP_TraceProfilerSetup();

  /* Initialize gpio */
  gpioSetup();

  /* Initialize LED driver */
  BSP_LedsInit();

  /* Infinite loop */
  while (1) ;
}
