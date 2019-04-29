/*
 *  FIPS-180-2 compliant SHA-1 & SHA-256 implementation
 *
 *  Copyright (C) 2016, Silicon Labs, http://www.silabs.com
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
/*
 * This file includes an alternative implementation of the standard
 * mbedtls/libary/sha[1][256].c using the secure element
 * incorporated in MCU devices from Silicon Laboratories.
 */
/*
 *  The SHA-1 standard was published by NIST in 1993.
 *
 *  http://www.itl.nist.gov/fipspubs/fip180-1.htm
 *
 *  The SHA-256 Secure Hash Standard was published by NIST in 2002.
 *
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"
#include "mbedtls/md.h"

#if (defined(MBEDTLS_SHA256_ALT) && defined(MBEDTLS_SHA256_C)) \
    || (defined(MBEDTLS_SHA1_ALT) && defined(MBEDTLS_SHA1_C)) \
    || (defined(MBEDTLS_SHA512_ALT) && defined(MBEDTLS_SHA512_C))

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT)
#include "em_se.h"
#include "shax.h"

int sha_x_process(SHA_Type_t algo,
                  uint8_t* state_in,
                  const unsigned char *blockdata,
                  uint8_t* state_out,
                  uint32_t num_blocks)
{
    SE_Command_t command = SE_COMMAND_DEFAULT(SE_COMMAND_HASHUPDATE);
    SE_DataTransfer_t data_in = SE_DATATRANSFER_DEFAULT((void *)blockdata, 0);
    SE_DataTransfer_t iv_in = SE_DATATRANSFER_DEFAULT(state_in, 0);
    SE_DataTransfer_t iv_out = SE_DATATRANSFER_DEFAULT(state_out, 0);

    switch(algo) {
#if defined(MBEDTLS_SHA1_ALT) && defined(MBEDTLS_SHA1_C)
        case SHA1:
            command.command |= SE_COMMAND_OPTION_HASH_SHA1;
            /* SHA1 block size is 64 bytes */
            SE_addParameter(&command, 64 * num_blocks);
            data_in.length |= 64 * num_blocks;
            /* SHA1 state size is 20 bytes */
            iv_in.length |= 20;
            iv_out.length |= 20;
            break;
#endif
#if defined(MBEDTLS_SHA256_ALT) && defined(MBEDTLS_SHA256_C)
        case SHA224:
            command.command |= SE_COMMAND_OPTION_HASH_SHA224;
            /* SHA224 block size is 64 bytes */
            SE_addParameter(&command, 64 * num_blocks);
            data_in.length |= 64 * num_blocks;
            /* SHA224 state size is 32 bytes */
            iv_in.length |= 32;
            iv_out.length |= 32;
            break;
        case SHA256:
            command.command |= SE_COMMAND_OPTION_HASH_SHA256;
            /* SHA256 block size is 64 bytes */
            SE_addParameter(&command, 64 * num_blocks);
            data_in.length |= 64 * num_blocks;
            /* SHA256 state size is 32 bytes */
            iv_in.length |= 32;
            iv_out.length |= 32;
            break;
#endif
#if defined(MBEDTLS_SHA512_ALT) && defined(MBEDTLS_SHA512_C)
        case SHA384:
            command.command |= SE_COMMAND_OPTION_HASH_SHA384;
            /* SHA384 block size is 128 bytes */
            SE_addParameter(&command, 128 * num_blocks);
            data_in.length |= 128 * num_blocks;
            /* SHA384 state size is 64 bytes */
            iv_in.length |= 64;
            iv_out.length |= 64;
            break;
        case SHA512:
            command.command |= SE_COMMAND_OPTION_HASH_SHA512;
            /* SHA512 block size is 128 bytes */
            SE_addParameter(&command, 128 * num_blocks);
            data_in.length |= 128 * num_blocks;
            /* SHA512 state size is 64 bytes */
            iv_in.length |= 64;
            iv_out.length |= 64;
            break;
#endif
    }

    SE_addDataInput(&command, &iv_in);
    SE_addDataInput(&command, &data_in);
    SE_addDataOutput(&command, &iv_out);

    SE_executeCommand(&command);
    SE_Response_t res = SE_readCommandResponse();
    if ( res == SE_RESPONSE_OK ) {
        return 0;
    } else {
        return MBEDTLS_ERR_MD_HW_ACCEL_FAILED;
    }
}

#endif /* #if defined(SEMAILBOX_PRESENT) */

#endif /* #if defined(MBEDTLS_SHA256_PROCESS_ALT) || defined(MBEDTLS_SHA1_PROCESS_ALT) || defined(MBEDTLS_SHA512_PROCESS_ALT) */
