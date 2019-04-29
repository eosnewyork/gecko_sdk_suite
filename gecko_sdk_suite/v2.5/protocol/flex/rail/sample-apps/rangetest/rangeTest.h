/***************************************************************************//**
 * @file
 * @brief Range Test Software Example.
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

#ifndef RANGETEST_H_
#define RANGETEST_H_

#include "bsp.h"
#include "retargetserialhalconfig.h"
#include "rail_config.h"
#include "rail_types.h"
#include "rail_chip_specific.h"
#include "rangetestconfigtypes.h"
#include "pushButton.h"

// ----------------------------------------------------------------------------
// Defines

// RSSI chart Y scale values
#define RADIO_CONFIG_RSSI_MIN_VALUE "-115"
#define RADIO_CONFIG_RSSI_MID_VALUE "-42"
#define RADIO_CONFIG_RSSI_MAX_VALUE "10"

// ----------------------------------------------------------------------------
// Types

/// This enumeration contains the supported modes of the Range Test.
typedef enum {
  RADIO_MODE_TRX,   ///> Transceiver mode, unused.
  RADIO_MODE_RX,    ///> Receiver Mode. Device listens for packet.
  RADIO_MODE_TX,    ///> Transmitter Mode. Device sends packets.
  NUMOF_RADIO_MODES ///> Number of enum states
} sRadioModes_e;

/// Range Test status variables that contain settings and data that is
/// used during the Range Test execution.
typedef struct rangeTest_t{
  float    PER;           ///> Current Packet Error Rate.
  float    MA;            ///> Current Moving Average.
  uint32_t maHistory[4u]; ///> Array that stores history to calculate MA.
  uint16_t pktsSent;      ///> Number of sent packets.
  uint16_t pktsCnt;       ///> Counter in received packet.
  uint16_t pktsRcvd;      ///> Number of CRC OK packets.
  uint16_t pktsOffset;    ///> Counter offset in first received packet.
  uint16_t pktsReq;       ///> Number of requested packets to send.
  uint16_t pktsCRC;       ///> Number of CRC error packets.
  uint16_t channel;       ///> Selected channel for TX and RX.
  uint8_t  phy;       ///> Selected phy
  uint8_t  radioMode;     ///> TX or RX operation.
  int16_t  txPower;       ///> Radio transmit power in 0.1 dBm steps
  uint8_t  destID;        ///> ID of the other device to send or receive.
  uint8_t  srcID;         ///> ID of this device.
  uint8_t  payload;       ///> Payload length of the packets.
  uint8_t  retransmitCnt;
  uint8_t  rssiMode;
  uint8_t  rssiLatch;     ///> RSSI value logged for the last RX packet.
  uint8_t  maSize;        ///> Moving Average window size (no. of values).
  uint8_t  maFinger;      ///> Points to current value in the MA window.
  bool     log;           ///> True if UART logging is enabled in menu.
  bool     isRunning;     ///> True if the Range Test is running.
} rangeTest_t;

/// Packet structure.
#pragma pack(1)
typedef struct rangeTestPacket_t{
  uint16_t pktCounter;    ///> Value showing the number of this packet.
  uint8_t  destID;        ///> Destination device ID this packet was sent to.
  uint8_t  srcID;         ///> Device ID which shows which device sent this packet.
  uint8_t  repeat;        ///> Unused.
} rangeTestPacket_t;
#pragma pack()

// Time spacing between two packets in ms
#define RANGETEST_TX_PERIOD               100

// Invalid counter value
#define RANGETEST_PACKET_COUNT_INVALID    0xFFFF

// Repeate TX mode
#define RANGETEST_TX_REPEAT               0xFFFF

// Default payload length value
#define RANGETEST_PAYLOAD_LEN_MIN   (7u)
#define RANGETEST_PAYLOAD_LEN_MAX   (64u)

// RAIL buffer sizes
#define RAIL_TX_BUFFER_SIZE         (64u)
#define RAIL_RX_BUFFER_SIZE         (256u)

// Tx output power min/max values in the LCD
#define TX_POWER_MAX           (200)     // 0.1 dBm / LSB
#define TX_POWER_MIN           (-150)    // 0.1 dBm / LSB
// Output power increment when the button is pressed
#define TX_POWER_INC           (5)       // 0.1 dBm / LSB

/**************************************************************************//**
 * @brief     Initialize GPIOs direction and default state for VCP.
 *
 * @param     None.
 *
 * @return    None.
 *****************************************************************************/
#define halInternalInitVCPPins() do                                          \
  {                                                                          \
    /* Configure GPIO pin for UART TX */                                     \
    /* To avoid false start, configure output as high. */                    \
    GPIO_PinModeSet(RETARGET_TXPORT, RETARGET_TXPIN, gpioModePushPull, 1u);  \
    /* Configure GPIO pin for UART RX */                                     \
    GPIO_PinModeSet(RETARGET_RXPORT, RETARGET_RXPIN, gpioModeInput, 1u);     \
    /* Enable the switch that enables UART communication. */                 \
    RETARGET_PERIPHERAL_ENABLE();                                            \
    RETARGET_UART->ROUTELOC0 |= USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN; \
  } while (0u)

// External RAIL handle, used to call RAIL functions
extern RAIL_Handle_t railHandle;

// External static Radio configuration
extern const uint8_t radio_config[];

//extern RAIL_ChannelConfig_t channelConfig;
extern volatile rangeTest_t rangeTest;

// for everyone to be able to iterate over the phy array
extern uint8_t phyCnt;

// ----------------------------------------------------------------------------
// Function declarations

void    rangeTestInit();
uint8_t rangeTestMAGet();
bool    runDemo();
void    setPowerLevels(bool init);
void    rangeTestStartPeriodicTx(void);
void    rangeTestStopPeriodicTx(void);

#endif /* RANGETEST_H_ */
