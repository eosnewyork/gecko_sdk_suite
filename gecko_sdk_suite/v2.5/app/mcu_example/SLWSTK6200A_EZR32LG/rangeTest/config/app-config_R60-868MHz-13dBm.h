/***************************************************************************//**
 * @file
 * @brief Range Test application configuration for SLWSTK6200A kit.
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

#ifndef RANGETESTCONFIG_H_
#define RANGETESTCONFIG_H_

#define RANGETEST_CHANNEL_DEFINITIONS \
  { /* Start, End, Spacing, Base */   \
    { 0, 16, 250000, 868000000 },     \
  }

#define RANGETEST_FIELD_CONFIG \
  {                            \
    /*sendCrc*/ true,          \
    /*calcCrc*/ true,          \
    /*checkCrc*/ true,         \
    /*whitenEnable*/ false,    \
    /*length*/ 24,             \
  }

//TX: 11 chars, RX: 7 chars, TRX: 7 chars
#define RANGETEST_RADIO_DATA_RATE      "9.6kbps"

//TX: 10 chars, RX: 5 chars, TRX: 5 chars
#define RANGETEST_RADIO_MODULATION     "2GFSK"

//TX: 11 chars, RX: 7 chars, TRX: 7 chars
#define RANGETEST_RADIO_DEVIATION      "4.8kHz"

//TX: 8 chars, TRX: 6 chars
#define RANGETEST_RADIO_OUTPUT_POWER   "+13dBm"

// RSSI chart Y scale values
#define RANGETEST_RADIO_RSSI_MIN_VALUE "-115"
#define RANGETEST_RADIO_RSSI_MID_VALUE "-42"
#define RANGETEST_RADIO_RSSI_MAX_VALUE "10"

#endif /* RANGETESTCONFIG_H_ */
