/***************************************************************************//**
 * @file
 * @brief Setup LCD for energy mode demo
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

#include "lcd_setup.h"
#include "bspconfig.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_pcnt.h"
#include "em_assert.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "segmentlcd.h"

static volatile Energy_Mode_TypeDef eMode; // Selected energy mode.
static volatile int startTest;             // Start selected energy mode test.
static volatile bool displayEnabled;       // Status of LCD display.
static volatile uint32_t seconds = 0;      // Seconds elapsed since reset.

static void gpioSetup(void);

/***************************************************************************//**
 * @brief  Selects eMode using the LCD and buttons.
 *
 * @return Energy_Mode_TypeDef selected eMode.
 ******************************************************************************/
Energy_Mode_TypeDef LCD_SelectMode(void)
{
  Energy_Mode_TypeDef currentEMode;

  // Setup GPIO for pushbuttons.
  gpioSetup();

  // Enable LCD without voltage boost
  SegmentLCD_Init(false);

  startTest = false;
  eMode = (Energy_Mode_TypeDef)0;
  currentEMode = NUM_EMODES;

  while (!startTest) {
    // Mode change ? If yes, update the display.
    if (eMode != currentEMode) {
      currentEMode = eMode;
      switch (eMode) {
        case EM0_HFXO_48MHZ_WHILE:
          SegmentLCD_Number(48000);
          SegmentLCD_Write("EM0-X-W");
          break;
        case EM0_HFRCO_48MHZ_WHILE:
          SegmentLCD_Number(48000);
          SegmentLCD_Write("EM0-R-W");
          break;
        case EM0_HFRCO_48MHZ_PRIME:
          SegmentLCD_Number(48000);
          SegmentLCD_Write("EM0-R-P");
          break;
        case EM0_HFRCO_48MHZ_COREMARK:
          SegmentLCD_Number(48000);
          SegmentLCD_Write("EM0-R-C");
          break;
        case EM0_HFRCO_19MHZ_WHILE:
          SegmentLCD_Number(19000);
          SegmentLCD_Write("EM0-R-W");
          break;
        case EM0_HFRCO_1MHZ_WHILE:
          SegmentLCD_Number(1000);
          SegmentLCD_Write("EM0-R-W");
          break;
        case EM1_HFXO_48MHZ:
          SegmentLCD_Number(48000);
          SegmentLCD_Write("EM1-X");
          break;
        case EM1_HFRCO_48MHZ:
          SegmentLCD_Number(48000);
          SegmentLCD_Write("EM1-R");
          break;
        case EM1_HFRCO_19MHZ:
          SegmentLCD_Number(19000);
          SegmentLCD_Write("EM1-R");
          break;
        case EM1_HFRCO_1MHZ:
          SegmentLCD_Number(1000);
          SegmentLCD_Write("EM1-R");
          break;
        case EM2_LFXO_RTCC:
          SegmentLCD_Number(32);
          SegmentLCD_Write("EM2-X-R");
          break;
        case EM2_LFRCO_RTCC:
          SegmentLCD_Number(32);
          SegmentLCD_Write("EM2-R-R");
          break;
        case EM3_ULFRCO_CRYO:
          SegmentLCD_Number(1);
          SegmentLCD_Write("EM3-R-C");
          break;
        case EM4H_LFXO_RTCC:
          SegmentLCD_Number(32);
          SegmentLCD_Write("EM4H-X-R");
          break;
        case EM4H_ULFRCO_CRYO:
          SegmentLCD_Number(1);
          SegmentLCD_Write("EM4H-R-C");
          break;
        case EM4H:
          SegmentLCD_NumberOff();
          SegmentLCD_Write("EM4H");
          break;
        case EM4S:
          SegmentLCD_NumberOff();
          SegmentLCD_Write("EM4S");
          break;
        case EM0_HFXO_48MHZ_WHILE_DCDC:
          SegmentLCD_Number(48000);
          SegmentLCD_Write("EM0-XDW");
          break;
        case EM0_HFRCO_48MHZ_WHILE_DCDC:
          SegmentLCD_Number(48000);
          SegmentLCD_Write("EM0-RDW");
          break;
        case EM0_HFRCO_48MHZ_PRIME_DCDC:
          SegmentLCD_Number(48000);
          SegmentLCD_Write("EM0-RDP");
          break;
        case EM0_HFRCO_48MHZ_COREMARK_DCDC:
          SegmentLCD_Number(48000);
          SegmentLCD_Write("EM0-RDC");
          break;
        case EM0_HFRCO_19MHZ_WHILE_DCDC:
          SegmentLCD_Number(19000);
          SegmentLCD_Write("EM0-RDW");
          break;
        case EM0_HFRCO_1MHZ_WHILE_DCDC:
          SegmentLCD_Number(1000);
          SegmentLCD_Write("EM0-RDW");
          break;
        case EM1_HFXO_48MHZ_DCDC:
          SegmentLCD_Number(48000);
          SegmentLCD_Write("EM1-XD");
          break;
        case EM1_HFRCO_48MHZ_DCDC:
          SegmentLCD_Number(48000);
          SegmentLCD_Write("EM1-RD");
          break;
        case EM1_HFRCO_19MHZ_DCDC:
          SegmentLCD_Number(19000);
          SegmentLCD_Write("EM1-RD");
          break;
        case EM1_HFRCO_1MHZ_DCDC:
          SegmentLCD_Number(1000);
          SegmentLCD_Write("EM1-RD");
          break;
        case EM2_LFXO_RTCC_DCDC:
          SegmentLCD_Number(32);
          SegmentLCD_Write("EM2-XDR");
          break;
        case EM2_LFRCO_RTCC_DCDC:
          SegmentLCD_Number(32);
          SegmentLCD_Write("EM2-RDR");
          break;
        case EM3_ULFRCO_CRYO_DCDC:
          SegmentLCD_Number(1);
          SegmentLCD_Write("EM3-RDC");
          break;
        case EM4H_LFXO_RTCC_DCDC:
          SegmentLCD_Number(32);
          SegmentLCD_Write("EM4H-XDR");
          break;
        case EM4H_ULFRCO_CRYO_DCDC:
          SegmentLCD_Number(1);
          SegmentLCD_Write("EM4H-RDC");
          break;
        case EM4H_DCDC:
          SegmentLCD_NumberOff();
          SegmentLCD_Write("EM4H-D");
          break;
        case EM4S_DCDC:
          SegmentLCD_NumberOff();
          SegmentLCD_Write("EM4S-D");
          break;
        default:
          EFM_ASSERT(false);
          break;
      }
    }
  }

  // Get ready to start the energy mode test. Turn off everything we do not need.

  // Disable GPIO.
  NVIC_DisableIRQ(GPIO_EVEN_IRQn);
  NVIC_DisableIRQ(GPIO_ODD_IRQn);
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeDisabled, 1);
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeDisabled, 1);

  // Clear LCD display.
  SegmentLCD_AllOff();

  // Power down LCD display and disable the RTCC.
  displayEnabled = false;
  SegmentLCD_Disable();
  NVIC_DisableIRQ(RTCC_IRQn);
  NVIC_DisableIRQ(PCNT0_IRQn);

  // Return desired energy mode.
  return eMode;
}

/***************************************************************************//**
 * @brief Setup GPIO interrupt for pushbuttons.
 ******************************************************************************/
static void gpioSetup(void)
{
  // Enable GPIO clock.
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PB0 as input and enable interrupt.
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);

  // Configure PB1 as input and enable interrupt.
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, false, true, true);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/***************************************************************************//**
 * @brief Unified GPIO Interrupt handler (pushbuttons).
 *        PB0 Starts selected test.
 *        PB1 Cycles through the available tests.
 ******************************************************************************/
void GPIO_Unified_IRQ(void)
{
  // Get and clear all pending GPIO interrupts.
  uint32_t interruptMask = GPIO_IntGet();
  GPIO_IntClear(interruptMask);

  // Act on interrupts.
  if (interruptMask & (1 << BSP_GPIO_PB0_PIN)) {
    // PB0: Start test.
    startTest = true;
  }

  if (interruptMask & (1 << BSP_GPIO_PB1_PIN)) {
    // PB1: cycle through tests.
    eMode = (Energy_Mode_TypeDef)(((int)eMode + 1) % (int)NUM_EMODES);
  }
}

/***************************************************************************//**
 * @brief GPIO Interrupt handler for even pins.
 ******************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  GPIO_Unified_IRQ();
}

/***************************************************************************//**
 * @brief GPIO Interrupt handler for odd pins.
 ******************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  GPIO_Unified_IRQ();
}

/***************************************************************************//**
 * @brief   This interrupt is triggered at every second by the PCNT.
 ******************************************************************************/
void PCNT0_IRQHandler(void)
{
  PCNT_IntClear(PCNT0, PCNT_IF_OF);

  seconds++;
}
