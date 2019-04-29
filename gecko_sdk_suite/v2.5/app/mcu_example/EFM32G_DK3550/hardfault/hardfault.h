/***************************************************************************//**
 * @file
 * @brief Hardfault handler for Cortex-M3, prototypes and definitions
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
/***************************************************************************//**
 * @author Joseph Yiu, Frank Van Hooft, Silicon Labs
 ******************************************************************************/

#ifndef __HARDFAULT_H
#define __HARDFAULT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void HardFault_TrapDivByZero(void);
void HardFault_TrapUnaligned(void);
void HardFault_HandlerC(uint32_t *stack_pointer);

#ifdef __cplusplus
}
#endif

#endif
