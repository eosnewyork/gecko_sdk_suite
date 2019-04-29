// Copyright 2016 Silicon Laboratories, Inc.

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
