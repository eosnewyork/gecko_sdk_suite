/***************************************************************************//**
 * @file
 * @brief helper functions for rendering capsense button states on the LCD
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

#ifndef RENDER_H
#define RENDER_H

#include "display.h"
#include <stdbool.h>

/* lcd display dimensions */
#define N_COLS                        2
#define N_ROWS                        4
#define N_BUTTONS                     (N_COLS * N_ROWS)
#define BUTTON_H                      out_off_height
#define BUTTON_W                      out_off_width
#define TOTAL_OUT_HEIGHT              (BUTTON_H * OUT_ROWS)
#define TOTAL_OUT_WIDTH               (BUTTON_W * OUT_COLS)

void RENDER_DrawButton(int posx, int posy, int status);
void RENDER_ClearFramebufferArea(int xstart, int ystart, int xend, int yend, int color);
void RENDER_UpdateDisplay(bool fullUpdate, DISPLAY_Device_t* displayDevice);

#endif /* RENDER_H */
