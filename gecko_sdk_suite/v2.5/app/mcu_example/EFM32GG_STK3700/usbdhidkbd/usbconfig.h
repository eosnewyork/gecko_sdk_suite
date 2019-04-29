/***************************************************************************//**
 * @file
 * @brief USB protocol stack library, application supplied configuration options.
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

#ifndef __USBCONFIG_H
#define __USBCONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

//#define BUSPOWERED      /* Uncomment to build buspowered device */

#if defined(BUSPOWERED)
  #define USB_PWRSAVE_MODE (USB_PWRSAVE_MODE_ONSUSPEND | USB_PWRSAVE_MODE_ENTEREM2)
#else
  #define USB_PWRSAVE_MODE  (USB_PWRSAVE_MODE_ONVBUSOFF | USB_PWRSAVE_MODE_ONSUSPEND)
#endif

#define USB_USBC_32kHz_CLK   USB_USBC_32kHz_CLK_LFRCO

#define USB_DEVICE        /* Compile stack for device mode. */

/****************************************************************************
**                                                                         **
** Specify number of endpoints used (in addition to EP0).                  **
**                                                                         **
*****************************************************************************/
#define NUM_EP_USED 1

/****************************************************************************
**                                                                         **
** Specify number of application timers you need.                          **
**                                                                         **
*****************************************************************************/
#define NUM_APP_TIMERS 2

/****************************************************************************
**                                                                         **
** USB HID keyboard class device driver definitions.                       **
**                                                                         **
*****************************************************************************/
#define HIDKBD_INTERFACE_NO     0
#define HIDKBD_INTR_IN_EP_ADDR  0x81    /* Bit 7 must be set.                 */
#define HIDKBD_POLL_RATE        24      /* The bInterval reported with the    */
/* interrupt IN endpoint descriptor.  */
#define HIDKBD_IDLE_TIMER       0       /* Timer used to implement the idle-  */
/* rate defined in the HID class spec.*/

#ifdef __cplusplus
}
#endif

#endif /* __USBCONFIG_H */
