/***************************************************************************//**
 * @file
 * @brief Sensor Puck demo for EFM32G
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

#ifndef SENSOR_PUCK_H_
#define SENSOR_PUCK_H_

#include "heart_rate_monitor.h"

#define GREEN_LED_PORT gpioPortC
#define GREEN_LED_PIN  1
#define RED_LED_PORT   gpioPortC
#define RED_LED_PIN    0

#define HRM_TIMEOUT_MS                90000
#define PERIODIC_UPDATE_MS            1000
#define SI114X_IRQ_PORT               gpioPortC
#define SI114X_IRQ_PIN                14
#define BLE_HOST_WAKEUP_PIN           11
#define PS1_SKIN_CONTACT_THRESHOLD    1500

#define SI114X_I2C_ADDR               0x60

typedef enum {
  ENVIRONMENTAL_MODE,
  BIOMETRIC_MODE
} Meas_Mode_TypeDef;

extern Meas_Mode_TypeDef measurementMode;

extern bool              bluetoothDataPending;

extern uint32_t          rhData;   /* milliperecent */
extern int32_t           tempData; /* millidegree C */
extern uint16_t          uvData;   /* index */
extern uint16_t          alsData;  /* lux */
extern uint32_t          vBat;     /* millivolt */

extern HRMSpO2State_t    HRMState;
extern int32_t           HRMRate;
extern uint16_t          HRMSample[];
extern HRMSpO2State_t    SpO2State;
extern int32_t           HRMSpO2Value;

void hrmSampleCallback(uint16_t sample);
void CalibrateLFRCO(void);
#endif /* SENSOR_PUCK_H_ */
