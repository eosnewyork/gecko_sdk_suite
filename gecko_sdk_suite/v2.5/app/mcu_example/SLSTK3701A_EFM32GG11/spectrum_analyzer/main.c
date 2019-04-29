/***************************************************************************//**
 * @file
 * @brief main file for Spectrum Analyzer
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
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "bsp.h"
#include "lcd.h"
#include "dsp.h"
#include "adc_mode.h"
#include "mic_mode.h"

// Sample frequency in Hz
// 8kHz * 512 / 120 = 34133 Hz
#define SAMPLE_FREQUENCY 34133

/// Buffer for frequency content data
static float freqBuffer[FFT_SIZE];

/// Dual buffer for time domain data
static volatile uint32_t bufferA[FFT_SIZE];
static volatile uint32_t bufferB[FFT_SIZE];

/// Buffer for floating point converted time domain data
float bufferFloat[FFT_SIZE];

// Status flags struct
typedef struct {
  // Indicates which buffer is safe to process (bufferA=true, bufferB=false)
  bool bufferFlag;

  // Indicates if spectrogram is out of date
  bool updateFlag;

  // Indicates if input source is microphone
  bool micInput;
} Status_t;

// Globally declared status struct for interrupt notification
static volatile Status_t status;

// Helper function and ISR prototypes
void LDMA_IRQHandler(void);
static void initGPIO(void);
static void initCMU(void);

/**************************************************************************//**
 * @brief Main Function
 *****************************************************************************/
int main(void)
{
  // Chip Errata
  CHIP_Init();

  // Running maximum magnitude value for main loop
  float maxMag = 0;

  // Initializations
  initCMU();
  initGPIO();

  // Initialize LCD to start screen, prompts user to select input
  LCD_Init();

  // Configure and initialize CMSIS FFT
  DSP_InitFFT();

  // Store BTN0 and BTN1 input
  bool btn0 = true;
  bool btn1 = true;

  // Wait and poll for user to press BTN0 or BTN1
  while (btn0 && btn1) {
    btn0 = GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN);
    btn1 = GPIO_PinInGet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN);
  }

  // If the user pressed BTN0, initialize microphone as input source
  if (!btn0) {
    // Initialize LDMA first to avoid losing bytes of microphone data
    MICMODE_InitLDMA((uint32_t *)bufferA, (uint32_t *)bufferB, FFT_SIZE);
    MICMODE_InitMIC(SAMPLE_FREQUENCY);

    // Declare status flag for microphone input mode
    status.micInput = true;
  } else if (!btn1) {
    // If the user pressed BTN1, initialize ADC0 as input (GPIO Pin PB11)
    ADCMODE_InitADC(SAMPLE_FREQUENCY);
    ADCMODE_InitLDMA((uint32_t *)bufferA, (uint32_t *)bufferB, FFT_SIZE);

    // Declare status flag for ADC input mode
    status.micInput = false;
  }

  // Set control flags, LDMA initially writes to bufferA
  status.bufferFlag = false;
  status.updateFlag = false;

  // Set up static display elements for spectrogram
  LCD_BuildGraph();

  // Main loop computing and displaying spectrum analyzer, will not exit
  while (1) {
    // Only update spectrogram once per LDMA interrupt
    if (status.updateFlag) {
      // Pointer to the buffer to be processed
      uint32_t *processBuffer;

      // Process the buffer not being written to by the LDMA
      if (status.bufferFlag) {
        processBuffer = (uint32_t *)bufferA;
      } else {
        processBuffer = (uint32_t *)bufferB;
      }

      // Convert raw data to floating point
      if (status.micInput) {
        for (int i = 0; i < FFT_SIZE; i++) {
          // Arrange microphone sample bytes as 20 bit sign extended Big Endian
          bufferFloat[i] = (float) (((int32_t) __REV(processBuffer[i])) >> 12);
        }
      } else {
        for (int i = 0; i < FFT_SIZE; i++) {
          bufferFloat[i] = (float) processBuffer[i];
        }
      }

      // Calculate frequency response and update display
      maxMag = DSP_AnalyzeSpectrum((float*) bufferFloat, (float*) freqBuffer);
      LCD_UpdateFrequencyResponse(freqBuffer, maxMag, status.micInput);

      // Indicate spectrogram is up to date
      status.updateFlag = false;
    }
  }
}

/***************************************************************************//**
 * @brief LDMA IRQ handler.
 * @detail Handler that triggers on each complete LDMA transfer and sets the
 * corresponding flag
 ******************************************************************************/
void LDMA_IRQHandler(void)
{
  // Clear all interrupt flags
  LDMA->IFC |= 0xFFFFFFFF;

  // Switch which buffer is declared as safe to process
  status.bufferFlag = !status.bufferFlag;

  // Indicate spectrogram is out of date
  status.updateFlag = true;
}

/***************************************************************************//**
 * @brief Initializes GPIO
 * @detail Enables the push buttons BTN0 and BTN1 on the STK board
 ******************************************************************************/
void initGPIO(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Enable Push Buttons, pull-up direction
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 1);
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInput, 1);
}

/***************************************************************************//**
 * @brief Initializes clocks
 * @detail Sets the HF clock speed
 ******************************************************************************/
void initCMU()
{
  // Set HF clock to 72MHz
  CMU_HFRCOBandSet(cmuHFRCOFreq_72M0Hz);
}
