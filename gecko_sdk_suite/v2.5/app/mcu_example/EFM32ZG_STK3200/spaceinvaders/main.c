/***************************************************************************//**
 * @file
 * @brief Space Invaders Game for EFM32ZG_STK3200
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
#include "em_emu.h"
#include "em_cmu.h"
#include "em_chip.h"
#include "spaceinvaders.h"

/***************************************************************************//**
 * @brief  SYSTICK interrupt handler
 ******************************************************************************/
void SysTick_Handler(void)
{
  SPACEINVADERS_TimerEventHandler();
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* Enable HFXO as the main clock */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
    while (1) ;
  }

  /* Initiate game */
  SPACEINVADERS_Init();

  /* Enter game loop. Will not return */
  SPACEINVADERS_GameLoop();

  return 0;
}
