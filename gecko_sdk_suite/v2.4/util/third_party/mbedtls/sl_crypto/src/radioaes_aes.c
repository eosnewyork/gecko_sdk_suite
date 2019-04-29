/*
 *  BLE-specific cipher implementations optimized for Silicon Labs devices
 *  with a Radio AES peripheral.
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

#include "radioaes_aes.h"
#include "em_device.h"

#if defined(RADIOAES_PRESENT)
#include "radioaes.h"

#include <stddef.h>
#include <stdbool.h>

#define AES_BLOCK_BYTES           16
#define RADIOAES_CONFIG_BYTES      4

int radioaes_aes_crypt_ecb_block(bool                   encrypt,
                                 const unsigned char   *key,
                                 unsigned int           keybits,
                                 const unsigned char    input[16],
                                 volatile unsigned char output[16])
{
  volatile uint32_t aesConfig = AES_MODEID_ECB | AES_MODEID_NO_CX;

  switch (keybits) {
    case 256:
      aesConfig = AES_MODEID_ECB | AES_MODEID_NO_CX | AES_MODEID_AES256;
      break;
    case 192:
      aesConfig = AES_MODEID_ECB | AES_MODEID_NO_CX | AES_MODEID_AES192;
      break;
    case 128:
    default:
      aesConfig = AES_MODEID_ECB | AES_MODEID_NO_CX | AES_MODEID_AES128;
      break;
  }

  aesConfig |= encrypt ? AES_MODEID_ENCRYPT : AES_MODEID_DECRYPT;

  // Clock for RADIOAES enabled by RADIOCLK enable bit
  CMU->RADIOCLKCTRL = CMU_RADIOCLKCTRL_EN;

  while (RADIOAES->STATUS & AES_STATUS_FETCHERBSY) ;

  // Configuration
  RADIOAES->FETCHADDR = (uint32_t) &aesConfig;
  RADIOAES->FETCHLEN  = RADIOAES_CONFIG_BYTES;
  RADIOAES->FETCHTAG  = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_CFG);
  RADIOAES->CMD = AES_CMD_STARTFETCHER;

  while (RADIOAES->STATUS & AES_STATUS_FETCHERBSY) ;

  // Key
  RADIOAES->FETCHADDR = (uint32_t) key;
  RADIOAES->FETCHLEN  = (keybits / 8) | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS);
  RADIOAES->FETCHTAG  = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_KEY);
  RADIOAES->CMD = AES_CMD_STARTFETCHER;

  while(RADIOAES->STATUS & AES_STATUS_PUSHERBSY);

  // Output
  RADIOAES->PUSHADDR = (uint32_t) output;
  RADIOAES->PUSHLEN  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS);
  RADIOAES->CMD = AES_CMD_STARTPUSHER;

  while (RADIOAES->STATUS & AES_STATUS_FETCHERBSY) ;

  // Input
  RADIOAES->FETCHADDR = (uint32_t) input;
  RADIOAES->FETCHLEN  = AES_BLOCK_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS);
  RADIOAES->FETCHTAG  = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISLAST | DMA_SG_TAG_ISDATA | DMA_SG_TAG_DATATYPE_AESPAYLOAD;
  RADIOAES->CMD = AES_CMD_STARTFETCHER;

  while (RADIOAES->STATUS & AES_STATUS_PUSHERBSY) ;

  return 0;
}

#endif //defined(RADIOAES_PRESENT)
