/***************************************************************************//**
 * @brief serial link protocol values, used over both SPI and UART links
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

//  Each message has a four-byte header:
//   '[' type high-length-byte low-length-byte
//
// (With only three types and a two-byte length we could encode the type in
// the high byte of the length.)

#define UART_LINK_HEADER_SIZE 4

// Non Management is not used for now.
// This is so we can easily add more functionality in the future.
typedef enum {
  UART_LINK_TYPE_MANAGEMENT        = 1,
  UART_LINK_TYPE_NON_MANAGEMENT    = 2,
} SerialLinkMessageType; // use 'serial' in new names, for both UART and SPI

#define MAX_UART_LINK_TYPE UART_LINK_TYPE_NON_MANAGEMENT
#define EMBER_APPLICATION_HAS_NOTIFY_SERIAL_RX
#define EMBER_APPLICATION_HAS_BINARY_COMMAND_INTERPRETER
