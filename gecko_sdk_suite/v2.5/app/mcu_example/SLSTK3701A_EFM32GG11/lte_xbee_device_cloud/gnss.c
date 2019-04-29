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

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_assert.h"
#include "kernel/include/os.h"
#include "xbee/byteorder.h"
#include "xbee/cbuf.h"
#include "gnss.h"

#define RX_BUF_LEN        31
#define TX_BUF_LEN        31
#define NAV_PVT           0x01, 0x07
#define NAV_PVT_PAY_LEN   92

static char gnssRxAlloc[XBEE_CBUF_OVERHEAD + RX_BUF_LEN];
static char gnssTxAlloc[XBEE_CBUF_OVERHEAD + TX_BUF_LEN];
static xbee_cbuf_t *gnssRx = (xbee_cbuf_t *) &gnssRxAlloc;
static xbee_cbuf_t *gnssTx = (xbee_cbuf_t *) &gnssTxAlloc;

static uint8_t gnssNavPvts[2][NAV_PVT_PAY_LEN];
static int gnssNavPvtFront = -1; // -1 for no packet yet
static bool gnssNavPvtLocks[2] = { false, false };

static void gnssPutAll(xbee_cbuf_t *cbuf, const void *data, size_t len)
{
  while (len) {
    size_t put = xbee_cbuf_put(cbuf, data, len);
    if (put) {
      USART_IntEnable(GNSS_USART, USART_IF_TXBL);
    }
    data = (char *) data + put;
    len = len - put;
  }
}

void gnssCksum(size_t n, const unsigned char *data, uint8_t cksumLe[2])
{
  uint8_t *lo = &cksumLe[0];
  uint8_t *hi = &cksumLe[1];
  for (size_t i = 0; i < n; i++) {
    *lo += data[i];
    *hi += *lo;
  }
}

static void gnssSendPacket(uint8_t cls,
                           uint8_t id,
                           uint16_t len,
                           const void *payload)
{
  uint8_t header[] = {
    0xB5,
    0x62,
    cls,
    id,
    len & 0xFF,
    len >> 8
  };

  uint8_t cksumLe[2] = { 0 };
  gnssCksum(sizeof(header) - 2, header + 2, cksumLe);
  gnssCksum(len, payload, cksumLe);

  gnssPutAll(gnssTx, &header, sizeof(header));
  gnssPutAll(gnssTx, payload, len);
  gnssPutAll(gnssTx, &cksumLe, sizeof(cksumLe));
}

static void gnssRxHandler(void)
{
  static enum {SYNC, CLSID, LENGTH, PAYLOAD, CKSUM, STATE_MAX} state = SYNC;
  static size_t i = 0;

  static const uint8_t sync[] = { 0xB5, 0x62 };
  static const uint8_t navPvtClsid[] = { NAV_PVT };
  static uint_fast16_t len = 0;
  static uint8_t cksumLe[] = { 0x0, 0x0 };
  // So far, this is a valid NAV-PVT packet:
  static bool validNavPvt = true;

  uint8_t ch = USART_Rx(GNSS_USART);

  if (!validNavPvt) {
    goto next;
  }

  switch (state) {
    case SYNC:
      if (ch != sync[i]) {
        goto reset;
      }
      break;
    case CLSID:
      if (ch != navPvtClsid[i]) {
        validNavPvt = false;
      }
      break;
    case LENGTH:
      len |= ((uint_fast16_t) ch) << (8 * i);
      if ((len ^ NAV_PVT_PAY_LEN) & (0xFF << (8 * i))) {
        validNavPvt = false;
      }
      break;
    case PAYLOAD:
      if (gnssNavPvtLocks[!gnssNavPvtFront]) {
        // Back buffer is locked. Drop this packet.
        validNavPvt = false;
      } else {
        EFM_ASSERT(validNavPvt);
        gnssNavPvts[!gnssNavPvtFront][i] = ch;
      }
      break;
    case CKSUM:
      if (ch != cksumLe[i]) {
        validNavPvt = false;
      }
      break;
    case STATE_MAX:
      EFM_ASSERT(false);
  }

  // Update checksum
  if ((state != SYNC) && (state != CKSUM)) {
    gnssCksum(1, &ch, cksumLe);
  }

  next:
  i += 1;
  if (i >= ((state == PAYLOAD) ? len : 2)) {
    state += 1;
    i = 0;
  }
  if (state == STATE_MAX) {
    if (validNavPvt) {
      // Flip the double buffer
      gnssNavPvtFront = !gnssNavPvtFront;
    }
    goto reset;
  }
  return;

  reset:
  state = SYNC;
  i = 0;
  len = 0;
  memset(cksumLe, 0, 2);
  validNavPvt = true;
}

static void gnssTxHandler(void)
{
  USART_IntDisable(GNSS_USART, USART_IF_TXBL);
  while (GNSS_USART->STATUS & USART_STATUS_TXBL) {
    int ch = xbee_cbuf_getch(gnssTx);
    if (ch == -1) {
      return;
    }
    USART_Tx(GNSS_USART, ch);
  }
  USART_IntEnable(GNSS_USART, USART_IF_TXBL);
}

void GNSS_USART_RX_IRQ_NAME(void)
{
  NVIC_EnableIRQ(GNSS_USART_TX_IRQn);
  gnssRxHandler();
}

void GNSS_USART_TX_IRQ_NAME(void)
{
  gnssTxHandler();
}

/** Power down the GNSS module, including V_BCKP, then power it back up. */
static void gnssInitPower(void)
{
  GPIO_PinModeSet(GNSS_PORT, GNSS_PWR_EN_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(GNSS_PORT, GNSS_VBCKP_PIN, gpioModePushPull, 0);

  // The CAM-M8's data sheet does not seem to specify how long the power must
  // be off to ensure a reset. Here, we use 2 s.
  RTOS_ERR err;
  OSTimeDlyHMSM(0, 0, 2, 0, OS_OPT_TIME_DLY, &err);
  EFM_ASSERT(err.Code == RTOS_ERR_NONE);

  GPIO_PinOutSet(GNSS_PORT, GNSS_VBCKP_PIN);
  GPIO_PinOutSet(GNSS_PORT, GNSS_PWR_EN_PIN);
}

static void gnssInitUART(void)
{
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(GNSS_USART_cmuClock, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  GPIO_PinModeSet((GPIO_Port_TypeDef) (GNSS_USART_RX_PORT(USART_ROUTELOC0_RXLOC_LOC0)),
                  GNSS_USART_RX_PIN(USART_ROUTELOC0_RXLOC_LOC0),
                  gpioModeInput,
                  0);
  GPIO_PinModeSet((GPIO_Port_TypeDef) (GNSS_USART_TX_PORT(USART_ROUTELOC0_TXLOC_LOC0)),
                  GNSS_USART_TX_PIN(USART_ROUTELOC0_TXLOC_LOC0),
                  gpioModePushPull,
                  1);

  USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;
  init.baudrate = 9600;
  init.enable = usartDisable;
  USART_InitAsync(GNSS_USART, &init);
  GNSS_USART->ROUTELOC0 = USART_ROUTELOC0_RXLOC_LOC0 | USART_ROUTELOC0_TXLOC_LOC0;
  GNSS_USART->ROUTEPEN = USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;

  // RX Interrupts
  USART_IntClear(GNSS_USART, USART_IF_RXDATAV);
  NVIC_ClearPendingIRQ(GNSS_USART_RX_IRQn);
  USART_IntEnable(GNSS_USART, USART_IF_RXDATAV);
  NVIC_EnableIRQ(GNSS_USART_RX_IRQn);

  // TX Interrupts
  USART_IntClear(GNSS_USART, USART_IF_TXBL);
  NVIC_ClearPendingIRQ(GNSS_USART_TX_IRQn);
  USART_IntEnable(GNSS_USART, USART_IF_TXBL);
  // RX IRQ will enable the TX IRQ once the GNSS module
  // starts transmitting (and so is presumably ready to receive).

  USART_Enable(GNSS_USART, usartEnable);
}

static void gnssInitCfg(void)
{
  uint8_t payloads[][3] = {
    { 0xF0, 0x00, 0 }, // GGA
    { 0xF0, 0x01, 0 }, // GLL
    { 0xF0, 0x02, 0 }, // GSA
    { 0xF0, 0x03, 0 }, // GSV
    { 0xF0, 0x04, 0 }, // RMC
    { 0xF0, 0x05, 0 }, // VTG
    { 0x01, 0x07, 1 }, // UBX-NAV-PVT
  };

  for (size_t i = 0; i < sizeof(payloads) / sizeof(*payloads); i++) {
    gnssSendPacket(0x06, 0x01, sizeof(payloads[i]), payloads[i]);
  }
}

void gnssInit(void)
{
  xbee_cbuf_init(gnssRx, RX_BUF_LEN);
  xbee_cbuf_init(gnssTx, TX_BUF_LEN);
  gnssInitPower();
  gnssInitUART();
  gnssInitCfg();
}

int gnssSln(GnssSln_t *sln)
{
  // This function may be unsafe to call from an ISR:
  EFM_ASSERT(OSIntNestingCtr == 0);

  USART_IntDisable(GNSS_USART, USART_IF_RXDATAV);
  if (gnssNavPvtFront == -1) {
    USART_IntEnable(GNSS_USART, USART_IF_RXDATAV);
    return -EAGAIN;
  }
  EFM_ASSERT(gnssNavPvtFront == !!gnssNavPvtFront); // 0 or 1
  uint8_t *nav_pvt = gnssNavPvts[gnssNavPvtFront];
  gnssNavPvtLocks[gnssNavPvtFront] = true; // Lock the front buffer
  USART_IntEnable(GNSS_USART, USART_IF_RXDATAV);

  sln->fixType = (GnssFix_t) nav_pvt[20];
  sln->fixOK = nav_pvt[21] & 1;
  sln->numSV = nav_pvt[23];
  sln->lat = le32toh(*(int32_t *) &nav_pvt[28]);
  sln->lon = le32toh(*(int32_t *) &nav_pvt[24]);
  sln->height = le32toh(*(int32_t *) &nav_pvt[32]);
  sln->heightMSL = le32toh(*(int32_t *) &nav_pvt[36]);
  sln->hAcc = le32toh(*(uint32_t *) &nav_pvt[40]);
  sln->vAcc = le32toh(*(uint32_t *) &nav_pvt[44]);
  sln->gSpeed = le32toh(*(int32_t *) &nav_pvt[60]);
  sln->sAcc = le32toh(*(uint32_t *) &nav_pvt[68]);

  USART_IntDisable(GNSS_USART, USART_IF_RXDATAV);
  gnssNavPvtLocks[gnssNavPvtFront] = false; // Unlock the buffer
  USART_IntEnable(GNSS_USART, USART_IF_RXDATAV);

  return 0;
}
