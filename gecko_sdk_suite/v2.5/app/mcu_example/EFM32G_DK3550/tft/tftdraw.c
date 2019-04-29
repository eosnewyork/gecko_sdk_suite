/***************************************************************************//**
 * @file
 * @brief EFM32G_DK3550 TFT address mapped example, drawing random rectangles
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

#include "glib/glib.h"
#include "glib/glib_color.h"
#include "dmd/ssd2119/dmd_ssd2119.h"

#include "tftdraw.h"

extern void Delay(uint32_t dlyTicks);

static uint32_t rnum = 0xabcd7381;

/***************************************************************************//**
 * @brief Really simple and silly random number generator
 * @param limit Upper limit of return value
 * @return Random number
 ******************************************************************************/
static int randomGenerator(int limit)
{
  rnum = ((rnum * 27) << 3) | (((rnum / 13) & 0xa5a7f196) + (rnum >> 13));

  return(rnum % limit);
}

/***************************************************************************//**
 * @brief Clears/updates entire background ready to be drawn
 * @param[in] gcPtr GLIB graphics context
 ******************************************************************************/
void TFT_displayUpdate(GLIB_Context_t *gcPtr)
{
  int            i, tmp;
  GLIB_Rectangle_t rect = {
    .xMin =   0,
    .yMin =   0,
    .xMax = 319,
    .yMax = 239,
  };
  static char    *efm32_hello = "32MHz / SSD2119 address mapped TFT\n";

  /* Set clipping region to entire image */
  GLIB_setClippingRegion(gcPtr, &rect);
  GLIB_resetDisplayClippingArea(gcPtr);

  /* Update text on top of picture  */
  gcPtr->foregroundColor = GLIB_rgbColor(200, 200, 200);
  GLIB_drawString(gcPtr, efm32_hello, strlen(efm32_hello), 0, 0, 1);

  /* Generate "wild" rectangle pattern  */
  for (i = 0; i < 20; i++) {
    rect.xMin = randomGenerator(320);
    rect.xMax = randomGenerator(320);
    rect.yMin = randomGenerator(230) + 10;
    rect.yMax = randomGenerator(230) + 10;
    if (rect.xMin > rect.xMax) {
      tmp       = rect.xMin;
      rect.xMin = rect.xMax;
      rect.xMax = tmp;
    }
    if (rect.yMin > rect.yMax) {
      tmp       = rect.yMin;
      rect.yMin = rect.yMax;
      rect.yMax = tmp;
    }
    gcPtr->foregroundColor = GLIB_rgbColor(128 + randomGenerator(127),
                                           randomGenerator(200),
                                           randomGenerator(255));
    GLIB_drawRectFilled(gcPtr, &rect);
  }
}
