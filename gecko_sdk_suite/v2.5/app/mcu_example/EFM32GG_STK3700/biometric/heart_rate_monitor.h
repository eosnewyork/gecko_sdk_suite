/***************************************************************************//**
 * @file
 * @brief Heart Rate and SpO2 state machine
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

#ifndef __HEARTRATEMONITOR_H
#define __HEARTRATEMONITOR_H
#include "si114x_sys_out.h"
typedef enum {
  BIOMETRIC_EXP,
  SI1143_PS,
  SI1147_PS
}HeartRateMonitor_Config_t;

void HeartRateMonitor_Init(Si114xPortConfig_t* i2c, HeartRateMonitor_Config_t);
bool HeartRateMonitor_Loop (bool displaySpO2, bool forceStop, bool checkSkinContact);
void HeartRateMonitor_TimerEventHandler(void);
void HeartRateMonitor_Interrupt (void);
bool HeartRateMonitor_SamplesPending (void);
void HeartRateMonitor_GetVersion (char *hrmVersion);

#endif /* __HEARTRATEMONITOR_H */
