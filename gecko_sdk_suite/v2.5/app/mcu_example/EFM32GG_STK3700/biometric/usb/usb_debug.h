/***************************************************************************//**
 * @file
 * @brief USB device setup for HRM debugging.
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
#include "si114xhrm.h"
#include "em_usb.h"

#ifndef USBDEBUG_H_
#define USBDEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define CDC_BULK_EP_SIZE  USB_FS_BULK_EP_MAXSIZE/* This is the max. ep size. */

void USBDebug_Init(void);
int USBDebug_SetupCmd(const USB_Setup_TypeDef *setup);
void USBDebug_StateChangeEvent(USBD_State_TypeDef oldState, USBD_State_TypeDef newState);
bool USBDebug_IsUSBConfigured(void);
void USBDebug_ProcessUSBOutput(int8_t *data_buffer);
void USBDebug_ProcessConfigurationMessage(char *message, Si114xhrmConfiguration_t *configuration);

#ifdef __cplusplus
}
#endif

#endif /* USBDEBUG_H_ */
