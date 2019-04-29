/***************************************************************************//**
 * @brief Marcos, includes, and typedefs for ota-broadcast-bootloader-client.
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

#ifndef _OTA_BROADCAST_BOOTLOADER_CLIENT_INTERNAL_H_
#define _OTA_BROADCAST_BOOTLOADER_CLIENT_INTERNAL_H_

#include "ota-broadcast-bootloader/ota-broadcast-bootloader-protocol.h"
#include "ota-broadcast-bootloader/ota-broadcast-bootloader-common-internal.h"

//------------------------------------------------------------------------------
// Internal states

#define STATE_OTA_CLIENT_IDLE                                               0x00
#define STATE_OTA_CLIENT_IMAGE_DOWNLOAD_WAITING                             0x01

#define clientIsIdle()  (internalState == STATE_OTA_CLIENT_IDLE)
#define clientInImageDownload() \
  (internalState == STATE_OTA_CLIENT_IMAGE_DOWNLOAD_WAITING)

//------------------------------------------------------------------------------
// Download history types and definitions.

#define HISTORY_TABLE_SIZE        5

typedef struct {
  uint8_t imageTag;
  uint8_t otaProtocolStatus;
  uint8_t applicationStatus;
  EmberNodeId serverId;
  EmberNodeId alternateServerId;
} EmHistoryEntry;

//------------------------------------------------------------------------------
// Test-related macros

#if defined(EMBER_SCRIPTED_TEST)
#include "core/ember-stack.h"
#include "core/parcel.h"
#include "core/scripted-stub.h"
#define scriptCheckHistoryTableAdd(index) \
  simpleScriptCheck("history table add", "history table add", "i", (index))
#define scriptCheckHistoryTableRemove(index, oldestIndex) \
  simpleScriptCheck("history table remove", "history table remove", "ii", (index), (oldestIndex))
#else
#define scriptCheckHistoryTableAdd(index)
#define scriptCheckHistoryTableRemove(index, oldestIndex)
#endif

#endif // _OTA_BROADCAST_BOOTLOADER_CLIENT_INTERNAL_H_
