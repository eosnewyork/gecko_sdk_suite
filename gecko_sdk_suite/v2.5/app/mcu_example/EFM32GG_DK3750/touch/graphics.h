/***************************************************************************//**
 * @file
 * @brief 16 x 28 Font Definition
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

#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 16 x 28 pixel font */
extern const struct {
  unsigned int width;      /* Image width */
  unsigned int height;     /* Image height */
  unsigned int c_width;    /* Character width */
  unsigned int c_height;   /* Character height */
  uint16_t     data[1504 * 28];
} font16x28;

#ifdef __cplusplus
}
#endif

#endif
