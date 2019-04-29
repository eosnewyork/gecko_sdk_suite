/***************************************************************************//**
 * @file
 * @brief This header file defines variables to be shared between the main
 * test application and customer specific sections.
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

#ifndef __APPS_COMMON_H__
#define __APPS_COMMON_H__

#include "rail_types.h"

bool inRadioState(RAIL_RadioState_t state, char *command);
const char *getRfStateName(RAIL_RadioState_t state);
bool inRadioState(RAIL_RadioState_t state, char *command);

// RAIL instance handle
extern RAIL_Handle_t railHandle;
extern uint16_t channel;

// Indicator of last power level requested for use
extern uint8_t lastSetTxPowerLevel;

extern bool redrawDisplay;
extern bool receiveModeEnabled;

typedef struct Stats{
  uint32_t samples;
  int32_t min;
  int32_t max;
  float mean;
  float varianceTimesSamples;
} Stats_t;

typedef struct Counters{
  // Counts all successful user transmits
  // "user" in this and following variable names refers to
  // a transmit that a user initiated, i.e. not an ack
  uint32_t userTx;
  // Counts all successful ack transmits
  uint32_t ackTx;
  uint32_t userTxAborted;
  uint32_t ackTxAborted;
  uint32_t userTxBlocked;
  uint32_t ackTxBlocked;
  uint32_t userTxUnderflow;
  uint32_t ackTxUnderflow;

  // Channel busy doesn't differentiate
  // between ack/user packets
  uint32_t txChannelBusy;

  uint32_t receive;
  uint32_t syncDetect;
  uint32_t preambleDetect;
  uint32_t frameError;
  uint32_t rxOfEvent;
  uint32_t addrFilterEvent;
  uint32_t rxFail;
  uint32_t calibrations;
  uint32_t noRxBuffer;
  uint32_t rfSensedEvent;
  uint32_t perTriggers;
  uint32_t ackTimeout;
  uint32_t lbtSuccess;
  uint32_t lbtRetry;
  uint32_t lbtStartCca;
  uint32_t txFifoAlmostEmpty;
  uint32_t rxFifoAlmostFull;
  uint32_t timingLost;
  uint32_t timingDetect;
  uint32_t radioConfigChanged;
  Stats_t rssi;
} Counters_t;

extern Counters_t counters;

#endif // __APPS_COMMON_H__
