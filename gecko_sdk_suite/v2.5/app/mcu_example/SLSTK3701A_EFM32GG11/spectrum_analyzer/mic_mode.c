/***************************************************************************//**
 * @file
 * @brief Microphone input mode functions for Spectrum Analyzer
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
#include <stdbool.h>
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_ldma.h"
#include "em_usart.h"

// Defined pin locations for microphone i2S interfacing
#define MIC_ENABLE_PORT gpioPortD
#define MIC_ENABLE_PIN  0
#define I2S_PORT        gpioPortI
#define I2S_TX_PIN      12
#define I2S_RX_PIN      13
#define I2S_CLK_PIN     14
#define I2S_CS_PIN      15

/// Globally declared LDMA link descriptors
LDMA_Descriptor_t leftDesc[2];
LDMA_Descriptor_t rightDesc;

/// Single byte used to dispose of right microphone data
uint8_t rightData;

/**************************************************************************//**
 * @brief Configure and start stereo microphone on USART3
 *****************************************************************************/
void MICMODE_InitMIC(uint32_t sampleFrequency)
{
  // Enable clock for USART3
  CMU_ClockEnable(cmuClock_USART3, true);

  // Enable GPIO clock and I2S pins
  GPIO_PinModeSet(I2S_PORT, I2S_RX_PIN, gpioModeInputPullFilter, 0);
  GPIO_PinModeSet(I2S_PORT, I2S_TX_PIN, gpioModePushPull, 1);
  GPIO_PinModeSet(I2S_PORT, I2S_CLK_PIN, gpioModePushPull, 1);
  GPIO_PinModeSet(I2S_PORT, I2S_CS_PIN, gpioModePushPull, 1);

  // Initialize USART3 to receive data from microphones synchronously
  USART_InitI2s_TypeDef def = USART_INITI2S_DEFAULT;

  //
  def.sync.databits = usartDatabits8;
  def.format = usartI2sFormatW32D32;
  def.sync.enable = usartDisable;
  def.sync.autoTx = true;
  def.justify = usartI2sJustifyLeft;

  // Separate DMA requests for left and right channel data
  def.dmaSplit = true;

  // Set baud rate to achieve desired sample frequency
  def.sync.baudrate = sampleFrequency * 64;

  USART_InitI2s(USART3, &def);

  // Enable route to GPIO pins for I2S transfer on route #5
  USART3->ROUTEPEN = USART_ROUTEPEN_TXPEN
                     | USART_ROUTEPEN_RXPEN
                     | USART_ROUTEPEN_CSPEN
                     | USART_ROUTEPEN_CLKPEN;
  USART3->ROUTELOC0 = USART_ROUTELOC0_TXLOC_LOC5
                      | USART_ROUTELOC0_RXLOC_LOC5
                      | USART_ROUTELOC0_CSLOC_LOC5
                      | USART_ROUTELOC0_CLKLOC_LOC5;

  // Enable USART3
  USART_Enable(USART3, usartEnable);

  // Initialize and set mic_enable pin (PD0)
  GPIO_PinModeSet(MIC_ENABLE_PORT, MIC_ENABLE_PIN, gpioModePushPull, 1);
}

/**************************************************************************//**
 * @brief Configure and start DMA
 *****************************************************************************/
void MICMODE_InitLDMA(uint32_t *bufferA, uint32_t *bufferB, uint32_t size)
{
  // Default LDMA init
  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init(&init);

  // Configure LDMA for transfer from USART to memory (left channel)
  // LDMA will loop continuously
  LDMA_TransferCfg_t leftCfg =
    LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_USART3_RXDATAV);

  // Set up descriptors for left speaker
  // Transferring one byte at a time, so cast transfer size and buffers as 8 bit
  LDMA_Descriptor_t leftXfer[] =
  {
    LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&USART3->RXDATA,
                                     (uint8_t *)bufferA,
                                     (4 * size),
                                     1),

    LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&USART3->RXDATA,
                                     (uint8_t *)bufferB,
                                     (4 * size),
                                     -1)
  };

  //Globally store and configure link descriptors for left microphone transfer
  leftDesc[0] = leftXfer[0];
  leftDesc[0].xfer.size = ldmaCtrlSizeByte;
  // trigger interrupt on left microphone transfer complete (buffer full)
  leftDesc[0].xfer.doneIfs = 1;
  leftDesc[0].xfer.ignoreSrec = 0;

  // Same descriptor settings for second buffer
  leftDesc[1] = leftXfer[1];
  leftDesc[1].xfer.size = ldmaCtrlSizeByte;
  leftDesc[1].xfer.doneIfs = 1;
  leftDesc[1].xfer.ignoreSrec = 0;

  // Configure LDMA for transfer from USART to memory (right channel)
  // LDMA will loop continuously and discard right channel data
  LDMA_TransferCfg_t rightCfg =
    LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_USART3_RXDATAVRIGHT);

  // Set up right microphone descriptor
  LDMA_Descriptor_t rightXfer =
    LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&USART3->RXDATA, &rightData, 1, 0);
  rightDesc = rightXfer;
  rightDesc.xfer.size = ldmaCtrlSizeByte;

  // Don't trigger interrupts on right microphone transfers
  rightDesc.xfer.doneIfs = 0;
  rightDesc.xfer.ignoreSrec = 0;

  // Ensure destination address does not increment
  rightDesc.xfer.dstInc = 0;

  // Start left and right transfers
  LDMA_StartTransfer(0, (void *)&leftCfg, (void *)&leftDesc);
  LDMA_StartTransfer(1, (void *)&rightCfg, (void *)&rightDesc);
}
