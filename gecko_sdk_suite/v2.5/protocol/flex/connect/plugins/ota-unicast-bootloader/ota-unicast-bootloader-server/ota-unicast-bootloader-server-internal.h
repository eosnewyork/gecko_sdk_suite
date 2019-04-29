/***************************************************************************//**
 * @brief Macros and includes for ota-unicast-bootloader servers.
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

#ifndef _OTA_UNICAST_BOOTLOADER_SERVER_INTERNAL_H_
#define _OTA_UNICAST_BOOTLOADER_SERVER_INTERNAL_H_

#include "ota-unicast-bootloader/ota-unicast-bootloader-protocol.h"
#include "ota-unicast-bootloader/ota-unicast-bootloader-common-internal.h"

//------------------------------------------------------------------------------
// Internal states

#define STATE_OTA_SERVER_IDLE                                               0x00
#define STATE_OTA_SERVER_SEGMENT_UNICAST_INTERVAL                           0x01
#define STATE_OTA_SERVER_SEGMENT_UNICAST_PENDING                            0x02
#define STATE_OTA_SERVER_SEGMENT_UNICAST_WAITING_RESPONSE                   0x03
#define STATE_OTA_SERVER_IMAGE_DISTRIBUTION_COMPLETED                       0x04
#define STATE_OTA_SERVER_BOOTLOAD_REQUEST_UNICAST_INTERVAL                  0x05
#define STATE_OTA_SERVER_BOOTLOAD_REQUEST_UNICAST_PENDING                   0x06
#define STATE_OTA_SERVER_BOOTLOAD_REQUEST_WAITING_RESPONSE                  0x07
#define STATE_OTA_SERVER_BOOTLOAD_REQUEST_COMPLETED                         0x08
#define STATE_OTA_SERVER_HANDSHAKE_INTERVAL                                 0x09
#define STATE_OTA_SERVER_HANDSHAKE_PENDING                                  0x0A
#define STATE_OTA_SERVER_HANDSHAKE_WAITING_RESPONSE                         0x0B

#define serverIsIdle()  (internalState == STATE_OTA_SERVER_IDLE)

#define serverIsCompletingProcess() \
  (processCompleteState != STATE_OTA_SERVER_IDLE)

#define serverInImageDistributionProcess()                    \
  (internalState >= STATE_OTA_SERVER_SEGMENT_UNICAST_INTERVAL \
   && internalState <= STATE_OTA_SERVER_SEGMENT_UNICAST_WAITING_RESPONSE)

#define serverCompletingDistributionProcess() \
  (processCompleteState == STATE_OTA_SERVER_IMAGE_DISTRIBUTION_COMPLETED)

#define serverInBootloadRequestProcess()                               \
  (internalState >= STATE_OTA_SERVER_BOOTLOAD_REQUEST_UNICAST_INTERVAL \
   && internalState <= STATE_OTA_SERVER_BOOTLOAD_REQUEST_WAITING_RESPONSE)

#define serverCompletingBootloadRequestProcess() \
  (processCompleteState == STATE_OTA_SERVER_BOOTLOAD_REQUEST_COMPLETED)

#endif // _OTA_UNICAST_BOOTLOADER_SERVER_INTERNAL_H_
