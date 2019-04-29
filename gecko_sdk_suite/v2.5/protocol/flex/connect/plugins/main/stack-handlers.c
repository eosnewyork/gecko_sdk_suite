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
  emberAfStackStatus(status);
  emberAfStackStatusCallback(status);
#endif
}

void emberChildJoinHandler(EmberNodeType nodeType,
                           EmberNodeId nodeId)
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  emAfPluginMicriumChildJoinHandler(nodeType, nodeId);
#else
  emberAfChildJoin(nodeType, nodeId);
  emberAfChildJoinCallback(nodeType, nodeId);
#endif
}

void emberRadioNeedsCalibratingHandler()
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  emAfPluginMicriumRadioNeedsCalibratingHandler();
#else
  emberAfRadioNeedsCalibrating();
  emberAfRadioNeedsCalibratingCallback();
#endif
}

void emberStackIsrHandler()
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  emAfPluginMicriumStackIsrHandler();
#else
  emberAfStackIsr();
  // We do not expose this to the application.

#endif
}

bool emberStackIdleHandler(uint32_t *idleTimeMs)
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  return emAfPluginMicriumStackIdleHandler(idleTimeMs);
#else
  // We don't use the bookkeeping mechanism here, we want to enforce a single
  // entity handling idling.
  return emberAfStackIdleCallback(idleTimeMs);
#endif
}

void emberMessageSentHandler(EmberStatus status,
                             EmberOutgoingMessage *message)
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  emAfPluginMicriumMessageSentHandler(status, message);
#else
  emberAfMessageSent(status, message);
  emberAfMessageSentCallback(status, message);
#endif
}

void emberIncomingMessageHandler(EmberIncomingMessage *message)
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  emAfPluginMicriumIncomingMessageHandler(message);
#else
  emberAfIncomingMessage(message);
  emberAfIncomingMessageCallback(message);
#endif
}

void emberIncomingMacMessageHandler(EmberIncomingMacMessage *message)
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  emAfPluginMicriumIncomingMacMessageHandler(message);
#else
  emberAfIncomingMacMessage(message);
  emberAfIncomingMacMessageCallback(message);
#endif
}

void emberMacMessageSentHandler(EmberStatus status,
                                EmberOutgoingMacMessage *message)
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  emAfPluginMicriumMacMessageSentHandler(status, message);
#else
  emberAfMacMessageSent(status, message);
  emberAfMacMessageSentCallback(status, message);
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
  emberAfIncomingBeacon(panId, nodeId, rssi, permitJoining, payloadLength, payload);
  emberAfIncomingBeaconCallback(panId, nodeId, rssi, permitJoining, payloadLength, payload);
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
  emberAfIncomingBeaconExtended(panId, source, rssi, permitJoining, beaconFieldsLength, beaconFields, beaconPayloadLength, beaconPayload);
  emberAfIncomingBeaconExtendedCallback(panId, source, rssi, permitJoining, beaconFieldsLength, beaconFields, beaconPayloadLength, beaconPayload);
#endif
}

void emberActiveScanCompleteHandler()
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  emAfPluginMicriumActiveScanCompleteHandler();
#else
  emberAfActiveScanComplete();
  emberAfActiveScanCompleteCallback();
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
  emberAfEnergyScanComplete(mean, min, max, variance);
  emberAfEnergyScanCompleteCallback(mean, min, max, variance);
#endif
}

void emberFrequencyHoppingStartClientCompleteHandler(EmberStatus status)
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS)
  emAfPluginMicriumFrequencyHoppingStartClientCompleteHandler(status);
#else
  emberAfFrequencyHoppingStartClientComplete(status);
  emberAfFrequencyHoppingStartClientCompleteCallback(status);
#endif
}
