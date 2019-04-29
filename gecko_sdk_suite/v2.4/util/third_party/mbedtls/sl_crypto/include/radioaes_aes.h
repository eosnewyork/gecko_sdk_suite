/*
 *  AES cipher implementations optimized for Silicon Labs devices with a Radio
 *  AES peripheral.
 *
 *  Copyright (C) 2017, Silicon Labs, http://www.silabs.com
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#ifndef RADIOAES_AES_H
#define RADIOAES_AES_H

/***************************************************************************//**
 * \addtogroup sl_radioaes
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_radioaes_aes Radio AES AES functions
 * \brief AES cryptographic primitives.
 * \{
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          AES-ECB block encryption/decryption
 *
 * \param encrypt  true for encryption, false for decryption
 * \param key      AES key
 * \param keybits  must be 128, 192 or 256
 * \param input    16-byte input block
 * \param output   16-byte output block
 *
 * \return         0 if successful
 */
int radioaes_aes_crypt_ecb_block(bool                   encrypt,
                                 const unsigned char    *key,
                                 unsigned int           keybits,
                                 const unsigned char    input[16],
                                 volatile unsigned char output[16]);

#ifdef __cplusplus
}
#endif

/** \} (end addtogroup sl_radioaes_aes) */
/** \} (end addtogroup sl_radioaes) */

#endif /* RADIOAES_AES_H */
