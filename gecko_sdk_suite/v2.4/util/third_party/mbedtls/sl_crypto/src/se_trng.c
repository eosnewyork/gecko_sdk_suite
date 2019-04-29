/*
 *  True Random Number Generator (TRNG) driver for Silicon Labs devices with
 *  Secure Element.
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

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_SELF_TEST) && defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf     printf
#endif

#include "em_device.h"
#include "em_se.h"

#if defined(SEMAILBOX_PRESENT)

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)

#include "mbedtls/entropy.h"

int mbedtls_hardware_poll( void *data,
                           unsigned char *output,
                           size_t len,
                           size_t *olen ) {
    SE_Response_t ret = SE_RESPONSE_OK;
    size_t surplus_bytes = 0;
    uint32_t surplus_word = 0;
    (void) data;

    surplus_bytes = len & 0x3;
    len &= ~0x3;

    if ( len > 0 ) {
      SE_Command_t command = SE_COMMAND_DEFAULT(SE_COMMAND_TRNG_GET_RANDOM);
      SE_DataTransfer_t data_out = SE_DATATRANSFER_DEFAULT(output, len);

      SE_addDataOutput(&command, &data_out);
      SE_addParameter(&command, len);

      SE_executeCommand(&command);
      ret = SE_readCommandResponse();

      if ( ret == SE_RESPONSE_OK ) {
        *olen = len;
      }
    }

    if ( surplus_bytes != 0 ) {
      SE_Command_t command2 = SE_COMMAND_DEFAULT(SE_COMMAND_TRNG_GET_RANDOM);
      SE_DataTransfer_t word_out = SE_DATATRANSFER_DEFAULT(&surplus_word, 4);

      SE_addDataOutput(&command2, &word_out);
      SE_addParameter(&command2, 4);

      SE_executeCommand(&command2);
      ret = SE_readCommandResponse();
      if ( ret == SE_RESPONSE_OK ) {
        for ( size_t i = 0; i < surplus_bytes; i++ ) {
          output[len+i] = (surplus_word >> (i*8)) & 0xFF;
        }

        *olen = len + surplus_bytes;
      }
    }

    if (ret == SE_RESPONSE_OK) {
      return 0;
    } else {
      return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    }
}

#endif /* MBEDTLS_ENTROPY_HARDWARE_ALT */

#endif /* SEMAILBOX_PRESENT */
