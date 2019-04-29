/***************************************************************************//**
 * @file
 * @brief BRD4163A specific configuration for on-board serial flash.
 * @version 5.6.0
 *******************************************************************************
 * # License
 * <b>Copyright 2017 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef MX25CONFIG_H
#define MX25CONFIG_H

#include "em_device.h"
#include "em_gpio.h"

#define MX25_PORT_MOSI         gpioPortD
#define MX25_PIN_MOSI          0
#define MX25_PORT_MISO         gpioPortD
#define MX25_PIN_MISO          1
#define MX25_PORT_SCLK         gpioPortD
#define MX25_PIN_SCLK          2
#define MX25_PORT_CS           gpioPortD
#define MX25_PIN_CS            3

#define MX25_USART             USART2
#define MX25_USART_ROUTE       GPIO->USARTROUTE[2]
#define MX25_USART_CLK         cmuClock_USART2

#endif // MX25CONFIG_H
