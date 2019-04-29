/***************************************************************************//**
 * @file
 * @brief Range Test static configuration source.
 * This file contains the different application configurations for each
 * separate radio cards.
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

#include <stdint.h>
#include <stdbool.h>

#include "radio-configuration.h"
#include "app-configuration.h"
#include "rangetestconfigtypes.h"

const rangeTestChannelConfig_t rangeTestChannelConf[] =
  RANGETEST_CHANNEL_DEFINITIONS;

const rangeTestStaticConfig_t rangeTestConf =
{
#if defined(RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ)
  RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ,
#elif defined(RADIO_CONFIGURATION_DATA_CRYSTAL_FREQUENCY)
  RADIO_CONFIGURATION_DATA_CRYSTAL_FREQUENCY,
#else
  RANGETEST_RADIO_XTAL_FREQ,
#endif
  { RANGETEST_RADIO_DATA_RATE },
  { RANGETEST_RADIO_MODULATION },
  { RANGETEST_RADIO_DEVIATION },
  { RANGETEST_RADIO_OUTPUT_POWER },
  { RANGETEST_RADIO_RSSI_MIN_VALUE },
  { RANGETEST_RADIO_RSSI_MID_VALUE },
  { RANGETEST_RADIO_RSSI_MAX_VALUE },
};

const uint8_t radio_config[] = RADIO_CONFIGURATION_DATA_ARRAY;

rangeTestFieldConfig_t rangeTestFieldConf = RANGETEST_FIELD_CONFIG;
