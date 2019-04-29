/***************************************************************************//**
 * @file
 * @brief Gecko Bootloader Interface API
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

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <application_properties.h>
#include "btlapi.h"
#include "btl_interface.h"
#include "em_core.h"
#include "em_assert.h"

// -----------------------------------------------------------------------------
// Defines
#define SLOT_ID        0                // Bootloader default storage space ID

// -----------------------------------------------------------------------------
// Local variables
static uint32_t diskStorageSpace;
static uint32_t appVersion;

extern const ApplicationProperties_t applicationProperties;

// -----------------------------------------------------------------------------
// Local Function Prototypes
static int32_t eraseStorageSlot(uint32_t slotID);

// -----------------------------------------------------------------------------
// Global function definitions

/***************************************************************************//**
 * @brief
 *   Initialize bootloader and save pointer to the bootloader storage space
 ******************************************************************************/
void BTLAPI_Init(void)
{
  BootloaderStorageSlot_t storageSlot;
  BootloaderInformation_t info;

  bootloader_getInfo(&info);
  if (info.type == NO_BOOTLOADER) {
    printf("\nNo bootloader is present (first stage or main stage invalid).\n");
    EFM_ASSERT(false);
  }

  // Initialize Boot Loader
  if (bootloader_init() & BOOTLOADER_ERROR_INIT_BASE) {
    EFM_ASSERT(false);
  }

  if (bootloader_getStorageSlotInfo(SLOT_ID, &storageSlot) & BOOTLOADER_ERROR_STORAGE_BASE) {
    EFM_ASSERT(false);
  }

  appVersion = applicationProperties.app.version;
  printf("Current APP version: %" PRIu32 "\n", appVersion);

  diskStorageSpace = storageSlot.length;
}

/***************************************************************************//**
 * @brief
 *   Validate the image stored in the bootloader storage space and
 *   upgrades the application and the bootloader.
 ******************************************************************************/
void BTLAPI_validateAndReinstall(void)
{
  uint32_t appVersionNewImg;
  uint32_t imgInfoVersion;
  ApplicationData_t appinfo;
  BootloaderInformation_t info;

  bootloader_getInfo(&info);

  if (bootloader_verifyImage(SLOT_ID, NULL) == BOOTLOADER_OK) {
    if (bootloader_setImageToBootload(SLOT_ID) == BOOTLOADER_OK) {
      bootloader_getImageInfo(SLOT_ID, &appinfo, &imgInfoVersion);
      appVersionNewImg = appinfo.version;

      if (appVersionNewImg > appVersion) {
        printf("\nA valid GBL with a newer application version found, rebooting.\n");
        bootloader_rebootAndInstall();
      } else {
        printf("\nThe downloaded GBL file contains an application upgrade image with same/older version\n");
      }
    }
  } else {
    printf("\nInvalid image.\n");
  }

  // Clean the bootloader storage space
  printf("Erasing the bootloader storage space...\n");
  if (eraseStorageSlot(SLOT_ID) == BOOTLOADER_OK) {
    printf("Erasing the bootloader storage space completed\n");
  }
}

/***************************************************************************//**
 * @brief
 *   Reprogram the bootloader storage area
 *
 * @param[in] data
 *   Pointer to the data buffer to be stored in the bootloader storage area
 ******************************************************************************/
void BTLAPI_reprogramBtlStorage(uint8_t *data, uint16_t dataLen)
{
  static bool dataCorrupted = false;
  static uint32_t offSet = 0;

  if (diskStorageSpace < (offSet + dataLen)) {
    offSet = 0;
    return;
  }

  if (data == 0) {
    offSet = 0;
    dataCorrupted = false;
  } else {
    if (!dataCorrupted) {
      if (bootloader_eraseWriteStorage(SLOT_ID, offSet, data, dataLen)
          != BOOTLOADER_OK) {
        printf("Writing to the bootloader storage space failed\n");
        dataCorrupted = true;
      }
    }
    offSet += dataLen;
  }
}

/***************************************************************************//**
 * @brief
 *   Erase the bootloader storage slot
 ******************************************************************************/
static int32_t eraseStorageSlot(uint32_t slotID)
{
  int32_t retVal;
  uint32_t eraseOffset;
  BootloaderStorageInformation_t infoStorage;
  bootloader_getStorageInfo(&infoStorage);
  uint32_t flashPageSize = infoStorage.info->pageSize;

  BootloaderStorageSlot_t storageSlot;
  retVal = bootloader_getStorageSlotInfo(slotID, &storageSlot);
  if (retVal != BOOTLOADER_OK) {
    return retVal;
  }
  uint32_t storageSpaceAddr = storageSlot.address;
  uint8_t storageBuf[1];

  while (storageSpaceAddr < (storageSlot.address + storageSlot.length)) {
    bootloader_readRawStorage(storageSpaceAddr, storageBuf, 1);
    if (storageBuf[0] == 0xFF) {
      storageSpaceAddr += 1;
    } else {
      eraseOffset = (storageSpaceAddr & ~(flashPageSize - 1));
      storageSpaceAddr = eraseOffset + flashPageSize;
      retVal = bootloader_eraseRawStorage(eraseOffset, flashPageSize);
      if (retVal != BOOTLOADER_OK) {
        return retVal;
      }
    }
  }
  return BOOTLOADER_OK;
}
