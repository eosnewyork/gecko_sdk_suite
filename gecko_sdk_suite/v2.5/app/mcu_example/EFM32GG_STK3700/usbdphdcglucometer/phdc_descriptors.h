/***************************************************************************//**
 * @file
 * @brief USB descriptors for PHDC Continua Medical Device.
 *         This file was modified from the LED example code demo
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

#ifndef __PHDC_DESCRIPTORS_H
#define __PHDC_DESCRIPTORS_H

#include "em_usb.h"
/*** Function prototypes. ***/

int  SetupCmd(const USB_Setup_TypeDef *setup);
void UsbStateChange(USBD_State_TypeDef oldState, USBD_State_TypeDef newState);
void UsbReset(void);

/****************************************************************
* PHDC Class Function Descriptor                               *
****************************************************************/
#define USB_PHDC_CLASSFUNCTION_DESCRIPTOR 0x20
#define PHDC_CLASS_bLength              0x04
#define PHDC_CLASS_bDescriptorType      USB_PHDC_CLASSFUNCTION_DESCRIPTOR
#define PHDC_CLASS_bPHDCDataCode        0x02
#define PHDC_CLASS_bmCapability         0x00

/****************************************************************
* PHDC Function Extension                                      *
****************************************************************/
#define PHDC_11073PHD_FUNCTION_DESCRIPTOR    0x30
#define PHDC_FUNC_bLength               0x06  /* Depends upon number of device specializations */
#define PHDC_FUNC_bDescriptorType       PHDC_11073PHD_FUNCTION_DESCRIPTOR
#define PHDC_FUNC_bReserved             0x00
#define PHDC_FUNC_bNumberDevSpecs       0x01
#define PHDC_FUNC_bGlucometer_L         0x11
#define PHDC_FUNC_bGlucometer_H         0x10

/****************************************************************
* PHDC QoS (Endpoints 1 & 2)                                   *
****************************************************************/
#define USB_PHDC_QOS_DESCRIPTOR         0x21
#define PHDC_QOS_bLength                0x04
#define PHDC_QOS_bDescriptorType        USB_PHDC_QOS_DESCRIPTOR
#define PHDC_QOS_bQoSEncodingVersion    0x01
#define PHDC_QOS_bmLatencyReliability   0x08

/****************************************************************
* Endpoint1 Descriptor (Endpoint that sends data to host)      *
****************************************************************/
#define BULK_IN_EP_ADDR                 0x81             /* Address for bulk in */

#define C_EP1_DSC_bLength               USB_ENDPOINT_DESCSIZE
#define C_EP1_DSC_bDescriptorType       USB_ENDPOINT_DESCRIPTOR
#define C_EP1_DSC_bEndpointAddress      BULK_IN_EP_ADDR  /*Bulk IN*/
#define C_EP1_DSC_bmAttributes          0x02             /*Bulk EndPoint*/
#define C_EP1_DSC_wMaxPacketSize_L      USB_FS_BULK_EP_MAXSIZE /*EP FIFO : 64byte*/
#define C_EP1_DSC_wMaxPacketSize_H      0x00
#define C_EP1_DSC_bInterval             0x00             /*no use*/

/****************************************************************
* Endpoint2 Descriptor (Endpoint that reads data from host)    *
****************************************************************/
#define BULK_OUT_EP_ADDR                0x02             /* Address for bulk out */

#define C_EP2_DSC_bLength               USB_ENDPOINT_DESCSIZE
#define C_EP2_DSC_bDescriptorType       USB_ENDPOINT_DESCRIPTOR
#define C_EP2_DSC_bEndpointAddress      BULK_OUT_EP_ADDR /*Bulk OUT*/
#define C_EP2_DSC_bmAttributes          0x02             /*Bulk EndPoint*/
#define C_EP2_DSC_wMaxPacketSize_L      USB_FS_BULK_EP_MAXSIZE /*EP FIFO : 64byte*/
#define C_EP2_DSC_wMaxPacketSize_H      0x00
#define C_EP2_DSC_bInterval             0x00             /*no use*/

#endif
