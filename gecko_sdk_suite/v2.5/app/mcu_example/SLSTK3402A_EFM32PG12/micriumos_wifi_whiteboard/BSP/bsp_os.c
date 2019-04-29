/***************************************************************************//**
 * @file
 * @brief uC/OS-III BSP source file
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

#include <cpu.h>
#include <os.h>

#include "em_cmu.h"

/**************************************************************************//**
 * @brief
 *   Initialize the os tick interrupt.
 *****************************************************************************/
void  BSP_OSTickInit (void)
{
  OS_CPU_SysTickInitFreq(SystemCoreClockGet());
}

/**************************************************************************//**
 * @brief
 *   Disable the os tick interrupt
 *****************************************************************************/
void BSP_OSTickDisable(void)
{
  SysTick->CTRL = 0x0000000;
}
