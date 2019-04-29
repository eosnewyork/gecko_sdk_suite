/***************************************************************************//**
 * @file
 * @brief helper functions for managing capsense GPIO inputs
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

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

/***************************************************************************//**
 * @brief Return a bitmask containing the current state for all capsense
 * buttons.
 ******************************************************************************/
uint8_t getCapsenseCurrent(void);

/***************************************************************************//**
 * @brief Return a bitmask containing the previous state for all capsense
 * buttons.
 ******************************************************************************/
uint8_t getCapsensePrevious(void);

/***************************************************************************//**
 * @brief GPIO data structure initialization
 ******************************************************************************/
void initGPIO(void);

/***************************************************************************//**
 * @brief update GPIO data structure from current values of seven GPIO pins
 ******************************************************************************/
void updateGPIO(void);

#endif /* GPIO_H */
