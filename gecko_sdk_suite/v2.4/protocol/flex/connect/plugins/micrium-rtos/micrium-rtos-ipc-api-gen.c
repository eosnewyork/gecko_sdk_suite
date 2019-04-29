// *******************************************************************
// * micrium-rtos-ipc-api-gen.c
// *
// *
// * Copyright 2018 Silicon Laboratories, Inc.                              *80*
// *******************************************************************
//
// *** Generated file. Do not edit! ***
//
// Description: Host functions for sending Connect management commands
// to the NCP.
//
// vNCP Version: 1.0

#include PLATFORM_HEADER
#ifdef CONFIGURATION_HEADER
  #include CONFIGURATION_HEADER
#endif

#include "stack/include/api-rename.h"
#include "stack/include/ember.h"
#include "stack/include/api-rename-undef.h"

#include <kernel/include/os.h>
#include "micrium-rtos-support.h"
#include "micrium-rtos-ipc-api-gen.h"

// networkState
EmberNetworkStatus emberNetworkState(void)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_NETWORK_STATE_IPC_COMMAND_ID,
                                           "");

  EmberNetworkStatus networkStatus;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &networkStatus);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return networkStatus;
}
static void networkStateCommandHandler(void)
{
  EmberNetworkStatus networkStatus = emApiNetworkState();
  emAfPluginMicriumRtosSendResponse(EMBER_NETWORK_STATE_IPC_COMMAND_ID,
                                    "u",
                                    networkStatus);
}

// stackIsUp
bool emberStackIsUp(void)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_STACK_IS_UP_IPC_COMMAND_ID,
                                           "");

  bool stackIsUp;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &stackIsUp);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return stackIsUp;
}
static void stackIsUpCommandHandler(void)
{
  bool stackIsUp = emApiStackIsUp();
  emAfPluginMicriumRtosSendResponse(EMBER_STACK_IS_UP_IPC_COMMAND_ID,
                                    "u",
                                    stackIsUp);
}

// setSecurityKey
EmberStatus emberSetSecurityKey(EmberKeyData *key)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_SET_SECURITY_KEY_IPC_COMMAND_ID,
                                           "b",
                                           key, sizeof(EmberKeyData));

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void setSecurityKeyCommandHandler(void)
{
  EmberKeyData key;
  uint8_t length;
  emAfPluginMicriumRtosFetchApiParams("b",
                                      &key,
                                      &length);
  EmberStatus status = emApiSetSecurityKey(&key);
  emAfPluginMicriumRtosSendResponse(EMBER_SET_SECURITY_KEY_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// getCounter
EmberStatus emberGetCounter(EmberCounterType counterType,
                            uint32_t* count)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_GET_COUNTER_IPC_COMMAND_ID,
                                           "u",
                                           counterType);

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("uw",
                                      &status,
                                      count);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void getCounterCommandHandler(void)
{
  EmberCounterType counterType;
  uint32_t count;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &counterType);
  EmberStatus status = emApiGetCounter(counterType,
                                       &count);
  emAfPluginMicriumRtosSendResponse(EMBER_GET_COUNTER_IPC_COMMAND_ID,
                                    "uw",
                                    status,
                                    count);
}

// setRadioChannel
EmberStatus emberSetRadioChannel(uint8_t channel)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_SET_RADIO_CHANNEL_IPC_COMMAND_ID,
                                           "u",
                                           channel);

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void setRadioChannelCommandHandler(void)
{
  uint8_t channel;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &channel);
  EmberStatus status = emApiSetRadioChannel(channel);
  emAfPluginMicriumRtosSendResponse(EMBER_SET_RADIO_CHANNEL_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// getRadioChannel
uint8_t emberGetRadioChannel(void)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_GET_RADIO_CHANNEL_IPC_COMMAND_ID,
                                           "");

  uint8_t channel;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &channel);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return channel;
}
static void getRadioChannelCommandHandler(void)
{
  uint8_t channel = emApiGetRadioChannel();
  emAfPluginMicriumRtosSendResponse(EMBER_GET_RADIO_CHANNEL_IPC_COMMAND_ID,
                                    "u",
                                    channel);
}

// setRadioPower
EmberStatus emberSetRadioPower(int8_t power)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_SET_RADIO_POWER_IPC_COMMAND_ID,
                                           "s",
                                           power);

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void setRadioPowerCommandHandler(void)
{
  int8_t power;
  emAfPluginMicriumRtosFetchApiParams("s",
                                      &power);
  EmberStatus status = emApiSetRadioPower(power);
  emAfPluginMicriumRtosSendResponse(EMBER_SET_RADIO_POWER_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// getRadioPower
int8_t emberGetRadioPower(void)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_GET_RADIO_POWER_IPC_COMMAND_ID,
                                           "");

  int8_t power;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &power);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return power;
}
static void getRadioPowerCommandHandler(void)
{
  int8_t power = emApiGetRadioPower();
  emAfPluginMicriumRtosSendResponse(EMBER_GET_RADIO_POWER_IPC_COMMAND_ID,
                                    "u",
                                    power);
}

// setRadioPowerMode
EmberStatus emberSetRadioPowerMode(bool radioOn)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_SET_RADIO_POWER_MODE_IPC_COMMAND_ID,
                                           "u",
                                           radioOn);

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void setRadioPowerModeCommandHandler(void)
{
  bool radioOn;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &radioOn);
  EmberStatus status = emApiSetRadioPowerMode(radioOn);
  emAfPluginMicriumRtosSendResponse(EMBER_SET_RADIO_POWER_MODE_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// setMacParams
EmberStatus emberSetMacParams(bool checkCca,
                              uint8_t maxCcaAttempts,
                              uint8_t minBackoffExp,
                              uint8_t maxBackoffExp,
                              uint8_t maxRetries)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_SET_MAC_PARAMS_IPC_COMMAND_ID,
                                           "uuuuu",
                                           checkCca,
                                           maxCcaAttempts,
                                           minBackoffExp,
                                           maxBackoffExp,
                                           maxRetries);

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void setMacParamsCommandHandler(void)
{
  bool checkCca;
  uint8_t maxCcaAttempts;
  uint8_t minBackoffExp;
  uint8_t maxBackoffExp;
  uint8_t maxRetries;
  emAfPluginMicriumRtosFetchApiParams("uuuuu",
                                      &checkCca,
                                      &maxCcaAttempts,
                                      &minBackoffExp,
                                      &maxBackoffExp,
                                      &maxRetries);
  EmberStatus status = emApiSetMacParams(checkCca,
                                         maxCcaAttempts,
                                         minBackoffExp,
                                         maxBackoffExp,
                                         maxRetries);
  emAfPluginMicriumRtosSendResponse(EMBER_SET_MAC_PARAMS_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// currentStackTasks
uint16_t emberCurrentStackTasks(void)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_CURRENT_STACK_TASKS_IPC_COMMAND_ID,
                                           "");

  uint16_t currentTasks;
  emAfPluginMicriumRtosFetchApiParams("v",
                                      &currentTasks);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return currentTasks;
}
static void currentStackTasksCommandHandler(void)
{
  uint16_t currentTasks = emApiCurrentStackTasks();
  emAfPluginMicriumRtosSendResponse(EMBER_CURRENT_STACK_TASKS_IPC_COMMAND_ID,
                                    "v",
                                    currentTasks);
}

// calibrateCurrentChannel
void emberCalibrateCurrentChannel(void)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_CALIBRATE_CURRENT_CHANNEL_IPC_COMMAND_ID,
                                           "");
  emAfPluginMicriumRtosReleaseCommandMutex();
}
static void calibrateCurrentChannelCommandHandler(void)
{
  emApiCalibrateCurrentChannel();
  emAfPluginMicriumRtosSendResponse(EMBER_CALIBRATE_CURRENT_CHANNEL_IPC_COMMAND_ID,
                                    "");
}

// getMaximumPayloadLength
uint8_t emberGetMaximumPayloadLength(EmberMacAddressMode srcAddressMode,
                                     EmberMacAddressMode dstAddressMode,
                                     bool interpan,
                                     bool secured)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_GET_MAXIMUM_PAYLOAD_LENGTH_IPC_COMMAND_ID,
                                           "uuuu",
                                           srcAddressMode,
                                           dstAddressMode,
                                           interpan,
                                           secured);

  uint8_t payloadLength;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &payloadLength);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return payloadLength;
}
static void getMaximumPayloadLengthCommandHandler(void)
{
  EmberMacAddressMode srcAddressMode;
  EmberMacAddressMode dstAddressMode;
  bool interpan;
  bool secured;
  emAfPluginMicriumRtosFetchApiParams("uuuu",
                                      &srcAddressMode,
                                      &dstAddressMode,
                                      &interpan,
                                      &secured);
  uint8_t payloadLength = emApiGetMaximumPayloadLength(srcAddressMode,
                                                       dstAddressMode,
                                                       interpan,
                                                       secured);
  emAfPluginMicriumRtosSendResponse(EMBER_GET_MAXIMUM_PAYLOAD_LENGTH_IPC_COMMAND_ID,
                                    "u",
                                    payloadLength);
}

// messageSend
EmberStatus emberMessageSend(EmberNodeId destination,
                             uint8_t endpoint,
                             uint8_t messageTag,
                             EmberMessageLength messageLength,
                             uint8_t *message,
                             EmberMessageOptions options)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_MESSAGE_SEND_IPC_COMMAND_ID,
                                           "vuuubu",
                                           destination,
                                           endpoint,
                                           messageTag,
                                           messageLength,
                                           message,
                                           messageLength,
                                           options);

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void messageSendCommandHandler(void)
{
  EmberNodeId destination;
  uint8_t endpoint;
  uint8_t messageTag;
  EmberMessageLength messageLength;
  EmberMessageOptions options;
  uint8_t *message;
  emAfPluginMicriumRtosFetchApiParams("vuuupu",
                                      &destination,
                                      &endpoint,
                                      &messageTag,
                                      &messageLength,
                                      &message,
                                      &messageLength,
                                      &options);
  EmberStatus status = emApiMessageSend(destination,
                                        endpoint,
                                        messageTag,
                                        messageLength,
                                        message,
                                        options);
  emAfPluginMicriumRtosSendResponse(EMBER_MESSAGE_SEND_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// pollForData
EmberStatus emberPollForData(void)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_POLL_FOR_DATA_IPC_COMMAND_ID,
                                           "");

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void pollForDataCommandHandler(void)
{
  EmberStatus status = emApiPollForData();
  emAfPluginMicriumRtosSendResponse(EMBER_POLL_FOR_DATA_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// macMessageSend
EmberStatus emberMacMessageSend(EmberMacFrame *macFrame,
                                uint8_t messageTag,
                                EmberMessageLength messageLength,
                                uint8_t *message,
                                EmberMessageOptions options)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_MAC_MESSAGE_SEND_IPC_COMMAND_ID,
                                           "vbuvbuvvuuuubu",
                                           macFrame->srcAddress.addr.shortAddress,
                                           macFrame->srcAddress.addr.longAddress,
                                           EUI64_SIZE,
                                           macFrame->srcAddress.mode,
                                           macFrame->dstAddress.addr.shortAddress,
                                           macFrame->dstAddress.addr.longAddress,
                                           EUI64_SIZE,
                                           macFrame->dstAddress.mode,
                                           macFrame->srcPanId,
                                           macFrame->dstPanId,
                                           macFrame->srcPanIdSpecified,
                                           macFrame->dstPanIdSpecified,
                                           messageTag,
                                           messageLength,
                                           message,
                                           messageLength,
                                           options);

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void macMessageSendCommandHandler(void)
{
  EmberMacFrame macFrame;
  uint8_t messageTag;
  EmberMessageLength messageLength;
  EmberMessageOptions options;
  uint8_t eui64Size = EUI64_SIZE;
  uint8_t *message;
  emAfPluginMicriumRtosFetchApiParams("vbuvbuvvuuuupu",
                                      &macFrame.srcAddress.addr.shortAddress,
                                      &macFrame.srcAddress.addr.longAddress,
                                      &eui64Size,
                                      &macFrame.srcAddress.mode,
                                      &macFrame.dstAddress.addr.shortAddress,
                                      &macFrame.dstAddress.addr.longAddress,
                                      &eui64Size,
                                      &macFrame.dstAddress.mode,
                                      &macFrame.srcPanId,
                                      &macFrame.dstPanId,
                                      &macFrame.srcPanIdSpecified,
                                      &macFrame.dstPanIdSpecified,
                                      &messageTag,
                                      &messageLength,
                                      &message,
                                      &messageLength,
                                      &options);
  EmberStatus status = emApiMacMessageSend(&macFrame,
                                           messageTag,
                                           messageLength,
                                           message,
                                           options);
  emAfPluginMicriumRtosSendResponse(EMBER_MAC_MESSAGE_SEND_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// macSetAllocateAddressFlag
EmberStatus emberMacSetAllocateAddressFlag(bool allocateAddress)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_MAC_SET_ALLOCATE_ADDRESS_FLAG_IPC_COMMAND_ID,
                                           "u",
                                           allocateAddress);

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void macSetAllocateAddressFlagCommandHandler(void)
{
  bool allocateAddress;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &allocateAddress);
  EmberStatus status = emApiMacSetAllocateAddressFlag(allocateAddress);
  emAfPluginMicriumRtosSendResponse(EMBER_MAC_SET_ALLOCATE_ADDRESS_FLAG_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// macSetPanCoordinator
EmberStatus emberMacSetPanCoordinator(bool isCoordinator)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_MAC_SET_PAN_COORDINATOR_IPC_COMMAND_ID,
                                           "u",
                                           isCoordinator);

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void macSetPanCoordinatorCommandHandler(void)
{
  bool isCoordinator;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &isCoordinator);
  EmberStatus status = emApiMacSetPanCoordinator(isCoordinator);
  emAfPluginMicriumRtosSendResponse(EMBER_MAC_SET_PAN_COORDINATOR_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// setPollDestinationAddress
EmberStatus emberSetPollDestinationAddress(EmberMacAddress *destination)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_SET_POLL_DESTINATION_ADDRESS_IPC_COMMAND_ID,
                                           "vbu",
                                           destination->addr.shortAddress,
                                           destination->addr.longAddress,
                                           EUI64_SIZE,
                                           destination->mode);

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void setPollDestinationAddressCommandHandler(void)
{
  EmberMacAddress destination;
  uint8_t eui64Size = EUI64_SIZE;
  emAfPluginMicriumRtosFetchApiParams("vbu",
                                      &destination.addr.shortAddress,
                                      &destination.addr.longAddress,
                                      &eui64Size,
                                      &destination.mode);
  EmberStatus status = emApiSetPollDestinationAddress(&destination);
  emAfPluginMicriumRtosSendResponse(EMBER_SET_POLL_DESTINATION_ADDRESS_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// removeChild
EmberStatus emberRemoveChild(EmberMacAddress *address)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_REMOVE_CHILD_IPC_COMMAND_ID,
                                           "vbu",
                                           address->addr.shortAddress,
                                           address->addr.longAddress,
                                           EUI64_SIZE,
                                           address->mode);

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void removeChildCommandHandler(void)
{
  EmberMacAddress address;
  uint8_t eui64Size = EUI64_SIZE;
  emAfPluginMicriumRtosFetchApiParams("vbu",
                                      &address.addr.shortAddress,
                                      &address.addr.longAddress,
                                      &eui64Size,
                                      &address.mode);
  EmberStatus status = emApiRemoveChild(&address);
  emAfPluginMicriumRtosSendResponse(EMBER_REMOVE_CHILD_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// getChildFlags
EmberStatus emberGetChildFlags(EmberMacAddress *address,
                               uint8_t* flags)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_GET_CHILD_FLAGS_IPC_COMMAND_ID,
                                           "vbu",
                                           address->addr.shortAddress,
                                           address->addr.longAddress,
                                           EUI64_SIZE,
                                           address->mode);

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("uu",
                                      &status,
                                      flags);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void getChildFlagsCommandHandler(void)
{
  EmberMacAddress address;
  uint8_t flags;
  uint8_t eui64Size = EUI64_SIZE;
  emAfPluginMicriumRtosFetchApiParams("vbu",
                                      &address.addr.shortAddress,
                                      &address.addr.longAddress,
                                      &eui64Size,
                                      &address.mode);
  EmberStatus status = emApiGetChildFlags(&address,
                                          &flags);
  emAfPluginMicriumRtosSendResponse(EMBER_GET_CHILD_FLAGS_IPC_COMMAND_ID,
                                    "uu",
                                    status,
                                    flags);
}

// purgeIndirectMessages
EmberStatus emberPurgeIndirectMessages(void)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_PURGE_INDIRECT_MESSAGES_IPC_COMMAND_ID,
                                           "");

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void purgeIndirectMessagesCommandHandler(void)
{
  EmberStatus status = emApiPurgeIndirectMessages();
  emAfPluginMicriumRtosSendResponse(EMBER_PURGE_INDIRECT_MESSAGES_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// networkInit
EmberStatus emberNetworkInit(void)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_NETWORK_INIT_IPC_COMMAND_ID,
                                           "");

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void networkInitCommandHandler(void)
{
  EmberStatus status = emApiNetworkInit();
  emAfPluginMicriumRtosSendResponse(EMBER_NETWORK_INIT_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// startActiveScan
EmberStatus emberStartActiveScan(uint8_t channel)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_START_ACTIVE_SCAN_IPC_COMMAND_ID,
                                           "u",
                                           channel);

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void startActiveScanCommandHandler(void)
{
  uint8_t channel;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &channel);
  EmberStatus status = emApiStartActiveScan(channel);
  emAfPluginMicriumRtosSendResponse(EMBER_START_ACTIVE_SCAN_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// startEnergyScan
EmberStatus emberStartEnergyScan(uint8_t channel,
                                 uint8_t samples)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_START_ENERGY_SCAN_IPC_COMMAND_ID,
                                           "uu",
                                           channel,
                                           samples);

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void startEnergyScanCommandHandler(void)
{
  uint8_t channel;
  uint8_t samples;
  emAfPluginMicriumRtosFetchApiParams("uu",
                                      &channel,
                                      &samples);
  EmberStatus status = emApiStartEnergyScan(channel,
                                            samples);
  emAfPluginMicriumRtosSendResponse(EMBER_START_ENERGY_SCAN_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// setApplicationBeaconPayload
EmberStatus emberSetApplicationBeaconPayload(uint8_t payloadLength,
                                             uint8_t *payload)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_SET_APPLICATION_BEACON_PAYLOAD_IPC_COMMAND_ID,
                                           "b",
                                           payload,
                                           payloadLength);

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void setApplicationBeaconPayloadCommandHandler(void)
{
  uint8_t payloadLength;
  uint8_t *payload;
  emAfPluginMicriumRtosFetchApiParams("p",
                                      &payload,
                                      &payloadLength);
  EmberStatus status = emApiSetApplicationBeaconPayload(payloadLength,
                                                        payload);
  emAfPluginMicriumRtosSendResponse(EMBER_SET_APPLICATION_BEACON_PAYLOAD_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// formNetwork
EmberStatus emberFormNetwork(EmberNetworkParameters *parameters)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_FORM_NETWORK_IPC_COMMAND_ID,
                                           "vuu",
                                           parameters->panId,
                                           parameters->radioTxPower,
                                           parameters->radioChannel);

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void formNetworkCommandHandler(void)
{
  EmberNetworkParameters parameters;
  emAfPluginMicriumRtosFetchApiParams("vuu",
                                      &parameters.panId,
                                      &parameters.radioTxPower,
                                      &parameters.radioChannel);
  EmberStatus status = emApiFormNetwork(&parameters);
  emAfPluginMicriumRtosSendResponse(EMBER_FORM_NETWORK_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// joinNetworkExtended
EmberStatus emberJoinNetworkExtended(EmberNodeType nodeType,
                                     EmberNodeId nodeId,
                                     EmberNetworkParameters *parameters)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_JOIN_NETWORK_EXTENDED_IPC_COMMAND_ID,
                                           "uvvuu",
                                           nodeType,
                                           nodeId,
                                           parameters->panId,
                                           parameters->radioTxPower,
                                           parameters->radioChannel);

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void joinNetworkExtendedCommandHandler(void)
{
  EmberNodeType nodeType;
  EmberNodeId nodeId;
  EmberNetworkParameters parameters;
  emAfPluginMicriumRtosFetchApiParams("uvvuu",
                                      &nodeType,
                                      &nodeId,
                                      &parameters.panId,
                                      &parameters.radioTxPower,
                                      &parameters.radioChannel);
  EmberStatus status = emApiJoinNetworkExtended(nodeType,
                                                nodeId,
                                                &parameters);
  emAfPluginMicriumRtosSendResponse(EMBER_JOIN_NETWORK_EXTENDED_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// permitJoining
EmberStatus emberPermitJoining(uint8_t duration)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_PERMIT_JOINING_IPC_COMMAND_ID,
                                           "u",
                                           duration);

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void permitJoiningCommandHandler(void)
{
  uint8_t duration;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &duration);
  EmberStatus status = emApiPermitJoining(duration);
  emAfPluginMicriumRtosSendResponse(EMBER_PERMIT_JOINING_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// joinCommissioned
EmberStatus emberJoinCommissioned(EmberNodeType nodeType,
                                  EmberNodeId nodeId,
                                  EmberNetworkParameters *parameters)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_JOIN_COMMISSIONED_IPC_COMMAND_ID,
                                           "uvvuu",
                                           nodeType,
                                           nodeId,
                                           parameters->panId,
                                           parameters->radioTxPower,
                                           parameters->radioChannel);

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void joinCommissionedCommandHandler(void)
{
  EmberNodeType nodeType;
  EmberNodeId nodeId;
  EmberNetworkParameters parameters;
  emAfPluginMicriumRtosFetchApiParams("uvvuu",
                                      &nodeType,
                                      &nodeId,
                                      &parameters.panId,
                                      &parameters.radioTxPower,
                                      &parameters.radioChannel);
  EmberStatus status = emApiJoinCommissioned(nodeType,
                                             nodeId,
                                             &parameters);
  emAfPluginMicriumRtosSendResponse(EMBER_JOIN_COMMISSIONED_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// resetNetworkState
void emberResetNetworkState(void)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_RESET_NETWORK_STATE_IPC_COMMAND_ID,
                                           "");
  emAfPluginMicriumRtosReleaseCommandMutex();
}
static void resetNetworkStateCommandHandler(void)
{
  emApiResetNetworkState();
  emAfPluginMicriumRtosSendResponse(EMBER_RESET_NETWORK_STATE_IPC_COMMAND_ID,
                                    "");
}

// frequencyHoppingStartServer
EmberStatus emberFrequencyHoppingStartServer(void)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_FREQUENCY_HOPPING_START_SERVER_IPC_COMMAND_ID,
                                           "");

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void frequencyHoppingStartServerCommandHandler(void)
{
  EmberStatus status = emApiFrequencyHoppingStartServer();
  emAfPluginMicriumRtosSendResponse(EMBER_FREQUENCY_HOPPING_START_SERVER_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// frequencyHoppingStartClient
EmberStatus emberFrequencyHoppingStartClient(EmberNodeId serverNodeId,
                                             EmberPanId serverPanId)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_FREQUENCY_HOPPING_START_CLIENT_IPC_COMMAND_ID,
                                           "vv",
                                           serverNodeId,
                                           serverPanId);

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void frequencyHoppingStartClientCommandHandler(void)
{
  EmberNodeId serverNodeId;
  EmberPanId serverPanId;
  emAfPluginMicriumRtosFetchApiParams("vv",
                                      &serverNodeId,
                                      &serverPanId);
  EmberStatus status = emApiFrequencyHoppingStartClient(serverNodeId,
                                                        serverPanId);
  emAfPluginMicriumRtosSendResponse(EMBER_FREQUENCY_HOPPING_START_CLIENT_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// frequencyHoppingStop
EmberStatus emberFrequencyHoppingStop(void)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_FREQUENCY_HOPPING_STOP_IPC_COMMAND_ID,
                                           "");

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void frequencyHoppingStopCommandHandler(void)
{
  EmberStatus status = emApiFrequencyHoppingStop();
  emAfPluginMicriumRtosSendResponse(EMBER_FREQUENCY_HOPPING_STOP_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// setAuxiliaryAddressFilteringEntry
EmberStatus emberSetAuxiliaryAddressFilteringEntry(EmberNodeId nodeId,
                                                   uint8_t entryIndex)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_SET_AUXILIARY_ADDRESS_FILTERING_ENTRY_IPC_COMMAND_ID,
                                           "vu",
                                           nodeId,
                                           entryIndex);

  EmberStatus status;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &status);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return status;
}
static void setAuxiliaryAddressFilteringEntryCommandHandler(void)
{
  EmberNodeId nodeId;
  uint8_t entryIndex;
  emAfPluginMicriumRtosFetchApiParams("vu",
                                      &nodeId,
                                      &entryIndex);
  EmberStatus status = emApiSetAuxiliaryAddressFilteringEntry(nodeId,
                                                              entryIndex);
  emAfPluginMicriumRtosSendResponse(EMBER_SET_AUXILIARY_ADDRESS_FILTERING_ENTRY_IPC_COMMAND_ID,
                                    "u",
                                    status);
}

// getAuxiliaryAddressFilteringEntry
EmberNodeId emberGetAuxiliaryAddressFilteringEntry(uint8_t entryIndex)
{
  emAfPluginMicriumRtosAcquireCommandMutex();
  emAfPluginMicriumRtosSendBlockingCommand(EMBER_GET_AUXILIARY_ADDRESS_FILTERING_ENTRY_IPC_COMMAND_ID,
                                           "u",
                                           entryIndex);

  EmberNodeId nodeId;
  emAfPluginMicriumRtosFetchApiParams("v",
                                      &nodeId);
  emAfPluginMicriumRtosReleaseCommandMutex();
  return nodeId;
}
static void getAuxiliaryAddressFilteringEntryCommandHandler(void)
{
  uint8_t entryIndex;
  emAfPluginMicriumRtosFetchApiParams("u",
                                      &entryIndex);
  EmberNodeId nodeId = emApiGetAuxiliaryAddressFilteringEntry(entryIndex);
  emAfPluginMicriumRtosSendResponse(EMBER_GET_AUXILIARY_ADDRESS_FILTERING_ENTRY_IPC_COMMAND_ID,
                                    "v",
                                    nodeId);
}

//------------------------------------------------------------------------------
// API command dispatcher (vNCP side)

void emAfPluginMicriumRtosHandleIncomingApiCommand(uint16_t commandId)
{
  assert(emAfPluginMicriumRtosIsCurrentTaskStackTask());

  switch (commandId) {
    case EMBER_NETWORK_STATE_IPC_COMMAND_ID:
      networkStateCommandHandler();
      break;
    case EMBER_STACK_IS_UP_IPC_COMMAND_ID:
      stackIsUpCommandHandler();
      break;
    case EMBER_SET_SECURITY_KEY_IPC_COMMAND_ID:
      setSecurityKeyCommandHandler();
      break;
    case EMBER_GET_COUNTER_IPC_COMMAND_ID:
      getCounterCommandHandler();
      break;
    case EMBER_SET_RADIO_CHANNEL_IPC_COMMAND_ID:
      setRadioChannelCommandHandler();
      break;
    case EMBER_GET_RADIO_CHANNEL_IPC_COMMAND_ID:
      getRadioChannelCommandHandler();
      break;
    case EMBER_SET_RADIO_POWER_IPC_COMMAND_ID:
      setRadioPowerCommandHandler();
      break;
    case EMBER_GET_RADIO_POWER_IPC_COMMAND_ID:
      getRadioPowerCommandHandler();
      break;
    case EMBER_SET_RADIO_POWER_MODE_IPC_COMMAND_ID:
      setRadioPowerModeCommandHandler();
      break;
    case EMBER_SET_MAC_PARAMS_IPC_COMMAND_ID:
      setMacParamsCommandHandler();
      break;
    case EMBER_CURRENT_STACK_TASKS_IPC_COMMAND_ID:
      currentStackTasksCommandHandler();
      break;
    case EMBER_CALIBRATE_CURRENT_CHANNEL_IPC_COMMAND_ID:
      calibrateCurrentChannelCommandHandler();
      break;
    case EMBER_GET_MAXIMUM_PAYLOAD_LENGTH_IPC_COMMAND_ID:
      getMaximumPayloadLengthCommandHandler();
      break;
    case EMBER_MESSAGE_SEND_IPC_COMMAND_ID:
      messageSendCommandHandler();
      break;
    case EMBER_POLL_FOR_DATA_IPC_COMMAND_ID:
      pollForDataCommandHandler();
      break;
    case EMBER_MAC_MESSAGE_SEND_IPC_COMMAND_ID:
      macMessageSendCommandHandler();
      break;
    case EMBER_MAC_SET_ALLOCATE_ADDRESS_FLAG_IPC_COMMAND_ID:
      macSetAllocateAddressFlagCommandHandler();
      break;
    case EMBER_MAC_SET_PAN_COORDINATOR_IPC_COMMAND_ID:
      macSetPanCoordinatorCommandHandler();
      break;
    case EMBER_SET_POLL_DESTINATION_ADDRESS_IPC_COMMAND_ID:
      setPollDestinationAddressCommandHandler();
      break;
    case EMBER_REMOVE_CHILD_IPC_COMMAND_ID:
      removeChildCommandHandler();
      break;
    case EMBER_GET_CHILD_FLAGS_IPC_COMMAND_ID:
      getChildFlagsCommandHandler();
      break;
    case EMBER_PURGE_INDIRECT_MESSAGES_IPC_COMMAND_ID:
      purgeIndirectMessagesCommandHandler();
      break;
    case EMBER_NETWORK_INIT_IPC_COMMAND_ID:
      networkInitCommandHandler();
      break;
    case EMBER_START_ACTIVE_SCAN_IPC_COMMAND_ID:
      startActiveScanCommandHandler();
      break;
    case EMBER_START_ENERGY_SCAN_IPC_COMMAND_ID:
      startEnergyScanCommandHandler();
      break;
    case EMBER_SET_APPLICATION_BEACON_PAYLOAD_IPC_COMMAND_ID:
      setApplicationBeaconPayloadCommandHandler();
      break;
    case EMBER_FORM_NETWORK_IPC_COMMAND_ID:
      formNetworkCommandHandler();
      break;
    case EMBER_JOIN_NETWORK_EXTENDED_IPC_COMMAND_ID:
      joinNetworkExtendedCommandHandler();
      break;
    case EMBER_PERMIT_JOINING_IPC_COMMAND_ID:
      permitJoiningCommandHandler();
      break;
    case EMBER_JOIN_COMMISSIONED_IPC_COMMAND_ID:
      joinCommissionedCommandHandler();
      break;
    case EMBER_RESET_NETWORK_STATE_IPC_COMMAND_ID:
      resetNetworkStateCommandHandler();
      break;
    case EMBER_FREQUENCY_HOPPING_START_SERVER_IPC_COMMAND_ID:
      frequencyHoppingStartServerCommandHandler();
      break;
    case EMBER_FREQUENCY_HOPPING_START_CLIENT_IPC_COMMAND_ID:
      frequencyHoppingStartClientCommandHandler();
      break;
    case EMBER_FREQUENCY_HOPPING_STOP_IPC_COMMAND_ID:
      frequencyHoppingStopCommandHandler();
      break;
    case EMBER_SET_AUXILIARY_ADDRESS_FILTERING_ENTRY_IPC_COMMAND_ID:
      setAuxiliaryAddressFilteringEntryCommandHandler();
      break;
    case EMBER_GET_AUXILIARY_ADDRESS_FILTERING_ENTRY_IPC_COMMAND_ID:
      getAuxiliaryAddressFilteringEntryCommandHandler();
      break;
    default: {
      assert(0);
    }
  }
}
