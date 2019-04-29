/***************************************************************************//**
 * @file
 * @brief HARDFAULT exception handler example for EFM32G_DK3550
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
#include <string.h>
#include <stdint.h>
#include "em_device.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "retargetserial.h"
#include "hardfault.h"

/* Note! You can set compile time define -DRETARGET_LEUART1 to build this
 * example to use LEUART instead of default UART1. See retargetserial.h for
 * details. */

/***************************************************************************//**
 * @brief  Dummy function to trigger mem access hardfault
 * @return Return value is just a dummy
 ******************************************************************************/
int BadFunctionAccess(void)
{
  int data = 0x00020100;

  memset(&data - 4, 0xff, 20);

  return data;
}

/***************************************************************************//**
 * @brief  Dummy function to trigger div by zero
 * @return Return value is just a dummy
 ******************************************************************************/
int BadFunctionDiv(void)
{
  uint32_t data = DEVINFO->UNIQUEL;
  volatile int x = 0;

  return data / x;
}

/***************************************************************************//**
 * @brief  Dummy function to trigger unaligned access
 * @return Return value is just a dummy
 ******************************************************************************/
uint32_t BadFunctionUnaligned(void)
{
  uint32_t *data = (uint32_t *)((uint8_t *)&DEVINFO->UNIQUEL + 1);

  return *data;
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  int result;

  /* Initialize DK board register access */
  BSP_Init(BSP_INIT_DEFAULT);

  /* If first word of user data page is non-zero, enable Energy Profiler trace */
  BSP_TraceProfilerSetup();

  /* Initialize USART and map LF to CRLF */
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

  printf("\nEFM32G_DK3550 HardFault handler example\n");

  HardFault_TrapDivByZero();
  HardFault_TrapUnaligned();

  /* Enable one of these function calls */
  // BadFunctionAccess();
  // BadFunctionDiv();
  result = BadFunctionUnaligned();

  printf("ERROR: The fault was not trapped (result=%d)\n", result);

  while (1) ;
}
