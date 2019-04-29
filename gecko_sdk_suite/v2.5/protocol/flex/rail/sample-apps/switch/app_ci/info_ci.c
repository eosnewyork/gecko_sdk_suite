/***************************************************************************//**
 * @file
 * @brief This file implements informational commands for RAIL test applications.
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
#include <string.h>
#include "command_interpreter.h"
#include "response_print.h"

#include "rail.h"
#include "rail_types.h"
#include "app_common.h"

const char *getRfStateName(RAIL_RadioState_t state)
{
  switch (state) {
    case RAIL_RF_STATE_IDLE:
      return "Idle";
    case RAIL_RF_STATE_RX:
      return "Rx";
    case RAIL_RF_STATE_TX:
      return "Tx";
    case RAIL_RF_STATE_RX_ACTIVE:
      return "RxActive";
    case RAIL_RF_STATE_TX_ACTIVE:
      return "TxActive";
    case RAIL_RF_STATE_INACTIVE:
      return "Inactive";
    default:
      //Check individual rail state bits if RAIL state is unknown
      return "Unknown";
  }
}

const char *getStatusMessage(RAIL_Status_t status)
{
  switch (status) {
    case RAIL_STATUS_NO_ERROR:
      return "Success";
    case RAIL_STATUS_INVALID_PARAMETER:
      return "InvalidParameter";
    case RAIL_STATUS_INVALID_STATE:
      return "InvalidState";
    case RAIL_STATUS_INVALID_CALL:
      return "InvalidCall";
    default:
      return "Unknown";
  }
}

void getRssi(int argc, char **argv)
{
  char bufRssi[10];
  bool wait = false;

  if (argc == 2) {
    wait = !!ciGetUnsigned(argv[1]);
  }
  int16_t rssi = RAIL_GetRssi(railHandle, wait);

  // The lowest negative value is used to indicate an error reading the RSSI
  if (rssi == RAIL_RSSI_INVALID) {
    responsePrintError(argv[0], 0x08, "Could not read RSSI. Ensure Rx is enabled");
    return;
  }

  sprintfFloat(bufRssi, sizeof(bufRssi), ((float) rssi / 4), 2);
  responsePrint(argv[0], "rssi:%s", bufRssi);
}
void startAvgRssi(int argc, char **argv)
{
  uint32_t averageTimeUs = ciGetUnsigned(argv[1]);
  uint8_t avgChannel = channel;
  if (argc == 3) {
    avgChannel = ciGetUnsigned(argv[2]);
  }
  if (!inRadioState(RAIL_RF_STATE_IDLE, NULL)) {
    responsePrintError(argv[0], 0x08, "Could not read RSSI. Ensure RX is disabled.");
    return;
  } else if (RAIL_StartAverageRssi(railHandle, avgChannel, averageTimeUs, NULL) != RAIL_STATUS_NO_ERROR) {
    responsePrintError(argv[0], 0x08, "Could not read RSSI.");
    return;
  }
}
