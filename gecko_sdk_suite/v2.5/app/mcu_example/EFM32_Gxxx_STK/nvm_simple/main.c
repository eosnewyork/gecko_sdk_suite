/***************************************************************************//**
 * @file
 * @brief Non-Volatile Memory Driver use example.
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

#include <stdlib.h>
#include <stdbool.h>

#include "em_device.h"

#include "bsp.h"
#include "bsp_trace.h"

#include "em_chip.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_gpio.h"

#include "segmentlcd.h"
#include "nvm.h"

/******************************************************************************
*  This example shows simple usage of NVM module.
*
*  It stores object (single int in this case) into flash region declared as
*  non-volatile memory area. It uses wear leveling and demonstrates simple use of
*  the module. There are two types of pages: "normal" dedicated for storing
*  multiple objects which doesn't change often and "wear" which can store single
*  object (but this could be structure containing many fields) that can often
*  change. In this example on two pages same object is stored but NVM could handle
*  multiple pages with different objects.
*
*  PB0 - short press recalls data from "normal" page
*  PB0 - long press store data to "normal" page
*  PB1 - counts up, and after releasing stores data to "wear" page
*
*  RESET - resets CPU and if there were valid data in NVM recovers last data value.
******************************************************************************/

#define KEY_PB0_PIN_NUMBER        9
#define KEY_PB1_PIN_NUMBER        10

#define KEY_PB0_PORT              gpioPortB
#define KEY_PB0_PIN               KEY_PB0_PIN_NUMBER
#define KEY_PB1_PORT              gpioPortB
#define KEY_PB1_PIN               KEY_PB1_PIN_NUMBER

#define MAX_DISPLAYABLE_NUMBER    9999

/* delays specified in miliseconds */
#define DELAY_BETWEEN_COUNTS      200
#define DELAY_AFTER_PB1           500
#define DELAY_1MS                 1
#define DELAY_LED_BLINK           250
#define DELAY_FATAL_BLINK         1000

/* time needed to enable store function - defined in miliseconds */
#define KEY_STORE_TIME            500

typedef enum {
  COUNTER_ID
} NVM_Object_Ids;

typedef enum {
  PAGE_NORMAL_ID,
  PAGE_WEAR_ID
} NVM_Page_Ids;

extern uint16_t nvmCounter;

/***************************************************************************//**
 * @brief GPIO Interrupt handler (PB0 key)
 *        Sets the hours
 ******************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  /* Acknowledge interrupt */
  GPIO_IntClear(1 << KEY_PB0_PIN_NUMBER);
}

/***************************************************************************//**
 * @brief GPIO Interrupt handler (PB1 key)
 *        Sets the minutes
 ******************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  /* Acknowledge interrupt */
  GPIO_IntClear(1 << KEY_PB1_PIN_NUMBER);
}

/***************************************************************************//**
 * @brief Setup GPIO interrupt to set the time
 ******************************************************************************/
void APP_GpioSetup(void)
{
  /* Enable GPIO in CMU */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Configure pins for keys PB0 and PB1 as input */
  GPIO_PinModeSet(KEY_PB0_PORT, KEY_PB0_PIN, gpioModeInput, 0);
  GPIO_PinModeSet(KEY_PB1_PORT, KEY_PB1_PIN, gpioModeInput, 0);

  /* Set falling edge interrupt for both pins */
  GPIO_IntConfig(KEY_PB0_PORT, KEY_PB0_PIN, false, true, true);
  GPIO_IntConfig(KEY_PB1_PORT, KEY_PB1_PIN, false, true, true);

  /* Enable interrupt in core for even and odd gpio interrupts */
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/***************************************************************************//**
 * @brief Simple busy wait with milisecond resolution
 ******************************************************************************/
void APP_BusyWait(uint32_t msWait)
{
  uint32_t waitTop = msWait * (CMU_ClockFreqGet(cmuClock_CORE) / 1000);

  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL        |= 1;
  DWT->CYCCNT       = 0;

  while (DWT->CYCCNT < waitTop) ;
}

/***************************************************************************//**
 * @brief Fatal error handling
 ******************************************************************************/
void APP_Fatal(void)
{
  while (1) {
    ;
  }
}

/***************************************************************************//**
 * @brief Store objects to specific page (volume)
 ******************************************************************************/
Ecode_t APP_StoreData(uint16_t page)
{
  Ecode_t result;

  result = NVM_Write(page, NVM_WRITE_ALL_CMD);

  if (result != ECODE_EMDRV_NVM_OK) {
    APP_Fatal();
  }

  return result;
}

/***************************************************************************//**
 * @brief Read objects from page (volume)
 ******************************************************************************/
Ecode_t APP_ReadData(uint16_t page)
{
  Ecode_t result;

  result = NVM_Read(page, COUNTER_ID);

  if (result != ECODE_EMDRV_NVM_OK) {
    APP_Fatal();
  }
  return result;
}

/***************************************************************************//**
 * @brief Initialize NVM and restore objects
 ******************************************************************************/
Ecode_t APP_RestoreData(void)
{
  Ecode_t result;

  /* initialize NVM module */
  result = NVM_Init(NVM_ConfigGet());

  if (result == ECODE_EMDRV_NVM_NO_PAGES_AVAILABLE) { /* Ups, looks like no valid data in flash! */
                                                      /* This could happen on first run after flashing. */
                                                      /* So, we have to erase NVM */
    result = NVM_Erase(0);

    /* Store initial data/configuration */
    if (result == ECODE_EMDRV_NVM_OK) {
      result = APP_StoreData(PAGE_NORMAL_ID);
    }
    /* if wear page contains different data/object than normal page */
    /* it could be resonable to write wear page here too. */
  }

  /* if init phase went correctly, try to restore data. */
  if (result == ECODE_EMDRV_NVM_OK) { /* Try to restore data from wear page, if failed read it from normal page */
    result = NVM_Read(PAGE_WEAR_ID, COUNTER_ID);
    if (result == ECODE_EMDRV_NVM_PAGE_INVALID) {
      result = APP_ReadData(PAGE_NORMAL_ID);
    }
  }

  return result;
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  uint32_t j = 0;

  /* Chip errata */
  CHIP_Init();

  /* If first word of user data page is non-zero, enable Energy Profiler trace */
  BSP_TraceProfilerSetup();

  /* Ensure core frequency has been updated */
  SystemCoreClockUpdate();

  /* Init LCD with no voltage boost */
  SegmentLCD_Init(false);

  /* configure GPIO */
  APP_GpioSetup();

  if (APP_RestoreData() != ECODE_EMDRV_NVM_OK) {
    APP_Fatal();
  }

  while (1) {
    SegmentLCD_Number(nvmCounter);
    SegmentLCD_Write("SLEEP");
    /* go to sleep and wait for key pressing */
    EMU_EnterEM2(false);
    if (!GPIO_PinInGet(KEY_PB0_PORT, KEY_PB0_PIN)) { /* PB0 */
      j = 0;
      SegmentLCD_Write("...");
      while (!GPIO_PinInGet(KEY_PB0_PORT, KEY_PB0_PIN) && (j++ < KEY_STORE_TIME))
        APP_BusyWait(DELAY_1MS);
      if (!GPIO_PinInGet(KEY_PB0_PORT, KEY_PB0_PIN)) { /* store */
        SegmentLCD_Write("STORE");
        APP_StoreData(PAGE_NORMAL_ID);
      } else {
        /* restore */
        SegmentLCD_Write("RECALL");
        APP_ReadData(PAGE_NORMAL_ID);
      }
      APP_BusyWait(DELAY_AFTER_PB1);  /* delay to keep displayed text */
    }
    if (!GPIO_PinInGet(KEY_PB1_PORT, KEY_PB1_PIN)) { /* PB1 */
      SegmentLCD_Write("count");
      while (!GPIO_PinInGet(KEY_PB1_PORT, KEY_PB1_PIN)) {
        APP_BusyWait(DELAY_BETWEEN_COUNTS);
        nvmCounter++;
        if (nvmCounter > MAX_DISPLAYABLE_NUMBER) {
          nvmCounter = 0;
        }
        SegmentLCD_Number(nvmCounter);
      }
      APP_StoreData(PAGE_WEAR_ID);
    }
  }
}
