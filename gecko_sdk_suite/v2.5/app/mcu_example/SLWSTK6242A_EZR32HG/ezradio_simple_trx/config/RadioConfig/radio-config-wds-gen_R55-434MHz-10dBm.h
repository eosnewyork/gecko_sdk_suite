/***************************************************************************//**
 * @file
 * @brief radio config wds gen_R55 434MHz 10dBm
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

// Modem configuration parameters
// version: 1.3.0.117
// afc_en: 1
// ant_div: 0
// BER_mode: 0
// Ch_Fil_Bw_AFC: 0
// Chip_Version: 2
// crystal_tol: 20
// dsa_mode: 0
// dsource: 0
// ETSI: 0
// fc: 434000000
// Fdev: 20000
// Fdev_error: 0
// fhch: 10
// fhst: 250000
// freq_xo: 30000000
// hi_pfm_div_mode: 0
// High_perf_Ch_Fil: 1
// if_mode: 2
// inputBW: 0
// Manchester: 0
// Max_Rb_Error: 0
// modulation_type: 2
// OSRtune: 0
// pm_len: 16
// pm_pattern: 0
// raw_demod: 0
// Rsymb: 10000
// RXBW: 150000
// TC: 29

// Modem configuration calculated values
// if_freq: -468750.0
// nb_filter: 4
// nb_filter_bw: 82.6375
// wb_filter: 4
// wb_filter_bw: 82.6375

// Enclosing macro to prevent multiple inclusion
#ifndef __SLAB_RADIO_CONFIG_WDS__
#define __SLAB_RADIO_CONFIG_WDS__


#include "ezradio_patch_revC2A_general.h"


#define RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ 30000000L

#define RADIO_CONFIGURATION_DATA_RADIO_CHIP_FAMILY RADIO_CHIP_FAMILY_EZRADIO

#define RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH (8)
#define RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP (3)
#define RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET (61440)
#define RADIO_CONFIGURATION_DATA_VARIABLEPACKETSTATE (0)
#define RADIO_CONFIGURATION_DATA_CUSTOM_PAYLOAD {0x1C, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5, 0xC5}


#define RF_POWER_UP 0x02, 0x81, 0x00, 0x01, 0xC9, 0xC3, 0x80

#define RF_SET_PROPERTY_INT_CTL_ENABLE 0x11, 0x01, 0x01, 0x00, 0x05

#define RF_SET_PROPERTY_INT_CTL_PH_ENABLE 0x11, 0x01, 0x01, 0x01, 0x38

#define RF_SET_PROPERTY_INT_CTL_MODEM_ENABLE 0x11, 0x01, 0x01, 0x02, 0x00

#define RF_SET_PROPERTY_INT_CTL_CHIP_ENABLE 0x11, 0x01, 0x01, 0x03, 0x20

#define RF_SET_PROPERTY_FRR_CTL_A_MODE 0x11, 0x02, 0x01, 0x00, 0x01

#define RF_SET_PROPERTY_FRR_CTL_B_MODE 0x11, 0x02, 0x01, 0x01, 0x02

#define RF_SET_PROPERTY_FRR_CTL_C_MODE 0x11, 0x02, 0x01, 0x02, 0x09

#define RF_SET_PROPERTY_FRR_CTL_D_MODE 0x11, 0x02, 0x01, 0x03, 0x00

#define RF_SET_PROPERTY_EZCONFIG_XO_TUNE 0x11, 0x24, 0x01, 0x03, 0x45

#define RF_EZCONFIG_ARRAY_WRITE_0 0x66, 0x38, 0x97, 0xF7, 0xDA, 0x6B, 0x8C, 0x58, 0x94, 0x5A, 0xA3, 0x3A, 0x4B, 0xF6, 0x31, 0x43, 0x87, 0x6A, 0x68, 0x23, 0x88, 0xA4, 0xFC, 0xF2, 0x48, 0x5D, 0xF0, 0xC8, 0x87, 0x53, 0x6F, 0xA4, 0x9F, 0x6F, 0x22, 0x70, 0x45, 0xE5, 0xED, 0xD2, 0xE4, 0x48, 0x97, 0xE4, 0xE5, 0x71, 0xBE, 0x3E, 0xA8, 0xA9, 0x02, 0x59, 0x53, 0x55, 0xDA, 0x05, 0x3D, 0x83, 0x96, 0xD9, 0xE0, 0x64, 0x5F, 0xAF, 0xD7, 0xD5, 0x67, 0x6C, 0x6F, 0x0D, 0x46, 0xB9, 0xA8, 0xC5, 0x6F, 0x5C, 0x40, 0x40, 0x5B, 0x7C, 0x83, 0x09, 0x37, 0xAF, 0xF6, 0x64, 0x32, 0xDB, 0xDB, 0xE3, 0xBC, 0x4B, 0xE9, 0xDE, 0x13, 0x6E, 0xA1, 0x13, 0x02, 0x23, 0x0F, 0xC2, 0x84, 0xBE, 0x05, 0x69, 0x9F

#define RF_EZCONFIG_ARRAY_WRITE_NOP_0 0x00

#define RF_EZCONFIG_ARRAY_WRITE_1 0x66, 0x61, 0xA5, 0xE5, 0x0F, 0xB8, 0xB1, 0x23, 0x68, 0xD1, 0x9F, 0x3E, 0xA4, 0xAA, 0xAA, 0xFA, 0xD8, 0xE5, 0xDE, 0x16, 0xB3, 0xCC, 0xD1, 0x26, 0x66, 0xAB, 0xEF, 0x65, 0xDE, 0xB5, 0x3F, 0xEE, 0xD2, 0xCE, 0xCC, 0xD7, 0x4F, 0x63, 0x63, 0xA8, 0xA1, 0x04, 0x92, 0xB1, 0x08, 0xBC, 0x11, 0xF2, 0xBE, 0x8E, 0x0D, 0xF8, 0x35, 0x80, 0x17, 0xB5, 0xF7, 0x0E, 0x1E, 0x91, 0xB0, 0xE7, 0xA1, 0x6C, 0x83, 0x4C, 0xF7, 0x6B, 0x40, 0xDB, 0xF5, 0x1A, 0x60, 0x31, 0x4C, 0xFC, 0xAD, 0xCD, 0xE3, 0x15, 0x36, 0xED, 0x76, 0xA1, 0xF0, 0xC1, 0x0C, 0x5A, 0xF0, 0xD8, 0x55, 0xD0, 0x16, 0xBF, 0x55, 0x81, 0xF5, 0x1F, 0xC4, 0x03, 0xC5, 0x7F, 0x37, 0x2D, 0x56, 0x72, 0x78

#define RF_EZCONFIG_ARRAY_WRITE_NOP_1 0x00

#define RF_EZCONFIG_ARRAY_WRITE_2 0x66, 0xC5, 0x81, 0x5D, 0x70, 0x1E, 0x63, 0x29, 0x5C, 0xCB, 0x87, 0xB7, 0x18

#define RF_EZCONFIG_CHECK 0x19, 0x12, 0x65

#define RF_GPIO_PIN_CFG 0x13, 0x1D, 0x1D, 0x20, 0x21, 0x00, 0x00, 0x00

#define RF_SET_PROPERTY_PTI_CTL 0x11, 0xF0, 0x01, 0x00, 0x80

#define RF_SET_PROPERTY_PTI_LOG_EN 0x11, 0xF0, 0x01, 0x03, 0x60



#ifndef FIRMWARE_LOAD_COMPILE

#define RADIO_CONFIGURATION_DATA_ARRAY {\
  SI446X_PATCH_CMDS, \
  7, RF_POWER_UP, \
  5, RF_SET_PROPERTY_INT_CTL_ENABLE, \
  5, RF_SET_PROPERTY_INT_CTL_PH_ENABLE, \
  5, RF_SET_PROPERTY_INT_CTL_MODEM_ENABLE, \
  5, RF_SET_PROPERTY_INT_CTL_CHIP_ENABLE, \
  5, RF_SET_PROPERTY_FRR_CTL_A_MODE, \
  5, RF_SET_PROPERTY_FRR_CTL_B_MODE, \
  5, RF_SET_PROPERTY_FRR_CTL_C_MODE, \
  5, RF_SET_PROPERTY_FRR_CTL_D_MODE, \
  5, RF_SET_PROPERTY_EZCONFIG_XO_TUNE, \
  107, RF_EZCONFIG_ARRAY_WRITE_0, \
  1, RF_EZCONFIG_ARRAY_WRITE_NOP_0, \
  107, RF_EZCONFIG_ARRAY_WRITE_1, \
  1, RF_EZCONFIG_ARRAY_WRITE_NOP_1, \
  13, RF_EZCONFIG_ARRAY_WRITE_2, \
  3, RF_EZCONFIG_CHECK, \
  8, RF_GPIO_PIN_CFG, \
  5, RF_SET_PROPERTY_PTI_CTL, \
  5, RF_SET_PROPERTY_PTI_LOG_EN, \
  0 \
  }

#else // ! FIRMWARE_LOAD_COMPILE

#define RADIO_CONFIGURATION_DATA_ARRAY { 0 }

#endif // FIRMWARE_LOAD_COMPILE


#define RADIO_CONFIGURATION_DATA_BASE_FREQUENCY 434000000
#define RADIO_CONFIGURATION_DATA_BIT_ORDER FALSE
#define RADIO_CONFIGURATION_DATA_CENTER_FREQUENCY 434000000
#define RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER 0
#define RADIO_CONFIGURATION_DATA_CHANNEL_SPACING 250000
#define RADIO_CONFIGURATION_DATA_CHECK_CRC_0 TRUE
#define RADIO_CONFIGURATION_DATA_CHECK_CRC_1 TRUE
#define RADIO_CONFIGURATION_DATA_CHECK_CRC_2 FALSE
#define RADIO_CONFIGURATION_DATA_CHECK_CRC_3 FALSE
#define RADIO_CONFIGURATION_DATA_CHECK_CRC_4 FALSE
#define RADIO_CONFIGURATION_DATA_CHIP_HW_INTERRUPT 32
#define RADIO_CONFIGURATION_DATA_CRC_PADDING FALSE
#define RADIO_CONFIGURATION_DATA_CRC_START TRUE
#define RADIO_CONFIGURATION_DATA_CRYSTAL_CAP_BANK 69
#define RADIO_CONFIGURATION_DATA_CRYSTAL_FREQUENCY 30000000
#define RADIO_CONFIGURATION_DATA_CRYSTAL_TOLERANCE_RX 20
#define RADIO_CONFIGURATION_DATA_CRYSTAL_TOLERANCE_TX 20
#define RADIO_CONFIGURATION_DATA_CRYSTAL_USE_EXTERNAL_SOURCE FALSE
#define RADIO_CONFIGURATION_DATA_DRIVE_STRENGTH 0
#define RADIO_CONFIGURATION_DATA_ENABLE_CHIP_HW_INTERRUPT TRUE
#define RADIO_CONFIGURATION_DATA_ENABLE_CRC_0 TRUE
#define RADIO_CONFIGURATION_DATA_ENABLE_CRC_1 TRUE
#define RADIO_CONFIGURATION_DATA_ENABLE_CRC_2 FALSE
#define RADIO_CONFIGURATION_DATA_ENABLE_CRC_3 FALSE
#define RADIO_CONFIGURATION_DATA_ENABLE_CRC_4 FALSE
#define RADIO_CONFIGURATION_DATA_ENABLE_MODEM_HW_INTERRUPT FALSE
#define RADIO_CONFIGURATION_DATA_ENABLE_PACKET_HANDELR_HW_INTERRUPT TRUE
#define RADIO_CONFIGURATION_DATA_FLD_CRC_BITORDER 0
#define RADIO_CONFIGURATION_DATA_FLD_CRC_ENDIAN 0
#define RADIO_CONFIGURATION_DATA_FLD_INVERT_CRC FALSE
#define RADIO_CONFIGURATION_DATA_FLD_MANCHESTER_PATTERN 0
#define RADIO_CONFIGURATION_DATA_FLD_MODEM_4GFSK_MODE FALSE
#define RADIO_CONFIGURATION_DATA_FRR_CTL_A_MODE 1
#define RADIO_CONFIGURATION_DATA_FRR_CTL_B_MODE 2
#define RADIO_CONFIGURATION_DATA_FRR_CTL_C_MODE 9
#define RADIO_CONFIGURATION_DATA_FRR_CTL_D_MODE 0
#define RADIO_CONFIGURATION_DATA_GPIO0 29
#define RADIO_CONFIGURATION_DATA_GPIO0_PULLUP FALSE
#define RADIO_CONFIGURATION_DATA_GPIO1 29
#define RADIO_CONFIGURATION_DATA_GPIO1_PULLUP FALSE
#define RADIO_CONFIGURATION_DATA_GPIO2 32
#define RADIO_CONFIGURATION_DATA_GPIO2_PULLUP FALSE
#define RADIO_CONFIGURATION_DATA_GPIO3 33
#define RADIO_CONFIGURATION_DATA_GPIO3_PULLUP FALSE
#define RADIO_CONFIGURATION_DATA_HOP_ENABLE 0
#define RADIO_CONFIGURATION_DATA_HOP_PREAMBLE_TIMEOUT 20
#define RADIO_CONFIGURATION_DATA_HOP_RSSI_TIMEOUT 10
#define RADIO_CONFIGURATION_DATA_HOP_TABLE 
#define RADIO_CONFIGURATION_DATA_HOP_TYPE MANUAL_FREQUENCY_HOP
#define RADIO_CONFIGURATION_DATA_LDC_RX_TIME_ADJUST 1
#define RADIO_CONFIGURATION_DATA_LDC_SLEEP_TIME_ADJUST 1
#define RADIO_CONFIGURATION_DATA_LDC_STEP_SIZE_ADJUST 0
#define RADIO_CONFIGURATION_DATA_LENGTH_SUB 0
#define RADIO_CONFIGURATION_DATA_MATCH_BYTES 0
#define RADIO_CONFIGURATION_DATA_MATCH_LOGIC_1 AND
#define RADIO_CONFIGURATION_DATA_MATCH_LOGIC_2 AND
#define RADIO_CONFIGURATION_DATA_MATCH_LOGIC_3 AND
#define RADIO_CONFIGURATION_DATA_MATCH_MASK_0 00
#define RADIO_CONFIGURATION_DATA_MATCH_MASK_1 00
#define RADIO_CONFIGURATION_DATA_MATCH_MASK_2 00
#define RADIO_CONFIGURATION_DATA_MATCH_MASK_3 00
#define RADIO_CONFIGURATION_DATA_MATCH_OFFSET_0 0
#define RADIO_CONFIGURATION_DATA_MATCH_OFFSET_1 0
#define RADIO_CONFIGURATION_DATA_MATCH_OFFSET_2 0
#define RADIO_CONFIGURATION_DATA_MATCH_OFFSET_3 0
#define RADIO_CONFIGURATION_DATA_MATCH_POLARITY_0 MATCH
#define RADIO_CONFIGURATION_DATA_MATCH_POLARITY_1 MATCH
#define RADIO_CONFIGURATION_DATA_MATCH_POLARITY_2 MATCH
#define RADIO_CONFIGURATION_DATA_MATCH_POLARITY_3 MATCH
#define RADIO_CONFIGURATION_DATA_MATCH_VALUE_0 00
#define RADIO_CONFIGURATION_DATA_MATCH_VALUE_1 00
#define RADIO_CONFIGURATION_DATA_MATCH_VALUE_2 00
#define RADIO_CONFIGURATION_DATA_MATCH_VALUE_3 00
#define RADIO_CONFIGURATION_DATA_MODEM_HW_INTERRUPT 0
#define RADIO_CONFIGURATION_DATA_NIRQ 0
#define RADIO_CONFIGURATION_DATA_NIRQ_PULLUP FALSE
#define RADIO_CONFIGURATION_DATA_PACKET_HANDELR_HW_INTERRUPT 56
#define RADIO_CONFIGURATION_DATA_PH_FIELD_SPLIT FALSE
#define RADIO_CONFIGURATION_DATA_PKT_ALLOW_BIT_ERR 0
#define RADIO_CONFIGURATION_DATA_PKT_CRC_POLY 0
#define RADIO_CONFIGURATION_DATA_PKT_CRC_SEED 0
#define RADIO_CONFIGURATION_DATA_PKT_DATA_WHITENING_SEED FF FF
#define RADIO_CONFIGURATION_DATA_PKT_ENABLE_PREAMBLE TRUE
#define RADIO_CONFIGURATION_DATA_PKT_FIELD_4GFSK_0 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_FIELD_4GFSK_1 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_FIELD_4GFSK_2 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_FIELD_4GFSK_3 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_FIELD_4GFSK_4 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_FIELD_CONTAINING_LENGTH 1
#define RADIO_CONFIGURATION_DATA_PKT_FIELD_LENGTH_0 8
#define RADIO_CONFIGURATION_DATA_PKT_FIELD_LENGTH_1 0
#define RADIO_CONFIGURATION_DATA_PKT_FIELD_LENGTH_2 0
#define RADIO_CONFIGURATION_DATA_PKT_FIELD_LENGTH_3 0
#define RADIO_CONFIGURATION_DATA_PKT_FIELD_LENGTH_4 0
#define RADIO_CONFIGURATION_DATA_PKT_FIELD_MANCHESTER_0 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_FIELD_MANCHESTER_1 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_FIELD_MANCHESTER_2 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_FIELD_MANCHESTER_3 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_FIELD_MANCHESTER_4 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_FIELD_WHITEN_0 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_FIELD_WHITEN_1 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_FIELD_WHITEN_2 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_FIELD_WHITEN_3 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_FIELD_WHITEN_4 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_LENGTH_ADJUST 0
#define RADIO_CONFIGURATION_DATA_PKT_LENGTH_SIZE 1
#define RADIO_CONFIGURATION_DATA_PKT_LEN_ADD_TO_RX_FIFO TRUE
#define RADIO_CONFIGURATION_DATA_PKT_LEN_IN_MSB TRUE
#define RADIO_CONFIGURATION_DATA_PKT_NUMBER_OF_FIELDS 1
#define RADIO_CONFIGURATION_DATA_PKT_NUMBER_OF_RX_FIELDS 1
#define RADIO_CONFIGURATION_DATA_PKT_PREAMBLE_EXTENDED_TIMEOUT 0
#define RADIO_CONFIGURATION_DATA_PKT_PREAMBLE_MANCHESTER_CONSTANT FALSE
#define RADIO_CONFIGURATION_DATA_PKT_PREAMBLE_MANCHESTER_ENABLED FALSE
#define RADIO_CONFIGURATION_DATA_PKT_PREAMBLE_NONE_STANDARD 00 00 00 00
#define RADIO_CONFIGURATION_DATA_PKT_PREAMBLE_PATTERN 1
#define RADIO_CONFIGURATION_DATA_PKT_PREAMBLE_PATTERN_LENGTH 0
#define RADIO_CONFIGURATION_DATA_PKT_PREAMBLE_RX_BIT_ERROR 0
#define RADIO_CONFIGURATION_DATA_PKT_PREAMBLE_RX_THRESHOLD 20
#define RADIO_CONFIGURATION_DATA_PKT_PREAMBLE_TIMEOUT 15
#define RADIO_CONFIGURATION_DATA_PKT_PREAMBLE_TX_LENGTH 8
#define RADIO_CONFIGURATION_DATA_PKT_RESET_DATAWHITENING FALSE
#define RADIO_CONFIGURATION_DATA_PKT_RX_FIELD_4GFSK_0 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_RX_FIELD_4GFSK_1 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_RX_FIELD_4GFSK_2 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_RX_FIELD_4GFSK_3 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_RX_FIELD_4GFSK_4 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_RX_FIELD_LENGTH_0 0
#define RADIO_CONFIGURATION_DATA_PKT_RX_FIELD_LENGTH_1 0
#define RADIO_CONFIGURATION_DATA_PKT_RX_FIELD_LENGTH_2 0
#define RADIO_CONFIGURATION_DATA_PKT_RX_FIELD_LENGTH_3 0
#define RADIO_CONFIGURATION_DATA_PKT_RX_FIELD_LENGTH_4 0
#define RADIO_CONFIGURATION_DATA_PKT_RX_FIELD_MANCHESTER_0 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_RX_FIELD_MANCHESTER_1 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_RX_FIELD_MANCHESTER_2 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_RX_FIELD_MANCHESTER_3 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_RX_FIELD_MANCHESTER_4 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_RX_FIELD_WHITEN_0 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_RX_FIELD_WHITEN_1 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_RX_FIELD_WHITEN_2 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_RX_FIELD_WHITEN_3 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_RX_FIELD_WHITEN_4 FALSE
#define RADIO_CONFIGURATION_DATA_PKT_RX_THRESHOLD 48
#define RADIO_CONFIGURATION_DATA_PKT_SKIP_SYNCWORD FALSE
#define RADIO_CONFIGURATION_DATA_PKT_SKIP_SYNC_TIMEOUT FALSE
#define RADIO_CONFIGURATION_DATA_PKT_SYNC_4GFSK FALSE
#define RADIO_CONFIGURATION_DATA_PKT_SYNC_LENGTH 2
#define RADIO_CONFIGURATION_DATA_PKT_SYNC_MANCHESTER FALSE
#define RADIO_CONFIGURATION_DATA_PKT_SYNC_WORD 2D D4
#define RADIO_CONFIGURATION_DATA_PKT_SYNC_WORD_API B4 2B
#define RADIO_CONFIGURATION_DATA_PKT_TX_THRESHOLD 48
#define RADIO_CONFIGURATION_DATA_PKT_VARIABLE_LENGTH_FIELD 2
#define RADIO_CONFIGURATION_DATA_PKT_VARIABLE_PACKET_LENGTH_ENABLE FALSE
#define RADIO_CONFIGURATION_DATA_PKT_WHITENING_POLY 1280
#define RADIO_CONFIGURATION_DATA_PN_DIRECTION 1
#define RADIO_CONFIGURATION_DATA_POWER_ENABLE_RAMP_CONTROL FALSE
#define RADIO_CONFIGURATION_DATA_POWER_PA_BIAS 12
#define RADIO_CONFIGURATION_DATA_POWER_PA_MODE 0
#define RADIO_CONFIGURATION_DATA_POWER_PA_POWERLEVEL 127
#define RADIO_CONFIGURATION_DATA_POWER_RAMPING_TIME 14
#define RADIO_CONFIGURATION_DATA_POWER_RAMP_HV 29
#define RADIO_CONFIGURATION_DATA_POWER_REGULATOR_VOLTAGE 15
#define RADIO_CONFIGURATION_DATA_RFPARAM_4GFSK_MAP 0
#define RADIO_CONFIGURATION_DATA_RFPARAM_ADAPTIVE_CHFILL FALSE
#define RADIO_CONFIGURATION_DATA_RFPARAM_BANDWIDTH_TIME 50
#define RADIO_CONFIGURATION_DATA_RFPARAM_BER_MODE FALSE
#define RADIO_CONFIGURATION_DATA_RFPARAM_CHECKTHRESHOLDATLATCH FALSE
#define RADIO_CONFIGURATION_DATA_RFPARAM_DATA_RATE 10000
#define RADIO_CONFIGURATION_DATA_RFPARAM_DEVIATION 20000
#define RADIO_CONFIGURATION_DATA_RFPARAM_DEVIATION_ERROR_NEGATIVE 0
#define RADIO_CONFIGURATION_DATA_RFPARAM_DEVIATION_ERROR_POSITIVE 0
#define RADIO_CONFIGURATION_DATA_RFPARAM_ENABLEANTENNADIVERSITY FALSE
#define RADIO_CONFIGURATION_DATA_RFPARAM_ENABLEIQCALIBRATION FALSE
#define RADIO_CONFIGURATION_DATA_RFPARAM_ENABLE_HIGH_PERFORMANCE_CHANNEL_FILLTER TRUE
#define RADIO_CONFIGURATION_DATA_RFPARAM_ENABLE_MANUAL_RXBANDWIDTH FALSE
#define RADIO_CONFIGURATION_DATA_RFPARAM_ENABLE_PLL_AFC TRUE
#define RADIO_CONFIGURATION_DATA_RFPARAM_IF_MODE 2
#define RADIO_CONFIGURATION_DATA_RFPARAM_MODULATION_MODE PACKET
#define RADIO_CONFIGURATION_DATA_RFPARAM_MODULATION_TYPE FSK2
#define RADIO_CONFIGURATION_DATA_RFPARAM_OSR_TUNE 0
#define RADIO_CONFIGURATION_DATA_RFPARAM_PREAMBLE_PATTERN 0
#define RADIO_CONFIGURATION_DATA_RFPARAM_RSSI_AVERAGE 0
#define RADIO_CONFIGURATION_DATA_RFPARAM_RSSI_JUMP_THRESHOLD 12
#define RADIO_CONFIGURATION_DATA_RFPARAM_RSSI_LATCH 0
#define RADIO_CONFIGURATION_DATA_RFPARAM_RSSI_THRESHOLD 255
#define RADIO_CONFIGURATION_DATA_RFPARAM_RXBANDWIDTH 150000
#define RADIO_CONFIGURATION_DATA_RFPARAM_RXDATARATEERROR 0
#define RADIO_CONFIGURATION_DATA_RFPARAM_RX_CURRENT 0
#define RADIO_CONFIGURATION_DATA_RFPARAM_RX_PERFORMANCE 0
#define RADIO_CONFIGURATION_DATA_RX_CHECK_CRC_0 FALSE
#define RADIO_CONFIGURATION_DATA_RX_CHECK_CRC_1 FALSE
#define RADIO_CONFIGURATION_DATA_RX_CHECK_CRC_2 FALSE
#define RADIO_CONFIGURATION_DATA_RX_CHECK_CRC_3 FALSE
#define RADIO_CONFIGURATION_DATA_RX_CHECK_CRC_4 FALSE
#define RADIO_CONFIGURATION_DATA_RX_ENABLE_CRC_0 FALSE
#define RADIO_CONFIGURATION_DATA_RX_ENABLE_CRC_1 FALSE
#define RADIO_CONFIGURATION_DATA_RX_ENABLE_CRC_2 FALSE
#define RADIO_CONFIGURATION_DATA_RX_ENABLE_CRC_3 FALSE
#define RADIO_CONFIGURATION_DATA_RX_ENABLE_CRC_4 FALSE
#define RADIO_CONFIGURATION_DATA_SDO 0
#define RADIO_CONFIGURATION_DATA_SDO_PULLUP FALSE
#define RADIO_CONFIGURATION_DATA_SELECTED_CHIP si4455~revision[REVC2A]
#define RADIO_CONFIGURATION_DATA_SW_CRC_CTRL FALSE
#define RADIO_CONFIGURATION_DATA_SW_WHT_CTRL FALSE
#define RADIO_CONFIGURATION_DATA_SYNC_ERROR_ONLY_BEGIN 0
#define RADIO_CONFIGURATION_DATA_SYSCLOCK_32KHZ_CLOCK 0
#define RADIO_CONFIGURATION_DATA_SYSCLOCK_CLOCK_OUTPUT 0
#define RADIO_CONFIGURATION_DATA_SYSCLOCK_ENABLE_DIVIDED_CLOCK_OUTPUT FALSE
#define RADIO_CONFIGURATION_DATA_TRANSMIT_CRC_0 TRUE
#define RADIO_CONFIGURATION_DATA_TRANSMIT_CRC_1 TRUE
#define RADIO_CONFIGURATION_DATA_TRANSMIT_CRC_2 FALSE
#define RADIO_CONFIGURATION_DATA_TRANSMIT_CRC_3 FALSE
#define RADIO_CONFIGURATION_DATA_TRANSMIT_CRC_4 FALSE
#define RADIO_CONFIGURATION_DATA_WHT_BIT_NUM 0


#define RADIO_CONFIG_DATA_FIELD1_LENGTH RADIO_CONFIGURATION_DATA_PKT_FIELD_LENGTH_0
#define RADIO_CONFIG_DATA_FIELD2_LENGTH RADIO_CONFIGURATION_DATA_PKT_FIELD_LENGTH_1
#define RADIO_CONFIG_DATA_FIELD3_LENGTH RADIO_CONFIGURATION_DATA_PKT_FIELD_LENGTH_2
#define RADIO_CONFIG_DATA_FIELD4_LENGTH RADIO_CONFIGURATION_DATA_PKT_FIELD_LENGTH_3
#define RADIO_CONFIG_DATA_FIELD5_LENGTH RADIO_CONFIGURATION_DATA_PKT_FIELD_LENGTH_4


#define RADIO_CONFIGURATION_DATA { \
                                   Radio_Configuration_Data_Array,                       \
                                   RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER,              \
                                   RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH,         \
                                   RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP,  \
                                   RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET, \
                                   RADIO_CONFIGURATION_DATA_VARIABLEPACKETSTATE,         \
                                   RADIO_CONFIGURATION_DATA_CUSTOM_PAYLOAD               \
                                  }

#endif // __SLAB_RADIO_CONFIG_WDS__


//        _  _                          
//       | )/ )         Wireless        
//    \\ |//,' __       Application     
//    (")(_)-"()))=-    Software        
//       (\\            Platform        
