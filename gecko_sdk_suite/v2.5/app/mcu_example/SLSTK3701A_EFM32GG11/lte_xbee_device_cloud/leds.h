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

#ifndef LEDS_H
#define LEDS_H

#include <stdint.h>

#define PWM_STEPS     255
#define RGB_LED_COUNT 1

#ifdef __cplusplus
extern "C" {
#endif

void setupRgbLed1(void);
void setLed1(uint32_t red, uint32_t green, uint32_t blue);

#ifdef __cplusplus
}
#endif

#endif /* LEDS_H */
