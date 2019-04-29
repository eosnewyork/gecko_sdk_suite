/***************************************************************************//**
 * @brief Implements overrides of weakly defined internal stack APIs to
 * facilitate stack AES operations through mbedtls.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories, Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of  Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software  is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include PLATFORM_HEADER
#include "ember.h"
#include "hal/hal.h"
#include "mbedtls/aes.h"

static mbedtls_aes_context aesContext;

void emLoadKeyIntoCore(const uint8_t* key)
{
  mbedtls_aes_setkey_enc(&aesContext,
                         key,
                         SECURITY_BLOCK_SIZE * 8U);
}

void emSecurityHardwareInit(void)
{
  mbedtls_aes_init(&aesContext);
}

void emStandAloneEncryptBlock(uint8_t* block)
{
  // Encrypt this block in place with the current key
  mbedtls_aes_crypt_ecb(&aesContext,
                        MBEDTLS_AES_ENCRYPT,
                        block,
                        block);
}

void emAesEncrypt(uint8_t* block, const uint8_t* key)
{
  mbedtls_aes_setkey_enc(&aesContext,
                         key,
                         SECURITY_BLOCK_SIZE * 8U);

  mbedtls_aes_crypt_ecb(&aesContext,
                        MBEDTLS_AES_ENCRYPT,
                        block,
                        block);
}
