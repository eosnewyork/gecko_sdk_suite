/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: APACHE-2.0
 *
 * This software is subject to an open source license and is distributed by
 * Silicon Laboratories Inc. pursuant to the terms of the Apache License,
 * Version 2.0 available at https://www.apache.org/licenses/LICENSE-2.0.
 * Such terms and conditions may be further supplemented by the Silicon Labs
 * Master Software License Agreement (MSLA) available at www.silabs.com and its
 * sections applicable to open source software.
 *
 ******************************************************************************/

/**
 * @defgroup sl_crypto_config Silicon Labs CRYPTO Hardware Acceleration Configuration
 * @addtogroup sl_crypto_config
 *
 * @brief
 *  mbed TLS configuration for Silicon Labs CRYPTO hardware acceleration
 *
 * @details
 *  mbed TLS configuration is composed of settings in this Silicon Labs specific CRYPTO hardware acceleration file located in mbedtls/configs and the mbed TLS configuration file in mbedtls/include/mbedtls/config.h.
 *  This configuration can be used as a starting point to evaluate hardware acceleration available on Silicon Labs devices.
 *
 * @{
 */

#ifndef MBEDTLS_CONFIG_SL_CRYPTO_ALL_ACCELERATION_H
#define MBEDTLS_CONFIG_SL_CRYPTO_ALL_ACCELERATION_H

#include "em_device.h"

#if !defined(NO_CRYPTO_ACCELERATION)
/**
 * @name SECTION: Silicon Labs Acceleration settings
 *
 * This section sets Silicon Labs Acceleration settings.
 * @{

 */

/**
 * \def MBEDTLS_AES_ALT
 *
 * Enable hardware acceleration for the AES block cipher
 *
 * Module:  sl_crypto/src/sl_aes.c
 *          or
 *          sl_crypto/src/slcl_aes.c if MBEDTLS_SLCL_PLUGINS is defined.
 *
 * See MBEDTLS_AES_C for more information.
 */
#define MBEDTLS_AES_ALT

/**
 * \def MBEDTLS_ECP_DEVICE_ALT
 * \def MBEDTLS_ECP_DEVICE_ALT
 * \def MBEDTLS_ECP_DOUBLE_JAC_ALT
 * \def MBEDTLS_ECP_DEVICE_ADD_MIXED_ALT
 * \def MBEDTLS_ECP_NORMALIZE_JAC_ALT
 * \def MBEDTLS_ECP_NORMALIZE_JAC_MANY_ALT
 * \def MBEDTLS_MPI_MODULAR_DIVISION_ALT
 *
 * Enable hardware acceleration for the elliptic curve over GF(p) library.
 *
 * Module:  sl_crypto/src/sl_ecp.c
 *          or
 *          sl_crypto/src/slcl_ecp.c if MBEDTLS_SLCL_PLUGINS is defined.
 * Caller:  library/ecp.c
 *          library/ecdh.c
 *          library/ecdsa.c
 *          library/ecjpake.c
 *
 * Requires: MBEDTLS_BIGNUM_C, MBEDTLS_ECP_C and at least one
 * MBEDTLS_ECP_DP_XXX_ENABLED and (CRYPTO_COUNT > 0)
 */
#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
#define MBEDTLS_ECP_INTERNAL_ALT
#define ECP_SHORTWEIERSTRASS
#define MBEDTLS_ECP_ADD_MIXED_ALT
#define MBEDTLS_ECP_DOUBLE_JAC_ALT
#define MBEDTLS_ECP_NORMALIZE_JAC_MANY_ALT
#define MBEDTLS_ECP_NORMALIZE_JAC_ALT
#define MBEDTLS_ECP_RANDOMIZE_JAC_ALT
#endif

/**
 * \def MBEDTLS_SHA1_ALT
 *
 * Enable hardware acceleration for the SHA1 cryptographic hash algorithm.
 *
 * Module:  sl_crypto/src/crypto_sha.c
 * Caller:  library/mbedtls_md.c
 *          library/ssl_cli.c
 *          library/ssl_srv.c
 *          library/ssl_tls.c
 *          library/x509write_crt.c
 *
 * Requires: MBEDTLS_SHA1_C and (CRYPTO_COUNT > 0)
 * See MBEDTLS_SHA1_C for more information.
 */
#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
#define MBEDTLS_SHA1_ALT
#endif

/**
 * \def MBEDTLS_SHA256_ALT
 *
 * Enable hardware acceleration for the SHA-224 and SHA-256 cryptographic
 * hash algorithms.
 *
 * Module:  sl_crypto/src/sl_sha256.c
 *          or
 *          sl_crypto/src/slcl_sha256.c if MBEDTLS_SLCL_PLUGINS is defined.
 * Caller:  library/entropy.c
 *          library/mbedtls_md.c
 *          library/ssl_cli.c
 *          library/ssl_srv.c
 *          library/ssl_tls.c
 *
 * Requires: MBEDTLS_SHA256_C and (CRYPTO_COUNT > 0)
 * See MBEDTLS_SHA256_C for more information.
 */
#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
#define MBEDTLS_SHA256_ALT
#endif

#endif /* #if !defined(NO_CRYPTO_ACCELERATION) */

/**
 * \def MBEDTLS_TRNG_C
 *
 * Enable software support for the True Random Number Generator (TRNG)
 * incorporated from Series 1 Configuration 2 devices (EFR32MG12, etc.)
 * from Silicon Labs.
 *
 * Requires TRNG_COUNT>0
 */
#if defined(TRNG_COUNT) && (TRNG_COUNT > 0)
#define MBEDTLS_TRNG_C
#endif

/**
 * \def MBEDTLS_ENTROPY_ALT
 * \def MBEDTLS_ENTROPY_INIT_ALT
 * \def MBEDTLS_ENTROPY_FREE_ALT
 *
 * Enable the TRNG as an entropy source which can serve the entropy module
 * of mbedtls with random data.
 *
 * Requires TRNG_COUNT>0 and MBEDTLS_TRNG_C.
 */
#if defined(TRNG_COUNT) && (TRNG_COUNT > 0) && defined(MBEDTLS_TRNG_C)
#define MBEDTLS_ENTROPY_HARDWARE_ALT
#endif

/**
 * \def MBEDTLS_NO_PLATFORM_ENTROPY
 *
 * Do not use built-in platform entropy functions.
 * This is useful if your platform does not support
 * standards like the /dev/urandom or Windows CryptoAPI.
 *
 * Uncomment this macro to disable the built-in platform entropy functions.
 */
#define MBEDTLS_NO_PLATFORM_ENTROPY

/* Default ECC configuration for Silicon Labs devices: */

/* ECC curves supported by CRYPTO hardware module: */
#define MBEDTLS_ECP_DP_SECP192R1_ENABLED
#define MBEDTLS_ECP_DP_SECP224R1_ENABLED
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED

/* Save RAM by adjusting to our exact needs */
#define MBEDTLS_ECP_MAX_BITS   256
#ifndef MBEDTLS_MPI_MAX_SIZE
#define MBEDTLS_MPI_MAX_SIZE    32 // 384 bits is 48 bytes
#endif

/*
   Set MBEDTLS_ECP_WINDOW_SIZE to configure
   ECC point multiplication window size, see ecp.h:
   2 = Save RAM at the expense of speed
   3 = Improve speed at the expense of RAM
   4 = Optimize speed at the expense of RAM
 */
#define MBEDTLS_ECP_WINDOW_SIZE        3
#define MBEDTLS_ECP_FIXED_POINT_OPTIM  0

/* Include the default mbed TLS config file */
#include "mbedtls/config.h"

/* Undefine SHA-512 in order to use CRYPTO SHA-256 acceleration in
   the entropy accumulator. */
#undef MBEDTLS_SHA512_C
#undef MBEDTLS_ENTROPY_SHA512_ACCUMULATOR
#undef MBEDTLS_FS_IO
#undef MBEDTLS_TIMING_C

/* Exclude and/or change default config here. E.g.: */
//#undef MBEDTLS_ECP_DP_SECP384R1_ENABLED
//#undef MBEDTLS_ECP_DP_SECP521R1_ENABLED
//#undef MBEDTLS_ECP_DP_BP384R1_ENABLED
//#undef MBEDTLS_ECP_DP_BP512R1_ENABLED
//#undef MBEDTLS_SHA512_C

#include "mbedtls/check_config.h"

/** @} (end section sl_crypto_config) */
/** @} (end addtogroup sl_crypto_config) */

#endif /* MBEDTLS_CONFIG_SL_CRYPTO_ALL_ACCELERATION_H */
