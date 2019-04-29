/***************************************************************************//**
 * @file
 * @brief USB descriptors for composite device example project.
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
#include "descriptors.h"

SL_ALIGN(4)
const USB_DeviceDescriptor_TypeDef USBDESC_deviceDesc SL_ATTRIBUTE_ALIGN(4) =
{
  .bLength            = USB_DEVICE_DESCSIZE,
  .bDescriptorType    = USB_DEVICE_DESCRIPTOR,
  .bcdUSB             = 0x0200,
  .bDeviceClass       = USB_CLASS_MISCELLANEOUS,
  .bDeviceSubClass    = USB_CLASS_MISC_COMMON_SUBCLASS,
  .bDeviceProtocol    = USB_CLASS_MISC_IAD_PROTOCOL,
  .bMaxPacketSize0    = USB_FS_CTRL_EP_MAXSIZE,
  .idVendor           = 0x10C4,
  .idProduct          = 0x000C,
  .bcdDevice          = 0x0000,
  .iManufacturer      = 1,
  .iProduct           = 2,
  .iSerialNumber      = 3,
  .bNumConfigurations = 1
};

#if (NUM_EP_USED != (HID_NUM_EP_USED + VUD_NUM_EP_USED + CDC_NUM_EP_USED) )
#error "Incorrect endpoint count."
#endif

#define CDC_MISC_DESCRIPTOR_LEN (USB_CDC_HEADER_FND_DESCSIZE          \
                                 + USB_CDC_CALLMNG_FND_DESCSIZE       \
                                 + USB_CDC_ACM_FND_DESCSIZE           \
                                 + USB_INTERFACE_ASSOCIATION_DESCSIZE \
                                 + 5)/*CDC Union Functional descriptor length*/

#define CONFIG_DESC_TOTAL_LEN                  \
  (USB_CONFIG_DESCSIZE                         \
   + (USB_INTERFACE_DESCSIZE * NUM_INTERFACES) \
   + (USB_ENDPOINT_DESCSIZE  * NUM_EP_USED)    \
   + CDC_MISC_DESCRIPTOR_LEN + USB_HID_DESCSIZE)

SL_ALIGN(4)
const uint8_t USBDESC_configDesc[] SL_ATTRIBUTE_ALIGN(4) =
{
  /*** Configuration descriptor ***/
  USB_CONFIG_DESCSIZE,    /* bLength                                   */
  USB_CONFIG_DESCRIPTOR,  /* bDescriptorType                           */
  CONFIG_DESC_TOTAL_LEN,  /* wTotalLength (LSB)                        */
  CONFIG_DESC_TOTAL_LEN >> 8, /* wTotalLength (MSB)                      */
  NUM_INTERFACES,         /* bNumInterfaces                            */
  1,                      /* bConfigurationValue                       */
  0,                      /* iConfiguration                            */
  CONFIG_DESC_BM_RESERVED_D7      /* bmAttrib: Self powered            */
  | CONFIG_DESC_BM_SELFPOWERED,
  CONFIG_DESC_MAXPOWER_mA(100),   /* bMaxPower: 100 mA                 */

  /*** VUD (Vendor Unique Device) Function ***/
  /*** Interface descriptor ***/
  USB_INTERFACE_DESCSIZE, /* bLength               */
  USB_INTERFACE_DESCRIPTOR,/* bDescriptorType      */
  VUD_INTERFACE_NO,       /* bInterfaceNumber      */
  0,                      /* bAlternateSetting     */
  VUD_NUM_EP_USED,        /* bNumEndpoints         */
  0xFF,                   /* bInterfaceClass       */
  0,                      /* bInterfaceSubClass    */
  0,                      /* bInterfaceProtocol    */
  0,                      /* iInterface            */

  /*** HID function ***/
  /*** Interface descriptor ***/
  USB_INTERFACE_DESCSIZE, /* bLength               */
  USB_INTERFACE_DESCRIPTOR,/* bDescriptorType      */
  HIDKBD_INTERFACE_NO,    /* bInterfaceNumber      */
  0,                      /* bAlternateSetting     */
  HID_NUM_EP_USED,        /* bNumEndpoints         */
  0x03,                   /* bInterfaceClass (HID) */
  0,                      /* bInterfaceSubClass    */
  1,                      /* bInterfaceProtocol    */
  0,                      /* iInterface            */

  /*** HID descriptor ***/
  USB_HID_DESCSIZE,       /* bLength               */
  USB_HID_DESCRIPTOR,     /* bDescriptorType       */
  0x11,                   /* bcdHID (LSB)          */
  0x01,                   /* bcdHID (MSB)          */
  0,                      /* bCountryCode          */
  1,                      /* bNumDescriptors       */
  USB_HID_REPORT_DESCRIPTOR,            /* bDecriptorType        */
  sizeof(HIDKBD_ReportDescriptor),      /* wDescriptorLength(LSB)*/
  sizeof(HIDKBD_ReportDescriptor) >> 8, /* wDescriptorLength(MSB)*/

  /*** Endpoint descriptor ***/
  USB_ENDPOINT_DESCSIZE,  /* bLength               */
  USB_ENDPOINT_DESCRIPTOR,/* bDescriptorType       */
  HIDKBD_INTR_IN_EP_ADDR, /* bEndpointAddress (IN) */
  USB_EPTYPE_INTR,        /* bmAttributes          */
  USB_FS_INTR_EP_MAXSIZE, /* wMaxPacketSize (LSB)  */
  0,                      /* wMaxPacketSize (MSB)  */
  HIDKBD_POLL_RATE,       /* bInterval             */

  /*** CDC Function                                                 ***/
  /*** IAD (Interface Association Descriptor) for the CDC function  ***/
  USB_INTERFACE_ASSOCIATION_DESCSIZE, /* bLength: Interface Descriptor size */
  USB_INTERFACE_ASSOCIATION_DESCRIPTOR, /* bDescriptorType      */
  CDC_CTRL_INTERFACE_NO,  /* bFirstInterface                    */
  2,                      /* bInterfaceCount                    */
  USB_CLASS_CDC,          /* bFunctionClass                     */
  USB_CLASS_CDC_ACM,      /* bFunctionSubClass                  */
  0,                      /* bFunctionProtocol                  */
  0,                      /* iFunction                          */

  /*** Communication Class Interface descriptor ***/
  USB_INTERFACE_DESCSIZE, /* bLength               */
  USB_INTERFACE_DESCRIPTOR,/* bDescriptorType      */
  CDC_CTRL_INTERFACE_NO,  /* bInterfaceNumber      */
  0,                      /* bAlternateSetting     */
  1,                      /* bNumEndpoints         */
  USB_CLASS_CDC,          /* bInterfaceClass       */
  USB_CLASS_CDC_ACM,      /* bInterfaceSubClass    */
  0,                      /* bInterfaceProtocol    */
  0,                      /* iInterface            */

  /*** CDC Header Functional descriptor ***/
  USB_CDC_HEADER_FND_DESCSIZE, /* bFunctionLength  */
  USB_CS_INTERFACE_DESCRIPTOR, /* bDescriptorType  */
  USB_CLASS_CDC_HFN,      /* bDescriptorSubtype    */
  0x20,                   /* bcdCDC spec.no LSB    */
  0x01,                   /* bcdCDC spec.no MSB    */

  /*** CDC Call Management Functional descriptor ***/
  USB_CDC_CALLMNG_FND_DESCSIZE, /* bFunctionLength */
  USB_CS_INTERFACE_DESCRIPTOR,  /* bDescriptorType */
  USB_CLASS_CDC_CMNGFN,   /* bDescriptorSubtype    */
  0,                      /* bmCapabilities        */
  CDC_DATA_INTERFACE_NO,  /* bDataInterface        */

  /*** CDC Abstract Control Management Functional descriptor ***/
  USB_CDC_ACM_FND_DESCSIZE, /* bFunctionLength     */
  USB_CS_INTERFACE_DESCRIPTOR, /* bDescriptorType  */
  USB_CLASS_CDC_ACMFN,    /* bDescriptorSubtype    */
  0x02,                   /* bmCapabilities        */
  /* The capabilities that this configuration supports:                   */
  /* D7..D4: RESERVED (Reset to zero)                                     */
  /* D3: 1 - Device supports the notification Network_Connection.         */
  /* D2: 1 - Device supports the request Send_Break                       */
  /* D1: 1 - Device supports the request combination of Set_Line_Coding,  */
  /*         Set_Control_Line_State, Get_Line_Coding, and the             */
  /*         notification Serial_State.                                   */
  /* D0: 1 - Device supports the request combination of Set_Comm_Feature, */
  /*         Clear_Comm_Feature, and Get_Comm_Feature.                    */

  /*** CDC Union Functional descriptor ***/
  5,                      /* bFunctionLength       */
  USB_CS_INTERFACE_DESCRIPTOR, /* bDescriptorType  */
  USB_CLASS_CDC_UNIONFN,  /* bDescriptorSubtype    */
  CDC_CTRL_INTERFACE_NO,  /* bControlInterface     */
  CDC_DATA_INTERFACE_NO,  /* bSubordinateInterface0*/

  /*** CDC Notification endpoint descriptor ***/
  USB_ENDPOINT_DESCSIZE,  /* bLength               */
  USB_ENDPOINT_DESCRIPTOR,/* bDescriptorType       */
  CDC_EP_NOTIFY,          /* bEndpointAddress (IN) */
  USB_EPTYPE_INTR,        /* bmAttributes          */
  USB_FS_INTR_EP_MAXSIZE, /* wMaxPacketSize (LSB)  */
  0,                      /* wMaxPacketSize (MSB)  */
  0xFF,                   /* bInterval             */

  /*** Data Class Interface descriptor ***/
  USB_INTERFACE_DESCSIZE, /* bLength               */
  USB_INTERFACE_DESCRIPTOR,/* bDescriptorType      */
  CDC_DATA_INTERFACE_NO,  /* bInterfaceNumber      */
  0,                      /* bAlternateSetting     */
  2,                      /* bNumEndpoints         */
  USB_CLASS_CDC_DATA,     /* bInterfaceClass       */
  0,                      /* bInterfaceSubClass    */
  0,                      /* bInterfaceProtocol    */
  0,                      /* iInterface            */

  /*** CDC Data interface endpoint descriptors ***/
  USB_ENDPOINT_DESCSIZE,  /* bLength               */
  USB_ENDPOINT_DESCRIPTOR,/* bDescriptorType       */
  CDC_EP_DATA_IN,         /* bEndpointAddress (IN) */
  USB_EPTYPE_BULK,        /* bmAttributes          */
  USB_FS_BULK_EP_MAXSIZE, /* wMaxPacketSize (LSB)  */
  0,                      /* wMaxPacketSize (MSB)  */
  0,                      /* bInterval             */

  USB_ENDPOINT_DESCSIZE,  /* bLength               */
  USB_ENDPOINT_DESCRIPTOR,/* bDescriptorType       */
  CDC_EP_DATA_OUT,        /* bEndpointAddress (OUT)*/
  USB_EPTYPE_BULK,        /* bmAttributes          */
  USB_FS_BULK_EP_MAXSIZE, /* wMaxPacketSize (LSB)  */
  0,                      /* wMaxPacketSize (MSB)  */
  0,                      /* bInterval             */
};

const void *USBDESC_HidDescriptor = (void*)
                                    &USBDESC_configDesc[USB_CONFIG_DESCSIZE + (2 * USB_INTERFACE_DESCSIZE)];

STATIC_CONST_STRING_DESC_LANGID(langID, 0x04, 0x09);
STATIC_CONST_STRING_DESC(iManufacturer, 'S', 'i', 'l', 'i', 'c', 'o', 'n', ' ', 'L', \
                         'a', 'b', 'o', 'r', 'a', 't', 'o', 'r', 'i',                \
                         'e', 's', ' ', 'I', 'n', 'c', '.');
STATIC_CONST_STRING_DESC(iProduct, 'E', 'F', 'M', '3', '2', ' ', 'H', 'a', 'p', \
                         'p', 'y', ' ', 'G', 'e', 'c', 'k', 'o', ' ',           \
                         'U', 'S', 'B', ' ', 'C', 'o', 'm', 'p', 'o',           \
                         's', 'i', 't', 'e', ' ', 'D', 'e', 'v', 'i',           \
                         'c', 'e');
STATIC_CONST_STRING_DESC(iSerialNumber, '0', '0', '0', '0', '1', '2', \
                         '3', '4', '5', '6', '7', '8');

const void * const USBDESC_strings[] =
{
  &langID,
  &iManufacturer,
  &iProduct,
  &iSerialNumber
};

/* Endpoint buffer sizes */
/* 1 = single buffer, 2 = double buffering, 3 = tripple buffering ... */
const uint8_t USBDESC_bufferingMultiplier[NUM_EP_USED + 1] =
{
  1,        /* Common Control endpoint.           */
  1,        /* HID interrupt IN endpoint.         */
  1, 2, 2   /* CDC interrupt and bulk endpoints.  */
};

const HIDKBD_KeyReport_t USBDESC_noKeyReport =
{ 0x00, 0x00, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

/* A sequence of keystroke input reports. */
SL_ALIGN(4)
const HIDKBD_KeyReport_t USBDESC_reportTable[15] SL_ATTRIBUTE_ALIGN(4) =
{
  { 0x02, 0x00, { 0x16, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* 'S'   */
  { 0x00, 0x00, { 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* 'i'   */
  { 0x00, 0x00, { 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* 'l'   */
  { 0x00, 0x00, { 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* 'i'   */
  { 0x00, 0x00, { 0x06, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* 'c'   */
  { 0x00, 0x00, { 0x12, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* 'o'   */
  { 0x00, 0x00, { 0x11, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* 'n'   */
  { 0x00, 0x00, { 0x2C, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* space */
  { 0x02, 0x00, { 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* 'L'   */
  { 0x00, 0x00, { 0x04, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* 'a'   */
  { 0x00, 0x00, { 0x05, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* 'b'   */
  { 0x00, 0x00, { 0x16, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* 's'   */
  { 0x00, 0x00, { 0x2C, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* space */
  { 0x00, 0x00, { 0x38, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* '-'   */
  { 0x00, 0x00, { 0x2C, 0x00, 0x00, 0x00, 0x00, 0x00 } },  /* space */
};
