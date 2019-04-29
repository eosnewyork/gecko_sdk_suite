/***************************************************************************//**
 * @file
 * @brief Scroll functions for LCD display.
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

#ifndef __SILICON_LABS_SCROLLLCD_H__
#define __SILICON_LABS_SCROLLLCD_H__

#include "display.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  scrollOff,
  scrollUp,
  scrollDown,
  scrollLeft
} scrollDirection_t;

void scrollLcd(DISPLAY_Device_t *displayDevice,
               scrollDirection_t direction,
               const char *pOldImg,
               const char *pNewImg);

#ifdef __cplusplus
}
#endif

#endif /* __SILICON_LABS_SCROLLLCD_H__ */
