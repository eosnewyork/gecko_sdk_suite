/***************************************************************************//**
 * @file
 * @brief Si114x Environmental mode functions
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

#ifndef __SI114x_ENVIRONMENTAL_H
#define __SI114x_ENVIRONMENTAL_H

int Si114x_MeasureEnvironmental(HANDLE si114x_handle, uint16_t *uvIndex, uint16_t *ps1, uint16_t *als);

int Si114x_ConfigureEnvironmental(HANDLE si114x_handle);

int Si114x_ConfigureHRM(HANDLE si114x_handle);

void Si114x_NoUV(void);

void Si114x_MeasureDarkOffset(HANDLE si114x_handle);
#endif
