/***************************************************************************//**
 * @file
 * @brief DSP functions for Spectrum Analyzer
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
#include "arm_math.h"
#include "em_core.h"
#include "dsp.h"
#include <math.h>

/// Instance structures for float32_t RFFT
static arm_rfft_instance_f32 rfft_instance;

/// Instance structure for float32_t CFFT used by the RFFT
static arm_cfft_radix4_instance_f32 cfft_instance;

// Temporary buffer to store complex frequency data
static float tempBuffer[2 * FFT_SIZE];

/**************************************************************************//**
 * @brief Initialize CMSIS ARM FFT instance
 *****************************************************************************/
void DSP_InitFFT(void)
{
  // Initialize CMSIS FFT instance structures
  arm_rfft_init_f32(&rfft_instance, &cfft_instance, FFT_SIZE, 0, 1);
}

/**************************************************************************//**
 * @brief Perform FFT and extract signal frequency content. Returns maximum
 * magnitude
 *****************************************************************************/
float DSP_AnalyzeSpectrum(float *timeBuffer, float *freqBuffer)
{
  // Maximum frequency bin
  float maxval;

  // Temporary pointer to principal frequency of bin
  uint32_t* principalIndex = 0;

  // Rectangular window on data
  timeBuffer[0] = 0;
  timeBuffer[FFT_SIZE - 1] = 0;

  // Perform FFT and extract real magnitude
  arm_rfft_f32(&rfft_instance, timeBuffer, tempBuffer);
  arm_cmplx_mag_f32(tempBuffer, freqBuffer, FFT_SIZE);

  // Remove DC component for visualization
  freqBuffer[0] = 0;

  // Calculate highest magnitude bin
  arm_max_f32(freqBuffer, 128, &maxval, principalIndex);

  // Return highest magnitude bin
  return maxval;
}
