// Copyright 2016 Silicon Laboratories, Inc.

#ifndef _OTA_UNICAST_BOOTLOADER_COMMON_INTERNAL_H_
#define _OTA_UNICAST_BOOTLOADER_COMMON_INTERNAL_H_

#if (defined(EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_SECURITY_ENABLED) \
  || defined(EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_CLIENT_SECURITY_ENABLED))
#define SECURITY_ENABLED
#endif

//------------------------------------------------------------------------------
// Image related defines

#if defined(SECURITY_ENABLED)
// Segment size: depends on whether we use security or not (plugin option).
#define MAX_APPLICATION_PAYLOAD_LENGTH \
  EMBER_MAX_SECURED_APPLICATION_PAYLOAD_LENGTH
#define UNICAST_TX_OPTIONS  (EMBER_OPTIONS_ACK_REQUESTED \
                             | EMBER_OPTIONS_SECURITY_ENABLED)
#else
#define MAX_APPLICATION_PAYLOAD_LENGTH \
  EMBER_MAX_UNSECURED_APPLICATION_PAYLOAD_LENGTH
#define UNICAST_TX_OPTIONS  (EMBER_OPTIONS_ACK_REQUESTED)
#endif // SECURITY_ENABLED

// Segment size is always set to be a multiple of 4 to facilitate flash writes
// to parts that require 4-bytes alignment.
#define MAX_SEGMENT_PAYLOAD_LENGTH                                       \
  ((MAX_APPLICATION_PAYLOAD_LENGTH                                       \
    - EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_HEADER_LENGTH) \
   & 0xFC)

#define MAXIMUM_IMAGE_SIZE (0xFFFE * MAX_SEGMENT_PAYLOAD_LENGTH)

// (Max) length of a missing segments bitmask in bytes.
#define MISSING_SEGMENTS_BITMASK_LENGTH   (MAX_SEGMENTS_IN_A_BLOCK / 8)

#define INVALID_SEGMENT_INDEX             0xFFFF

#endif // _OTA_UNICAST_BOOTLOADER_COMMON_INTERNAL_H_
