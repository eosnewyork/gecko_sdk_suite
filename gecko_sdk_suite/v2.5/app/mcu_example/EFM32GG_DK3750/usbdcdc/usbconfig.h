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

#define USB_DEVICE        /* Compile stack for device mode. */

/****************************************************************************
**                                                                         **
** Specify number of endpoints used (in addition to EP0).                  **
**                                                                         **
*****************************************************************************/
#define NUM_EP_USED 3

/****************************************************************************
**                                                                         **
** Specify number of application timers you need.                          **
**                                                                         **
*****************************************************************************/
#define NUM_APP_TIMERS 1

/****************************************************************************
**                                                                         **
** Configuration options for the CDC driver.                               **
**                                                                         **
*****************************************************************************/

#define CDC_CTRL_INTERFACE_NO   (0)
#define CDC_DATA_INTERFACE_NO   (1)

/* Endpoint definitions. */
#define CDC_EP_DATA_OUT   (0x01)   /* Endpoint for CDC data reception.       */
#define CDC_EP_DATA_IN    (0x81)   /* Endpoint for CDC data transmission.    */
#define CDC_EP_NOTIFY     (0x82)   /* The notification endpoint (not used).  */

#define CDC_TIMER_ID              (0)
#define CDC_UART_TX_DMA_CHANNEL   (0)
#define CDC_UART_RX_DMA_CHANNEL   (1)
#define CDC_TX_DMA_SIGNAL         DMAREQ_UART1_TXBL
#define CDC_RX_DMA_SIGNAL         DMAREQ_UART1_RXDATAV
#define CDC_UART                  UART1
#define CDC_UART_CLOCK            cmuClock_UART1
#define CDC_UART_ROUTE            (UART_ROUTE_RXPEN   \
                                   | UART_ROUTE_TXPEN \
                                   | UART_ROUTE_LOCATION_LOC2)
#define CDC_UART_TX_PORT          gpioPortB
#define CDC_UART_TX_PIN           9
#define CDC_UART_RX_PORT          gpioPortB
#define CDC_UART_RX_PIN           10
#define CDC_ENABLE_DK_UART_SWITCH() BSP_PeripheralAccess(BSP_RS232_UART, true)

#ifdef __cplusplus
}
#endif

#endif /* __USBCONFIG_H */
