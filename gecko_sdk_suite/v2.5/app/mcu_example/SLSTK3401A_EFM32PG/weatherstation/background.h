/***************************************************************************//**
 * @file
 * @brief Background image for the weatherstation demo
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

#ifndef __BACKGROUND_H__
#define __BACKGROUND_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BACKGROUND_XSIZE 384
#define BACKGROUND_YSIZE 128
extern const uint8_t background[6144];

#ifdef __cplusplus
}
#endif

#endif /* __BACKGROUND_H__ */
