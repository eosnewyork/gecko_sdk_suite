/***************************************************************************//**
 * @file bspconfig.h
 * @brief Provide BSP (board support package) configuration parameters.
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

#ifndef BSPCONFIG_H
#define BSPCONFIG_H

#define BSP_STK
#define BSP_WSTK
#define BSP_WSTK_BRD4171A

#define BSP_BCC_USART         USART0
#define BSP_BCC_CLK           cmuClock_USART0
#define BSP_BCC_TXPORT        gpioPortA
#define BSP_BCC_TXPIN         5
#define BSP_BCC_RXPORT        gpioPortA
#define BSP_BCC_RXPIN         6

#define BSP_DISP_ENABLE_PORT  gpioPortB
#define BSP_DISP_ENABLE_PIN   1                 /* MemLCD display enable */

#define BSP_EXTFLASH_PRESENT  1

#define BSP_GPIO_LEDS
#define BSP_NO_OF_LEDS          2
#define BSP_GPIO_LED0_PORT      gpioPortC
#define BSP_GPIO_LED0_PIN       0
#define BSP_GPIO_LED1_PORT      gpioPortC
#define BSP_GPIO_LED1_PIN       1
#define BSP_GPIO_LEDARRAY_INIT  { { BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN }, { BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN } }

#define BSP_GPIO_BUTTONS
#define BSP_NO_OF_BUTTONS       2
#define BSP_GPIO_PB0_PORT       gpioPortC
#define BSP_GPIO_PB0_PIN        2
#define BSP_GPIO_PB1_PORT       gpioPortC
#define BSP_GPIO_PB1_PIN        3

#define BSP_GPIO_BUTTONARRAY_INIT { { BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN }, { BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN } }

#define BSP_INIT_DEFAULT  0

#if !defined(CMU_HFXOINIT_WSTK_DEFAULT)
/* HFXO initialization values for XTAL mode. */
#define CMU_HFXOINIT_WSTK_DEFAULT   CMU_HFXOINIT_DEFAULT
#endif

#if !defined(RADIO_PTI_INIT)
#define RADIO_PTI_INIT                                                     \
  {                                                                        \
    RADIO_PTI_MODE_UART,    /* Simplest output mode is UART mode */        \
    1600000,                /* Choose 1.6 MHz for best compatibility */    \
    0,                      /* WSTK uses location x for DOUT */            \
    gpioPortC,              /* Get the port for this loc */                \
    4,                      /* Get the pin, location should match above */ \
    0,                      /* WSTK uses location x for DCLK */            \
    gpioPortB,              /* Get the port for this loc */                \
    11,                     /* Get the pin, location should match above */ \
    0,                      /* WSTK uses location x for DFRAME */          \
    gpioPortC,              /* Get the port for this loc */                \
    5,                      /* Get the pin, location should match above */ \
  }
#endif

#if !defined(RAIL_PTI_CONFIG)
#define RAIL_PTI_CONFIG                                                    \
  {                                                                        \
    RAIL_PTI_MODE_UART,     /* Simplest output mode is UART mode */        \
    1600000,                /* Choose 1.6 MHz for best compatibility */    \
    0,                      /* WSTK uses location x for DOUT */            \
    gpioPortC,              /* Get the port for this loc */                \
    4,                      /* Get the pin, location should match above */ \
    0,                      /* WSTK uses location x for DCLK */            \
    gpioPortB,              /* Get the port for this loc */                \
    11,                     /* Get the pin, location should match above */ \
    0,                      /* WSTK uses location x for DFRAME */          \
    gpioPortC,              /* Get the port for this loc */                \
    5,                      /* Get the pin, location should match above */ \
  }
#endif

#if !defined(RADIO_PA_2P4_INIT)
#define RADIO_PA_2P4_INIT                                    \
  {                                                          \
    PA_SEL_2P4_MP,    /* Power Amplifier mode */             \
    PA_VOLTMODE_VBAT, /* Power Amplifier vPA Voltage mode */ \
    100,              /* Desired output power in dBm * 10 */ \
    0,                /* Output power offset in dBm * 10 */  \
    2,                /* Desired ramp time in us */          \
  }
#endif

#if !defined(RAIL_PA_2P4_CONFIG)
#define RAIL_PA_2P4_CONFIG                                            \
  {                                                                   \
    RAIL_TX_POWER_MODE_2P4_MP, /* Power Amplifier mode */             \
    3300,                      /* Power Amplifier vPA Voltage mode */ \
    2,                         /* Desired ramp time in us */          \
  }
#endif

#if !defined(RAIL_PA_DEFAULT_POWER)
#define RAIL_PA_DEFAULT_POWER 100
#endif

#define BSP_BCP_VERSION 2
#include "bsp_bcp.h"

#endif /* BSPCONFIG_H */
