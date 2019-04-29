/***************************************************************************//**
 * @file
 * @brief TFT Display refresh handling
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

#ifndef __SLIDES_H
#define __SLIDES_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RGB_BUFFER_SIZE    512
#define PALETTE_SIZE       1024

void SLIDES_showError(bool fatal, const char* fmt, ...);
void SLIDES_showBMP(char *fileName);
void SLIDES_init(void);

#ifdef __cplusplus
}
#endif

#endif
