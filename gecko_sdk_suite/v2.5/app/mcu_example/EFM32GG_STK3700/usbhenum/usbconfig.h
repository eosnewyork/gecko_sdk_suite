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

#define USB_HOST          /* Compile stack for host mode. */

/****************************************************************************
**                                                                         **
** Specify number of host channels used (in addition to EP0).              **
**                                                                         **
*****************************************************************************/
#define NUM_HC_USED 0       /* Not counting default control ep which  */
/* is assigned to host channels 0 and 1   */

/****************************************************************************
**                                                                         **
** Configure USB overcurrent flag                                          **
**                                                                         **
*****************************************************************************/
#define USB_VBUSOVRCUR_PORT       gpioPortF
#define USB_VBUSOVRCUR_PIN        6
#define USB_VBUSOVRCUR_POLARITY   USB_VBUSOVRCUR_POLARITY_LOW

#ifdef __cplusplus
}
#endif

#endif /* __USBCONFIG_H */
