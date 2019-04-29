/**
 * @file wstk-sensors.h
 * Copyright 2015 Silicon Laboratories, Inc.                              *80*
 */

#ifndef _WSTK_SENSORS_H_
#define _WSTK_SENSORS_H_

#ifndef __HAL_H__
  #error hal/hal.h should be included first
#endif

/**
 * @addtogroup wstk_sensors
 *
 * See wstk-sensors.h for source code.
 * @{
 */

/** @brief Reads a sample of the temperature and relative humidity
 * values from the WSTK Si7013 sensors.
 *
 * @param rhData  A pointer to a 32-bit unsigned integer where the relative
 * humidity sample is written.
 *
 * @param tData  A pointer to a 32-bit signed integer where the temperature
 * is written. Temperature is sampled in "millicelsius".
 *
 * @return An ::EmberStatus value of ::EMBER_SUCCESS if the relative humidity
 * and temperature sample were successfully read from the Si7013 sensors.
 * Otherwise, an ::EmberStatus value of ::EMBER_ERR_FATAL.
 */
EmberStatus emberAfPluginWstkSensorsGetSample(uint32_t *rhData, int32_t *tData);

/** @} // END addtogroup
 */

#endif // _WSTK_SENSORS_H_
