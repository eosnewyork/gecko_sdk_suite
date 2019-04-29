/***************************************************************************//**
 * @file
 * @brief
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

#ifndef GNSS_H
#define GNSS_H

#include <stdint.h>
#include <stdbool.h>

#define GNSS_USARTn         5
#define GNSS_PORT           gpioPortC
#define GNSS_PWR_EN_PIN     4
#define GNSS_VBCKP_PIN      5

#define CONCAT_OP(a, b, c)  a ## b ## c
#define CONCAT(a, b, c)     CONCAT_OP(a, b, c)

#define GNSS_USART CONCAT(USART, GNSS_USARTn, )
#define GNSS_USART_RX_IRQn CONCAT(USART, GNSS_USARTn, _RX_IRQn)
#define GNSS_USART_TX_IRQn CONCAT(USART, GNSS_USARTn, _TX_IRQn)
#define GNSS_USART_RX_IRQ_NAME CONCAT(USART, GNSS_USARTn, _RX_IRQHandler)
#define GNSS_USART_TX_IRQ_NAME CONCAT(USART, GNSS_USARTn, _TX_IRQHandler)
#define GNSS_USART_RX_PORT CONCAT(AF_USART, GNSS_USARTn, _RX_PORT)
#define GNSS_USART_RX_PIN CONCAT(AF_USART, GNSS_USARTn, _RX_PIN)
#define GNSS_USART_TX_PORT CONCAT(AF_USART, GNSS_USARTn, _TX_PORT)
#define GNSS_USART_TX_PIN CONCAT(AF_USART, GNSS_USARTn, _TX_PIN)
#define GNSS_USART_cmuClock CONCAT(cmuClock_USART, GNSS_USARTn, )

typedef enum {
  GNSS_FIX_NONE = 0,
  GNSS_FIX_DR_ONLY = 1, // Dead reckoning only
  GNSS_FIX_2D = 2,
  GNSS_FIX_3D = 3,
  GNSS_FIX_DR
} GnssFix_t;

typedef struct {
  GnssFix_t fixType;
  bool fixOK;
  uint_fast8_t numSV; // Number of satellites used
  int_fast32_t lat; // 10**-7 deg
  int_fast32_t lon; // 10**-7 deg
  int_fast32_t height; // height above ellipsoid, mm
  int_fast32_t heightMSL; // height above MSL, mm
  uint_fast32_t hAcc; // Horizontal accuracy, mm
  uint_fast32_t vAcc; // Vertical accuracy, mm
  int_fast32_t gSpeed; // Ground speed (2D), mm/s
  uint_fast32_t sAcc; // Speed accuracy, mm/s
} GnssSln_t;

void gnssInit(void);
int gnssSln(GnssSln_t *sln);

#endif // GNSS_H
