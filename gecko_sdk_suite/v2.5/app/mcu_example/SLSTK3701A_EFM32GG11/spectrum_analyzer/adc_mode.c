/***************************************************************************//**
 * @file
 * @brief ADC input mode functions for Spectrum Analyzer
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
#include "em_adc.h"
#include "em_ldma.h"

// ADC resolution
#define ADC_RESOLUTION adcRes12Bit

// Reference voltage
#define VREF adcRef5V

// ADC acquisition time
#define ADC_ACQTIME adcAcqTime8

/// Globally declared LDMA link descriptors
LDMA_Descriptor_t descLink[2];

/**************************************************************************//**
 * @brief ADC initialization
 *****************************************************************************/
void ADCMODE_InitADC(uint32_t sampleFreq)
{
  // Enable ADC clock
  CMU_ClockEnable(cmuClock_ADC0, true);

  // Configure ADC to sample at desired frequency
  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
  init.timebase = ADC_TimebaseCalc(0);

  // Set prescale for desired sample frequency
  // prescale = ADC0_ClockFreq / (sampleFreq * (ACQTIME + ADC_RESOLUTION + 1))
  init.prescale =
    (CMU_ClockFreqGet(cmuClock_ADC0)) / (sampleFreq * (8 + 12 + 1));

  init.warmUpMode = adcWarmupKeepADCWarm;
  ADC_Init(ADC0, &init);

  // Configure ADC Single sample mode
  ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;

  // Set desired reference and set acq. time
  singleInit.reference  = VREF;
  singleInit.acqTime = ADC_ACQTIME;

  // Highest available resolution
  singleInit.resolution = ADC_RESOLUTION;

  // Loop continuously
  singleInit.rep = true;

  // Set GPIO Pin PB11 (Expansion Header Pin 11) as input
  singleInit.posSel = adcPosSelAPORT2XCH27;
  singleInit.negSel = adcNegSelVSS;

  // Start ADC
  ADC_InitSingle(ADC0, &singleInit);
  ADC_Start(ADC0, adcStartSingle);
}

/**************************************************************************//**
 * @brief Configure and start DMA
 *****************************************************************************/
void ADCMODE_InitLDMA(uint32_t *bufferA, uint32_t *bufferB, uint32_t size)
{
  // Default LDMA init
  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init(&init);

  // Configure LDMA for transfer from ADC to memory
  // LDMA will loop continuously
  LDMA_TransferCfg_t periTransferTx =
    LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_ADC0_SINGLE);

  // Set up descriptors for dual buffer transfer
  LDMA_Descriptor_t xfer[] = {
    LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&ADC0->SINGLEDATA, bufferA, size, 1),
    LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&ADC0->SINGLEDATA, bufferB, size, -1)
  };

  descLink[0] = xfer[0];
  descLink[1] = xfer[1];

  // Transfer 16 bits per unit, increment by 32 bits
  descLink[0].xfer.size = ldmaCtrlSizeWord;
  // Interrupt after transfer complete
  descLink[0].xfer.doneIfs = 1;
  descLink[0].xfer.ignoreSrec = 0;

  // Transfer 16 bits per unit, increment by 32 bits
  descLink[1].xfer.size = ldmaCtrlSizeWord;
  // Interrupt after transfer complete
  descLink[1].xfer.doneIfs = 1;
  descLink[1].xfer.ignoreSrec = 0;

  // Start transfer, LDMA will continuously sample the ADC
  LDMA_StartTransfer(0, (void*)&periTransferTx, (void*)&descLink);
}
