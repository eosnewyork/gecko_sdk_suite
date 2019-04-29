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

#include "crypto_ble.h"

#include "em_device.h"

#if defined(RADIOAES_PRESENT)
#include "radioaes.h"

#include <stddef.h>
#include <stdbool.h>

#define AES_BLOCK_BYTES       16U

#define BLE_CCM_NONCE_BYTES   13U
#define BLE_CCM_KEY_BYTES     16U
#define BLE_CCM_B_BYTES       16U
#define BLE_CCM_TAG_BYTES      4U
#define BLE_CCM_VER_BYTES      1U
#define BLE_CCM_B0_FLAGS    0x49U
#define BLE_CCM_AUTH_BLOCKS    1U

#define BLE_RPA_KEY_BYTES     16U
#define BLE_RPA_DATA_BYTES    16U

#define RADIOAES_CONFIG_BYTES  4U

#ifndef RADIOAES_BLE_RPA_MAX_KEYS
#define RADIOAES_BLE_RPA_MAX_KEYS 32
#endif

#define RADIOAES_DESC_FETCHERS_RAM 7
#define RADIOAES_DESC_PUSHERS_RAM  4

// Local CCM variables
static volatile uint32_t aesCcmConfig = AES_MODEID_CCM
                                     | AES_MODEID_NO_CX
                                     | AES_MODEID_AES128;
static volatile uint8_t verFailed;
static const uint32_t zeros = 0;
static volatile uint8_t b0b1[BLE_CCM_B_BYTES * 2] = { 0 };

// FETCHERS
static const struct radioaesDmaSgDescr ccmDescFetcherTagPadding =
{
  .address       = (uint32_t) &zeros,
  .nextDescr     = DMA_AXI_DESCR_NEXT_STOP,
  .lengthAndIrq  = (AES_BLOCK_BYTES - BLE_CCM_TAG_BYTES) | (BLOCK_S_CONST_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
  .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA | DMA_SG_TAG_ISLAST | DMA_SG_TAG_DATATYPE_AESPAYLOAD | DMA_SG_TAG_SETINVALIDBYTES(AES_BLOCK_BYTES - BLE_CCM_TAG_BYTES)
};

static volatile struct radioaesDmaSgDescr ccmDescFetcherTag =
{
  .address       = (uint32_t) NULL,// Written in function
  .nextDescr     = (uint32_t) &ccmDescFetcherTagPadding,
  .lengthAndIrq  = BLE_CCM_TAG_BYTES,
  .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA | DMA_SG_TAG_DATATYPE_AESPAYLOAD
};

static volatile struct radioaesDmaSgDescr ccmDescFetcherDataPadding =
{
  .address       = (uint32_t) &zeros,
  .nextDescr     = (uint32_t) NULL,  // Written in function
  .lengthAndIrq  = (uint32_t) NULL,  // Written in function
  .tag           = (uint32_t) NULL,  // Written in function
};

static volatile struct radioaesDmaSgDescr ccmDescFetcherData =
{
  .address       = (uint32_t) NULL, // Written in function
  .nextDescr     = (uint32_t) NULL, // Written in function
  .lengthAndIrq  = (uint32_t) NULL, // Written in function
  .tag           = (uint32_t) NULL, // Written in function
};

static const struct radioaesDmaSgDescr ccmDescFetcherB0B1 =
{
  .address       = (uint32_t) b0b1,
  .nextDescr     = (uint32_t) &ccmDescFetcherData,
  .lengthAndIrq  = BLE_CCM_B_BYTES * 2 | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
  .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA | DMA_SG_TAG_DATATYPE_AESHEADER
};

static volatile struct radioaesDmaSgDescr ccmDescFetcherKey =
{
  .address       = (uint32_t) NULL,// Written in function
  .nextDescr     = (uint32_t) &ccmDescFetcherB0B1,
  .lengthAndIrq  = BLE_CCM_KEY_BYTES | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS),
  .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_KEY)
};

static const struct radioaesDmaSgDescr ccmDescFetcherConfig =
{
  .address       = (uint32_t) &aesCcmConfig,
  .nextDescr     = (uint32_t) &ccmDescFetcherKey,
  .lengthAndIrq  = RADIOAES_CONFIG_BYTES,
  .tag           = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_CFG)
};

// PUSHERS
static const struct radioaesDmaSgDescr ccmDescPusherVerPadding =
{
  .address       = (uint32_t) NULL,
  .nextDescr     = DMA_AXI_DESCR_NEXT_STOP,
  .lengthAndIrq  = (AES_BLOCK_BYTES - BLE_CCM_VER_BYTES) | DMA_AXI_DESCR_DISCARD
};

static const struct radioaesDmaSgDescr ccmDescPusherVer =
{
  .address       = (uint32_t) &verFailed,
  .nextDescr     = (uint32_t) &ccmDescPusherVerPadding,
  .lengthAndIrq  = BLE_CCM_VER_BYTES
};

static const struct radioaesDmaSgDescr ccmDescPusherTagPadding =
{
  .address       = (uint32_t) NULL,
  .nextDescr     = DMA_AXI_DESCR_NEXT_STOP,
  .lengthAndIrq  = (AES_BLOCK_BYTES - BLE_CCM_TAG_BYTES) | DMA_AXI_DESCR_DISCARD
};

static volatile struct radioaesDmaSgDescr ccmDescPusherTag =
{
  .address       = (uint32_t) NULL,// Written in function
  .nextDescr     = (uint32_t) &ccmDescPusherTagPadding,
  .lengthAndIrq  = BLE_CCM_TAG_BYTES
};

static volatile struct radioaesDmaSgDescr ccmDescPusherDataPadding =
{
  .address       = (uint32_t) NULL,
  .nextDescr     = (uint32_t) NULL,// Written in function
  .lengthAndIrq  = (uint32_t) NULL,// Written in function
};

static volatile struct radioaesDmaSgDescr ccmDescPusherData =
{
  .address       = (uint32_t) NULL,// Written in function
  .nextDescr     = (uint32_t) NULL,// Written in function
  .lengthAndIrq  = (uint32_t) NULL,// Written in function
};

static const struct radioaesDmaSgDescr ccmDescPusherB0B1 =
{
  .address       = (uint32_t) NULL,
  .nextDescr     = (uint32_t) &ccmDescPusherData,
  .lengthAndIrq  = (BLE_CCM_B_BYTES * 2) | DMA_AXI_DESCR_DISCARD
};

static int aes_ccm_ble(bool                 encrypt,
                       unsigned char       *data,
                       size_t               length,
                       const unsigned char *key,
                       const unsigned char *iv,
                       unsigned char        header,
                       unsigned char       *tag)

{
  uint32_t i;

  uint8_t dataPadBytes = AES_BLOCK_BYTES - 1 - ((length - 1) % AES_BLOCK_BYTES);

  // Modify AES configuration according to mode
  aesCcmConfig = (aesCcmConfig & ~AES_MODEID_DECRYPT) | (encrypt ? AES_MODEID_ENCRYPT : AES_MODEID_DECRYPT);

  // Modifying fetchers
  ccmDescFetcherKey.address       = (uint32_t) key;
  ccmDescFetcherData.address      = (uint32_t) data;
  ccmDescFetcherTag.address       = (uint32_t) tag;
  ccmDescFetcherData.lengthAndIrq = length | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS);

  if (dataPadBytes) {
    ccmDescFetcherData.nextDescr            = (uint32_t) &ccmDescFetcherDataPadding;
    ccmDescFetcherDataPadding.lengthAndIrq  = dataPadBytes | (BLOCK_S_CONST_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS);
    ccmDescFetcherData.tag         = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA | DMA_SG_TAG_DATATYPE_AESPAYLOAD;
    if (encrypt) {
      ccmDescFetcherDataPadding.nextDescr   = DMA_AXI_DESCR_NEXT_STOP;
      ccmDescFetcherDataPadding.tag         = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA | DMA_SG_TAG_ISLAST | DMA_SG_TAG_DATATYPE_AESPAYLOAD | DMA_SG_TAG_SETINVALIDBYTES(dataPadBytes);
    } else {
      ccmDescFetcherDataPadding.nextDescr   = (uint32_t) &ccmDescFetcherTag;
      ccmDescFetcherDataPadding.tag         = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA | DMA_SG_TAG_DATATYPE_AESPAYLOAD | DMA_SG_TAG_SETINVALIDBYTES(dataPadBytes);
    }
  } else {
    if (encrypt) {
      ccmDescFetcherData.nextDescr   = DMA_AXI_DESCR_NEXT_STOP;
      ccmDescFetcherData.tag         = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA | DMA_SG_TAG_ISLAST | DMA_SG_TAG_DATATYPE_AESPAYLOAD;
    } else {
      ccmDescFetcherData.nextDescr   = (uint32_t) &ccmDescFetcherTag;
      ccmDescFetcherData.tag         = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA | DMA_SG_TAG_DATATYPE_AESPAYLOAD;
    } 
  }

  // Modifying pushers
  ccmDescPusherData.address              = (uint32_t) data;
  ccmDescPusherTag.address               = (uint32_t) tag;
  ccmDescPusherData.lengthAndIrq         = length | (BLOCK_S_INCR_ADDR & BLOCK_S_FLAG_MASK_DMA_PROPS);

  if (dataPadBytes){
    ccmDescPusherData.nextDescr     = (uint32_t) &ccmDescPusherDataPadding;
    ccmDescPusherDataPadding.lengthAndIrq  = dataPadBytes | DMA_AXI_DESCR_DISCARD;
    if (encrypt) {
      ccmDescPusherDataPadding.nextDescr     = (uint32_t) &ccmDescPusherTag;
    } else {
      ccmDescPusherDataPadding.nextDescr     = (uint32_t) &ccmDescPusherVer;
    }
  } else {
    if (encrypt) {
      ccmDescPusherData.nextDescr     = (uint32_t) &ccmDescPusherTag;
    } else {
      ccmDescPusherData.nextDescr     = (uint32_t) &ccmDescPusherVer;
    }
  }

// Fill in B0 block according to BLE spec
  b0b1[0] = BLE_CCM_B0_FLAGS;

  for (i = 0; i < BLE_CCM_NONCE_BYTES; i++) {
    b0b1[i + 1] = iv[i];
  }

  b0b1[14] = (uint8_t) length >> 8;
  b0b1[15] = (uint8_t) length;

// Fill in B1 block according to BLE spec
  for (i = BLE_CCM_B_BYTES; i < BLE_CCM_B_BYTES * 2; i++) {
    b0b1[i] = 0;
  }
  b0b1[BLE_CCM_B_BYTES + 1] = BLE_CCM_AUTH_BLOCKS;
  b0b1[BLE_CCM_B_BYTES + 2] = header;

  CMU->RADIOCLKCTRL = CMU_RADIOCLKCTRL_EN;

  RADIOAES->CTRL       =  AES_CTRL_FETCHERSCATTERGATHER | AES_CTRL_PUSHERSCATTERGATHER;

  RADIOAES->FETCHDESCR = (uint32_t) &ccmDescFetcherConfig;
  RADIOAES->PUSHDESCR  = (uint32_t) &ccmDescPusherB0B1;

  RADIOAES->CMD =  AES_CMD_STARTPUSHER | AES_CMD_STARTFETCHER;
  while (RADIOAES->STATUS & (AES_STATUS_FETCHERBSY | AES_STATUS_PUSHERBSY)) ;

// Check MIC
  if (verFailed) {
    return MBEDTLS_ERR_CCM_AUTH_FAILED;
  }
  return 0;
}

/*
 * CCM buffer encryption optimized for BLE
 */
int mbedtls_ccm_encrypt_and_tag_ble(unsigned char       *data,
                                    size_t               length,
                                    const unsigned char *key,
                                    const unsigned char *iv,
                                    unsigned char        header,
                                    unsigned char       *tag)
{
  return aes_ccm_ble(true,
                     data,
                     length,
                     key,
                     iv,
                     header,
                     tag);
}

/*
 * CCM buffer authenticated decryption optimized for BLE
 */
int mbedtls_ccm_auth_decrypt_ble(unsigned char       *data,
                                 size_t               length,
                                 const unsigned char *key,
                                 const unsigned char *iv,
                                 unsigned char        header,
                                 unsigned char *tag)
{
  return aes_ccm_ble(false,
                     data,
                     length,
                     key,
                     iv,
                     header,
                     (uint8_t *) tag);
}

/*
 * Process a table of BLE RPA device keys and look for a
 * match against the supplied hash
 */
int mbedtls_process_ble_rpa(const unsigned char   keytable[],
                            uint32_t              keymask,
                            uint32_t              prand,
                            uint32_t              hash)
{
  int block;
  int previousBlock = -1;
  static const uint32_t  aesRpaConfig = AES_MODEID_ECB
                                      | AES_MODEID_NO_CX
                                      | AES_MODEID_AES128
                                      | AES_MODEID_ENCRYPT;

  uint32_t rpaDataIn[BLE_RPA_DATA_BYTES / 4] = { 0 };
  volatile uint32_t rpaDataOut[BLE_RPA_DATA_BYTES / 4];

  rpaDataIn[3] = __REV(prand);

  CMU->RADIOCLKCTRL = CMU_RADIOCLKCTRL_EN;

  // Configuration
  RADIOAES->FETCHADDR = (uint32_t) &aesRpaConfig;
  RADIOAES->FETCHLEN  = RADIOAES_CONFIG_BYTES;
  RADIOAES->FETCHTAG  = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_CFG);
  RADIOAES->CMD       = AES_CMD_STARTFETCHER;

  while (RADIOAES->STATUS & AES_STATUS_FETCHERBSY) ;

  RADIOAES->FETCHLEN = AES_BLOCK_BYTES;
  RADIOAES->PUSHLEN  = AES_BLOCK_BYTES;
  RADIOAES->PUSHADDR = (uint32_t) rpaDataOut;

  // Data output contains hash in the most significant word (WORD3).
  // Descriptors for blocks that are not included in key mask will be skipped.
  for (block = 0; block < RADIOAES_BLE_RPA_MAX_KEYS; block++) {
    if ( keymask & (1U << block) ) {  // Skip masked keys
      // Write key
      while (RADIOAES->STATUS & AES_STATUS_FETCHERBSY) ;
      RADIOAES->FETCHADDR = (uint32_t) &keytable[block * BLE_RPA_KEY_BYTES];
      RADIOAES->FETCHTAG  = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_KEY);
      RADIOAES->CMD       = AES_CMD_STARTFETCHER;

      // Write data
      while (RADIOAES->STATUS & AES_STATUS_FETCHERBSY) ;
      RADIOAES->FETCHADDR = (uint32_t) rpaDataIn;
      RADIOAES->FETCHTAG  = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA | DMA_SG_TAG_ISLAST | DMA_SG_TAG_DATATYPE_AESPAYLOAD;
      RADIOAES->CMD       = AES_CMD_STARTFETCHER;

      // Wait for pusher from previous round to finish
      while (RADIOAES->STATUS & AES_STATUS_PUSHERBSY) ;

      // Check previous results while AES is processing
      if ((previousBlock >= 0) && ((rpaDataOut[3] & 0xFFFFFF00) == __REV(hash)) ) {
        // Make sure AES is finished before returning
        RADIOAES->CMD = AES_CMD_STARTPUSHER;
        while (RADIOAES->STATUS & AES_STATUS_PUSHERBSY) ;
        return previousBlock;
      }

      // Start pusher so it is ready to push results when encryption is done
      RADIOAES->CMD = AES_CMD_STARTPUSHER;
      previousBlock = block;
    }
  }

  // Wait for last data and check it
  while (RADIOAES->STATUS & AES_STATUS_PUSHERBSY) ;

  if ((rpaDataOut[3] & 0xFFFFFF00) == __REV(hash) ) {
    return previousBlock;
  }

  // No match
  return -1;
}
#endif //defined(RADIOAES_PRESENT)
