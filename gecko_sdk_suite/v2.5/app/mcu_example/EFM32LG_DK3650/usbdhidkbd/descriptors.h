/***************************************************************************//**
 * @file
 * @brief USB descriptor prototypes for HID keyboard example project.
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
#ifndef __SILICON_LABS_DESCRIPTORS_H__
#define __SILICON_LABS_DESCRIPTORS_H__

#include "em_usb.h"
#include "hidkbd.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const HIDKBD_KeyReport_t             USBDESC_reportTable[15];
extern const HIDKBD_KeyReport_t             USBDESC_noKeyReport;
extern const void *                         USBDESC_HidDescriptor;

extern const USB_DeviceDescriptor_TypeDef   USBDESC_deviceDesc;
extern const uint8_t                        USBDESC_configDesc[];
extern const void * const                   USBDESC_strings[4];
extern const uint8_t                        USBDESC_bufferingMultiplier[];

#ifdef __cplusplus
}
#endif

#endif /* __SILICON_LABS_DESCRIPTORS_H__ */
