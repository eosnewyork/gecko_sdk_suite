/***************************************************************************//**
 * @file
 * @brief Draws the graphics on the display
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

#include "graphics.h"
#include "em_types.h"
#include "glib.h"
#include "dmd.h"
#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"
#include <string.h>
#include <stdio.h>

static GLIB_Context_t glibContext;          /* Global glib context */

/***************************************************************************//**
 * @brief Initializes the graphics stack.
 * @note This function will /hang/ if errors occur (usually
 *       caused by faulty displays.
 ******************************************************************************/
void GRAPHICS_Init(void)
{
  EMSTATUS status;

  /* Initialize the display module. */
  status = DISPLAY_Init();
  if (DISPLAY_EMSTATUS_OK != status) {
    while (1)
      ;
  }

  /* Initialize the DMD module for the DISPLAY device driver. */
  status = DMD_init(0);
  if (DMD_OK != status) {
    while (1)
      ;
  }

  status = GLIB_contextInit(&glibContext);
  if (GLIB_OK != status) {
    while (1)
      ;
  }

  glibContext.backgroundColor = Black;
  glibContext.foregroundColor = White;

  /* Use Narrow font */
  GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
}

/***************************************************************************//**
 * @brief This function prints the temperature on the display
 * @param temp
 *        The temperature in celsius
 ******************************************************************************/
void GRAPHICS_ShowTemp(float temp)
{
  char buffer[9];
  snprintf(buffer, sizeof(buffer), "%3.1f C", temp);
  GLIB_clear(&glibContext);
  GLIB_drawString(&glibContext, "ADC temp. sensor: ", 32, 5, 5, 0);
  GLIB_drawString(&glibContext, buffer, 32, 5, 15, 0);
  if (temp == -100.0) {
    GLIB_drawString(&glibContext, "It is either very", 32, 5, 35, 0);
    GLIB_drawString(&glibContext, "cold today, or the", 32, 5, 45, 0);
    GLIB_drawString(&glibContext, "ADC temp. sensor", 32, 5, 55, 0);
    GLIB_drawString(&glibContext, "is not calibrated.", 32, 5, 65, 0);
  }
  DMD_updateDisplay();
}
