/*
 * File: wstk/sensors.c
 * Description: Si701x Temperature/Humidity sensor driver
 *
 * Author(s): Szilveszter Papp, szilveszter.papp@silabs.com
 *
 * Copyright 2015 by Silicon Labs. All rights reserved.
 */

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "i2cspm.h"
#include "si7013.h"

#if !(HAL_I2CSENSOR_ENABLE)
  #error "Humidity sensor not enabled!"
#endif

static I2CSPM_Init_TypeDef i2cInit = I2CSPM_INIT_DEFAULT;

bool emberAfPluginWstkSensorsInit(void)
{
  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // MCU may not have direct connection to I2CSENSOR_ENABLE
  #ifdef BSP_I2CSENSOR_ENABLE_PORT
  // Enable si7021 sensor isolation switch
  GPIO_PinModeSet(BSP_I2CSENSOR_ENABLE_PORT,
                  BSP_I2CSENSOR_ENABLE_PIN,
                  gpioModePushPull,
                  1);
  #endif //BSP_I2CSENSOR_ENABLE_PORT

  I2CSPM_Init(&i2cInit);

  return Si7013_Detect(i2cInit.port, SI7021_ADDR, NULL);
}

EmberStatus emberAfPluginWstkSensorsGetSample(uint32_t *rhData, int32_t *tData)
{
  return ((Si7013_MeasureRHAndTemp(i2cInit.port,
                                   SI7021_ADDR,
                                   rhData,
                                   tData) == 0)
          ? EMBER_SUCCESS
          : EMBER_ERR_FATAL);
}
