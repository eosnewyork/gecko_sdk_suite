// Copyright 2015 Silicon Laboratories, Inc.

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"
#include "debug-print/debug-print.h"
#include "ota-bootloader-test-common/ota-bootloader-test-common.h"

#include EMBER_AF_API_BOOTLOADER_INTERFACE
#include EMBER_AF_API_COMMAND_INTERPRETER2

#if defined(EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER)
#include EMBER_AF_API_OTA_BOOTLOADER_SERVER
#endif // EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER

#if defined(EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_CLIENT)
#include EMBER_AF_API_OTA_BOOTLOADER_CLIENT
#endif // EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_CLIENT

#define MAX_TARGET_LIST_SIZE                     50

EmberEventControl emAfPluginOtaBootloaderTestEventControl;

static uint8_t targetListLength;
static EmberNodeId targetList[MAX_TARGET_LIST_SIZE];

//------------------------------------------------------------------------------
// Event code (used to schedule a bootload action at the client).

void bootloaderFlashImage(void);

void emAfPluginOtaBootloaderTestEventHandler(void)
{
  emberEventControlSetInactive(emAfPluginOtaBootloaderTestEventControl);

  bootloaderFlashImage();
}

//------------------------------------------------------------------------------
// OTA Bootloader Server implemented callbacks.

#if defined(EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER)

bool emberAfPluginOtaBootloaderServerGetImageSegmentCallback(uint32_t startIndex,
                                                             uint32_t endIndex,
                                                             uint8_t imageTag,
                                                             uint8_t *imageSegment)
{
  emberAfCorePrintln("(server): get segment, start: %d, end: %d, tag: 0x%x",
                     startIndex, endIndex, imageTag);

  //Initialize bootloader (and flash part) if not yet initialized or in shutdown.
  if ( !emberAfPluginBootloaderInterfaceIsBootloaderInitialized() ) {
    if ( !emberAfPluginBootloaderInterfaceInit() ) {
      return false;
    }
  }

  if ( !emberAfPluginBootloaderInterfaceRead(startIndex, endIndex - startIndex + 1, imageSegment) ) {
    return false;
  }

  emberAfCorePrint(".");

  return true;
}

void emberAfPluginOtaBootloaderServerImageDistributionCompleteCallback(EmberAfOtaBootloaderStatus status)
{
  uint8_t i;

  emberAfCorePrintln("image distribution completed, 0x%x", status);

  for (i = 0; i < targetListLength; i++) {
    uint8_t applicationStatus;

    EmberAfOtaBootloaderTargetStatus status =
      emberAfPluginBootloaderServerGetTargetStatus(targetList[i],
                                                   &applicationStatus);

    emberAfCorePrintln("Target index %d ID 0x%2X: status 0x%x appStatus 0x%x",
                       i, targetList[i], status, applicationStatus);
  }

  emberAfPluginBootloaderInterfaceSleep();
}

void emberAfPluginBootloaderServerRequestTargetsBootloadCompleteCallback(EmberAfOtaBootloaderStatus status)
{
  uint8_t i;

  emberAfCorePrintln("bootload request completed, 0x%x", status);

  for (i = 0; i < targetListLength; i++) {
    uint8_t applicationStatus;

    EmberAfOtaBootloaderTargetStatus status =
      emberAfPluginBootloaderServerGetTargetStatus(targetList[i],
                                                   &applicationStatus);

    emberAfCorePrintln("Target index %d ID 0x%2X: status 0x%x appStatus 0x%x",
                       i, targetList[i], status, applicationStatus);
  }
}

#endif // EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER

//------------------------------------------------------------------------------
// OTA Bootloader Client implemented callbacks.

#if defined(EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_CLIENT)

bool emberAfPluginOtaBootloaderClientNewIncomingImageCallback(EmberNodeId serverId,
                                                              EmberNodeId *alternateServerId,
                                                              uint8_t imageTag)
{
  // The client shall accept images with matching tag
  bool accept = (imageTag == otaBootloaderTestImageTag);

  emberAfCorePrintln("new incoming image %s (tag=0x%x)",
                     ((accept) ? "ACCEPTED" : "REFUSED"),
                     imageTag);

  return accept;
}

void emberAfPluginOtaBootloaderClientIncomingImageSegmentCallback(EmberNodeId serverId,
                                                                  uint32_t startIndex,
                                                                  uint32_t endIndex,
                                                                  uint8_t imageTag,
                                                                  uint8_t *imageSegment)
{
  emberAfCorePrintln("(client): incoming segment, start: %d, end: %d, tag: 0x%x",
                     startIndex, endIndex, imageTag);

  //Initialize bootloader (and flash part) if not yet initialized or in shutdown.
  if ( !emberAfPluginBootloaderInterfaceIsBootloaderInitialized() ) {
    if ( !emberAfPluginBootloaderInterfaceInit() ) {
      emberAfCorePrintln("init failed");

      emberAfPluginOtaBootloaderClientAbortImageDownload(imageTag,
                                                         APPLICATION_STATUS_FLASH_INIT_FAILED);
      return;
    }
  }

  if ( !emberAfPluginBootloaderInterfaceWrite(startIndex,
                                              endIndex - startIndex + 1,
                                              imageSegment) ) {
    emberAfCorePrintln("write failed");

    emberAfPluginOtaBootloaderClientAbortImageDownload(imageTag,
                                                       APPLICATION_STATUS_FLASH_WRITE_FAILED);
    return;
  }

  emberAfCorePrint(".");
}

void emberAfPluginOtaBootloaderClientImageDownloadCompleteCallback(EmberAfOtaBootloaderStatus status,
                                                                   uint8_t imageTag,
                                                                   uint32_t imageSize)
{
  if (status == EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_SUCCESS) {
    emberAfCorePrintln("Image download COMPLETED tag=0x%x size=%d",
                       imageTag, imageSize);
  } else {
    emberAfCorePrintln("Image download FAILED status=0x%x", status);
  }
}

bool emberAfPluginOtaBootloaderClientIncomingRequestBootloadCallback(EmberNodeId serverId,
                                                                     uint8_t imageTag,
                                                                     uint32_t bootloadDelayMs,
                                                                     uint8_t *applicationStatus)
{
  // The client shall bootload an image with matching tag.

  // TODO: we should also maintain a state machine to keep track whether this
  // image was previously correctly received. For now we assume the server would
  // issue a bootload request only if the client reported a successful image
  // download.
  bool accept = (imageTag == otaBootloaderTestImageTag);

  if (accept) {
    emberAfCorePrintln("bootload request for image with tag 0x%x accepted, will bootload in %d ms",
                       imageTag, bootloadDelayMs);
    // Schedule a bootload action.
    emberEventControlSetDelayMS(emAfPluginOtaBootloaderTestEventControl,
                                bootloadDelayMs);
  } else {
    *applicationStatus = APPLICATION_STATUS_WRONG_IMAGE_TAG;
    emberAfCorePrintln("bootload request refused (tag 0x%x doesn't match)",
                       imageTag);
  }

  return accept;
}

#endif // EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_CLIENT

//------------------------------------------------------------------------------
// CLI commands.

void otaBootloaderSetTarget(void)
{
  uint8_t targetIndex = emberUnsignedCommandArgument(0);
  EmberNodeId targetId = emberUnsignedCommandArgument(1);

  if (targetIndex >= MAX_TARGET_LIST_SIZE) {
    emberAfCorePrintln("invalid index!");
    return;
  }

  targetList[targetIndex] = targetId;
  emberAfCorePrintln("target set");
}

void otaBootloaderStartDistribution(void)
{
#if defined(EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER)
  uint32_t imageSize = emberUnsignedCommandArgument(0);
  uint8_t imageTag = emberUnsignedCommandArgument(1);
  targetListLength = emberUnsignedCommandArgument(2);

  EmberAfOtaBootloaderStatus status =
    emberAfPluginOtaBootloaderServerInitiateImageDistribution(imageSize,
                                                              imageTag,
                                                              targetList,
                                                              targetListLength);
  if (status == EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_SUCCESS) {
    emberAfCorePrintln("image distribution initiated");
  } else {
    emberAfCorePrintln("image distribution failed 0x%x", status);
  }
#else
  emberAfCorePrintln("OTA bootloader server plugin not included");
#endif // EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER
}

void otaBootloaderRequestBootload(void)
{
#if defined(EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER)
  uint32_t delayMs = emberUnsignedCommandArgument(0);
  uint8_t imageTag = emberUnsignedCommandArgument(1);
  targetListLength = emberUnsignedCommandArgument(2);

  EmberAfOtaBootloaderStatus status =
    emberAfPluginBootloaderServerInitiateRequestTargetsBootload(delayMs,
                                                                imageTag,
                                                                targetList,
                                                                targetListLength);
  if (status == EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_SUCCESS) {
    emberAfCorePrintln("bootload request initiated");
  } else {
    emberAfCorePrintln("bootload request failed 0x%x", status);
  }
#else
  emberAfCorePrintln("OTA bootloader server plugin not included");
#endif // EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER
}
