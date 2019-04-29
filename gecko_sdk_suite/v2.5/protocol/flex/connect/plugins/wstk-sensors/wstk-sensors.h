/***************************************************************************//**
 * @brief  * Reads a sample of the temperature and relative humidity
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

EmberStatus emberAfPluginWstkSensorsGetSample(uint32_t *rhData, int32_t *tData);

/** @} // END addtogroup
 */

#endif // _WSTK_SENSORS_H_
