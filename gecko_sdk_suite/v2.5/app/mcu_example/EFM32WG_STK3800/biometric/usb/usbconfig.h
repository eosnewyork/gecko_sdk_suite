/***************************************************************************//**
 * @file
 * @brief USB configuration.
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

#ifndef USBCONFIG_H_
#define USBCONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define USB_DEVICE        /* Compile stack for device mode. */
#define USB_PWRSAVE_MODE (USB_PWRSAVE_MODE_ONSUSPEND | USB_PWRSAVE_MODE_ONVBUSOFF)

/****************************************************************************
**                                                                         **
** Specify total number of endpoints used (in addition to EP0).            **
**                                                                         **
*****************************************************************************/
#define NUM_EP_USED 3

/****************************************************************************
**                                                                         **
** Specify number of application timers you need.                          **
**                                                                         **
*****************************************************************************/
#define NUM_APP_TIMERS 2

/*** Misc. definitions for the interfaces of the composite device. ***/

/* Define interface numbers */
#define CDC_CTRL_INTERFACE_NO   (0)
#define CDC_DATA_INTERFACE_NO   (1)
#define NUM_INTERFACES          (2)

#define CDC_NUM_EP_USED         (3)   /* Number of EP's used by CDC function */

/* Define USB endpoint addresses for the interfaces */
#define CDC_EP_DATA_OUT   (0x01)   /* Endpoint for CDC data reception.       */
#define CDC_EP_DATA_IN    (0x81)   /* Endpoint for CDC data transmission.    */
#define CDC_EP_NOTIFY     (0x82)   /* The notification endpoint (not used).  */

/* Define timer ID's */
#define CDC_TIMER_ID        (1)

#ifdef __cplusplus
}
#endif

#endif /* USBCONFIG_H_ */
