/***************************************************************************//**
 * @file
 * @brief Prototype definitions for Range Test application and radio
 * configuration structs.
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

#ifndef RANGETESTSTATICCONFIG_H_
#define RANGETESTSTATICCONFIG_H_

typedef struct rangeTestFieldConfig{
  bool sendCrc;
  bool calcCrc;
  bool checkCrc;
  bool whEnable;
  uint16_t length;
} rangeTestFieldConfig_t;

typedef struct rangeTestChannelConfig{
  uint16_t chStart;
  uint16_t chEnd;
  uint32_t chSpacing;
  uint32_t baseFreq;
} rangeTestChannelConfig_t;

typedef struct rangeTestStaticConfig{
  uint32_t radioXTALFreq;
  char     radioDataRate[20u];
  char     radioModulation[20u];
  char     radioDeviation[20u];
  char     radioOutputPower[20u];
  char     rssiMinValue[5u];
  char     rssiMidValue[5u];
  char     rssiMaxValue[5u];
} rangeTestStaticConfig_t;

#endif /* RANGETESTSTATICCONFIG_H_ */
