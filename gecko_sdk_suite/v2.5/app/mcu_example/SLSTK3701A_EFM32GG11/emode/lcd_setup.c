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

#define USE_RETARGETSERIAL      0
#define VT100_CURSOR_HOME       "\033[H"
#define VT100_CLEAR_SCREEN      "\033[2J"

#include "lcd_setup.h"
#if USE_RETARGETSERIAL
#include "retargetserial.h"
#else
#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"
#endif
#include "bspconfig.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_pcnt.h"
#include "em_assert.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

static volatile Energy_Mode_TypeDef eMode; // Selected energy mode.
static volatile int startTest;             // Start selected energy mode test.
static volatile bool displayEnabled;       // Status of LCD display.
static volatile uint32_t seconds = 0;      // Seconds elapsed since reset.
#if USE_RETARGETSERIAL
#else
static DISPLAY_Device_t displayDevice;     // Display device handle.
#endif
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

#if USE_RETARGETSERIAL
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);
  printf(VT100_CURSOR_HOME);
  printf(VT100_CLEAR_SCREEN);
#else
  // Initialize the display module.
  displayEnabled = true;
  DISPLAY_Init();

  // Retrieve the properties of the display.
  if (DISPLAY_DeviceGet(0, &displayDevice) != DISPLAY_EMSTATUS_OK) {
    // Unable to get display handle.
    EFM_ASSERT(false);
  }

  // Retarget stdio to the display.
  if (TEXTDISPLAY_EMSTATUS_OK != RETARGET_TextDisplayInit()) {
    // Text display initialization failed.
    EFM_ASSERT(false);
  }
#endif

  printf("\n  ENERGY MODE"
         "\n    CURRENT"
         "\n  CONSUMPTION"
         "\n\n\n  Push PB1 to\n"
         "  cycle modes\n"
         "\n  Push PB0 to\n"
         "  start test \n\n");

  startTest = false;
  eMode = (Energy_Mode_TypeDef)0;
  currentEMode = NUM_EMODES;

  while (!startTest) {
    // Mode change ? If yes, update the display.
    if (eMode != currentEMode) {
      currentEMode = eMode;
      switch (eMode) {
        case EM0_HFXO_50MHZ_WHILE:
          printf("\r  EM0 50MHz     \n"
                 "  Without DC/DC \n"
                 "  (while loop)  \n");
          break;
        case EM0_HFRCO_72MHZ_PRIME:
          printf("\r  EM0 72MHz     \n"
                 "  Without DC/DC \n"
                 "  (primes calc) \n");
          break;
        case EM0_HFRCO_72MHZ_WHILE:
          printf("\r  EM0 72MHz     \n"
                 "  Without DC/DC \n"
                 "  (while loop)  \n");
          break;
        case EM0_HFRCO_72MHZ_COREMARK:
          printf("\r  EM0 72MHz     \n"
                 "  Without DC/DC \n"
                 "  (CoreMark)    \n");
          break;
        case EM0_HFRCO_64MHZ_WHILE:
          printf("\r  EM0 64MHz     \n"
                 "  Without DC/DC \n"
                 "  (while loop)  \n");
          break;
        case EM0_HFRCO_56MHZ_WHILE:
          printf("\r  EM0 56MHz     \n"
                 "  Without DC/DC \n"
                 "  (while loop)  \n");
          break;
        case EM0_HFRCO_48MHZ_WHILE:
          printf("\r  EM0 48MHz     \n"
                 "  Without DC/DC \n"
                 "  (while loop)  \n");
          break;
        case EM0_HFRCO_38MHZ_PRIME:
          printf("\r  EM0 38MHz     \n"
                 "  Without DC/DC \n"
                 "  (primes calc) \n");
          break;
        case EM0_HFRCO_38MHZ_WHILE:
          printf("\r  EM0 38MHz     \n"
                 "  Without DC/DC \n"
                 "  (while loop)  \n");
          break;
        case EM0_HFRCO_38MHZ_COREMARK:
          printf("\r  EM0 38MHz     \n"
                 "  Without DC/DC \n"
                 "  (CoreMark)    \n");
          break;
        case EM0_HFRCO_26MHZ_WHILE:
          printf("\r  EM0 26MHz     \n"
                 "  Without DC/DC \n"
                 "  (while loop)  \n");
          break;
        case EM0_HFRCO_1MHZ_WHILE:
          printf("\r  EM0 1MHz      \n"
                 "  Without DC/DC \n"
                 "  (while loop)  \n");
          break;
        case EM1_HFXO_50MHZ:
          printf("\r  EM1 50MHz     \n"
                 "  Without DC/DC \n"
                 "                \n");
          break;
        case EM1_HFRCO_72MHZ:
          printf("\r  EM1 72MHz     \n"
                 "  Without DC/DC \n");
          break;
        case EM1_HFRCO_64MHZ:
          printf("\r  EM1 64MHz     \n"
                 "  Without DC/DC \n");
          break;
        case EM1_HFRCO_56MHZ:
          printf("\r  EM1 56MHz     \n"
                 "  Without DC/DC \n");
          break;
        case EM1_HFRCO_48MHZ:
          printf("\r  EM1 48MHz     \n"
                 "  Without DC/DC \n");
          break;
        case EM1_HFRCO_38MHZ:
          printf("\r  EM1 38MHz     \n"
                 "  Without DC/DC \n");
          break;
        case EM1_HFRCO_26MHZ:
          printf("\r  EM1 26MHz     \n"
                 "  Without DC/DC \n");
          break;
        case EM1_HFRCO_1MHZ:
          printf("\r  EM1 1MHz      \n"
                 "  Without DC/DC \n");
          break;
        case EM2_LFXO_RTCC:
          printf("\r  EM2 RTCC LFXO \n"
                 "  Without DC/DC \n"
                 "  (full RAM)    \n");
          break;
        case EM2_LFRCO_RTCC:
          printf("\r  EM2 RTCC LFRCO\n"
                 "  Without DC/DC \n"
                 "  (16kB RAM)     \n");
          break;
        case EM3_ULFRCO_CRYO:
          printf("\r EM3 CRYO ULFRCO\n"
                 "  Without DC/DC \n"
                 "  (full RAM)    \n");
          break;
        case EM4H_LFXO_RTCC:
          printf("\r EM4H RTCC LFXO \n"
                 "  Without DC/DC \n"
                 "  (128b RAM)    \n");
          break;
        case EM4H_ULFRCO_CRYO:
          printf("\rEM4H CRYO ULFRCO\n"
                 "  Without DC/DC \n"
                 "  (128b RAM)    \n");
          break;
        case EM4H:
          printf("\r  EM4H          \n"
                 "  Without DC/DC \n"
                 "  (128b RAM)    \n");
          break;
        case EM4S:
          printf("\r  EM4S          \n"
                 "  Without DC/DC \n"
                 "  (no RAM)      \n");
          break;
        case EM0_HFXO_50MHZ_WHILE_DCDC:
          printf("\r  EM0 50MHz     \n"
                 "  With DC/DC    \n"
                 "  (while loop)  \n");
          break;
        case EM0_HFRCO_72MHZ_PRIME_DCDC:
          printf("\r  EM0 72MHz     \n"
                 "  With DC/DC    \n"
                 "  (primes calc) \n");
          break;
        case EM0_HFRCO_72MHZ_WHILE_DCDC:
          printf("\r  EM0 72MHz     \n"
                 "  With DC/DC    \n"
                 "  (while loop)  \n");
          break;
        case EM0_HFRCO_72MHZ_COREMARK_DCDC:
          printf("\r  EM0 72MHz     \n"
                 "  With DC/DC    \n"
                 "  (CoreMark)    \n");
          break;
        case EM0_HFRCO_64MHZ_WHILE_DCDC:
          printf("\r  EM0 64MHz     \n"
                 "  With DC/DC    \n"
                 "  (while loop)  \n");
          break;
        case EM0_HFRCO_56MHZ_WHILE_DCDC:
          printf("\r  EM0 56MHz     \n"
                 "  With DC/DC    \n"
                 "  (while loop)  \n");
          break;
        case EM0_HFRCO_48MHZ_WHILE_DCDC:
          printf("\r  EM0 48MHz     \n"
                 "  With DC/DC    \n"
                 "  (while loop)  \n");
          break;
        case EM0_HFRCO_38MHZ_PRIME_DCDC:
          printf("\r  EM0 38MHz     \n"
                 "  With DC/DC    \n"
                 "  (primes calc) \n");
          break;
        case EM0_HFRCO_38MHZ_WHILE_DCDC:
          printf("\r  EM0 38MHz     \n"
                 "  With DC/DC    \n"
                 "  (while loop)  \n");
          break;
        case EM0_HFRCO_38MHZ_COREMARK_DCDC:
          printf("\r  EM0 38MHz     \n"
                 "  With DC/DC    \n"
                 "  (CoreMark)    \n");
          break;
        case EM0_HFRCO_26MHZ_WHILE_DCDC:
          printf("\r  EM0 26MHz     \n"
                 "  With DC/DC    \n"
                 "  (while loop)  \n");
          break;
        case EM1_HFXO_50MHZ_DCDC:
          printf("\r  EM1 50MHz     \n"
                 "  With DC/DC    \n"
                 "                \n");
          break;
        case EM1_HFRCO_72MHZ_DCDC:
          printf("\r  EM1 72MHz     \n"
                 "  With DC/DC    \n");
          break;
        case EM1_HFRCO_64MHZ_DCDC:
          printf("\r  EM1 64MHz     \n"
                 "  With DC/DC    \n");
          break;
        case EM1_HFRCO_56MHZ_DCDC:
          printf("\r  EM1 56MHz     \n"
                 "  With DC/DC    \n");
          break;
        case EM1_HFRCO_48MHZ_DCDC:
          printf("\r  EM1 48MHz     \n"
                 "  With DC/DC    \n");
          break;
        case EM1_HFRCO_38MHZ_DCDC:
          printf("\r  EM1 38MHz     \n"
                 "  With DC/DC    \n");
          break;
        case EM1_HFRCO_26MHZ_DCDC:
          printf("\r  EM1 26MHz     \n"
                 "  With DC/DC    \n");
          break;
        case EM1_HFRCO_1MHZ_DCDC:
          printf("\r  EM1 1MHz      \n"
                 "  With DC/DC    \n");
          break;
        case EM2_LFXO_RTCC_DCDC:
          printf("\r  EM2 RTCC LFXO \n"
                 "  With DC/DC    \n"
                 "  (full RAM)    \n");
          break;
        case EM2_LFRCO_RTCC_DCDC:
          printf("\r  EM2 RTCC LFRCO\n"
                 "  With DC/DC    \n"
                 "  (16kB RAM)     \n");
          break;
        case EM3_ULFRCO_CRYO_DCDC:
          printf("\r EM3 CRYO ULFRCO\n"
                 "  With DC/DC    \n"
                 "  (full RAM)    \n");
          break;
        case EM4H_LFXO_RTCC_DCDC:
          printf("\r EM4H RTCC LFXO \n"
                 "  With DC/DC    \n"
                 "  (128b RAM)    \n");
          break;
        case EM4H_ULFRCO_CRYO_DCDC:
          printf("\rEM4H CRYO ULFRCO\n"
                 "  With DC/DC    \n"
                 "  (128b RAM)    \n");
          break;
        case EM4H_DCDC:
          printf("\r  EM4H          \n"
                 "  With DC/DC    \n"
                 "  (128b RAM)    \n");
          break;
        case EM4S_DCDC:
          printf("\r  EM4S          \n"
                 "  With DC/DC    \n"
                 "  (no RAM)      \n");
          break;
        default:
          EFM_ASSERT(false);
          break;
      }
#if USE_RETARGETSERIAL
      printf(VT100_CURSOR_HOME);
#else
      printf(TEXTDISPLAY_ESC_SEQ_CURSOR_HOME_VT100);
#endif
      printf("\n\n\n\n\n\n\n\n\n\n\n\n");
    }
  }

  // Get ready to start the energy mode test. Turn off everything we do not need.

  // Disable GPIO.
  NVIC_DisableIRQ(GPIO_EVEN_IRQn);
  NVIC_DisableIRQ(GPIO_ODD_IRQn);
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeDisabled, 1);
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeDisabled, 1);

  // Clear LCD display.
  printf("\f");

  // Power down LCD display and disable the RTCC.
  displayEnabled = false;
  NVIC_DisableIRQ(RTCC_IRQn);
  NVIC_DisableIRQ(PCNT0_IRQn);
#if USE_RETARGETSERIAL
#else
  displayDevice.pDisplayPowerOn(&displayDevice, false);
#endif

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
