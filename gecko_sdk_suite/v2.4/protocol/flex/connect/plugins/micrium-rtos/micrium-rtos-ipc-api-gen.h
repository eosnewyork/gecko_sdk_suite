/**************************************************************************//**
 * Copyright 2018 Silicon Laboratories, Inc.
 *
 *****************************************************************************/
//
// *** Generated file. Do not edit! ***
//
// Description: Enumerations for vNCP.

#ifndef __MICRIUM_RTOS_IPC_API_GEN_H__
#define __MICRIUM_RTOS_IPC_API_GEN_H__

#define VNCP_VERSION 0x0100
//------------------------------------------------------------------------------
// Frame IDs
enum {
// Core
  EMBER_NETWORK_STATE_IPC_COMMAND_ID                                   = 0x6902,
  EMBER_STACK_IS_UP_IPC_COMMAND_ID                                     = 0x6903,
  EMBER_SET_SECURITY_KEY_IPC_COMMAND_ID                                = 0x6904,
  EMBER_GET_COUNTER_IPC_COMMAND_ID                                     = 0x6905,
  EMBER_SET_RADIO_CHANNEL_IPC_COMMAND_ID                               = 0x6906,
  EMBER_GET_RADIO_CHANNEL_IPC_COMMAND_ID                               = 0x6907,
  EMBER_SET_RADIO_POWER_IPC_COMMAND_ID                                 = 0x6908,
  EMBER_GET_RADIO_POWER_IPC_COMMAND_ID                                 = 0x6909,
  EMBER_SET_RADIO_POWER_MODE_IPC_COMMAND_ID                            = 0x690A,
  EMBER_SET_MAC_PARAMS_IPC_COMMAND_ID                                  = 0x690B,
  EMBER_CURRENT_STACK_TASKS_IPC_COMMAND_ID                             = 0x690C,
  EMBER_OK_TO_NAP_IPC_COMMAND_ID                                       = 0x690D,
  EMBER_OK_TO_HIBERNATE_IPC_COMMAND_ID                                 = 0x690E,
  EMBER_GET_EUI64_IPC_COMMAND_ID                                       = 0x690F,
  EMBER_IS_LOCAL_EUI64_IPC_COMMAND_ID                                  = 0x6910,
  EMBER_GET_NODE_ID_IPC_COMMAND_ID                                     = 0x6911,
  EMBER_GET_PAN_ID_IPC_COMMAND_ID                                      = 0x6912,
  EMBER_GET_PARENT_ID_IPC_COMMAND_ID                                   = 0x6913,
  EMBER_GET_NODE_TYPE_IPC_COMMAND_ID                                   = 0x6914,
  EMBER_GET_CSP_VERSION_IPC_COMMAND_ID                                 = 0x6915,
  EMBER_CALIBRATE_CURRENT_CHANNEL_IPC_COMMAND_ID                       = 0x692F,
  EMBER_GET_MAXIMUM_PAYLOAD_LENGTH_IPC_COMMAND_ID                      = 0x6930,
  EMBER_STACK_STATUS_HANDLER_IPC_COMMAND_ID                            = 0x6300,
  EMBER_CHILD_JOIN_HANDLER_IPC_COMMAND_ID                              = 0x6301,
  EMBER_RADIO_NEEDS_CALIBRATING_HANDLER_IPC_COMMAND_ID                 = 0x6330,
// Messaging
  EMBER_MESSAGE_SEND_IPC_COMMAND_ID                                    = 0x6916,
  EMBER_POLL_FOR_DATA_IPC_COMMAND_ID                                   = 0x6917,
  EMBER_MAC_MESSAGE_SEND_IPC_COMMAND_ID                                = 0x692B,
  EMBER_MAC_SET_ALLOCATE_ADDRESS_FLAG_IPC_COMMAND_ID                   = 0x692C,
  EMBER_MAC_SET_PAN_COORDINATOR_IPC_COMMAND_ID                         = 0x6938,
  EMBER_SET_POLL_DESTINATION_ADDRESS_IPC_COMMAND_ID                    = 0x692D,
  EMBER_REMOVE_CHILD_IPC_COMMAND_ID                                    = 0x6931,
  EMBER_GET_CHILD_FLAGS_IPC_COMMAND_ID                                 = 0x6932,
  EMBER_PURGE_INDIRECT_MESSAGES_IPC_COMMAND_ID                         = 0x692E,
  EMBER_MESSAGE_SENT_HANDLER_IPC_COMMAND_ID                            = 0x6302,
  EMBER_INCOMING_MESSAGE_HANDLER_IPC_COMMAND_ID                        = 0x6303,
  EMBER_INCOMING_MAC_MESSAGE_HANDLER_IPC_COMMAND_ID                    = 0x630B,
  EMBER_MAC_MESSAGE_SENT_HANDLER_IPC_COMMAND_ID                        = 0x630C,
// Network Management
  EMBER_NETWORK_INIT_IPC_COMMAND_ID                                    = 0x6918,
  EMBER_START_ACTIVE_SCAN_IPC_COMMAND_ID                               = 0x6919,
  EMBER_START_ENERGY_SCAN_IPC_COMMAND_ID                               = 0x691A,
  EMBER_SET_APPLICATION_BEACON_PAYLOAD_IPC_COMMAND_ID                  = 0x691B,
  EMBER_FORM_NETWORK_IPC_COMMAND_ID                                    = 0x691C,
  EMBER_JOIN_NETWORK_EXTENDED_IPC_COMMAND_ID                           = 0x691D,
  EMBER_JOIN_NETWORK_IPC_COMMAND_ID                                    = 0x691E,
  EMBER_PERMIT_JOINING_IPC_COMMAND_ID                                  = 0x691F,
  EMBER_JOIN_COMMISSIONED_IPC_COMMAND_ID                               = 0x6920,
  EMBER_RESET_NETWORK_STATE_IPC_COMMAND_ID                             = 0x6921,
  EMBER_GET_STANDALONE_BOOTLOADER_INFO_IPC_COMMAND_ID                  = 0x6922,
  EMBER_LAUNCH_STANDALONE_BOOTLOADER_IPC_COMMAND_ID                    = 0x6923,
  EMBER_FREQUENCY_HOPPING_START_SERVER_IPC_COMMAND_ID                  = 0x6926,
  EMBER_FREQUENCY_HOPPING_START_CLIENT_IPC_COMMAND_ID                  = 0x6927,
  EMBER_FREQUENCY_HOPPING_STOP_IPC_COMMAND_ID                          = 0x6928,
  EMBER_SET_AUXILIARY_ADDRESS_FILTERING_ENTRY_IPC_COMMAND_ID           = 0x6929,
  EMBER_GET_AUXILIARY_ADDRESS_FILTERING_ENTRY_IPC_COMMAND_ID           = 0x692A,
  EMBER_INCOMING_BEACON_HANDLER_IPC_COMMAND_ID                         = 0x6304,
  EMBER_INCOMING_BEACON_EXTENDED_HANDLER_IPC_COMMAND_ID                = 0x6339,
  EMBER_ACTIVE_SCAN_COMPLETE_HANDLER_IPC_COMMAND_ID                    = 0x6305,
  EMBER_ENERGY_SCAN_COMPLETE_HANDLER_IPC_COMMAND_ID                    = 0x6306,
  EMBER_GET_STANDALONE_BOOTLOADER_INFO_RETURN_IPC_COMMAND_ID           = 0x6307,
  EMBER_LAUNCH_STANDALONE_BOOTLOADER_RETURN_IPC_COMMAND_ID             = 0x6308,
  EMBER_FREQUENCY_HOPPING_START_CLIENT_COMPLETE_HANDLER_IPC_COMMAND_ID = 0x630A,
// EMBER_TEST
  EMBER_ECHO_IPC_COMMAND_ID                                            = 0x6924,
  EMBER_ECHO_RETURN_IPC_COMMAND_ID                                     = 0x6309,
// APP_USES_SOFTWARE_FLOW_CONTROL
  EMBER_START_XON_XOFF_TEST_IPC_COMMAND_ID                             = 0x6925,
};

#endif // __MICRIUM_RTOS_IPC_API_GEN_H__
