/***************************************************************************//**
 * @file
 * @brief Helper functions for PWM control of RGB LEDs
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

#define PWM_GAIN 12
#define PWM_STEPS (60 * PWM_GAIN) - 1
#define PWM_CCVB_INIT 59

#define RGB_LED_COUNT 2
#define LED_PWM_CHANNELS { 0, 1, 2, 2, 3, 0 }

#include "em_cmu.h"
#include "em_timer.h"

#include "bsp.h"
#include "bspconfig.h"

#include "app_rgbled.h"

static uint8_t rgbLedChannels[RGB_LED_COUNT * 3] = LED_PWM_CHANNELS;

static const TIMER_Init_TypeDef rgbLedTimerInit = { \
  true,                   // Enable timer when init complete.
  false,                  // Stop counter during debug halt.
  timerPrescale4,         // Prescale clock by 4
  timerClkSelHFPerClk,    // Select HFPER clock.
  false,                  // Not 2x count mode.
  false,                  // No ATI.
  timerInputActionNone,   // No action on falling input edge.
  timerInputActionNone,   // No action on rising input edge.
  timerModeUp,            // Up-counting.
  false,                  // Do not clear DMA requests when DMA channel is active.
  false,                  // Select X2 quadrature decode mode (if used).
  false,                  // Disable one shot.
  false                   // Not started/stopped/reloaded by other timers.
};

static const TIMER_InitCC_TypeDef rgbLedCcInit = { \
  timerEventEveryEdge,      // Event on every capture.
  timerEdgeRising,          // Input capture edge on rising edge.
  timerPRSSELCh0,           // Not used by default, select PRS channel 0.
  timerOutputActionNone,    // No action on underflow.
  timerOutputActionNone,    // No action on overflow.
  timerOutputActionNone,    // No action on match.
  timerCCModePWM,           // PWM mode compare/capture channel.
  false,                    // Disable filter.
  false,                    // Select TIMERnCCx input.
  false,                    // Clear output when counter disabled.
  false,                    // Do not invert output.
  timerPrsOutputDefault     // Not used, just select default.
};

void setupRGBLed(void)
{
  uint8_t i;

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
    TIMER_CompareBufSet(TIMER5,
                        rgbLedChannels[i],
                        PWM_CCVB_INIT);
  }
  TIMER_Enable(TIMER5, true);

  // LED0 is controlled by WTIMER1 PWM channels
  CMU_ClockEnable(cmuClock_WTIMER1, true);
  TIMER_Init(WTIMER1, &rgbLedTimerInit);
  TIMER_TopBufSet(WTIMER1, PWM_STEPS);

  for (i = 0; i < 3; i++) {
    TIMER_InitCC(WTIMER1,
                 rgbLedChannels[i + 3],
                 &rgbLedCcInit);
    TIMER_CompareBufSet(WTIMER1,
                        rgbLedChannels[i + 3],
                        PWM_CCVB_INIT);
  }
  TIMER_Enable(WTIMER1, true);

  // Enable output capture compare pins
  TIMER5->ROUTEPEN = TIMER_ROUTEPEN_CC0PEN | TIMER_ROUTEPEN_CC1PEN | TIMER_ROUTEPEN_CC2PEN;
  TIMER5->ROUTELOC0 = TIMER_ROUTELOC0_CC0LOC_LOC2 | TIMER_ROUTELOC0_CC1LOC_LOC2 | TIMER_ROUTELOC0_CC2LOC_LOC2;

  WTIMER1->ROUTEPEN = TIMER_ROUTEPEN_CC0PEN | TIMER_ROUTEPEN_CC2PEN | TIMER_ROUTEPEN_CC3PEN;
  WTIMER1->ROUTELOC0 = TIMER_ROUTELOC0_CC0LOC_LOC7 | TIMER_ROUTELOC0_CC2LOC_LOC6 | TIMER_ROUTELOC0_CC3LOC_LOC6;
}

void setLedSettings(RGBLED_Settings_t * ledSettings)
{
  ColorMix led0Color, led1Color;
  int16_t led0Pos, led1Pos;

  // convert position on color bar to a position on a color wheel,
  // which is offset 30 degrees
  led0Pos = (360 + (ledSettings->led0Color - 30)) % 360;
  led1Pos = (360 + (ledSettings->led1Color - 30)) % 360;

  getColorMix(led0Pos, &led0Color, ledSettings->led0Power);
  getColorMix(led1Pos, &led1Color, ledSettings->led1Power);

  if (ledSettings->led0Power == 0) {
    led0Color.red = PWM_STEPS;
    led0Color.green = PWM_STEPS;
    led0Color.blue = PWM_STEPS;
  }
  if (ledSettings->led1Power == 0) {
    led1Color.red = PWM_STEPS;
    led1Color.green = PWM_STEPS;
    led1Color.blue = PWM_STEPS;
  }

  TIMER_CompareBufSet(TIMER5,
                      rgbLedChannels[0],
                      led1Color.red);
  TIMER_CompareBufSet(TIMER5,
                      rgbLedChannels[1],
                      led1Color.green);
  TIMER_CompareBufSet(TIMER5,
                      rgbLedChannels[2],
                      led1Color.blue);
  TIMER_CompareBufSet(WTIMER1,
                      rgbLedChannels[3],
                      led0Color.red);
  TIMER_CompareBufSet(WTIMER1,
                      rgbLedChannels[4],
                      led0Color.green);
  TIMER_CompareBufSet(WTIMER1,
                      rgbLedChannels[5],
                      led0Color.blue);
}

void getColorMix(int16_t wheelpos, ColorMix * inColor, uint32_t gain)
{
  ColorMix color = { 0, 0, 0 };
  uint32_t level = PWM_STEPS - ((60 * gain) - 1);

  // Red color level
  if (wheelpos < 120) {
    color.red = level;
  } else if (wheelpos < 180) {
    color.red = (gain * (wheelpos - 119)) + level;
  } else if (wheelpos >= 300) {
    color.red = (gain * (360 - wheelpos)) + level;
  } else {
    color.red = PWM_STEPS;
  }

  // Blue color level
  if (wheelpos < 60 || wheelpos >= 300) {
    color.blue = PWM_STEPS;
  } else if (wheelpos < 120) {
    color.blue = (gain * (120 - wheelpos)) + level;
  } else if (wheelpos >= 240) {
    color.blue = (gain * (wheelpos - 239)) + level;
  } else {
    color.blue = level;
  }

  // Green color level
  if (wheelpos < 60) {
    color.green = (gain * (1 + wheelpos)) + level;
  } else if (wheelpos < 180) {
    color.green = PWM_STEPS;
  } else if (wheelpos < 240) {
    color.green = (gain * (240 - wheelpos)) + level;
  } else {
    color.green = level;
  }

  *inColor = color;
}
