/***************************************************************************//**
 * @file
 * @brief token.h
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

#ifndef __TOKEN_H_
#define __TOKEN_H_

#include <stdint.h>

#include "token_macros.h"
#include "token_config.h"

void     TOKEN_init     (void);

/*
   uint8_t  TOKEN_getU8    ( uint32_t token );
   uint16_t TOKEN_getU16   ( uint32_t token );
   uint32_t TOKEN_get32    ( uint32_t token );
   uint16_t TOKEN_getCount ( void );
 */

#endif
