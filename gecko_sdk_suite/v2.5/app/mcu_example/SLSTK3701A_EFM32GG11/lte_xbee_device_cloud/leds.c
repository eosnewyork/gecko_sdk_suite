/***************************************************************************//**
 * @file
 * @brief
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

#include "em_cmu.h"
#include "em_timer.h"
#include "bsp.h"
#include "bspconfig.h"

#include "leds.h"

#define PWM_CCVB_INIT     59
#define LED_PWM_CHANNELS  { 0, 1, 2 }

static uint8_t rgbLedChannels[RGB_LED_COUNT * 3] = LED_PWM_CHANNELS;

void setupRgbLed1(void)
{
  uint8_t i;

  // Create Timer_Init initialization structure
  TIMER_Init_TypeDef rgbLedTimerInit = TIMER_INIT_DEFAULT;
  rgbLedTimerInit.prescale = timerPrescale4;

  // Create TIMER_InitCC initialization structure
  TIMER_InitCC_TypeDef rgbLedCcInit = TIMER_INITCC_DEFAULT;
  rgbLedCcInit.mode = timerCCModePWM;
  rgbLedCcInit.outInvert = true;

  // Setup RGB LED GPIO pins
  BSP_LedsInit();

  // LED1 is controlled by TIMER5 PWM channels
  CMU_ClockEnable(cmuClock_TIMER5, true);
  TIMER_Init(TIMER5, &rgbLedTimerInit);
  TIMER_TopBufSet(TIMER5, PWM_STEPS);

  for (i = 0; i < 3; i++) {
    TIMER_InitCC(TIMER5,
                 rgbLedChannels[i],
                 &rgbLedCcInit);
    TIMER_CompareBufSet(TIMER5, rgbLedChannels[i], 0);
  }
  TIMER_Enable(TIMER5, true);

  // Enable output capture compare pins
  TIMER5->ROUTEPEN = TIMER_ROUTEPEN_CC0PEN | TIMER_ROUTEPEN_CC1PEN | TIMER_ROUTEPEN_CC2PEN;
  TIMER5->ROUTELOC0 = TIMER_ROUTELOC0_CC0LOC_LOC2 | TIMER_ROUTELOC0_CC1LOC_LOC2 | TIMER_ROUTELOC0_CC2LOC_LOC2;
}

void setLed1(uint32_t red, uint32_t green, uint32_t blue)
{
  if (red > PWM_STEPS) {
    red = PWM_STEPS;
  }
  if (green > PWM_STEPS) {
    green = PWM_STEPS;
  }
  if (blue > PWM_STEPS) {
    blue = PWM_STEPS;
  }
  TIMER_CompareBufSet(TIMER5, rgbLedChannels[0], red);
  TIMER_CompareBufSet(TIMER5, rgbLedChannels[1], green);
  TIMER_CompareBufSet(TIMER5, rgbLedChannels[2], blue);
}
