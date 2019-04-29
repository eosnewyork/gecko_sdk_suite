/***************************************************************************//**
 * @brief Macros defined for the wMBus plugin.
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

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

#if !defined(NO_CRYPTO_ACCELERATION)
/* SiliconLabs plugins with CRYPTO acceleration support. */
#define MBEDTLS_AES_ALT
#endif

/* mbed TLS modules */
#define MBEDTLS_AES_C
#define MBEDTLS_CIPHER_MODE_CBC

/* Save RAM at the expense of ROM */
#define MBEDTLS_AES_ROM_TABLES

#include "mbedtls/check_config.h"

#endif /* MBEDTLS_CONFIG_H */
