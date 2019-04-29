/***************************************************************************//**
 * @file
 * @brief Bluetooth Low Energy UART interface driver
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

#ifndef BLE_UART_H_
#define BLE_UART_H_

void BLE_UART_Init(void);
void BLE_UART_Send(uint8_t *pMsg, int MsgLength);
#endif /* BLE_UART_H_ */
