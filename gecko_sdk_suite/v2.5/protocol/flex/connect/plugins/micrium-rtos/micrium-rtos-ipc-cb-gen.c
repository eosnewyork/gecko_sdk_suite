/***************************************************************************//**
 * @brief
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

// *** Generated file. Do not edit! ***
// vNCP Version: 1.0

#include PLATFORM_HEADER
#ifdef CONFIGURATION_HEADER
  #include CONFIGURATION_HEADER
#endif

#include "stack/include/ember.h"
#include "micrium-rtos-support.h"

#include "flex-bookkeeping.h"
#include "flex-callbacks.h"
#include "micrium-rtos-ipc-api-gen.h"
void emAfPluginMicriumStackStatusHandler(EmberStatus status)
{
  emAfPluginMicriumRtosSendCallbackCommand(EMBER_STACK_STATUS_HANDLER_IPC_COMMAND_ID,
                                           "u",
                                           status);
}
static void stackStatusCommandHandler(uint8_t *callbackParams)
{
  EmberStatus status;
  emAfPluginMicriumRtosFetchCallbackParams(callbackParams,
                                           "u",
                                           &status);

  emberAfStackStatusCallback(status);
  emberAfStackStatus(status);
}
void emAfPluginMicriumChildJoinHandler(EmberNodeType nodeType,
                                       EmberNodeId nodeId)
{
  emAfPluginMicriumRtosSendCallbackCommand(EMBER_CHILD_JOIN_HANDLER_IPC_COMMAND_ID,
                                           "uv",
                                           nodeType,
                                           nodeId);
}
static void childJoinCommandHandler(uint8_t *callbackParams)
{
  EmberNodeType nodeType;
  EmberNodeId nodeId;
  emAfPluginMicriumRtosFetchCallbackParams(callbackParams,
                                           "uv",
                                           &nodeType,
                                           &nodeId);

  emberAfChildJoinCallback(nodeType,
                           nodeId);
  emberAfChildJoin(nodeType,
                   nodeId);
}
void emAfPluginMicriumRadioNeedsCalibratingHandler(void)
{
  emAfPluginMicriumRtosSendCallbackCommand(EMBER_RADIO_NEEDS_CALIBRATING_HANDLER_IPC_COMMAND_ID,
                                           "");
}
static void radioNeedsCalibratingCommandHandler(uint8_t *callbackParams)
{
  emberAfRadioNeedsCalibratingCallback();
  emberAfRadioNeedsCalibrating();
}
void emAfPluginMicriumMessageSentHandler(EmberStatus status,
                                         EmberOutgoingMessage *message)
{
  emAfPluginMicriumRtosSendCallbackCommand(EMBER_MESSAGE_SENT_HANDLER_IPC_COMMAND_ID,
                                           "uuvuuub",
                                           status,
                                           message->options,
                                           message->destination,
                                           message->endpoint,
                                           message->tag,
                                           message->length,
                                           message->payload,
                                           message->length);
}
static void messageSentCommandHandler(uint8_t *callbackParams)
{
  EmberStatus status;
  EmberOutgoingMessage message;
  uint8_t payload[127];
  message.payload = payload;
  emAfPluginMicriumRtosFetchCallbackParams(callbackParams,
                                           "uuvuuub",
                                           &status,
                                           &message.options,
                                           &message.destination,
                                           &message.endpoint,
                                           &message.tag,
                                           &message.length,
                                           message.payload,
                                           &message.length);

  emberAfMessageSentCallback(status,
                             &message);
  emberAfMessageSent(status,
                     &message);
}
void emAfPluginMicriumIncomingMessageHandler(EmberIncomingMessage *message)
{
  emAfPluginMicriumRtosSendCallbackCommand(EMBER_INCOMING_MESSAGE_HANDLER_IPC_COMMAND_ID,
                                           "uvuuub",
                                           message->options,
                                           message->source,
                                           message->endpoint,
                                           message->rssi,
                                           message->length,
                                           message->payload,
                                           message->length);
}
static void incomingMessageCommandHandler(uint8_t *callbackParams)
{
  EmberIncomingMessage message;
  uint8_t payload[127];
  message.payload = payload;
  emAfPluginMicriumRtosFetchCallbackParams(callbackParams,
                                           "uvuuub",
                                           &message.options,
                                           &message.source,
                                           &message.endpoint,
                                           &message.rssi,
                                           &message.length,
                                           message.payload,
                                           &message.length);

  emberAfIncomingMessageCallback(&message);
  emberAfIncomingMessage(&message);
}
void emAfPluginMicriumIncomingMacMessageHandler(EmberIncomingMacMessage *message)
{
  emAfPluginMicriumRtosSendCallbackCommand(EMBER_INCOMING_MAC_MESSAGE_HANDLER_IPC_COMMAND_ID,
                                           "uvbuvbuvvuuuuwub",
                                           message->options,
                                           message->macFrame.srcAddress.addr.shortAddress,
                                           message->macFrame.srcAddress.addr.longAddress,
                                           EUI64_SIZE,
                                           message->macFrame.srcAddress.mode,
                                           message->macFrame.dstAddress.addr.shortAddress,
                                           message->macFrame.dstAddress.addr.longAddress,
                                           EUI64_SIZE,
                                           message->macFrame.dstAddress.mode,
                                           message->macFrame.srcPanId,
                                           message->macFrame.dstPanId,
                                           message->macFrame.srcPanIdSpecified,
                                           message->macFrame.dstPanIdSpecified,
                                           message->rssi,
                                           message->lqi,
                                           message->frameCounter,
                                           message->length,
                                           message->payload,
                                           message->length);
}
static void incomingMacMessageCommandHandler(uint8_t *callbackParams)
{
  EmberIncomingMacMessage message;
  uint8_t eui64Size = EUI64_SIZE;
  uint8_t payload[127];
  message.payload = payload;
  emAfPluginMicriumRtosFetchCallbackParams(callbackParams,
                                           "uvbuvbuvvuuuuwub",
                                           &message.options,
                                           &message.macFrame.srcAddress.addr.shortAddress,
                                           message.macFrame.srcAddress.addr.longAddress,
                                           &eui64Size,
                                           &message.macFrame.srcAddress.mode,
                                           &message.macFrame.dstAddress.addr.shortAddress,
                                           message.macFrame.dstAddress.addr.longAddress,
                                           &eui64Size,
                                           &message.macFrame.dstAddress.mode,
                                           &message.macFrame.srcPanId,
                                           &message.macFrame.dstPanId,
                                           &message.macFrame.srcPanIdSpecified,
                                           &message.macFrame.dstPanIdSpecified,
                                           &message.rssi,
                                           &message.lqi,
                                           &message.frameCounter,
                                           &message.length,
                                           message.payload,
                                           &message.length);

  emberAfIncomingMacMessageCallback(&message);
  emberAfIncomingMacMessage(&message);
}
void emAfPluginMicriumMacMessageSentHandler(EmberStatus status,
                                            EmberOutgoingMacMessage *message)
{
  emAfPluginMicriumRtosSendCallbackCommand(EMBER_MAC_MESSAGE_SENT_HANDLER_IPC_COMMAND_ID,
                                           "uuvbuvbuvvuuuwub",
                                           status,
                                           message->options,
                                           message->macFrame.srcAddress.addr.shortAddress,
                                           message->macFrame.srcAddress.addr.longAddress,
                                           EUI64_SIZE,
                                           message->macFrame.srcAddress.mode,
                                           message->macFrame.dstAddress.addr.shortAddress,
                                           message->macFrame.dstAddress.addr.longAddress,
                                           EUI64_SIZE,
                                           message->macFrame.dstAddress.mode,
                                           message->macFrame.srcPanId,
                                           message->macFrame.dstPanId,
                                           message->macFrame.srcPanIdSpecified,
                                           message->macFrame.dstPanIdSpecified,
                                           message->tag,
                                           message->frameCounter,
                                           message->length,
                                           message->payload,
                                           message->length);
}
static void macMessageSentCommandHandler(uint8_t *callbackParams)
{
  EmberStatus status;
  EmberOutgoingMacMessage message;
  uint8_t eui64Size = EUI64_SIZE;
  uint8_t payload[127];
  message.payload = payload;
  emAfPluginMicriumRtosFetchCallbackParams(callbackParams,
                                           "uuvbuvbuvvuuuwub",
                                           &status,
                                           &message.options,
                                           &message.macFrame.srcAddress.addr.shortAddress,
                                           message.macFrame.srcAddress.addr.longAddress,
                                           &eui64Size,
                                           &message.macFrame.srcAddress.mode,
                                           &message.macFrame.dstAddress.addr.shortAddress,
                                           message.macFrame.dstAddress.addr.longAddress,
                                           &eui64Size,
                                           &message.macFrame.dstAddress.mode,
                                           &message.macFrame.srcPanId,
                                           &message.macFrame.dstPanId,
                                           &message.macFrame.srcPanIdSpecified,
                                           &message.macFrame.dstPanIdSpecified,
                                           &message.tag,
                                           &message.frameCounter,
                                           &message.length,
                                           message.payload,
                                           &message.length);

  emberAfMacMessageSentCallback(status,
                                &message);
  emberAfMacMessageSent(status,
                        &message);
}
void emAfPluginMicriumIncomingBeaconHandler(EmberPanId panId,
                                            EmberNodeId nodeId,
                                            int8_t rssi,
                                            bool permitJoining,
                                            uint8_t payloadLength,
                                            uint8_t *payload)
{
  emAfPluginMicriumRtosSendCallbackCommand(EMBER_INCOMING_BEACON_HANDLER_IPC_COMMAND_ID,
                                           "vvuuub",
                                           panId,
                                           nodeId,
                                           rssi,
                                           permitJoining,
                                           payloadLength,
                                           payload,
                                           payloadLength);
}
static void incomingBeaconCommandHandler(uint8_t *callbackParams)
{
  EmberPanId panId;
  EmberNodeId nodeId;
  int8_t rssi;
  bool permitJoining;
  uint8_t payloadLength;
  static uint8_t payload[EMBER_MAC_MAX_APP_BEACON_PAYLOAD_LENGTH];
  emAfPluginMicriumRtosFetchCallbackParams(callbackParams,
                                           "vvuuub",
                                           &panId,
                                           &nodeId,
                                           &rssi,
                                           &permitJoining,
                                           &payloadLength,
                                           payload,
                                           &payloadLength);

  emberAfIncomingBeaconCallback(panId,
                                nodeId,
                                rssi,
                                permitJoining,
                                payloadLength,
                                payload);
  emberAfIncomingBeacon(panId,
                        nodeId,
                        rssi,
                        permitJoining,
                        payloadLength,
                        payload);
}
void emAfPluginMicriumIncomingBeaconExtendedHandler(EmberPanId panId,
                                                    EmberMacAddress *source,
                                                    int8_t rssi,
                                                    bool permitJoining,
                                                    uint8_t beaconFieldsLength,
                                                    uint8_t *beaconFields,
                                                    uint8_t beaconPayloadLength,
                                                    uint8_t *beaconPayload)
{
  emAfPluginMicriumRtosSendCallbackCommand(EMBER_INCOMING_BEACON_EXTENDED_HANDLER_IPC_COMMAND_ID,
                                           "vvbuuuubub",
                                           panId,
                                           source->addr.shortAddress,
                                           source->addr.longAddress,
                                           EUI64_SIZE,
                                           source->mode,
                                           rssi,
                                           permitJoining,
                                           beaconFieldsLength,
                                           beaconFields,
                                           beaconFieldsLength,
                                           beaconPayloadLength,
                                           beaconPayload,
                                           beaconPayloadLength);
}
static void incomingBeaconExtendedCommandHandler(uint8_t *callbackParams)
{
  EmberPanId panId;
  EmberMacAddress source;
  int8_t rssi;
  bool permitJoining;
  uint8_t beaconFieldsLength;
  uint8_t beaconPayloadLength;
  uint8_t eui64Size = EUI64_SIZE;
  static uint8_t beaconFields[EMBER_MAC_MAX_BEACON_FIELDS_LENGTH];
  static uint8_t beaconPayload[EMBER_MAC_MAX_APP_BEACON_PAYLOAD_LENGTH];
  emAfPluginMicriumRtosFetchCallbackParams(callbackParams,
                                           "vvbuuuubub",
                                           &panId,
                                           &source.addr.shortAddress,
                                           source.addr.longAddress,
                                           &eui64Size,
                                           &source.mode,
                                           &rssi,
                                           &permitJoining,
                                           &beaconFieldsLength,
                                           beaconFields,
                                           &beaconFieldsLength,
                                           &beaconPayloadLength,
                                           beaconPayload,
                                           &beaconPayloadLength);

  emberAfIncomingBeaconExtendedCallback(panId,
                                        &source,
                                        rssi,
                                        permitJoining,
                                        beaconFieldsLength,
                                        beaconFields,
                                        beaconPayloadLength,
                                        beaconPayload);
  emberAfIncomingBeaconExtended(panId,
                                &source,
                                rssi,
                                permitJoining,
                                beaconFieldsLength,
                                beaconFields,
                                beaconPayloadLength,
                                beaconPayload);
}
void emAfPluginMicriumActiveScanCompleteHandler(void)
{
  emAfPluginMicriumRtosSendCallbackCommand(EMBER_ACTIVE_SCAN_COMPLETE_HANDLER_IPC_COMMAND_ID,
                                           "");
}
static void activeScanCompleteCommandHandler(uint8_t *callbackParams)
{
  emberAfActiveScanCompleteCallback();
  emberAfActiveScanComplete();
}
void emAfPluginMicriumEnergyScanCompleteHandler(int8_t mean,
                                                int8_t min,
                                                int8_t max,
                                                uint16_t variance)
{
  emAfPluginMicriumRtosSendCallbackCommand(EMBER_ENERGY_SCAN_COMPLETE_HANDLER_IPC_COMMAND_ID,
                                           "uuuv",
                                           mean,
                                           min,
                                           max,
                                           variance);
}
static void energyScanCompleteCommandHandler(uint8_t *callbackParams)
{
  int8_t mean;
  int8_t min;
  int8_t max;
  uint16_t variance;
  emAfPluginMicriumRtosFetchCallbackParams(callbackParams,
                                           "uuuv",
                                           &mean,
                                           &min,
                                           &max,
                                           &variance);

  emberAfEnergyScanCompleteCallback(mean,
                                    min,
                                    max,
                                    variance);
  emberAfEnergyScanComplete(mean,
                            min,
                            max,
                            variance);
}
void emAfPluginMicriumFrequencyHoppingStartClientCompleteHandler(EmberStatus status)
{
  emAfPluginMicriumRtosSendCallbackCommand(EMBER_FREQUENCY_HOPPING_START_CLIENT_COMPLETE_HANDLER_IPC_COMMAND_ID,
                                           "u",
                                           status);
}
static void frequencyHoppingStartClientCompleteCommandHandler(uint8_t *callbackParams)
{
  EmberStatus status;
  emAfPluginMicriumRtosFetchCallbackParams(callbackParams,
                                           "u",
                                           &status);

  emberAfFrequencyHoppingStartClientCompleteCallback(status);
  emberAfFrequencyHoppingStartClientComplete(status);
}

//------------------------------------------------------------------------------
// API command dispatcher (vNCP side)

void emAfPluginMicriumRtosHandleIncomingCallbackCommand(uint16_t commandId,
                                                        uint8_t *callbackParams)
{
  assert(!emAfPluginMicriumRtosIsCurrentTaskStackTask());

  switch (commandId) {
    case EMBER_STACK_STATUS_HANDLER_IPC_COMMAND_ID:
      stackStatusCommandHandler(callbackParams);
      break;
    case EMBER_CHILD_JOIN_HANDLER_IPC_COMMAND_ID:
      childJoinCommandHandler(callbackParams);
      break;
    case EMBER_RADIO_NEEDS_CALIBRATING_HANDLER_IPC_COMMAND_ID:
      radioNeedsCalibratingCommandHandler(callbackParams);
      break;
    case EMBER_MESSAGE_SENT_HANDLER_IPC_COMMAND_ID:
      messageSentCommandHandler(callbackParams);
      break;
    case EMBER_INCOMING_MESSAGE_HANDLER_IPC_COMMAND_ID:
      incomingMessageCommandHandler(callbackParams);
      break;
    case EMBER_INCOMING_MAC_MESSAGE_HANDLER_IPC_COMMAND_ID:
      incomingMacMessageCommandHandler(callbackParams);
      break;
    case EMBER_MAC_MESSAGE_SENT_HANDLER_IPC_COMMAND_ID:
      macMessageSentCommandHandler(callbackParams);
      break;
    case EMBER_INCOMING_BEACON_HANDLER_IPC_COMMAND_ID:
      incomingBeaconCommandHandler(callbackParams);
      break;
    case EMBER_INCOMING_BEACON_EXTENDED_HANDLER_IPC_COMMAND_ID:
      incomingBeaconExtendedCommandHandler(callbackParams);
      break;
    case EMBER_ACTIVE_SCAN_COMPLETE_HANDLER_IPC_COMMAND_ID:
      activeScanCompleteCommandHandler(callbackParams);
      break;
    case EMBER_ENERGY_SCAN_COMPLETE_HANDLER_IPC_COMMAND_ID:
      energyScanCompleteCommandHandler(callbackParams);
      break;
    case EMBER_FREQUENCY_HOPPING_START_CLIENT_COMPLETE_HANDLER_IPC_COMMAND_ID:
      frequencyHoppingStartClientCompleteCommandHandler(callbackParams);
      break;
    default: {
      assert(0);
    }
  }
}
