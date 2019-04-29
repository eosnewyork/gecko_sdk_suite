/***************************************************************************//**
 * @file
 * @brief LCD controller and Energy Mode/RTC demo for EFM32TG_STK3300
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

#include <stdint.h>
#include <stdbool.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_rtc.h"
#include "em_lcd.h"
#include "vddcheck.h"
#include "segmentlcd.h"
#include "bsp_trace.h"

#define RTC_FREQ    32768

/* Initial setup to 12:00 */
uint32_t hours   = 12;
uint32_t minutes = 0;

/* This flag enables/disables vboost on the LCD */
bool oldBoost = false;

/***************************************************************************//**
 * @brief GPIO Interrupt handler (PB11)
 *        Sets the hours
 ******************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  /* Acknowledge interrupt */
  GPIO_IntClear(1 << 11);

  /* Increase minutes */
  hours = (hours + 1) % 24;
}

/***************************************************************************//**
 * @brief GPIO Interrupt handler (PD8)
 *        Sets the minutes
 ******************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  /* Acknowledge interrupt */
  GPIO_IntClear(1 << 8);

  /* Increase minutes */
  minutes = (minutes + 1) % 60;
}

/***************************************************************************//**
 * @brief Setup GPIO interrupt to set the time
 ******************************************************************************/
void gpioSetup(void)
{
  /* Configure PD8 as input */
  GPIO_PinModeSet(gpioPortD, 8, gpioModeInput, 0);

  /* Set falling edge interrupt */
  GPIO_IntConfig(gpioPortD, 8, false, true, true);
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  /* Configure PB11 as input */
  GPIO_PinModeSet(gpioPortB, 11, gpioModeInput, 0);

  /* Set falling edge interrupt */
  GPIO_IntConfig(gpioPortB, 11, false, true, true);
  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/***************************************************************************//**
 * @brief RTC Interrupt Handler.
 *        Updates minutes and hours.
 ******************************************************************************/
void RTC_IRQHandler(void)
{
  /* Clear interrupt source */
  RTC_IntClear(RTC_IFC_COMP0);

  /* Increase time by one minute */
  minutes++;
  if (minutes > 59) {
    minutes = 0;
    hours++;
    if (hours > 23) {
      hours = 0;
    }
  }
}

/***************************************************************************//**
 * @brief Enables LFACLK and selects LFXO as clock source for RTC
 *        Sets up the RTC to generate an interrupt every minute.
 ******************************************************************************/
void rtcSetup(void)
{
  RTC_Init_TypeDef rtcInit = RTC_INIT_DEFAULT;

  /* Enable LE domain registers */
  CMU_ClockEnable(cmuClock_CORELE, true);

  /* Enable LFXO as LFACLK in CMU. This will also start LFXO */
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);

  /* Set a clock divisor of 32 to reduce power conumption. */
  CMU_ClockDivSet(cmuClock_RTC, cmuClkDiv_32);

  /* Enable RTC clock */
  CMU_ClockEnable(cmuClock_RTC, true);

  /* Initialize RTC */
  rtcInit.enable   = false;  /* Do not start RTC after initialization is complete. */
  rtcInit.debugRun = false;  /* Halt RTC when debugging. */
  rtcInit.comp0Top = true;   /* Wrap around on COMP0 match. */
  RTC_Init(&rtcInit);

  /* Interrupt every minute */
  RTC_CompareSet(0, ((RTC_FREQ / 32) * 60) - 1);

  /* Enable interrupt */
  NVIC_EnableIRQ(RTC_IRQn);
  RTC_IntEnable(RTC_IEN_COMP0);

  /* Start Counter */
  RTC_Enable(true);
}

/***************************************************************************//**
 * @brief Check input voltage and enable vboost if it drops too low.
 ******************************************************************************/
void checkVoltage(void)
{
  bool vboost;

  /* Initialize voltage comparator */
  VDDCHECK_Init();

  /* Check if voltage is below 3V, if so use voltage boost */
  if (VDDCHECK_LowVoltage(2.9)) {
    vboost = true;
  } else {
    vboost = false;
  }

  /* Disable Voltage Comparator */
  VDDCHECK_Disable();

  if (vboost != oldBoost) {
    /* Reinitialize with new vboost setting */
    SegmentLCD_Init(vboost);
    /* Use Antenna symbol to signify enabling of vboost */
    SegmentLCD_Symbol(LCD_SYMBOL_ANT, vboost);
    oldBoost = vboost;
  }
}

/***************************************************************************//**
 * @brief Update clock and wait in EM2 for RTC tick.
 ******************************************************************************/
void clockLoop(void)
{
  LCD_FrameCountInit_TypeDef frameInit;
  LCD_AnimInit_TypeDef animInit;

  /* Write Gecko and display, and light up the colon between hours and minutes. */
  SegmentLCD_Symbol(LCD_SYMBOL_COL10, 1);
  SegmentLCD_Write("Gecko");

  /* Setup frame counter */
  frameInit.enable   = true;           /* Enable framecounter */
  frameInit.top      = 15;             /* Generate event every 15 frames. */
  frameInit.prescale = lcdFCPrescDiv1; /* No prescaling */

  LCD_FrameCountInit(&frameInit);

  /* Animate rings */
  animInit.enable    = true;             /* Enable animation after initialization. */
  animInit.AReg      = 0x00;             /* Initial A register value */
  animInit.BReg      = 0x33;             /* Initial B register value */
  animInit.AShift    = lcdAnimShiftLeft; /* Shift A register left */
  animInit.BShift    = lcdAnimShiftLeft; /* Shift A register left */
  animInit.animLogic = lcdAnimLogicOr;   /* Enable segment if A or B */

  LCD_AnimInit(&animInit);

  while (1) {
    checkVoltage();
    SegmentLCD_Number(hours * 100 + minutes);
    EMU_EnterEM2(true);
  }
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* Ensure core frequency has been updated */
  SystemCoreClockUpdate();

  /* If first word of user data page is non-zero, enable Energy Profiler trace */
  BSP_TraceProfilerSetup();

  /* Init LCD with no voltage boost */
  SegmentLCD_Init(oldBoost);

  /* Setup RTC to generate an interrupt every minute */
  rtcSetup();

  /* Enable GPIO clock */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Setup GPIO interrupt to set the time */
  gpioSetup();

  /* Main function loop */
  clockLoop();

  return 0;
}
