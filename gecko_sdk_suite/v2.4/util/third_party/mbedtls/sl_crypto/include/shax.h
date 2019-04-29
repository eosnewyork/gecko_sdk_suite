/**
 * \file shax.h
 *
 * \brief Generic SHA functionality
 *
 *  Copyright (C) 2015-2017, Silicon Labs, http://www.silabs.com
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
#ifndef SHAX_H
#define SHAX_H

#if (defined(MBEDTLS_SHA256_ALT) && defined(MBEDTLS_SHA256_C)) \
    || (defined(MBEDTLS_SHA1_ALT) && defined(MBEDTLS_SHA1_C)) \
    || (defined(MBEDTLS_SHA512_ALT) && defined(MBEDTLS_SHA512_C))

typedef enum {
#if defined(MBEDTLS_SHA1_C)
    SHA1,
#endif
#if defined(MBEDTLS_SHA256_C)
    SHA224,
    SHA256,
#endif
#if defined(MBEDTLS_SHA512_C)
    SHA384,
    SHA512,
#endif
} SHA_Type_t;

/**
 * \brief                   Process (a) block(s) of data to be hashed
 *
 * \note                    Watch the blocksize! Blocks are 64 bytes for SHA-1 through
 *                          SHA-256, and 128 bytes for SHA-384 through SHA-512.
 * \note                    Watch the state size! State size is half the block size.
 *
 * \param algo              Which hashing algorithm to use
 * \param[in] state_in      Previous state of the hashing algorithm
 * \param[in] blockdata     Pointer to the block(s) of data
 * \param[out] state_out    Pointer to block of memory to store state
 * \param num_blocks        Number of SHA blocks in data block
 */
int sha_x_process(SHA_Type_t algo,
                  uint8_t* state_in,
                  const unsigned char *blockdata,
                  uint8_t* state_out,
                  uint32_t num_blocks);

/**
 * \brief                   Process an arbitrary number of bytes to be hashed
 *
 * \note                    Watch the blocksize! Blocks are 64 bytes for SHA-1 through
 *                          SHA-256, and 128 bytes for SHA-384 through SHA-512.
 * \note                    Watch the state size! State size is half the block size.
 * \note                    Watch the counter size! Counter is 64 bytes for SHA-1 through
 *                          SHA-256, and 128 bytes for SHA-384 through SHA-512.
 *
 * \param algo              Which hashing algorithm to use
 * \param[inout] state      Pointer to the hashing algorithm's state buffer
 * \param[in] data          Pointer to the data to add to the hash
 * \param[inout] buffer     Pointer to a block buffer to retrieve/store a partial block
 *                          in between calls to this function. Needs to have a size equal
 *                          to the block size.
 * \param[inout] counter    Counter variable keeping track of the amount of bytes hashed,
 *                          to later be used for hash finalization. For first use, initialize
 *                          with zeroes.
 */
int sha_x_update(SHA_Type_t algo,
                 const unsigned char *data,
                 size_t data_len,
                 uint8_t* state,
                 unsigned char *buffer,
                 uint32_t* counter);

/**
 * \brief                   Process an arbitrary number of bytes to be hashed
 *
 * \note                    Watch the blocksize! Blocks are 64 bytes for SHA-1 through
 *                          SHA-256, and 128 bytes for SHA-384 through SHA-512.
 * \note                    Watch the state size! State size is half the block size.
 * \note                    Watch the counter size! Counter is 64 bytes for SHA-1 through
 *                          SHA-256, and 128 bytes for SHA-384 through SHA-512.
 *
 * \param algo              Which hashing algorithm to use
 * \param[in] state         Pointer to the hashing algorithm's state buffer
 * \param[in] buffer        Pointer to a block buffer to retrieve/store a partial block
 *                          in between calls to this function. Needs to have a size equal
 *                          to the block size.
 * \param[inout] counter    Counter variable keeping track of the amount of bytes hashed,
 *                          to later be used for hash finalization. For first use, initialize
 *                          with zeroes.
 * \param[out] output       Pointer to the destination of the hash.
 */
int sha_x_finish(SHA_Type_t algo,
                 uint8_t* state,
                 uint8_t* buffer,
                 uint32_t* counter,
                 uint8_t* output);

#ifdef __cplusplus
}
#endif

#endif /* if any of the mbedTLS SHA accelerations are active */

#endif /* #ifndef SHAX_H */
