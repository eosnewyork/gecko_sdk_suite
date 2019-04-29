/***************************************************************************//**
 * @file
 * @brief
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

#include "temp_sensor.h"
#include "em_acmp.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_i2c.h"

#include "i2cspm.h"
#include "si7013.h"

I2CSPM_Init_TypeDef i2cInit = I2CSPM_INIT_DEFAULT;

void initTempSensor(void)
{
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet(gpioPortB, 3, gpioModePushPull, 1);
  I2CSPM_Init(&i2cInit);
}

int tempSensorReadHumTemp(uint32_t *rhData, int32_t *tempData)
{
  return Si7013_MeasureRHAndTemp(i2cInit.port, SI7021_ADDR, rhData, tempData);
}
