/***************************************************************************//**
 * @file
 * @brief USB descriptors for headphone audio device example.
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
  .bDeviceClass       = 0,
  .bDeviceSubClass    = 0,
  .bDeviceProtocol    = 0,
  .bMaxPacketSize0    = USB_FS_CTRL_EP_MAXSIZE,
  .idVendor           = SILABS_USB_VID,
  .idProduct          = 0x000D,
  .bcdDevice          = 0x0000,
  .iManufacturer      = 1,
  .iProduct           = 2,
  .iSerialNumber      = 0,
  .bNumConfigurations = 1
};

/*** Total size of configuration descriptor. ***/
#define CONFIG_DESC_TOTAL_LEN             \
  (USB_CONFIG_DESCSIZE                    \
   + (3 * USB_INTERFACE_DESCSIZE)         \
   + 9 + USB_CA_INPUT_TERMINAL_DESCSIZE   \
   + 13 + USB_CA_OUTPUT_TERMINAL_DESCSIZE \
   + USB_CA_AS_GENERAL_DESCSIZE           \
   + 11 + USB_CA_STD_AS_ENDPOINT_DESCSZIE \
   + USB_CA_EP_GENERAL_DESCSIZE           \
  )

/*** Combined size of class-specific AudioControl interface ***/
/*** descriptor and all Unit and Terminal descriptors.      ***/
#define AC_ITF_DESC_TOTAL_LEN         \
  (9 + USB_CA_INPUT_TERMINAL_DESCSIZE \
   + 13 + USB_CA_OUTPUT_TERMINAL_DESCSIZE)

SL_ALIGN(4)
const uint8_t USBDESC_configDesc[] SL_ATTRIBUTE_ALIGN(4) =
{
  /*** Configuration descriptor. ***/
  USB_CONFIG_DESCSIZE,              /* bLength                  */
  USB_CONFIG_DESCRIPTOR,            /* bDescriptorType          */
  CONFIG_DESC_TOTAL_LEN,            /* wTotalLength (LSB)       */
  CONFIG_DESC_TOTAL_LEN >> 8,         /* wTotalLength (MSB)       */
  2,                                /* bNumInterfaces           */
  1,                                /* bConfigurationValue      */
  0,                                /* iConfiguration           */
  CONFIG_DESC_BM_RESERVED_D7        /* bmAttrib: Self powered   */
  | CONFIG_DESC_BM_SELFPOWERED,
  CONFIG_DESC_MAXPOWER_mA(100),     /* bMaxPower: 100 mA        */

  /* ===================== First interface ==================== */

  /*** Interface descriptor for the first interface. ***/
  /*** This is the Audio Control (AC) interface.     ***/
  USB_INTERFACE_DESCSIZE,           /* bLength                  */
  USB_INTERFACE_DESCRIPTOR,         /* bDescriptorType          */
  0,                                /* bInterfaceNumber         */
  0,                                /* bAlternateSetting        */
  0,                                /* bNumEndpoints            */
  USB_CLASS_AUDIO,                  /* bInterfaceClass          */
  USB_CLASS_AUDIO_CONTROL,          /* bInterfaceSubClass       */
  0,                                /* bInterfaceProtocol       */
  0,                                /* iInterface               */

  /*** Class-Specific AC Interface Header descriptor. ***/
  9,                                /* bLength                  */
  USB_CS_INTERFACE_DESCRIPTOR,      /* bDescriptorType          */
  USB_CA_HEADER_DESCRIPTOR,         /* bDescriptorSubtype       */
  0x00, 0x01,                       /* bcdADC (1.0)             */
  AC_ITF_DESC_TOTAL_LEN,            /* wTotalLength (LSB)       */
  AC_ITF_DESC_TOTAL_LEN >> 8,         /* wTotalLength (MSB)       */
  1,                                /* bInCollection            */
  1,                                /* baInterfaceNr(1)         */

  /*** Input Terminal ID1 descriptor. ***/
  USB_CA_INPUT_TERMINAL_DESCSIZE,   /* bLength                  */
  USB_CS_INTERFACE_DESCRIPTOR,      /* bDescriptorType          */
  USB_CA_INPUT_TERMINAL_DESCRIPTOR, /* bDescriptorSubtype       */
  1,                                /* bTerminalID              */
  0x01, 0x01,                       /* wTerminalType (USB Streaming) */
  0,                                /* bAssocTerminal           */
  2,                                /* bNrChannels              */
  0x03, 0x00,                       /* wChannelConfig (Stereo)  */
  0,                                /* iChannelNames            */
  0,                                /* iTerminal                */

  /*** Feature Unit ID2 descriptor. ***/
  13,                               /* bLength                  */
  USB_CS_INTERFACE_DESCRIPTOR,      /* bDescriptorType          */
  USB_CA_FEATURE_UNIT_DESCRIPTOR,   /* bDescriptorSubtype       */
  2,                                /* bUnitID                  */
  1,                                /* bSourceID                */
  2,                                /* bControlSize             */
  0x01, 0x00,                       /* bmaControls(0) (Mute)    */
  0x00, 0x00,                       /* bmaControls(1) (Left front)  */
  0x00, 0x00,                       /* bmaControls(2) (Right front) */
  0,                                /* iFeature                 */

  /*** Output Terminal ID3 descriptor. ***/
  USB_CA_OUTPUT_TERMINAL_DESCSIZE,  /* bLength                  */
  USB_CS_INTERFACE_DESCRIPTOR,      /* bDescriptorType          */
  USB_CA_OUTPUT_TERMINAL_DESCRIPTOR,/* bDescriptorSubtype       */
  3,                                /* bTerminalID              */
  0x02, 0x03,                       /* wTerminalType (Speaker)  */
  0,                                /* bAssocTerminal           */
  2,                                /* bSourceID                */
  0,                                /* iTerminal                */

  /* =================== End first interface ================== */

  /* =================== Second interface ===================== */

  /*** Interface descriptor for the second interface. ***/
  /*** This is the Audio Stream (AS) interface.       ***/
  /*** Alternate setting 0 (zero bandwidth stream).   ***/
  USB_INTERFACE_DESCSIZE,           /* bLength                  */
  USB_INTERFACE_DESCRIPTOR,         /* bDescriptorType          */
  1,                                /* bInterfaceNumber         */
  0,                                /* bAlternateSetting        */
  0,                                /* bNumEndpoints            */
  USB_CLASS_AUDIO,                  /* bInterfaceClass          */
  USB_CLASS_AUDIO_STREAMING,        /* bInterfaceSubClass       */
  0,                                /* bInterfaceProtocol       */
  0,                                /* iInterface               */

  /*** Alternate setting 1 (operational stream). ***/
  USB_INTERFACE_DESCSIZE,           /* bLength                  */
  USB_INTERFACE_DESCRIPTOR,         /* bDescriptorType          */
  1,                                /* bInterfaceNumber         */
  1,                                /* bAlternateSetting        */
  1,                                /* bNumEndpoints            */
  USB_CLASS_AUDIO,                  /* bInterfaceClass          */
  USB_CLASS_AUDIO_STREAMING,        /* bInterfaceSubClass       */
  0,                                /* bInterfaceProtocol       */
  0,                                /* iInterface               */

  /*** Class-Specific AS General Interface descriptor. ***/
  USB_CA_AS_GENERAL_DESCSIZE,       /* bLength                  */
  USB_CS_INTERFACE_DESCRIPTOR,      /* bDescriptorType          */
  USB_CA_AS_GENERAL_DESCRIPTOR,     /* bDescriptorSubtype       */
  1,                                /* bTerminalLink            */
  0,                                /* bDelay                   */
  0x01, 0x00,                       /* wFormatTag (PCM)         */

  /*** Stereo Type I Format interface descriptor. ***/
  11,                               /* bLength                  */
  USB_CS_INTERFACE_DESCRIPTOR,      /* bDescriptorType          */
  USB_CA_FORMAT_TYPE_DESCRIPTOR,    /* bDescriptorSubtype       */
  1,                                /* bFormatType              */
  2,                                /* bNrChannels              */
  2,                                /* bSubFrameSize            */
  16,                               /* bBitResolution           */
  1,                                /* bSamFreqType (one)       */
  (uint8_t)(AUDIO_SAMPLE_RATE),     /* tSamFreq                 */
  (uint8_t)(AUDIO_SAMPLE_RATE >> 8),
  (uint8_t)(AUDIO_SAMPLE_RATE >> 16),

  /*** Standard audio stream isochronous endpoint descriptor. ***/
  USB_CA_STD_AS_ENDPOINT_DESCSZIE,  /* bLength                  */
  USB_ENDPOINT_DESCRIPTOR,          /* bDescriptorType          */
  0x01,                             /* bEndpointAddress         */
  USB_EPTYPE_ISOC | USB_EPSYNC_ADAPTIVE, /* bmAttributes        */
  (uint8_t)(96000 / 250),             /* wMaxPacketSize, 384 is   */
  (uint8_t)((96000 / 250) >> 8),        /* required for 96kHz audio */
  1,                                /* bInterval                */
  0,                                /* bRefresh                 */
  0,                                /* bSynchAddress            */

  /*** Class specific isochronous audio data endpoint descriptor. ***/
  USB_CA_EP_GENERAL_DESCSIZE,       /* bLength                  */
  USB_CS_ENDPOINT_DESCRIPTOR,       /* bDescriptorType          */
  USB_CA_EP_GENERAL_DESCRIPTOR,     /* bDescriptorSubtype       */
  0,                                /* bmAttributes             */
  0,                                /* bLockDelayUnits          */
  0, 0,                             /* wLockDelay               */

  /* ================= End second interface =================== */
};

STATIC_CONST_STRING_DESC_LANGID(langID, 0x04, 0x09);
STATIC_CONST_STRING_DESC(iManufacturer, 'S', 'i', 'l', 'i', 'c', 'o', 'n', ' ', 'L', \
                         'a', 'b', 'o', 'r', 'a', 't', 'o', 'r', 'i',                \
                         'e', 's', ' ', 'I', 'n', 'c', '.');
STATIC_CONST_STRING_DESC(iProduct, 'E', 'F', 'M', '3', '2', ' ', 'U', 'S', 'B', \
                         ' ', 'H', 'e', 'a', 'd', 'p', 'h', 'o', 'n',           \
                         'e', ' ', 'A', 'u', 'd', 'i', 'o', ' ',                \
                         'D', 'e', 'v', 'i', 'c', 'e');

const void * const USBDESC_strings[] =
{
  &langID,
  &iManufacturer,
  &iProduct,
};

/* Endpoint buffer sizes */
/* 1 = single buffer, 2 = double buffering, 3 = tripple buffering ... */
const uint8_t USBDESC_bufferingMultiplier[NUM_EP_USED + 1] = { 1, 2 };
