/***************************************************************************//**
 * @brief Macros and types defined for ota-unicast-bootloaders.
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

#ifndef _OTA_UNICAST_BOOTLOADER_TYPES_H_
#define _OTA_UNICAST_BOOTLOADER_TYPES_H_

/**
 * @addtogroup ota_unicast_bootloader_common
 * @{
 */

/**
 * @brief OTA Unicast Bootloader return status codes.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberAfOtaUnicastBootloaderStatus
#else
typedef uint8_t EmberAfOtaUnicastBootloaderStatus;
enum
#endif
{
  EMBER_OTA_UNICAST_BOOTLOADER_STATUS_SUCCESS                        = 0x00,
  EMBER_OTA_UNICAST_BOOTLOADER_STATUS_INVALID_CALL                   = 0x01,
  EMBER_OTA_UNICAST_BOOTLOADER_STATUS_BUSY                           = 0x02,
  EMBER_OTA_UNICAST_BOOTLOADER_STATUS_DATA_UNDERFLOW                 = 0x03,
  EMBER_OTA_UNICAST_BOOTLOADER_STATUS_STACK_ERROR                    = 0x04,
  EMBER_OTA_UNICAST_BOOTLOADER_STATUS_TIMEOUT                        = 0x05,
  EMBER_OTA_UNICAST_BOOTLOADER_STATUS_FAILED                         = 0x06,
  EMBER_OTA_UNICAST_BOOTLOADER_STATUS_ABORTED                        = 0x07,
  EMBER_OTA_UNICAST_BOOTLOADER_STATUS_REFUSED                        = 0x08,
  EMBER_OTA_UNICAST_BOOTLOADER_STATUS_UNREACHABLE                    = 0x09,
};

/**
 * @brief OTA Unicast Bootloader target status codes.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberAfOtaUnicastBootloaderTargetStatus
#else
typedef uint8_t EmberAfOtaUnicastBootloaderTargetStatus;
enum
#endif
{
  EMBER_OTA_UNICAST_BOOTLOADER_TARGET_STATUS_INVALID                        = 0x00,
  EMBER_OTA_UNICAST_BOOTLOADER_TARGET_STATUS_UNREACHABLE                    = 0x01,
  EMBER_OTA_UNICAST_BOOTLOADER_TARGET_STATUS_DISTRIBUTION_COMPLETED         = 0x02,
  EMBER_OTA_UNICAST_BOOTLOADER_TARGET_STATUS_DISTRIBUTION_ONGOING           = 0x03,
  EMBER_OTA_UNICAST_BOOTLOADER_TARGET_STATUS_DISTRIBUTION_FAILED            = 0x04,
  EMBER_OTA_UNICAST_BOOTLOADER_TARGET_STATUS_DISTRIBUTION_REFUSED           = 0x05,
  EMBER_OTA_UNICAST_BOOTLOADER_TARGET_STATUS_DISTRIBUTION_ABORTED           = 0x06,
  EMBER_OTA_UNICAST_BOOTLOADER_TARGET_STATUS_DISTRIBUTION_OUT_OF_SEQ        = 0x07,
  EMBER_OTA_UNICAST_BOOTLOADER_TARGET_STATUS_BOOTLOAD_REQUEST_ACCEPTED      = 0x08,
  EMBER_OTA_UNICAST_BOOTLOADER_TARGET_STATUS_BOOTLOAD_REQUEST_ONGOING       = 0x09,
  EMBER_OTA_UNICAST_BOOTLOADER_TARGET_STATUS_BOOTLOAD_REQUEST_REFUSED       = 0x0A,
};

/**
 * @brief A value indicating that client application did not set the application
 * level target status in any of the client callbacks.
 */
#define EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_INVALID_APPLICATION_TARGET_STATUS    0xFF

/** @} // END addtogroup
 */

#endif // _OTA_UNICAST_BOOTLOADER_TYPES_H_
