/***************************************************************************//**
 * @file
 * @brief wmbus_sample_frame.h
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
#include "wmbus/wmbus.h"

#ifndef WMBUS_SAMPLE_FRAM_H_
#define WMBUS_SAMPLE_FRAM_H_

/**
 * Creates a standard mbus frame with volume and flow rate fields
 * (typical water meter) and water (0x07) device type
 *
 * @param[out] buffer
 *    The buffer the frame will be written to
 * @param[in] accessNumber
 *    Access number of the field (part of the STL header)
 * @param[in] accessibility
 *    Accessibility setting of the device. The application is responsible
 *    for implementing the claimed accessibility
 * @param[in] volume
 *    Volume in 10^-3 m^3
 * @param[in] volumeFlow
 *    Flow rate in 10^-3 m^3/h
 * @param[in] periodic
 *    Set it true if this is a periodic frame. Note that periodic frames
 *    have strict timing requirements, which should be kept by the application
 * @param[in] encrypt
 *    Set it to true to enable mode5 encryption on the frame
 */
uint16_t WMBUS_SAMPLE_setupFrame(uint8_t *buffer, uint8_t accessNumber, WMBUS_accessibility_t accessibility, int32_t volume, int16_t volumeFlow, bool periodic, bool encrypt);

#endif /* WMBUS_SAMPLE_FRAM_H_ */
