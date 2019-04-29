/***************************************************************************//**
 * @file displaypalconfig.h
 * @brief Configuration file for PAL (Platform Abstraction Layer)
 * @version 5.6.0
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef DISPLAYPALCONFIG_H
#define DISPLAYPALCONFIG_H

/*
 * Select which oscillator should source the RTC clock.
 */
#undef  PAL_RTCC_CLOCK_LFXO
#define PAL_RTCC_CLOCK_LFRCO
#undef  PAL_RTCC_CLOCK_ULFRCO

/*
 * PAL SPI / USART configuration for the SLWSTK6001A.
 * Select which USART and location is connected to the device via SPI.
 */
#define PAL_SPI_USART_UNIT          USART0
#define PAL_SPI_USART_INDEX         0
#define PAL_SPI_USART_CLOCK         cmuClock_USART0

/*
 * Specify the SPI baud rate:
 */
#define PAL_SPI_BAUDRATE       (3500000)

/*
 * On BRD4176A GPIO port A, pin 0 is connected to the polarity inversion
 * (EXTCOMIN) pin on the Sharp Memory LCD. By defining
 * INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE_HW_ONLY the toggling of EXTCOMIN will
 * be handled by PRS, without software intervention, which saves power.
 */
#define INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE_HW_ONLY

#endif /* DISPLAYPALCONFIG_H */
