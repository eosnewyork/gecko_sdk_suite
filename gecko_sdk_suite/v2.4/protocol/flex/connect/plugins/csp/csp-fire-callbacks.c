// *******************************************************************
// * csp-fire-callbacks.c
// *
// *
// * Copyright 2018 Silicon Laboratories, Inc.                              *80*
// *******************************************************************
//
// *** Generated file. Do not edit! ***
//
// CSP Version: 1.0
#include "csp-fire-callbacks.h"
#include "csp-callbacks.h"
#include "csp-enum.h"

void processGeneratedCallbacks(uint16_t callbackIdentifier)
{
  switch (callbackIdentifier) {
    case CB_STACK_STATUS_COMMAND_IDENTIFIER:
      fireStackStatusHandlerCallback();
      break;

    case CB_CHILD_JOIN_COMMAND_IDENTIFIER:
      fireChildJoinHandlerCallback();
      break;

    case CB_RADIO_NEEDS_CALIBRATING_COMMAND_IDENTIFIER:
      fireRadioNeedsCalibratingHandlerCallback();
      break;

    case CB_MESSAGE_SENT_COMMAND_IDENTIFIER:
      fireMessageSentHandlerCallback();
      break;

    case CB_INCOMING_MESSAGE_COMMAND_IDENTIFIER:
      fireIncomingMessageHandlerCallback();
      break;

    case CB_INCOMING_MAC_MESSAGE_COMMAND_IDENTIFIER:
      fireIncomingMacMessageHandlerCallback();
      break;

    case CB_MAC_MESSAGE_SENT_COMMAND_IDENTIFIER:
      fireMacMessageSentHandlerCallback();
      break;

    case CB_INCOMING_BEACON_COMMAND_IDENTIFIER:
      fireIncomingBeaconHandlerCallback();
      break;

    case CB_INCOMING_BEACON_EXTENDED_COMMAND_IDENTIFIER:
      fireIncomingBeaconExtendedHandlerCallback();
      break;

    case CB_ACTIVE_SCAN_COMPLETE_COMMAND_IDENTIFIER:
      fireActiveScanCompleteHandlerCallback();
      break;

    case CB_ENERGY_SCAN_COMPLETE_COMMAND_IDENTIFIER:
      fireEnergyScanCompleteHandlerCallback();
      break;

    case CB_GET_STANDALONE_BOOTLOADER_INFO_COMMAND_IDENTIFIER:
      fireGetStandaloneBootloaderInfoReturnCallback();
      break;

    case CB_LAUNCH_STANDALONE_BOOTLOADER_COMMAND_IDENTIFIER:
      fireLaunchStandaloneBootloaderReturnCallback();
      break;

    case CB_FREQUENCY_HOPPING_START_CLIENT_COMPLETE_COMMAND_IDENTIFIER:
      fireFrequencyHoppingStartClientCompleteHandlerCallback();
      break;

    case CB_ECHO_COMMAND_IDENTIFIER:
      fireEchoReturnCallback();
      break;

    default: {
      break;
    }
  }
}
