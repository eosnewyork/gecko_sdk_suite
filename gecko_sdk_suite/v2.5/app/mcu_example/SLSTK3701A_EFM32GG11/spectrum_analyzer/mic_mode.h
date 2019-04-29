/***************************************************************************//**
 * @file
 * @brief microphone input functions for Spectrum Analyzer
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

#ifndef SRC_MICMODE_H_
#define SRC_MICMODE_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************//**
 * @brief Configure and start stereo microphone on USART3
 *
 * @param sampleFrequency is the desired sampling frequency of the microphone
 * @returns void
 *
 * @detail Initializes USART3 and GPIO pins to communicate with the STK
 * microphone codec over I2S
 *****************************************************************************/
void MICMODE_InitMIC(uint32_t sampleFrequency);

/**************************************************************************//**
 * @brief Configure and start DMA
 *
 * @param bufferA is the 'Ping' buffer to transfer ADC data to
 * @param bufferB is the 'Pong' buffer to transfer ADC data to
 * @param size is the length of the buffers
 *
 * @detail Initializes the DMA for tranfer from the USART3 RX register to
 * memory in a Ping-Pong type transfer. The DMA also is configured to read
 * only left microphone data, but clear and discard right microphone data from
 * the RX buffer. The DMA generates an interrupt after each buffer filled event
 *****************************************************************************/
void MICMODE_InitLDMA(uint32_t *bufferA, uint32_t *bufferB, uint32_t size);

#ifdef __cplusplus
// *INDENT-OFF*
}
// *INDENT-ON*
#endif

#endif /* SRC_MICMODE_H_ */
