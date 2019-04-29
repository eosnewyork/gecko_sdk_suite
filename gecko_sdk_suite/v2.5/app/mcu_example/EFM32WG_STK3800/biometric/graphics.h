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

#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>
#include "heart_rate_monitor.h"
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/
void GRAPHICS_Init(bool boost);
bool GRAPHICS_ShowHRMStatus(bool acquiring, int32_t pulse, bool scrollInfo);
bool GRAPHICS_ShowSpO2Status(bool acquiring, int32_t spo2, bool scrollInfo);
void GRAPHICS_SetBatteryStatus(bool lowBat);
void GRAPHICS_DrawUV(uint32_t uvData);
void GRAPHICS_DrawHumidity(uint32_t rhData);
void GRAPHICS_DrawTemperatureC(int32_t tempData);
void GRAPHICS_DrawTemperatureF(int32_t tempData);
void GRAPHICS_DrawError (void);
void GRAPHICS_DrawInit (HeartRateMonitor_Config_t hrm_config, char *hrmVersion, char*biodemoVersion, bool usbStatus);
void GRAPHICS_ShowLEDSelectMenu(void);

typedef enum {
  LCD_HRM,
  LCD_SPO2,
  LCD_RH,
  LCD_TEMPC,
  LCD_TEMPF,
  LCD_UV
}displayType_t;

#ifdef __cplusplus
}
#endif

#endif /* __GRAHPHICS_H */
