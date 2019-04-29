/*
 * Bare minimum config for wMBus mode5 (AES-CBC) encryption,
 * using the CRYPTO hardware
 *
 */

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
