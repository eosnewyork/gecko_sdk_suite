/***************************************************************************//**
 * @file
 * @brief Port and pin configurations.
 * @version 3.20.12
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

#ifndef _PLATFORMCONFIG_H_
#define _PLATFORMCONFIG_H_

/*---------------------------------------------------------------------*/
/*            Platform specific global definitions                     */
/*---------------------------------------------------------------------*/

/* MCM pin definitions */

/* Button assignments. */
//#define UIF_BUTTON_PORT gpioPortE
//#define UIF_BUTTON_PB0_PIN 3
//#define UIF_BUTTON_PB1_PIN 2
//
//#define LED0_PORT           gpioPortF
//#define LED0_PIN            6
//#define LED1_PORT           gpioPortF
//#define LED1_PIN            7
//
//#define LED0_SET            (GPIO_PinOutSet(LED0_PORT, LED0_PIN))
//#define LED0_CLEAR          (GPIO_PinOutClear(LED0_PORT, LED0_PIN))
//#define LED0_TOGGLE         (GPIO_PinOutToggle(LED0_PORT, LED0_PIN))
//#define LED0_GET            (GPIO_PinOutGet(LED0_PORT, LED0_PIN))
//
//#define LED1_SET            (GPIO_PinOutSet(LED1_PORT, LED1_PIN))
//#define LED1_CLEAR          (GPIO_PinOutClear(LED1_PORT, LED1_PIN))
//#define LED1_TOGGLE         (GPIO_PinOutToggle(LED1_PORT, LED1_PIN))
//#define LED1_GET            (GPIO_PinOutGet(LED1_PORT, LED1_PIN))
//
//#define LED_SET(NR)         LED##NR##_SET
//#define LED_CLEAR(NR)       LED##NR##_CLEAR
//#define LED_TOGGLE(NR)      LED##NR##_TOGGLE
//#define LED_GET(NR)         LED##NR##_GET

/* Define radio type */
/* TODO: Find out how radio should be referenced */
//#define SILABS_RADIO_SI446X

#endif /* _PLATFORMCONFIG_H_ */
