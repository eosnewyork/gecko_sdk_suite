/***************************************************************************//**
 * @brief Set of macros for the ota-bootloader-test-common plugin.
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

#ifndef _OTA_BOOTLOADER_TEST_COMMON_H_
#define _OTA_BOOTLOADER_TEST_COMMON_H_

// Application error status codes.
#define APPLICATION_STATUS_FLASH_INIT_FAILED     0xA0
#define APPLICATION_STATUS_FLASH_WRITE_FAILED    0xA1
#define APPLICATION_STATUS_WRONG_IMAGE_TAG       0xA2

// Arbitrary image tag default value.
#define DEFAULT_IMAGE_TAG                        0x89

// Common OTA bootloader Image Tag.
extern uint8_t otaBootloaderTestImageTag;

#endif // _OTA_BOOTLOADER_TEST_COMMON_H_
