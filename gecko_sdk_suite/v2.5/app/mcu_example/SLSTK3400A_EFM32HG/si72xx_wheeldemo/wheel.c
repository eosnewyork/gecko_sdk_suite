/***************************************************************************//**
 * @file
 * @brief Hall Effect Wheel Demo for SLSTK3400A_EFM32HG
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

#include "em_device.h"
#include "em_chip.h"
#include "em_system.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_i2c.h"
#include "i2cspm.h"
#include "rtcdriver.h"
#include "bspconfig.h"

#include "si72xx.h"

#include "graphics.h"
#include "wheel.h"
#include "exp_si72xx.h"
#include "exp_si72xx_cal.h"
#include "ps_si7210.h"
#include "ps_si7211.h"
#include "ps_si7212.h"
#include "ps_si7213.h"

#define REFRESH_RATE              1000
RTCDRV_TimerID_t periodicUpdateTimerId;

volatile DemoNo_t demoSelection = menuScreen;
volatile bool pb0Pressed = false;
volatile bool pb1Pressed = false;
volatile bool expCalibrationFlag = false;
volatile bool expOutToggled = false;
volatile bool demoInitiated = false;

/**************************************************************************//**
 * Local prototype declarations
 *****************************************************************************/
static void setupAndStartRtcTimer(void);
static void i2cSetup(void);
static void gpioSetup (void);
void GPIO_Unified_IRQ(void);
static bool isPb1Pressed(void);
static void disableDemos(void);

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  gpioSetup();
  i2cSetup();
  GRAPHICS_SetupGraphics();
  setupAndStartRtcTimer();

  for (;; ) {
    switch (demoSelection) {
      case menuScreen:
        EXP_SI72XX_runMenuScreen();
        break;
      case expAnglePosition:
        EXP_SI72XX_runAngleDemo();
        break;
      case expRevolutionCounter:
        EXP_SI72XX_runRevolutionCounterDemo();
        break;
      case psI2cFieldData:
        PS_SI7210_runMagneticFieldDemo();
        break;
      case psI2cTempData:
        PS_SI7210_runTemperatureDemo();
        break;
      case psSwitchLatch:
        PS_SI7210_runSwitchDemo();
        break;
      case psAnalogOut:
        PS_SI7211_runAnalogDemo();
        break;
      case psPwmOut:
        PS_SI7212_runPwmDemo();
        break;
      case psSentOut:
        PS_SI7213_runSentDemo();
        break;

      default:
        demoSelection = menuScreen;
        break;
    }
    if (!pb1Pressed) {
      EMU_EnterEM2(true);
    }
  }
}

/**************************************************************************//**
 * @brief Configure RTC Timer for 1 second interrupts
 *****************************************************************************/
static void setupAndStartRtcTimer(void)
{
  CMU_OscillatorEnable(cmuOsc_LFXO, true, false);
  RTCDRV_Init();
  RTCDRV_AllocateTimer(&periodicUpdateTimerId);
  RTCDRV_StartTimer(periodicUpdateTimerId, rtcdrvTimerTypePeriodic,
                    REFRESH_RATE, NULL, NULL);
}

/**************************************************************************//**
 * @brief I2C Initialization
 *****************************************************************************/
static void i2cSetup(void)
{
  I2CSPM_Init_TypeDef    i2c = I2CSPM_INIT_DEFAULT;
  I2CSPM_Init(&i2c);
}

/**************************************************************************//**
 * @brief Setup GPIO interrupt for pushbuttons.
 *****************************************************************************/
static void gpioSetup(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);
  /* Enable PRS sense on GPIO */
  GPIO_InputSenseSet((GPIO_INSENSE_PRS | GPIO_INSENSE_INT),
                     _GPIO_INSENSE_RESETVALUE);

  /* Configure PB0 as input and enable interrupt  */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);
  /* Configure PB1 as input and enable interrupt */
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, false, true, true);

  /* Rev2 Wheel-EXP OUTs C_0:(U2:0x31) & C_1:(U1:0x32) */
  GPIO_PinModeSet(SI72XXEXP_PORT, SI72XXEXP_U2_PIN, gpioModeInput, 1);
  GPIO_PinModeSet(SI72XXEXP_PORT, SI72XXEXP_U1_PIN, gpioModeInput, 1);
  GPIO_IntConfig(SI72XXEXP_PORT, SI72XXEXP_U2_PIN, true, true, false);
  GPIO_IntConfig(SI72XXEXP_PORT, SI72XXEXP_U1_PIN, true, true, false);

  /* Postage Stamp OUT D_4:(U0:0x30) */
  GPIO_PinModeSet(SI72XXPS_OUT_PORT, SI72XXPS_OUT_PIN, gpioModeInput, 0);
  GPIO_IntConfig(SI72XXPS_OUT_PORT, SI72XXPS_OUT_PIN, true, true, false);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);
  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/**************************************************************************//**
 * @brief Unified GPIO Interrupt handler (pushbuttons)
 *        PB0 Starts selected test
 *        PB1 Cycles through the available tests
 *****************************************************************************/
void GPIO_Unified_IRQ(void)
{
  /* Get and clear all pending GPIO interrupts */
  uint32_t interruptMask = GPIO_IntGet();
  GPIO_IntClear(interruptMask);

  if (interruptMask & (1 << BSP_GPIO_PB0_PIN)) {
    /* Push Button PB0 */
    if (demoSelection == menuScreen) {
      pb0Pressed = true;
    } else {
      if ((demoSelection == expAnglePosition) && (isPb1Pressed() == true)) {
        expCalibrationFlag = true;
      } else {
        demoSelection = menuScreen;
        disableDemos();
      }
    }
  }

  if (interruptMask & (1 << BSP_GPIO_PB1_PIN)) {
    /* Push Button PB1 */
    pb1Pressed = true;
  }

  if (interruptMask & ((1 << SI72XXEXP_U1_PIN) | (1 << SI72XXEXP_U2_PIN))) {
    /* EXP U1 or U2 output pin toggled */
    if (demoSelection == menuScreen) {
      expOutToggled = true;
    }
  }
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler for even pins
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  GPIO_Unified_IRQ();
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler for odd pins
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  GPIO_Unified_IRQ();
}

void gpioEnablePushButton1(void)
{
  GPIO_IntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, false, true, true);
}

void gpioDisablePushButton1(void)
{
  GPIO_IntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, false, true, false);
}

static bool isPb1Pressed(void)
{
  return !GPIO_PinInGet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN);
}

static void disableDemos(void)
{
  EXP_SI72XX_disableRevolutionCounterDemo();
  PS_SI7210_disableSwitchDemo();
  PS_SI7212_disablePwmDemo();
  PS_SI7213_disableSentDemo();
}
