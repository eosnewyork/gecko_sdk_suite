/***************************************************************************//**
 * @file
 * @brief LCD controller demo
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
#include <stdint.h>
#include <stdbool.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_rtcc.h"
#include "em_gpio.h"

#include "bsp.h"
#include "segmentlcd.h"
#include "rtcdriver.h"
#include "gpiointerrupt.h"

// -----------------------------------------------------------------------------
// Local variables
static RTCDRV_TimerID_t timerId;
static volatile bool dynamicChgRedist = true;
static volatile bool freezeScreen = false;

// -----------------------------------------------------------------------------
// Local prototypes
static void button0Event(uint8_t pin);
static void button1Event(uint8_t pin);
static void sleep(uint32_t ms);
static void chargeRedistEnable(void);
static void setupTimer(void);
static void setupButton(void);

/***************************************************************************//**
 * @brief  Called when the BTN0 interrupt is fired
 * @param  pin pin number for the callback.
 ******************************************************************************/
static void button0Event(uint8_t pin)
{
  // Toggle dynamic charge redistribution
  (void) pin;
  dynamicChgRedist = !dynamicChgRedist;
  chargeRedistEnable();
}

/***************************************************************************//**
 * @brief  Called when the BTN1 interrupt is fired
 * @param  pin pin number for the callback.
 ******************************************************************************/
static void button1Event(uint8_t pin)
{
  // Toggle freezeScreen
  (void) pin;
  freezeScreen = !freezeScreen;
}

/***************************************************************************//**
 * @brief  Sleeps given number of milliseconds if freezeScreen sleep forever
 * @param  ms milliseconds to sleep
 ******************************************************************************/
static void sleep(uint32_t ms)
{
  RTCDRV_StartTimer(timerId,
                    rtcdrvTimerTypeOneshot,
                    ms,
                    NULL,
                    NULL);
  EMU_EnterEM2(true);

  while (freezeScreen) {
    EMU_EnterEM2(true);
  }
}

/**************************************************************************//**
 * @brief  Enabling/disabling dynamic charge redistribution
 *****************************************************************************/
static void chargeRedistEnable(void)
{
  // Enable/disable dynamic charge redistribution
  SegmentLCD_chargeRedistributionEnable(dynamicChgRedist);
  SegmentLCD_Symbol(LCD_SYMBOL_GECKO, dynamicChgRedist);
}

/***************************************************************************//**
 * @brief
 *   Initialization Wake-up interrupt using RTCDRV
 ******************************************************************************/
static void setupTimer(void)
{
  RTCDRV_Init();
  RTCDRV_AllocateTimer(&timerId);
}

/***************************************************************************//**
 * @brief  Initialization of GPIO interrupt for BTN0 and BTN1
 ******************************************************************************/
static void setupButton(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true); // GPIO clock enable
  GPIOINT_Init();

  // Pin mode set-up for BTN0
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 0);
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, BSP_GPIO_PB0_PIN,
                    false, true, true);

  // Pin mode set-up for BTN1
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInput, 0);
  GPIO_ExtIntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, BSP_GPIO_PB1_PIN,
                    false, true, true);

  GPIOINT_CallbackRegister(BSP_GPIO_PB0_PIN, button0Event);
  GPIOINT_CallbackRegister(BSP_GPIO_PB1_PIN, button1Event);
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  int i;

  // Chip errata
  CHIP_Init();

  // Enable LCD without voltage boost
  SegmentLCD_Init(false);

  // Set up BTN0 for IRQ
  setupButton();

  // Set up timer for wake-up
  setupTimer();

  while (1) {
    // Enable all segments
    SegmentLCD_AllOn();
    sleep(1000);

    // Disable all segments
    SegmentLCD_AllOff();
    SegmentLCD_Symbol(LCD_SYMBOL_GECKO, dynamicChgRedist);

    // Write some text
    SegmentLCD_Write("Silicon");
    sleep(500);
    SegmentLCD_Write("Labs");
    sleep(500);
    SegmentLCD_Write("EFM TG11");
    sleep(1000);

    // Write some numbers
    for (i = 0; i < 10; i++) {
      SegmentLCD_Number(i * 111111);
      sleep(200);
    }

    SegmentLCD_LowerNumber(12345678);
    sleep(1000);
    SegmentLCD_LowerNumber(-1234567);
    sleep(1000);
    SegmentLCD_AlphaNumberOff();

    // Test segments
    SegmentLCD_AllOff();
    SegmentLCD_Symbol(LCD_SYMBOL_GECKO, dynamicChgRedist);
    SegmentLCD_Symbol(LCD_SYMBOL_EFM32, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_COL1, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_COL2, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_DEGC, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_DEGF, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_C1, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_C2, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_C3, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_C4, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_C5, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_C6, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_C7, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_C8, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_C9, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_C10, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_C11, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_C12, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_C13, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_C14, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_C15, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_C16, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_C17, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_C18, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_C19, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_S2, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_S3, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_S4, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_S5, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_S6, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_S7, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_S8, 1);
    SegmentLCD_Symbol(LCD_SYMBOL_S9, 1);
    sleep(1000);

    for (i = 0; i < 35; i++) {
      SegmentLCD_Array(i, true);
      sleep(50);
    }
    sleep(1000);
  }
}
