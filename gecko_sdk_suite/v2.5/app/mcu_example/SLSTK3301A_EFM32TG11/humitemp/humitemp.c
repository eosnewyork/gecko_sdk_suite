/***************************************************************************//**
 * @file
 * @brief Relative humidity and temperature sensor demo
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
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "i2cspm.h"
#include "si7013.h"
#include "rtcdriver.h"
#include "bspconfig.h"
#include "segmentlcd.h"

/***************************************************************************//**
 * Local defines
 ******************************************************************************/

/** Time (in ms) between periodic updates of the measurements. */
#define PERIODIC_UPDATE_MS      1000

#define SENSOR_EN_PORT          gpioPortC
#define SENSOR_EN_PIN           12

/***************************************************************************//**
 * Local variables
 ******************************************************************************/
/* RTC callback parameters. */

/** This flag tracks if we need to update the display
 *  (animations or measurements). */
static volatile bool updateDisplay = true;

/** This flag tracks if we need to perform a new
 *  measurement. */
static volatile bool updateMeasurement = true;

/** Timer used for periodic update of the measurements. */
RTCDRV_TimerID_t periodicUpdateTimerId;

/***************************************************************************//**
 * @brief Setup GPIO, enable sensor isolation switch
 ******************************************************************************/
static void gpioSetup(void)
{
  /* Enable GPIO clock */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Enable sensors power and I2C isolation switch */
  GPIO_PinModeSet(SENSOR_EN_PORT, SENSOR_EN_PIN, gpioModePushPull, 1);
}

/***************************************************************************//**
 * @brief Callback used to count between measurement updates
 ******************************************************************************/
static void periodicUpdateCallback(RTCDRV_TimerID_t id, void *user)
{
  (void) id;
  (void) user;
  updateDisplay = true;
  updateMeasurement = true;
}

/***************************************************************************//**
 * @brief  Helper function to perform data measurements.
 ******************************************************************************/
static void performMeasurements(I2C_TypeDef *i2c, uint32_t *rhData, int32_t *tData)
{
  Si7013_MeasureRHAndTemp(i2c, SI7021_ADDR, rhData, tData);
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  I2CSPM_Init_TypeDef i2cInit = I2CSPM_INIT_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;
  bool             detected;
  uint32_t         rhData = 0;
  int32_t          tempData = 0;

  /* Chip errata */
  CHIP_Init();

  /* Initalize hardware */
  CMU_HFXOInit(&hfxoInit);

  /* Switch HFCLK to HFXO and disable HFRCO */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);

  /* Enable LCD without voltage boost */
  SegmentLCD_Init(false);

  /* Initalize other peripherals and drivers */
  gpioSetup();
  RTCDRV_Init();
  I2CSPM_Init(&i2cInit);

  // Show some nice segments
  SegmentLCD_Symbol(LCD_SYMBOL_GECKO, true);
  SegmentLCD_Symbol(LCD_SYMBOL_EFM32, true);

  /* Get initial sensor status */
  detected = Si7013_Detect(i2cInit.port, SI7021_ADDR, 0);
  SegmentLCD_Write(detected ? "Detected" : "-");

  /* Set up periodic update of the display. */
  RTCDRV_AllocateTimer(&periodicUpdateTimerId);
  RTCDRV_StartTimer(periodicUpdateTimerId, rtcdrvTimerTypePeriodic,
                    PERIODIC_UPDATE_MS, periodicUpdateCallback, 0);

  EMU_EnterEM2(false);

  while (true) {
    if (updateMeasurement) {
      performMeasurements(i2cInit.port, &rhData, &tempData);
      updateMeasurement = false;
    }

    if (updateDisplay) {
      updateDisplay = false;
      SegmentLCD_Number(rhData);
      SegmentLCD_Symbol(LCD_SYMBOL_C17, true);
      SegmentLCD_Symbol(LCD_SYMBOL_S11, true);
      SegmentLCD_Symbol(LCD_SYMBOL_S12, true);
      SegmentLCD_LowerNumber(tempData);
      SegmentLCD_Symbol(LCD_SYMBOL_C13, true);
      SegmentLCD_Symbol(LCD_SYMBOL_S13, true);
    }
    EMU_EnterEM2(false);
  }
}
