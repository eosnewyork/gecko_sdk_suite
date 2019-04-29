/***************************************************************************//**
 * @brief Macros defined for the serial-bootloader plugin.
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

#define EMBER_APP_NAME "bootload-ncp-uart-app"
#define APP_SERIAL 0
#define EMBER_ASSERT_SERIAL_PORT 0
#define EMBER_SERIAL0_BLOCKING
#define EMBER_SERIAL0_MODE EMBER_SERIAL_FIFO
#define EMBER_SERIAL0_TX_QUEUE_SIZE 128
#define EMBER_SERIAL0_RX_QUEUE_SIZE 128
#define ARG_LENGTH 40
