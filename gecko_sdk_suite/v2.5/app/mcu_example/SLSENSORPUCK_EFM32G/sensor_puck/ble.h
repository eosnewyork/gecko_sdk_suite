/***************************************************************************//**
 * @file
 * @brief Bluetooth Low Energy driver
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

#ifndef BLE_H_
#define BLE_H_

#define BLE_SAMPLE_COUNT    5

extern volatile bool BLE_Update_Mode;
extern volatile bool BLE_Ready;
extern volatile bool BLE_Initialized;
extern volatile bool BLE_Chg_Pending;

void BLE_Init(void);
void BLE_Update(void);
bool BLE_OnMeasurementModeChange(void);
void BLE_OnMsgReceived(uint8_t Msg[]);
void BLE_OnHostWakeup(void);
#endif /* BLE_H_ */
