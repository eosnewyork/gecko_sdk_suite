/***************************************************************************//**
 * @file
 * @brief Vendor unique USB device example.
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

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "bsp.h"
#include "em_usb.h"
#include "descriptors.h"
#include "display.h"
#include "scrolllcd.h"
#include "image.h"
#include "retargetserial.h"
#include <stdio.h>

/***************************************************************************//**
 *
 * This example shows how a vendor unique device can be implemented.
 * A vendor unique device is a device which does not belong to any
 * USB class.
 *
 * Use the file EFM32_Vendor_Unique_Device.inf to install libusb device driver
 * on the host PC. This file reside in example subdirectory:
 * ./host/libusb/efm32-vendor-unique-device-1.2.5.0
 *
 ******************************************************************************/

#define LED0            0
#define LED1            1
#define LED2            2
#define LED3            3
#define LED4            4

#define VND_GET_LEDS    0x10
#define VND_SET_LED     0x11

static int SetupCmd(const USB_Setup_TypeDef *setup);
static void StateChange(USBD_State_TypeDef oldState,
                        USBD_State_TypeDef newState);

static const USBD_Callbacks_TypeDef callbacks =
{
  .usbReset        = NULL,
  .usbStateChange  = StateChange,
  .setupCmd        = SetupCmd,
  .isSelfPowered   = NULL,
  .sofInt          = NULL
};

static const USBD_Init_TypeDef usbInitStruct =
{
  .deviceDescriptor    = &USBDESC_deviceDesc,
  .configDescriptor    = USBDESC_configDesc,
  .stringDescriptors   = USBDESC_strings,
  .numberOfStrings     = sizeof(USBDESC_strings) / sizeof(void*),
  .callbacks           = &callbacks,
  .bufferingMultiplier = USBDESC_bufferingMultiplier,
  .reserved            = 0
};

static DISPLAY_Device_t displayDevice;            /* Display device handle. */
static volatile scrollDirection_t scrollDisplay;
static char blank_image[128 * 16 * 3];
static uint32_t rgbLed0 = 0;
static uint32_t rgbLed1 = 0;
/***************************************************************************//**
 * @brief main - the entrypoint after reset.
 ******************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  /* Initialize VCOM port for USB "printf" debug output. */
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);
  printf("\nEFM32 USB LED Vendor Unique Device example\n");

  /* Initialize the display module. */
  DISPLAY_Init();

  /* Retrieve the properties of the display. */
  if ( DISPLAY_DeviceGet(0, &displayDevice) != DISPLAY_EMSTATUS_OK ) {
    /* Unable to get display handle. */
    while ( 1 ) ;
  }

  memset( (void*)blank_image, 0xFF, 128 * 16 * 3);
  displayDevice.pPixelMatrixDraw(&displayDevice, (void*)blank_image,
                                 /* start coloumn, width */
                                 0, displayDevice.geometry.width,
                                 /* start row, height */
                                 0, displayDevice.geometry.height);
  scrollDisplay = scrollOff;

  /* Initialize LED driver */
  BSP_LedsInit();
  BSP_ExtLedSet(0, rgbLed0);
  BSP_ExtLedSet(1, rgbLed1);

  /* Initialize and start USB device stack. */
  USBD_Init(&usbInitStruct);

  /*
   * When using a debugger the tree following lines are pratical to
   * force host to re-enumerate the device.
   */
  /*USBD_Disconnect();        */
  /*USBTIMER_DelayMs( 1000 ); */
  /*USBD_Connect();           */

  scrollLcd(&displayDevice, scrollLeft, blank_image, usb_image);

  for (;; ) {
    if ( scrollDisplay != scrollOff ) {
      scrollLcd(&displayDevice, scrollDisplay, usb_image, usb_image);
      scrollDisplay = scrollOff;
    }
  }
}

/***************************************************************************//**
 * @brief
 *   Handle USB setup commands.
 *
 * @param[in] setup Pointer to the setup packet received.
 *
 * @return USB_STATUS_OK if command accepted.
 *         USB_STATUS_REQ_UNHANDLED when command is unknown, the USB device
 *         stack will handle the request.
 ******************************************************************************/
static int SetupCmd(const USB_Setup_TypeDef *setup)
{
  int             retVal, ledNo;
  uint32_t        *rgbLed, subLed;
  static uint32_t buffer;
  uint8_t         *pBuffer = (uint8_t*) &buffer;

  retVal = USB_STATUS_REQ_UNHANDLED;

  if ( setup->Type == USB_SETUP_TYPE_VENDOR ) {
    switch ( setup->bRequest ) {
      case VND_GET_LEDS:
        /********************/
        *pBuffer = (rgbLed1 << 3U) | rgbLed0;
        retVal   = USBD_Write(0, pBuffer, setup->wLength, NULL);
        break;

      case VND_SET_LED:
        /********************/
        if ( setup->wIndex <= LED2 ) {
          ledNo  = 0;
          rgbLed = &rgbLed0;
          subLed = setup->wIndex;
        } else {
          ledNo = 1;
          rgbLed = &rgbLed1;
          subLed = setup->wIndex - 3U;
        }
        if ( setup->wValue ) {
          *rgbLed |= 1U << subLed;
        } else {
          *rgbLed &= ~(1U << subLed);
        }
        BSP_ExtLedSet(ledNo, *rgbLed);
        putchar('.');
        retVal = USB_STATUS_OK;
        break;
    }
  }

  return retVal;
}

/***************************************************************************//**
 * @brief
 *   Callback function called each time the USB device state is changed.
 *
 * @param[in] oldState The device state the device has just left.
 * @param[in] newState The new device state.
 ******************************************************************************/
static void StateChange(USBD_State_TypeDef oldState,
                        USBD_State_TypeDef newState)
{
  if ( scrollDisplay == scrollOff ) {
    if ( newState == USBD_STATE_CONFIGURED ) {
      scrollDisplay = scrollDown;
    } else if ( oldState == USBD_STATE_CONFIGURED ) {
      scrollDisplay = scrollUp;
    }
  }
}
