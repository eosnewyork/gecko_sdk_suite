/***************************************************************************//**
 * @file
 * @brief token_macros.h
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

#ifndef __TOKEN_MACROS_H_
#define __TOKEN_MACROS_H_

#include <stdint.h>

#include "token_config.h"

#define TOKEN_ADDR_(x, y) x##y
#define TOKEN_ADDR(name) TOKEN_ADDR_(name, _ADDR)

#define TOKEN_getU8(name) (*(uint8_t *)TOKEN_ADDR(name))
#define TOKEN_getU16(name) (*(uint16_t *)TOKEN_ADDR(name))
#define TOKEN_getU32(name) (*(uint32_t *)TOKEN_ADDR(name))

void     TOKEN_init     (void);
uint16_t TOKEN_getCount (void);

#endif
