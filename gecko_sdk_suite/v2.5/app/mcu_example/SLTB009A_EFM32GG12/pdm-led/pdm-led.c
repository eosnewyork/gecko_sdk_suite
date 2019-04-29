/***************************************************************************//**
 * @file pdm-led.c
 * @PDM microphone input amplitude is translated to LED brightness.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Labs, Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_timer.h"
#include "bsp.h"
#include "bsp_trace.h"

#define sampleMax 128

// Declare global variables
uint32_t tempFIFO[4];
uint32_t samplespace = sampleMax;
int16_t right[sampleMax];
int16_t left[sampleMax];
uint32_t samplePointer = 0;
uint32_t i = 0;

// Timerconfig for use with PWM
static const TIMER_Init_TypeDef TimeInitStruct = { \
  true,                   // Enable timer when init complete.
  false,                  // Stop counter during debug halt.
  timerPrescale1,         // Prescale clock by 1
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

/***************************************************************************//**
 * @brief  Set up timers 1 and 2, and use CC modules to generate PWM
 ******************************************************************************/
void pwmInit(void)
{
  // Set up clock
  CMU_ClockEnable(cmuClock_TIMER1, true); // Timer1 = LEFT = LED1
  CMU_ClockEnable(cmuClock_TIMER2, true); // Timer2 = RIGHT = LED0

  // Initiate timer
  TIMER_Init(TIMER1, &TimeInitStruct);
  TIMER_Init(TIMER2, &TimeInitStruct);

  // Set up output to leds
  GPIO_PinModeSet(gpioPortA, 12, gpioModePushPull, 0); // Red LED0
  GPIO_PinModeSet(gpioPortD, 6, gpioModePushPull, 0);  // Red LED1

  // Route output to te timers
  TIMER1->ROUTEPEN = TIMER_ROUTEPEN_CC0PEN;
  TIMER2->ROUTEPEN = TIMER_ROUTEPEN_CC0PEN;
  TIMER1->ROUTELOC0 = (0x4 << _TIMER_ROUTELOC0_CC0LOC_SHIFT);
  TIMER2->ROUTELOC0 = (0x1 << _TIMER_ROUTELOC0_CC0LOC_SHIFT);

  // Config Capture & Compare module
  // Set output on overflow, clear on compare match, PWM mode
  // Output inverted for leds connected to Cathode
  TIMER1->CC->CTRL = (0x3 << _TIMER_CC_CTRL_COFOA_SHIFT)
                     | (0x2 << _TIMER_CC_CTRL_CMOA_SHIFT)
                     | (0x1 << _TIMER_CC_CTRL_OUTINV_SHIFT)
                     | (0x3 << _TIMER_CC_CTRL_MODE_SHIFT);
  TIMER2->CC->CTRL = (0x3 << _TIMER_CC_CTRL_COFOA_SHIFT)
                     | (0x2 << _TIMER_CC_CTRL_CMOA_SHIFT)
                     | (0x1 << _TIMER_CC_CTRL_OUTINV_SHIFT)
                     | (0x3 << _TIMER_CC_CTRL_MODE_SHIFT);

  // 16 bit top value
  TIMER1->TOP = 0x3000;
  TIMER2->TOP = 0x3000;

  // Initial compare value (CCVB for buffer)
  TIMER1->CC->CCV = 0x000E;
  TIMER2->CC->CCV = 0x000E;

  // Resetting counter
  TIMER1->CNT = 0;
  TIMER2->CNT = 0;
}

/***************************************************************************//**
 * @brief Sets up PDM microphone
 ******************************************************************************/
void pdmInit(void)
{
  // Set up clocks
  CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_GPIO;
  CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_PDM;
  CMU->PDMCTRL |= CMU_PDMCTRL_PDMCLKSEL_HFRCO | CMU_PDMCTRL_PDMCLKEN;

  // Config GPIO and pin routing
  GPIO_PinModeSet(gpioPortB, 12, gpioModePushPull, 0);  // CLK, Alternate
  GPIO_PinModeSet(gpioPortB, 11, gpioModeInput, 0);     // DATA
  GPIO_SlewrateSet(gpioPortB, 7, 7);
  // MCU PA8 = MIC_EN
  GPIO_PinOutSet(gpioPortA, 8); // Enable mic connection through TS3A4751
  //(PDM_DATA0#3, PDM_CLK#3)
  PDM->ROUTELOC0 = (3 << _PDM_ROUTELOC0_DAT0LOC_SHIFT); //(PB11, DATA)
  PDM->ROUTELOC1 = (3 << _PDM_ROUTELOC1_CLKLOC_SHIFT);  //(PB12, CLK)
  PDM->ROUTEPEN = (1 << _PDM_ROUTEPEN_DAT0PEN_SHIFT) | (1 << _PDM_ROUTEPEN_CLKPEN_SHIFT);

  // Config prescale value
  // Prescale value = (input clock / wanted clock)-1
  uint32_t prescaler = 5;
  while (PDM->SYNCBUSY != 0) ;
  PDM->CFG1 = (prescaler << _PDM_CFG1_PRESC_SHIFT);

  // Config PDM
  PDM->CFG0 = PDM_CFG0_STEREOMODECH01_CH01ENABLE
              | PDM_CFG0_CH0CLKPOL_NORMAL
              | PDM_CFG0_CH1CLKPOL_INVERT
              | PDM_CFG0_FIFODVL_FOUR
              | PDM_CFG0_DATAFORMAT_DOUBLE16
              | PDM_CFG0_NUMCH_TWO
              | PDM_CFG0_FORDER_FIFTH;

  // Config DSR/Gain
  while (PDM->SYNCBUSY != 0) ;
  PDM->CTRL = (PDM_CTRL_OUTCLKEN | (3 << _PDM_CTRL_GAIN_SHIFT) | (32 << _PDM_CTRL_DSR_SHIFT));

  // Enable module
  PDM->EN = PDM_EN_EN;

  // Start filter
  while (PDM->SYNCBUSY != 0) ;
  PDM->CMD = PDM_CMD_START;
}

/***************************************************************************//**
 * @brief  Translate audio data into PWM compare value
 * PA12 = RED_LED_0 = right = timer2
 * PD6  = RED_LED_1 = left  = timer1
 ******************************************************************************/
void updateLED(void)
{
  uint32_t sumRight = 0;
  uint32_t sumLeft = 0;
  uint32_t outRight = 0;
  uint32_t outLeft = 0;

  for (i = 0; i < samplespace; i++) {
    // Rectify samples
    if (right[i] < 0) {
      right[i] = -right[i];
    }
    if (left[i] < 0) {
      left[i] = -left[i];
    }

    // Accumulate for averaging
    sumRight = sumRight + right[i];
    sumLeft = sumLeft + left[i];
  }

  // Sum is proportional to an average and is used for more dynamic range
  outRight = sumRight;
  outLeft = sumLeft;

  // PWM compare value (CCVB for buffer)
  TIMER1->CC->CCVB = outLeft;
  TIMER2->CC->CCVB = outRight;
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;

  // Chip errata
  CHIP_Init();

  EMU_DCDCInit(&dcdcInit);

  // If first word of user data page is non-zero, enable Energy Profiler trace
  BSP_TraceProfilerSetup();

  // Initialize modules
  pdmInit();
  pwmInit();

  // Infinite loop that repeat every time FIFO is full
  while (1) {
    // Wait for full FIFO
    while ( !(PDM->STATUS & PDM_STATUS_FULL) ) ;

    // Pop FIFO
    for (i = 0; i < 4; i++) {
      tempFIFO[i] = PDM->RXDATA;
    }
    // Flush the FIFO
    PDM->CMD = PDM_CMD_FIFOFL;

    // Transfer to buffer
    for (i = 0; i < 4; i++) {
      left[samplePointer] = tempFIFO[i] & 0x0000FFFF;
      right[samplePointer] = (tempFIFO[i] >> 16) & 0x0000FFFF;
      samplePointer++;
    }

    // Update LED brightness
    if (samplePointer >= sampleMax) {
      updateLED();
      samplePointer = 0;
    }

    // Wait until FIFO flush is done before next round
    while ( (PDM->SYNCBUSY) ) {
    }
  }
}
