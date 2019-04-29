/***************************************************************************//**
 * @file
 * @brief Auxiliary header for the RAIL library. Includes consistent definitions
 *   of features available across different chips.
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

#ifndef __RAIL_FEATURES_H__
#define __RAIL_FEATURES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "em_device.h"

// Radio capabilities
// Provides a consistent define to know whether a device supports the SubGHz,
// 2P4GHZ or both bands
#define RAIL_FEAT_DUAL_BAND_RADIO                                         \
  ((_SILICON_LABS_EFR32_RADIO_TYPE == _SILICON_LABS_EFR32_RADIO_DUALBAND) \
   || ((FEAT_RF_2G4 == 1) && (FEAT_RF_SUBG == 1)))

#define RAIL_FEAT_SUBGIG_RADIO                                               \
  (((_SILICON_LABS_EFR32_RADIO_TYPE == _SILICON_LABS_EFR32_RADIO_DUALBAND)   \
    || (_SILICON_LABS_EFR32_RADIO_TYPE == _SILICON_LABS_EFR32_RADIO_SUBGHZ)) \
   || (FEAT_RF_SUBG == 1))

#define RAIL_FEAT_2G4_RADIO                                                 \
  (((_SILICON_LABS_EFR32_RADIO_TYPE == _SILICON_LABS_EFR32_RADIO_DUALBAND)  \
    || (_SILICON_LABS_EFR32_RADIO_TYPE == _SILICON_LABS_EFR32_RADIO_2G4HZ)) \
   || (FEAT_RF_2G4 == 1))

// Antenna Diversity
#define RAIL_FEAT_ANTENNA_DIVERSITY \
  ((_SILICON_LABS_32B_SERIES_1_CONFIG >= 2) || (_SILICON_LABS_32B_SERIES == 2))

// Z-Wave is supported on all SubGHz-capable platforms for HW series 1,
// configuration 3+.
#define RAIL_FEAT_ZWAVE_SUPPORTED \
  ((_SILICON_LABS_32B_SERIES_1_CONFIG >= 3) && (RAIL_FEAT_SUBGIG_RADIO))

#ifdef __cplusplus
}
#endif

#endif // __RAIL_FEATURES_H__
