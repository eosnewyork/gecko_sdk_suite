/***************************************************************************//**
 * @file
 * @brief Capacitive sense driver
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

#ifndef __CAPSENSE_H_
#define __CAPSENSE_H_

#include <stdint.h>
#include <stdbool.h>

/***************************************************************************//**
 * @addtogroup Drivers
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup CapSense
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

uint8_t  CAPSENSE_getSegmentChannel(uint8_t capSegment);
uint8_t  CAPSENSE_getButton0Channel(void);
uint8_t  CAPSENSE_getButton1Channel(void);
uint32_t CAPSENSE_getVal(uint8_t channel);
uint32_t CAPSENSE_getNormalizedVal(uint8_t channel);
bool CAPSENSE_getPressed(uint8_t channel);
int32_t CAPSENSE_getSliderPosition(void);
void CAPSENSE_Sense(void);
void CAPSENSE_Init(void);
void CAPSENSE_timerEventHandler(void);

#ifdef __cplusplus
}
#endif

/** @} (end group CapSense) */
/** @} (end group Drivers) */

#endif /* __CAPSENSE_H_ */
