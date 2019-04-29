/***************************************************************************//**
 * @file main.c
 * @brief Application specific overrides of weak functions defined as part of
 * the test application.
 * @copyright Copyright 2015 Silicon Laboratories, Inc. http://www.silabs.com
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_emu.h"

#include "radio/radio.h"
#include "rail.h"
#include "rail_types.h"
#include "rail_chip_specific.h"
#include "pa_conversions_efr32.h"
#include "pa_curves_efr32.h"
#include "rail_config.h"

#include "hal_common.h"
#include "bsp.h"

// Memory manager configuration
#define RADIO_RX_PACKET_BUFFER_SIZE 256

// TX FIFO size
#define RADIO_TX_FIFO_SIZE 256

// General application memory sizes
#define APP_MAX_PACKET_LENGTH  (RADIO_RX_PACKET_BUFFER_SIZE - 12)   /* sizeof(RAIL_RxPacketInfo_t) == 12) */
#define RAIL_RF_XTAL_FREQ      (38400000)

void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events);
void RAILCb_RfReady(RAIL_Handle_t railHandle);

static int  channel = 0;
static int  currentConfig = 0; //default is first in list
static volatile uint16_t rxBufferSize = 0;
static uint8_t *rxBuffer = NULL;
static uint8_t txFifo[RADIO_TX_FIFO_SIZE];

static bool    radioRFReady = false;
static volatile bool    packetSent = false;
static bool    packetRx   = false;
//static uint8_t packetSentCount;
static uint32_t RADIO_cTune;

static uint8_t currentTxMode;
static bool    receiveModeEnabled;
static int32_t txPower;

static RAIL_RxPacketDetails_t RADIO_rxPacketInfo;
static RAIL_TxPacketDetails_t RADIO_txPacketInfo;

RAIL_Handle_t railHandle = NULL;

// Parameters used in the initialization of rail
#if BSP_PA_VOLTAGE == 3300
RAIL_DECLARE_TX_POWER_VBAT_CURVES(piecewiseSegments, curvesSg, curves24Hp, curves24Lp);
#else
RAIL_DECLARE_TX_POWER_DCDC_CURVES(piecewiseSegments, curvesSg, curves24Hp, curves24Lp);
#endif

static RAIL_Config_t railCfg = {
  .eventsCallback = &RAILCb_Generic,
};

static const RAIL_TxPowerConfig_t powerConfig = {
  .mode = RAIL_TX_POWER_MODE_2P4_HP,
  .voltage = 3300,
  .rampTime = 10,
};

uint32_t RADIO_changeConfig(int newConfig)
{
  uint32_t status = 0;

  /* Stop TX before changing the config */
  if ((currentTxMode == RADIO_RAIL_TX_MODE_TONE)
      || (currentTxMode == RADIO_RAIL_TX_MODE_STREAM)) {
    RAIL_StopTxStream(railHandle);
  }

  /* Turn off the radio before reconfiguring it */
  RAIL_Idle(railHandle, RAIL_IDLE, true);

  /* Set the radio to a valid channel */
  channel = RAIL_ConfigChannels(railHandle, channelConfigs[0], NULL);
  currentConfig = newConfig;

  return status;
}

void RADIO_init(uint32_t xoTune)
{
  // Initialize the system clocks and other HAL components
  halInit();

  /* */
  //packetSentCount = 0;

  // Initialize Radio
  radioRFReady = false;
  railHandle = RAIL_Init(&railCfg, &RAILCb_RfReady);
  if (railHandle == NULL) {
    while (1) ;
  }

  RAIL_TxPowerCurvesConfig_t txPowerCurvesConfig = {
    curves24Hp, curvesSg, curves24Lp, piecewiseSegments
  };
  RAIL_InitTxPowerCurves(&txPowerCurvesConfig);

  while ( !radioRFReady ) {
  }
  RAIL_ConfigCal(railHandle, RAIL_CAL_ALL);

  // Initialize the PA now that the HFXO is up and the timing is correct
  RAIL_TxPowerConfig_t txPowerConfig = {
#if HAL_PA_2P4_LOWPOWER
    .mode = RAIL_TX_POWER_MODE_2P4_LP,
#else
    .mode = RAIL_TX_POWER_MODE_2P4_HP,
#endif
    .voltage = BSP_PA_VOLTAGE,
    .rampTime = HAL_PA_RAMP,
  };
  if (RAIL_ConfigTxPower(railHandle, &txPowerConfig) != RAIL_STATUS_NO_ERROR) {
    // Error: The PA could not be initialized due to an improper configuration.
    // Please ensure your configuration is valid for the selected part.
    while (1) ;
  }

  // Configure max output power
  RAIL_SetTxPower(railHandle, HAL_PA_POWER);

  /* Set XO tune value if any */
  if ( xoTune != 0 ) {
    RADIO_cTune = xoTune;
    //RAIL_SetTune(railHandle, xoTune);
  }

  // Configure modem, packet handler
  RADIO_changeConfig(currentConfig);

  // Configure RAIL callbacks
  RAIL_ConfigEvents(railHandle,
                    RAIL_EVENTS_ALL,
                    (RAIL_EVENT_RX_PACKET_RECEIVED
                     | RAIL_EVENT_TX_PACKET_SENT));
  RAIL_SetTxFifo(railHandle, txFifo, 0, RADIO_TX_FIFO_SIZE);

  currentTxMode      = RADIO_RAIL_TX_MODE_OFF;
  receiveModeEnabled = false;
  txPower            = RAIL_GetTxPowerDbm(railHandle);

  return;
}

uint8_t RADIO_txCWEnable(bool enable)
{
  if ( enable ) {
    RAIL_StartTxStream(railHandle, channel, RAIL_STREAM_CARRIER_WAVE);
  } else {
    RAIL_StopTxStream(railHandle);
  }

  return channel;
}

uint32_t RADIO_getCTune(void)
{
  return RAIL_GetTune(railHandle);
}

void RADIO_setCTune(uint32_t tune)
{
  RADIO_cTune = tune;
  RAIL_SetTune(railHandle, RADIO_cTune);
  return;
}

int RADIO_receivePacket(uint8_t buffer[], uint16_t bufferSize, uint16_t timeout,
                        RAIL_RxPacketDetails_t **rxPacket)
{
  int ret = 0;

  volatile uint16_t loopCount;

  /* Setup */
  packetRx      = false;
  rxBuffer      = &buffer[0];
  rxBufferSize  = bufferSize;

  /* Start receiver */
  RAIL_Idle(railHandle, RAIL_IDLE, false);
  RAIL_StartRx(railHandle, channel, NULL);

  /* Wait for packet to arrive */
  loopCount = 0;
  while ( (packetRx == false) && (loopCount < timeout) ) {
    loopCount++;
    EMU_EnterEM1();
  }
  RAIL_Idle(railHandle, RAIL_IDLE, false);

  if ( packetRx ) {
    ret = 1;
    *rxPacket = &RADIO_rxPacketInfo;
  }

  return ret;
}

int RADIO_sendPacket(uint8_t buffer[],
                     uint16_t bufferSize,
                     uint16_t txPayloadSize,
                     uint16_t timeout,
                     RAIL_TxPacketDetails_t **txPacketInfo)
{
  int ret;
  uint8_t radioStat;
  uint32_t loopCount;
  uint32_t loopLimit;
  uint16_t writtenBytes;

  packetSent = false;
  ret        = 0;
  loopCount  = 0;
  loopLimit  = 0;

  loopLimit = timeout * 1000;

  RADIO_txPacketInfo.isAck = false;
  RADIO_txPacketInfo.timeSent.timePosition = RAIL_PACKET_TIME_AT_PACKET_END;
  RADIO_txPacketInfo.timeSent.totalPacketBytes = txPayloadSize;

  /* Send packet */
  RAIL_Idle(railHandle, RAIL_IDLE, true);
  writtenBytes = RAIL_WriteTxFifo(railHandle, buffer, bufferSize, true);
  if ( writtenBytes != bufferSize ) {
    printf("WriteTxFifo() failed! Wrote %02X of %02X\r\n", writtenBytes, bufferSize);
  }
  radioStat = RAIL_StartTx(railHandle, channel, RAIL_TX_OPTIONS_DEFAULT, NULL);
  if ( radioStat != 0 ) {
    printf("StartTx() failed! stat = %02X\r\n", radioStat);
  }

  /* Wait for packet to show up */
  while ( (packetSent != true) && (loopCount < loopLimit) ) {
    loopCount++;
  }
  packetSent = false;
  RAIL_Idle(railHandle, RAIL_IDLE, true);

  if ( loopCount < loopLimit ) {
    *txPacketInfo = &RADIO_txPacketInfo;
    ret = 1;
  }

  return ret;
}

uint32_t RADIO_getReg(uint32_t addr)
{
  volatile uint32_t *reg = (volatile uint32_t *)addr;

  return *reg;
}

void RADIO_setReg(uint32_t addr, uint32_t value)
{
  volatile uint32_t *reg = (volatile uint32_t *)addr;

  *reg = value;

  return;
}

void RADIO_setRxMode(bool enable)
{
  if ( currentTxMode == RADIO_RAIL_TX_MODE_OFF ) {
    RAIL_Idle(railHandle, RAIL_IDLE, true);
    if ( enable ) {
      RAIL_StartRx(railHandle, channel, NULL);
    }
  }

  receiveModeEnabled = enable;
}

bool RADIO_getRxMode(void)
{
  return receiveModeEnabled;
}

uint32_t RADIO_setChannel(uint16_t newChannel)
{
  uint32_t status;

  status = RAIL_IsValidChannel(railHandle, newChannel);

  if ( status == RAIL_STATUS_NO_ERROR ) {
    channel = newChannel;

    RAIL_Idle(railHandle, RAIL_IDLE, true);

    if ( currentTxMode == RADIO_RAIL_TX_MODE_TONE ) {
      RAIL_StartTxStream(railHandle, channel, RAIL_STREAM_CARRIER_WAVE);
    } else if ( currentTxMode == RADIO_RAIL_TX_MODE_STREAM ) {
      RAIL_StartTxStream(railHandle, channel, RAIL_STREAM_PN9_STREAM);
    } else if ( receiveModeEnabled ) {
      RAIL_StartRx(railHandle, channel, NULL);
    }
  }
  return status;
}

uint16_t RADIO_getChannel(void)
{
  return channel;
}

void RADIO_setTxMode(uint8_t mode)
{
  if ( currentTxMode == RADIO_RAIL_TX_MODE_OFF ) {
    RAIL_Idle(railHandle, RAIL_IDLE, true);
    if ( mode == RADIO_RAIL_TX_MODE_TONE ) {
      RAIL_StartTxStream(railHandle, channel, RAIL_STREAM_CARRIER_WAVE);
      currentTxMode = mode;
    }
  } else if ( mode == RADIO_RAIL_TX_MODE_OFF ) {
    if ( currentTxMode == RADIO_RAIL_TX_MODE_TONE ) {
      RAIL_StopTxStream(railHandle);
      currentTxMode = mode;
    }
  }
}

uint8_t RADIO_getTxMode(void)
{
  return currentTxMode;
}

int RADIO_getConfig(void)
{
  return currentConfig;
}

void RADIO_setPower(int power)
{
  RAIL_SetTxPowerDbm(railHandle, power);
  txPower = RAIL_GetTxPowerDbm(railHandle);

  RAIL_Idle(railHandle, RAIL_IDLE, true);
  if ( currentTxMode == RADIO_RAIL_TX_MODE_TONE ) {
    RAIL_StartTxStream(railHandle, channel, RAIL_STREAM_CARRIER_WAVE);
  } else if ( currentTxMode == RADIO_RAIL_TX_MODE_STREAM ) {
    RAIL_StartTxStream(railHandle, channel, RAIL_STREAM_PN9_STREAM);
  } else if ( receiveModeEnabled ) {
    RAIL_StartRx(railHandle, channel, NULL);
  }
}

int RADIO_getPower(void)
{
  return txPower;
}

/******************************************************************************
 * RAIL Callback Implementation
 *****************************************************************************/
void RAILCb_RfReady(RAIL_Handle_t railHandle)
{
  radioRFReady = true;
  return;
}

void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events)
{
  if ( events & RAIL_EVENT_TX_PACKET_SENT ) {
    RAIL_GetTxPacketDetails(railHandle, &RADIO_txPacketInfo);
    packetSent = true;
  }
  if ( events & RAIL_EVENT_RX_PACKET_RECEIVED ) {
    RAIL_RxPacketInfo_t packetInfo;
    RAIL_GetRxPacketInfo(railHandle,
                         RAIL_RX_PACKET_HANDLE_NEWEST,
                         &packetInfo);

    if ((packetInfo.packetStatus != RAIL_RX_PACKET_READY_SUCCESS)
        && (packetInfo.packetStatus != RAIL_RX_PACKET_READY_CRC_ERROR)) {
      // RAIL_EVENT_RX_PACKET_RECEIVED must be handled last in order to return
      // early on aborted packets here.
      return;
    }

    RADIO_rxPacketInfo.timeReceived.totalPacketBytes = packetInfo.packetBytes;
    RADIO_rxPacketInfo.timeReceived.timePosition = RAIL_PACKET_TIME_AT_SYNC_END;
    RAIL_GetRxPacketDetails(railHandle,
                            RAIL_RX_PACKET_HANDLE_NEWEST,
                            &RADIO_rxPacketInfo);
    if ( packetInfo.packetBytes <= rxBufferSize ) {
      // Read packet data into our packet structure
      memcpy(rxBuffer,
             packetInfo.firstPortionData,
             packetInfo.firstPortionBytes);
      memcpy(rxBuffer + packetInfo.firstPortionBytes,
             packetInfo.lastPortionData,
             packetInfo.packetBytes - packetInfo.firstPortionBytes);
    }
    packetRx = true;
  }
}
