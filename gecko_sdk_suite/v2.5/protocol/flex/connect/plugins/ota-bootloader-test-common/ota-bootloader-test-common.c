/***************************************************************************//**
 * @brief Set of APIs for the ota-bootloader-test-common plugin.
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

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"
#include "debug-print/debug-print.h"
#include "ota-bootloader-test-common/ota-bootloader-test-common.h"

#include EMBER_AF_API_BOOTLOADER_INTERFACE
#include EMBER_AF_API_COMMAND_INTERPRETER2

// The image tag the client shall accept.
uint8_t otaBootloaderTestImageTag = DEFAULT_IMAGE_TAG;

//------------------------------------------------------------------------------
// CLI commands.

void bootloaderGetVersionCommand(void)
{
  uint16_t blVersion;

  emberAfPluginBootloaderInterfaceGetVersion(&blVersion);

  emberAfCorePrintln("bootloader version: %d", blVersion);
}

void bootloaderInitCommand(void)
{
  if (emberAfPluginBootloaderInterfaceInit()) {
    emberAfCorePrintln("bootloader init succeeded!");
  } else {
    emberAfCorePrintln("bootloader init failed! wrong chip?");
  }
}

void bootloaderSleepCommand(void)
{
  emberAfPluginBootloaderInterfaceSleep();
  emberAfCorePrintln("sleep bootloader and flash part");
}

void bootloaderFlashEraseCommand(void)
{
  emberAfCorePrintln("flash erase started");
  emberAfPluginBootloaderInterfaceChipErase();
}

void bootloaderFlashEraseSlotCommand(void)
{
  uint32_t slot = emberUnsignedCommandArgument(0);

  emberAfCorePrintln("flash erasing slot %d started", slot);

  if ( emberAfPluginBootloaderInterfaceChipEraseSlot(slot) ) {
    emberAfCorePrintln("flash erase successful!");
  } else {
    emberAfCorePrintln("flash erase failed!");
  }
}

void bootloaderFlashReadCommand(void)
{
  uint32_t address = emberUnsignedCommandArgument(0);
  uint8_t length = emberUnsignedCommandArgument(1);
  uint8_t buff[255];
  uint8_t i;

  if (emberAfPluginBootloaderInterfaceRead(address, length, buff)) {
    emberAfCorePrintln("flash read succeeded!");
    emberAfCorePrintln("address: %d, length: %d, data:", address, length);
    for (i = 0; i < length; i++) {
      emberAfCorePrint("0x%x ", buff[i]);
    }
    emberAfCorePrintln("");
  } else {
    emberAfCorePrintln("flash read failed!");
  }
}

void bootloaderFlashWriteCommand(void)
{
  uint32_t address = emberUnsignedCommandArgument(0);
  uint8_t length;
  uint8_t *dataBuff = emberStringCommandArgument(1, &length);

  if (emberAfPluginBootloaderInterfaceWrite(address, length, dataBuff)) {
    emberAfCorePrintln("flash write succeeded!");
  } else {
    emberAfCorePrintln("flash write failed!");
  }
}

void bootloaderValidateImage(void)
{
  if (!emberAfPluginBootloaderInterfaceValidateImage()) {
    emberAfCorePrintln("Image is invalid!");
  } else {
    emberAfCorePrintln("Image is valid!");
  }
}

void bootloaderFlashImage(void)
{
  if (!emberAfPluginBootloaderInterfaceIsBootloaderInitialized()) {
    if (!emberAfPluginBootloaderInterfaceInit()) {
      emberAfCorePrintln("bootloader init failed");
      return;
    }
  }

  emberAfPluginBootloaderInterfaceBootload();

  // If we get here bootload process failed.
  emberAfCorePrintln("bootload failed!");
}

void otaBootloaderSetImageTag(void)
{
  otaBootloaderTestImageTag = emberUnsignedCommandArgument(0);
  emberAfCorePrintln("image tag set");
}
