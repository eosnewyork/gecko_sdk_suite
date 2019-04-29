/***************************************************************************//**
 * @file
 * @brief  Simple wrapper for some emWin functions.
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

#ifndef PLOT_H
#define PLOT_H

#include <GUI.h>

#if defined(__cplusplus)
extern "C" {
#endif

void PLOT_Bar(int height, int lowerRightXpos, int lowerYpos, int width, int maxHeight);
void PLOT_BarSolid(int height, int lowerRightXpos, int lowerYpos, int width, int maxHeight);
void PLOT_DisplayInit(void);
void PLOT_DisplayUpdate(void);
void PLOT_Init(void);
void PLOT_Puts(const char *str, int xpos, int ypos);
void PLOT_PutsCentered(const char *str, int xpos, int ypos);
void PLOT_ToneBar(int level, int lowerRightXpos);
void PLOT_VolumeBar(float level, int lowerRightXpos);

#if defined(__cplusplus)
}
#endif

#endif