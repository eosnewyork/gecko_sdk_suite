/***************************************************************************//**
 * @file
 * @brief A demo that uses NVM3 to store random numbers.
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

#include <stdio.h>
#include <string.h>

#include "em_device.h"
#include "em_common.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "bsp.h"
#include "bsp_trace.h"

#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"

#include "nvm3.h"
#include "nvm3_hal_flash.h"
#include "rtcdriver.h"

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

/*******************************************************************************
   Functions and config
 ******************************************************************************/

/* Declare NVM area and cache */
NVM3_DEFINE_SECTION_STATIC_DATA(nvm3Data, 6 * FLASH_PAGE_SIZE, 100);

/* Print error and set both LEDs. */
void reportError(const char * errorMsg)
{
  printf("\f");
  printf(errorMsg);
  BSP_LedToggle(0);
  BSP_LedToggle(1);
  while (true) {
  }
}

/* Check and report NVM3 errors */
void checkNvm3Status(Ecode_t nvm3status, int line)
{
  char errorMessage[128];
  if (nvm3status != ECODE_NVM3_OK) {
    snprintf(errorMessage, sizeof(errorMessage), "NVM3 error: %x on line %d", (int)nvm3status, line);
    reportError((const char *)&errorMessage);
  }
}

/* Generate random data with mbedTLS RNG and write to NVM3 */
void genRngObjects(nvm3_Handle_t *nvm3Handle)
{
  /* Sufficient object count and size to fill the display */
  #define RNG_OBJ_BYTE_CNT        17
  #define RNG_OBJ_CNT             16

  static bool mbedtlsInit = false;
  static mbedtls_entropy_context entropyCtx;
  static mbedtls_ctr_drbg_context drbgCtx;
  static const char pers[] = "persistent_trng";
  int mbedtlsStatus;

  unsigned char rngDataLen[RNG_OBJ_CNT];
  unsigned char rngData[RNG_OBJ_BYTE_CNT + 1];
  uint32_t objKey;
  uint32_t i;

  /* Initialize mbedTLS RNG module for random number gen */
  if (!mbedtlsInit) {
    mbedtls_entropy_init(&entropyCtx);
    mbedtlsStatus = mbedtls_ctr_drbg_seed(&drbgCtx, mbedtls_entropy_func, &entropyCtx,
                                          (const unsigned char *) pers,
                                          sizeof pers);
    if (mbedtlsStatus != 0) {
      reportError("mbedTLS failed to initialize");
    }
    mbedtlsInit = true;
  }

  /* Get random object sizes */
  checkNvm3Status(mbedtls_ctr_drbg_random(&drbgCtx, (unsigned char *)rngDataLen, RNG_OBJ_CNT), __LINE__);

  for (i = 0; i < RNG_OBJ_CNT; i++) {
    /* At least one random byte  */
    rngDataLen[i] = (rngDataLen[i] % RNG_OBJ_BYTE_CNT) + 1;
  }

  /* For each object key */
  for (objKey = 0; objKey < RNG_OBJ_CNT; objKey++) {
    /* Get random object data */
    mbedtlsStatus = mbedtls_ctr_drbg_random(&drbgCtx, (unsigned char *)rngData, rngDataLen[objKey]);
    if (mbedtlsStatus != 0) {
      while (true) ;
    }

    /* Convert to printf-able string of random decimals */
    for (i = 0; i < rngDataLen[objKey]; i++) {
      rngData[i] = (rngData[i] % 10) + 0x30;
    }
    rngData[rngDataLen[objKey]] = '\0';

    /* Write to NVM3 */
    checkNvm3Status(nvm3_writeData(nvm3Handle, objKey, &rngData, rngDataLen[objKey] + 1), __LINE__);
  }
}

/* Print data to display */
void displayRandomObjects(nvm3_Handle_t *nvm3Handle)
{
  size_t dataLen;
  uint32_t keyCnt;
  /* Some sufficient sizes. Actual sizes are acquired from NVM3 and checked against available buffer size. */
  unsigned char data[30];
  nvm3_ObjectKey_t keys[20];
  uint32_t objectType;
  uint32_t i;

  /* Clear display */
  printf("\f");

  /* Get number of keys/objects and a list of keys */
  keyCnt = nvm3_enumObjects(nvm3Handle, (uint32_t *)&keys, sizeof(keys) / sizeof(keys[0]), NVM3_KEY_MIN, NVM3_KEY_MAX);

  if (keyCnt > sizeof(keys)) {
    reportError("keyCnt > sizeof(keys)");
  }

  /* For each key */
  for (i = 0; i < keyCnt; i++) {
    /* Read from NVM3 and display */
    checkNvm3Status(nvm3_getObjectInfo(nvm3Handle, keys[i], &objectType, &dataLen), __LINE__);
    if (objectType != NVM3_OBJECTTYPE_DATA) {
      reportError("Not data type");
    }
    if (dataLen > sizeof(data)) {
      reportError("dataLen > sizeof(data)");
    }
    checkNvm3Status(nvm3_readData(nvm3Handle, keys[i], &data, dataLen), __LINE__);

    /* Write object to display */
    printf(">%u:", (unsigned int)keys[i]);
    printf((const char *)data);
    /* Don't overflow display */
    if (i < (keyCnt - 1)) {
      printf("\n");
    }
  }
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  NVM3_DEFINE_SECTION_INIT_DATA(nvm3Data, &nvm3_halFlashHandle);
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;

  static nvm3_Handle_t nvm3Handle;
  RTCDRV_TimerID_t rtcId;

  /* Chip errata */
  CHIP_Init();

  EMU_DCDCInit(&dcdcInit);
  CMU_HFXOInit(&hfxoInit);
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);
  em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;
  EMU_EM23Init(&em23Init);

  /* If first word of user data page is non-zero, enable Energy Profiler trace */
  BSP_TraceProfilerSetup();

  /* Initialize LED driver */
  BSP_LedsInit();
  BSP_LedClear(0);
  BSP_LedClear(1);

  /* Initialize the display module and redirect stdio. Clear display. */
  DISPLAY_Init();
  if (RETARGET_TextDisplayInit() != TEXTDISPLAY_EMSTATUS_OK) {
    reportError("RETARGET_TextDisplayInit failed");
  }
  printf("\f");

  /* Initialize RTCDRV */
  RTCDRV_Init();
  RTCDRV_AllocateTimer(&rtcId);

  /* Open NVM3 instance */
  checkNvm3Status(nvm3_open(&nvm3Handle, &nvm3Data), __LINE__);

  /* Do we have any valid keys? Generate if not. */
  if (nvm3_countObjects(&nvm3Handle) == 0) {
    genRngObjects(&nvm3Handle);
  }

  /* Display, generate and store in NVM3 forever */
  while (true) {
    /* Read objects from NVM3 and display */
    displayRandomObjects(&nvm3Handle);

    /* Do housekeeping if needed */
    if (nvm3_repackNeeded(&nvm3Handle)) {
      checkNvm3Status(nvm3_repack(&nvm3Handle), __LINE__);
    }

    /* Set wakeup timer and go to sleep */
    RTCDRV_StartTimer(rtcId, rtcdrvTimerTypeOneshot, 2000 /* ms */, NULL, NULL);
    EMU_EnterEM2(true);

    /* Generate new objects and write to NVM3 */
    genRngObjects(&nvm3Handle);
  }
}
