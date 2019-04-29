// *******************************************************************
// * stack-handlers.c
// *
// *
// * Copyright 2018 Silicon Laboratories, Inc.                              *80*
// *******************************************************************
//
// *** Generated file. Do not edit! ***
//
// vNCP Version: 1.0

#include PLATFORM_HEADER
#ifdef CONFIGURATION_HEADER
  #include CONFIGURATION_HEADER
#endif
#include "stack/include/ember.h"
#include "flex-bookkeeping.h"
#include "flex-callbacks.h"

#if defined(EMBER_AF_API_MICRIUM_RTOS)
#include EMBER_AF_API_MICRIUM_RTOS
#endif

void emberStackStatusHandler(EmberStatus status)
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  emAfPluginMicriumStackStatusHandler(status);
#else
  emberAfStackStatusCallback(status);
  emberAfStackStatus(status);
#endif
}

void emberChildJoinHandler(EmberNodeType nodeType,
                           EmberNodeId nodeId)
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  emAfPluginMicriumChildJoinHandler(nodeType, nodeId);
#else
  emberAfChildJoinCallback(nodeType, nodeId);
  emberAfChildJoin(nodeType, nodeId);
#endif
}

void emberRadioNeedsCalibratingHandler()
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  emAfPluginMicriumRadioNeedsCalibratingHandler();
#else
  emberAfRadioNeedsCalibratingCallback();
  emberAfRadioNeedsCalibrating();
#endif
}

void emberStackIsrHandler()
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  emAfPluginMicriumStackIsrHandler();
#else
  // We do not expose this to the application.
  emberAfStackIsr();
#endif
}

bool emberStackIdleHandler(uint32_t *idleTimeMs)
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  return emAfPluginMicriumStackIdleHandler(idleTimeMs);
#else
  return emberAfStackIdleCallback(idleTimeMs);
  // We don't use the bookkeeping mechanism here, we want to enforce a single
  // entity handling idling.
#endif
}

void emberMessageSentHandler(EmberStatus status,
                             EmberOutgoingMessage *message)
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  emAfPluginMicriumMessageSentHandler(status, message);
#else
  emberAfMessageSentCallback(status, message);
  emberAfMessageSent(status, message);
#endif
}

void emberIncomingMessageHandler(EmberIncomingMessage *message)
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  emAfPluginMicriumIncomingMessageHandler(message);
#else
  emberAfIncomingMessageCallback(message);
  emberAfIncomingMessage(message);
#endif
}

void emberIncomingMacMessageHandler(EmberIncomingMacMessage *message)
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  emAfPluginMicriumIncomingMacMessageHandler(message);
#else
  emberAfIncomingMacMessageCallback(message);
  emberAfIncomingMacMessage(message);
#endif
}

void emberMacMessageSentHandler(EmberStatus status,
                                EmberOutgoingMacMessage *message)
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  emAfPluginMicriumMacMessageSentHandler(status, message);
#else
  emberAfMacMessageSentCallback(status, message);
  emberAfMacMessageSent(status, message);
#endif
}

void emberIncomingBeaconHandler(EmberPanId panId,
                                EmberNodeId nodeId,
                                int8_t rssi,
                                bool permitJoining,
                                uint8_t payloadLength,
                                uint8_t *payload)
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  emAfPluginMicriumIncomingBeaconHandler(panId, nodeId, rssi, permitJoining, payloadLength, payload);
#else
  emberAfIncomingBeaconCallback(panId, nodeId, rssi, permitJoining, payloadLength, payload);
  emberAfIncomingBeacon(panId, nodeId, rssi, permitJoining, payloadLength, payload);
#endif
}

void emberIncomingBeaconExtendedHandler(EmberPanId panId,
                                        EmberMacAddress *source,
                                        int8_t rssi,
                                        bool permitJoining,
                                        uint8_t beaconFieldsLength,
                                        uint8_t *beaconFields,
                                        uint8_t beaconPayloadLength,
                                        uint8_t *beaconPayload)
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  emAfPluginMicriumIncomingBeaconExtendedHandler(panId, source, rssi, permitJoining, beaconFieldsLength, beaconFields, beaconPayloadLength, beaconPayload);
#else
  emberAfIncomingBeaconExtendedCallback(panId, source, rssi, permitJoining, beaconFieldsLength, beaconFields, beaconPayloadLength, beaconPayload);
  emberAfIncomingBeaconExtended(panId, source, rssi, permitJoining, beaconFieldsLength, beaconFields, beaconPayloadLength, beaconPayload);
#endif
}

void emberActiveScanCompleteHandler()
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  emAfPluginMicriumActiveScanCompleteHandler();
#else
  emberAfActiveScanCompleteCallback();
  emberAfActiveScanComplete();
#endif
}

void emberEnergyScanCompleteHandler(int8_t mean,
                                    int8_t min,
                                    int8_t max,
                                    uint16_t variance)
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  emAfPluginMicriumEnergyScanCompleteHandler(mean, min, max, variance);
#else
  emberAfEnergyScanCompleteCallback(mean, min, max, variance);
  emberAfEnergyScanComplete(mean, min, max, variance);
#endif
}

void emberFrequencyHoppingStartClientCompleteHandler(EmberStatus status)
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  emAfPluginMicriumFrequencyHoppingStartClientCompleteHandler(status);
#else
  emberAfFrequencyHoppingStartClientCompleteCallback(status);
  emberAfFrequencyHoppingStartClientComplete(status);
#endif
}
