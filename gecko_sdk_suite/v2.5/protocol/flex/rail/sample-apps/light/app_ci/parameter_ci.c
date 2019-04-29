/***************************************************************************//**
 * @file
 * @brief This file implements the parameter commands for RAIL test applications.
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
#include <stdio.h>

#include "command_interpreter.h"
#include "response_print.h"
#include "app_common.h"

#include "rail.h"
#include "rail_types.h"
#include "rail_config.h"

void changeChannel(uint32_t i)
{
  channel = i;
  // redrawDisplay = true;
  // Automatically apply the new channel to future Tx/Rx
  // if (receiveModeEnabled) {
  RAIL_StartRx(railHandle, channel, NULL);
  // }
}

void getChannel(int argc, char **argv)
{
  if (RAIL_GetDebugMode(railHandle) & RAIL_DEBUG_MODE_FREQ_OVERRIDE) {
    responsePrintError(argv[0], 0x12, "Channels are not valid in Debug Mode");
  } else {
    responsePrint(argv[0], "channel:%d", channel);
  }
}

void setChannel(int argc, char **argv)
{
  int proposedChannel = ciGetUnsigned(argv[1]);
  bool success = false;

  // Make sure this is a valid channel
  if (RAIL_IsValidChannel(railHandle, proposedChannel)
      == RAIL_STATUS_NO_ERROR) {
    changeChannel(proposedChannel);
    success = true;
  }

  if (!success) {
    responsePrintError(argv[0], 0x11, "Invalid channel '%d'", proposedChannel);
    return;
  }

  getChannel(1, argv);
}

void getPower(int argc, char **argv)
{
  responsePrint(argv[0],
                "powerLevel:%d,power:%d",
                RAIL_GetTxPower(railHandle),
                RAIL_GetTxPowerDbm(railHandle));
}

void setPower(int argc, char **argv)
{
  if (!inRadioState(RAIL_RF_STATE_IDLE, argv[0])) {
    return;
  }
  RAIL_TxPowerLevel_t powerLevel;
  RAIL_TxPower_t power;

  if (argc >= 3 && strcmp(argv[2], "raw") == 0) {
    RAIL_SetTxPower(railHandle, ciGetUnsigned(argv[1]));
  } else {
    RAIL_SetTxPowerDbm(railHandle, ciGetSigned(argv[1]));
  }

  powerLevel = RAIL_GetTxPower(railHandle);
  // lastSetTxPowerLevel = powerLevel;
  power = RAIL_GetTxPowerDbm(railHandle);

  responsePrint(argv[0], "powerLevel:%d,power:%d", powerLevel, power);
}

void getVersion(int argc, char **argv)
{
  RAIL_Version_t rail_ver;
  RAIL_GetVersion(&rail_ver, false);
  responsePrint(argv[0], "App:%d.%d.%d,RAIL:%d.%d.%d,Multiprotocol:%s",
                rail_ver.major, rail_ver.minor, rail_ver.rev,
                rail_ver.major, rail_ver.minor, rail_ver.rev,
                rail_ver.multiprotocol ? "True" : "False");
}
