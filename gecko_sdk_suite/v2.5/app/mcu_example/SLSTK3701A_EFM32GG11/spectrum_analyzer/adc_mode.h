/***************************************************************************//**
 * @file
 * @brief ADC input functions for Spectrum Analyzer
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

#ifndef SRC_ADCMODE_H_
#define SRC_ADCMODE_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************//**
 * @brief ADC initialization
 *
 * @param sampleFreq is the desired sampling freqency of the ADC
 *
 * @detail Initializes the ADC to continously convert 12 bit samples from
 * GPIO Pin PB11 at the desired sample frequency
 *****************************************************************************/
void ADCMODE_InitADC(uint32_t sampleFreq);

/**************************************************************************//**
 * @brief Configure and start DMA
 *
 * @param bufferA is the 'Ping' buffer to transfer ADC data to
 * @param bufferB is the 'Pong' buffer to transfer ADC data to
 * @param size is the length of the buffers
 * @returns void
 *
 * @detail Initializes the DMA to transfer ADC samples to memory in a
 * Ping-Pong type transfer. Generates an interrupt on each buffer full event
 *****************************************************************************/
void ADCMODE_InitLDMA(uint32_t *bufferA, uint32_t *bufferB, uint32_t size);

#ifdef __cplusplus
// *INDENT-OFF*
}
// *INDENT-ON*
#endif

#endif /* SRC_ADCMODE_H_ */
