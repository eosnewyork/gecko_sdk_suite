/***************************************************************************//**
 * @file
 * @brief Wireless Whiteboard's UART Application Header File
 * WGM110 and GLIB demo for the SLSTK3401A running on MicOS.
 * This module initializes the UART to be used by the WiFi module.
 * It also manages the RX interrupts by using a ring buffer.
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

#ifndef APP_UART_H
#define APP_UART_H

// -----------------------------------------------------------------------------
// Externs

#ifdef APP_UART_MODULE
#define APP_UART_MODULE_EXT
#else
#define APP_UART_MODULE_EXT extern
#endif

// -----------------------------------------------------------------------------
// Data Types

// Callback function pointer data type
typedef void (*APP_UART_RxCallback_t)(void);

// -----------------------------------------------------------------------------
// Global Variables

/// Number of unread bytes in the ring buffer
APP_UART_MODULE_EXT volatile uint16_t APP_UART_RxCount;

// -----------------------------------------------------------------------------
// Function Prototypes

void APP_UART_Init(void);
void APP_UART_RegisterCallback(APP_UART_RxCallback_t callbackPtr);
char APP_UART_Read(void);
void APP_UART_Write(uint8 len1, uint8* data1, uint16 len2, uint8* data2);
void APP_UART_ClearBuffer(void);
void APP_UART_Start(void);
void APP_UART_Stop(void);

#endif  // APP_UART_H
