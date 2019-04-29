/***************************************************************************//**
 * @file rangetest.c
 * @brief Range Test Software Example.
 * @copyright Copyright 2015 Silicon Laboratories, Inc. http://www.silabs.com
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rail.h"
#include "rail_types.h"
#include "rail_chip_specific.h"
#include "pa_conversions_efr32.h"
#include "pa_curves_efr32.h"
#include "rail_config.h"

#include "spidrv.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_core.h"
#include "bsp.h"
#include "retargetserialhalconfig.h"
#include "gpiointerrupt.h"

#include "app_common.h"

#include "pushButton.h"

#include "uartdrv.h"

#include "hal_common.h"

#include "rtcdriver.h"
#include "bsp.h"
#include "graphics.h"
#include "menu.h"
#include "seq.h"

#include "rangeTest.h"

//#include "radio-configuration.h"

// ----------------------------------------------------------------------------
// Constant holding the settings for UART Log
#define USART_INIT                                                                      \
  {                                                                                     \
    RETARGET_UART,                                   /* USART port */                   \
    115200,                                          /* Baud rate */                    \
    RETARGET_TX_LOCATION,                            /* USART Tx pin location number */ \
    RETARGET_RX_LOCATION,                            /* USART Rx pin location number */ \
    (USART_Stopbits_TypeDef)USART_FRAME_STOPBITS_ONE, /* Stop bits */                   \
    (USART_Parity_TypeDef)USART_FRAME_PARITY_NONE,   /* Parity */                       \
    (USART_OVS_TypeDef)USART_CTRL_OVS_X16,           /* Oversampling mode*/             \
    false,                                           /* Majority vote disable */        \
    uartdrvFlowControlNone,                          /* Flow control */                 \
    RETARGET_CTSPORT,                                /* CTS port number */              \
    RETARGET_CTSPIN,                                 /* CTS pin number */               \
    RETARGET_RTSPORT,                                /* RTS port number */              \
    RETARGET_RTSPIN,                                 /* RTS pin number */               \
    NULL,                                            /* RX operation queue */           \
    (UARTDRV_Buffer_FifoQueue_t *)&UartTxQueue       /* TX operation queue           */ \
  }

//Tx buffer for the UART logging
DEFINE_BUF_QUEUE(64u, UartTxQueue);

// ----------------------------------------------------------------------------
// Function Prototypes
void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events);
void rangeTestMASet(uint32_t nr);
void rangeTestMAClear();
void rangeTestMAClearAll();

// Variables that is used to exchange data between the event events and
// scheduled routines
volatile bool pktRx, pktLog, txReady;
// setting and states of the Range Test
volatile rangeTest_t rangeTest;

// Memory allocation for RAIL
static uint8_t txBuffer[RAIL_TX_BUFFER_SIZE] =
{ 0x01, 0x10, 0x02, 0x20, 0x03, 0x30, 0x04, 0x40, 0x05, 0x50, };

uint8_t receiveBuffer[RAIL_RX_BUFFER_SIZE];

RAIL_Handle_t railHandle = NULL;

#if BSP_PA_VOLTAGE == 3300
RAIL_DECLARE_TX_POWER_VBAT_CURVES(piecewiseSegments, curvesSg, curves24Hp, curves24Lp);
#else
RAIL_DECLARE_TX_POWER_DCDC_CURVES(piecewiseSegments, curvesSg, curves24Hp, curves24Lp);
#endif

static RAIL_Config_t railCfg = {
  .eventsCallback = &RAILCb_Generic,
};

// UART logging handles
UARTDRV_HandleData_t  UARTHandleData;
UARTDRV_Handle_t      UARTHandle = &UARTHandleData;

void (*repeatCallbackFn)(void) = NULL;
uint32_t repeatCallbackTimeout;

/**************************************************************************//**
 * @brief Setup GPIO interrupt for pushbuttons.
 *****************************************************************************/
static void GpioSetup(void)
{
  /* Enable GPIO clock */
  CMU_ClockEnable(cmuClock_GPIO, true);

//  /* Configure PB0 as input and enable interrupt */
//  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
//
//  /* Configure PB1 as input and enable interrupt */
//  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPull, 1);

  // Enable the buttons on the board
  for (int i = 0; i < BSP_NO_OF_BUTTONS; i++) {
    GPIO_PinModeSet(buttonArray[i].port, buttonArray[i].pin, gpioModeInputPull, 1);
  }

//  /* Configure LED0 output */
//  GPIO_PinModeSet((GPIO_Port_TypeDef)ledArray[0][0], ledArray[0][1], gpioModePushPull, 0);
//
//  /* Configure LED1 output */
//  GPIO_PinModeSet((GPIO_Port_TypeDef)ledArray[1][0], ledArray[1][1], gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief   Register a callback function to be called repeatedly at the
 *          specified frequency.
 *
 * @param   pFunction: Pointer to function that should be called at the
 *                     given frequency.
 * @param   pParameter: Pointer argument to be passed to the callback function.
 * @param   frequency: Frequency at which to call function at.
 *
 * @return  0 for successful or
 *         -1 if the requested frequency is not supported.
 *****************************************************************************/
int RepeatCallbackRegister(void(*pFunction)(void*),
                           void* pParameter,
                           unsigned int timeout)
{
  repeatCallbackFn = (void(*)(void))pFunction;
  repeatCallbackTimeout = timeout;

  return 0;
}

// ----------------------------------------------------------------------------
// RAIL Callbacks

/**************************************************************************//**
 * @brief      Interrupt level callback for RAIL events.
 *
 * @return     None
 *****************************************************************************/
void  RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events)
{
  if (events & RAIL_EVENT_TX_PACKET_SENT) {
    txReady = true;

    if (rangeTest.log) {
      pktLog = true;
    }

    //LED1_TOGGLE;
  }
  if (events & RAIL_EVENT_RX_FRAME_ERROR) {
    // CRC error callback enabled
    // Put radio back to RX (RAIL doesn't support it as of now..)
    RAIL_StartRx(railHandle, rangeTest.channel, NULL);

    // Count CRC errors
    if (rangeTest.pktsCRC < 0xFFFF) {
      rangeTest.pktsCRC++;
    }
  }
  if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
    RAIL_RxPacketInfo_t packetInfo;
    RAIL_RxPacketDetails_t packetDetails;
    RAIL_RxPacketHandle_t packetHandle =
      RAIL_GetRxPacketInfo(railHandle,
                           RAIL_RX_PACKET_HANDLE_NEWEST,
                           &packetInfo);

    if ((packetInfo.packetStatus != RAIL_RX_PACKET_READY_SUCCESS)
        && (packetInfo.packetStatus != RAIL_RX_PACKET_READY_CRC_ERROR)) {
      // RAIL_EVENT_RX_PACKET_RECEIVED must be handled last in order to return
      // early on aborted packets here.
      return;
    }

    RAIL_GetRxPacketDetails(railHandle, packetHandle, &packetDetails);

    uint16_t length = packetInfo.packetBytes;

    // Read packet data into our packet structure
    memcpy(receiveBuffer,
           packetInfo.firstPortionData,
           packetInfo.firstPortionBytes);
    memcpy(receiveBuffer + packetInfo.firstPortionBytes,
           packetInfo.lastPortionData,
           length - packetInfo.firstPortionBytes);

    static uint32_t lastPktCnt = 0u;
    int8_t RssiValue = 0;

    rangeTestPacket_t * pRxPacket = (rangeTestPacket_t *) receiveBuffer;
    //LED0_TOGGLE;

    if (rangeTest.isRunning) {
      // Buffer variables for  volatile fields
      uint16_t pktsCnt;
      uint16_t pktsRcvd;

      // Put radio back to RX (RAIL doesn't support it as of now..)
      RAIL_StartRx(railHandle, rangeTest.channel, NULL);

      // Make sure the packet addressed to me
      if (pRxPacket->destID != rangeTest.srcID) {
        return;
      }

      // Make sure the packet sent by the selected remote
      if (pRxPacket->srcID != rangeTest.destID) {
        return;
      }

      if ( (RANGETEST_PACKET_COUNT_INVALID == rangeTest.pktsRcvd)
           || (pRxPacket->pktCounter <= rangeTest.pktsCnt) ) {
        // First packet received OR
        // Received packet counter lower than already received counter.

        // Reset received counter
        rangeTest.pktsRcvd = 0u;
        // Set counter offset
        rangeTest.pktsOffset = pRxPacket->pktCounter - 1u;

        // Clear RSSI Chart
        GRAPHICS_RSSIClear();

        // Clear Moving-Average history
        rangeTestMAClearAll();

        // Restart Moving-Average calculation
        lastPktCnt = 0u;
      }

      if (rangeTest.pktsRcvd < 0xFFFF) {
        rangeTest.pktsRcvd++;
      }

      rangeTest.pktsCnt = pRxPacket->pktCounter - rangeTest.pktsOffset;
      rangeTest.rssiLatch = packetDetails.rssi;

      // Calculate recently lost packets number based on newest counter
      if ((rangeTest.pktsCnt - lastPktCnt) > 1u) {
        // At least one packet lost
        rangeTestMASet(rangeTest.pktsCnt - lastPktCnt - 1u);
      }
      // Current packet is received
      rangeTestMAClear();

      lastPktCnt = rangeTest.pktsCnt;

      // Store RSSI value from the latch
      RssiValue = (int8_t)(rangeTest.rssiLatch);
      // Limit stored RSSI values to the displayable range
      RssiValue = (RssiChartAxis[GRAPHICS_RSSI_MIN_INDEX] > RssiValue)
                  // If lower than minimum -> minimum
                  ? (RssiChartAxis[GRAPHICS_RSSI_MIN_INDEX])
                  // else check if higher than maximum
                  : ((RssiChartAxis[GRAPHICS_RSSI_MAX_INDEX] < RssiValue)
                     // Higher than maximum -> maximum
                     ? (RssiChartAxis[GRAPHICS_RSSI_MAX_INDEX])
                     // else value is OK
                     : (RssiValue));

      // Store RSSI value in ring buffer
      GRAPHICS_RSSIAdd(RssiValue);

      // Calculate Error Rates here to get numbers to print in case log is enabled
      // These calculation shouldn't take too long.

      // Calculate Moving-Average Error Rate
      rangeTest.MA =  (rangeTestMAGet() * 100.0f) / rangeTest.maSize;

      // Buffering volatile values
      pktsCnt = rangeTest.pktsCnt;
      pktsRcvd = rangeTest.pktsRcvd;

      // Calculate Packet Error Rate
      rangeTest.PER = (pktsCnt) // Avoid zero division
                      ? (((float)(pktsCnt - pktsRcvd) * 100.0f)
                         / pktsCnt) // Calculate PER
                      : 0.0f;     // By default PER is 0.0%
      if (rangeTest.log) {
        pktLog = true;
      }
      pktRx = true;
    }
  }
}

/**************************************************************************//**
 * @brief  Function to generate the payload of the packet to be sent.
 *
 * @param  data: TX buffer to fill out with the gene.
 *
 * @return None.
 *****************************************************************************/
void rangeTestGeneratePayload(uint8_t *data, uint16_t dataLength)
{
  uint8_t i = sizeof(rangeTestPacket_t);

  for (; i < dataLength; i++) {
    data[i] = (i % 2u) ? (0x55u) : (0xAAu);
  }
}

/**************************************************************************//**
 * @brief  Function to count how many bits has the value of 1.
 *
 * @param  u: Input value to count its '1' bits.
 *
 * @return Number of '1' bits in the input.
 *****************************************************************************/
uint32_t rangeTestCountBits(uint32_t u)
{
  uint32_t uCount = u
                    - ((u >> 1u) & 033333333333)
                    - ((u >> 2u) & 011111111111);

  return  (((uCount + (uCount >> 3u)) & 030707070707) % 63u);
}

/**************************************************************************//**
 * @brief  This function inserts a number of bits into the moving average
 *         history.
 *
 * @param  nr: The value to be inserted into the history.
 *
 * @return None.
 *****************************************************************************/
void rangeTestMASet(uint32_t nr)
{
  uint8_t i;
  // Buffering volatile fields
  uint8_t  maFinger = rangeTest.maFinger;

  if (nr >= rangeTest.maSize) {
    // Set all bits to 1's
    i = rangeTest.maSize;

    while (i >> 5u) {
      rangeTest.maHistory[(i >> 5u) - 1u] = 0xFFFFFFFFul;
      i -= 32u;
    }
    return;
  }

  while (nr) {
    rangeTest.maHistory[maFinger >> 5u] |= (1u << maFinger % 32u);
    maFinger++;
    if (maFinger >= rangeTest.maSize) {
      maFinger = 0u;
    }
    nr--;
  }
  // Update the bufferd value back to the volatile field
  rangeTest.maFinger = maFinger;
}

/**************************************************************************//**
 * @brief  This function clears the most recent bit in the moving average
 *         history. This indicates that last time we did not see any missing
 *         packages.
 *
 * @param  nr: The value to be inserted into the history.
 *
 * @return None.
 *****************************************************************************/
void rangeTestMAClear()
{
  // Buffering volatile value
  uint8_t  maFinger = rangeTest.maFinger;

  rangeTest.maHistory[maFinger >> 5u] &= ~(1u << (maFinger % 32u));

  maFinger++;
  if (maFinger >= rangeTest.maSize) {
    maFinger = 0u;
  }
  // Updating new value back to volatile
  rangeTest.maFinger = maFinger;
}

/**************************************************************************//**
 * @brief  Clears the history of the moving average calculation.
 *
 * @return None.
 *****************************************************************************/
void rangeTestMAClearAll()
{
  rangeTest.maHistory[0u] = rangeTest.maHistory[1u]
                              = rangeTest.maHistory[2u]
                                  = rangeTest.maHistory[3u]
                                      = 0u;
}

/**************************************************************************//**
 * @brief  Returns the moving average of missing pacakges based on the
 *         history data.
 *
 * @return The current moving average .
 *****************************************************************************/
uint8_t rangeTestMAGet()
{
  uint8_t i;
  uint8_t retVal = 0u;

  for (i = 0u; i < (rangeTest.maSize >> 5u); i++) {
    retVal += rangeTestCountBits(rangeTest.maHistory[i]);
  }
  return retVal;
}

/**************************************************************************//**
 * @brief  Resets the internal status of the Range Test.
 *
 * @return None.
 *****************************************************************************/
void rangeTestInit()
{
  rangeTest.pktsCnt = 0u;
  rangeTest.pktsOffset = 0u;
  rangeTest.pktsRcvd = RANGETEST_PACKET_COUNT_INVALID;
  rangeTest.pktsSent = 0u;
  rangeTest.pktsCRC = 0u;
  rangeTest.isRunning = false;

  // Clear RSSI chart
  GRAPHICS_RSSIClear();
}

/**************************************************************************//**
 * @brief  Main function of Range Test.
 *****************************************************************************/
int main(void)
{
  uint32_t lastTick = 0u;
  UARTDRV_Init_t uartInit = USART_INIT;

  halInit();

  // RTC Init -- for system timekeeping and other useful things
  RTCDRV_Init();

  // Initialize the BSP
  BSP_Init(BSP_INIT_BCC);

  /* Setup GPIO for pushbuttons. */
  GpioSetup();

  railHandle = RAIL_Init(&railCfg, NULL);
  if (railHandle == NULL) {
    while (1) ;
  }
  RAIL_ConfigCal(railHandle, RAIL_CAL_ALL);

  // Set us to a valid channel for this config and force an update in the main
  // loop to restart whatever action was going on
  RAIL_ConfigChannels(railHandle, channelConfigs[0], NULL);

  // Configure RAIL callbacks with appended info
  RAIL_ConfigEvents(railHandle,
                    RAIL_EVENTS_ALL,
                    (RAIL_EVENT_TX_PACKET_SENT
                     | RAIL_EVENT_RX_PACKET_RECEIVED
                     | RAIL_EVENT_RX_FRAME_ERROR));

  RAIL_TxPowerCurvesConfig_t txPowerCurvesConfig = {
    curves24Hp, curvesSg, curves24Lp, piecewiseSegments
  };
  RAIL_InitTxPowerCurves(&txPowerCurvesConfig);

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
  if (channelConfigs[0]->configs[0].baseFrequency < 1000000000UL) {
    // Use the Sub-GHz PA if required
    txPowerConfig.mode = RAIL_TX_POWER_MODE_SUBGIG;
  }
  if (RAIL_ConfigTxPower(railHandle, &txPowerConfig) != RAIL_STATUS_NO_ERROR) {
    // Error: The PA could not be initialized due to an improper configuration.
    // Please ensure your configuration is valid for the selected part.
    while (1) ;
  }
  RAIL_SetTxPower(railHandle, HAL_PA_POWER);

  GRAPHICS_Init();
  pbInit();
  menuInit();
  seqInit();

  halInternalInitVCPPins();
  UARTDRV_Init(UARTHandle, &uartInit);
  GPIOINT_Init();

  rangeTestInit();

  UARTDRV_Transmit(UARTHandle, (uint8_t *) "\nRange Test EFR32\n\n", 19u, NULL);

  while (1u) {
    if ((RTCDRV_TicksToMsec(RTCDRV_GetWallClockTicks64()) - lastTick) >= 5u) {
      // 5ms loop

      pbPoll();
      seqRun();

      // Callback to replace RTCDRV stuff
      if (repeatCallbackFn != NULL) {
        static uint32_t cnt;

        if (cnt++ >= repeatCallbackTimeout) {
          repeatCallbackFn();
          cnt = 0u;
        }
      }

      lastTick = RTCDRV_TicksToMsec(RTCDRV_GetWallClockTicks64());
    }

    if (pktLog) {
      // Print log info
      char buff[115u];

      rangeTest_t rangeTestBuffered;
      memcpy((void*)&rangeTestBuffered, (void*)&rangeTest, sizeof(rangeTest));
      if (RADIO_MODE_RX == rangeTest.radioMode) {
        sprintf(buff, "Rcvd, "          //6
                      "OK:%u, "         //10
                      "CRC:%u, "        //11
                      "Sent:%u, "       //12
                      "Payld:%u, "      //10
                      "MASize:%u, "     //12
                      "PER:%3.1f, "     //11
                      "MA:%3.1f, "      //10
                      "RSSI:% 3d, "    //12
                      "IdS:%u, "        //8
                      "IdR:%u"          //8
                      "\n",             //1+1
                rangeTestBuffered.pktsRcvd,
                rangeTestBuffered.pktsCRC,
                rangeTestBuffered.pktsCnt,
                rangeTestBuffered.payload,
                rangeTestBuffered.maSize,
                rangeTestBuffered.PER,
                rangeTestBuffered.MA,
                (int8_t)rangeTestBuffered.rssiLatch,
                rangeTestBuffered.srcID,
                rangeTestBuffered.destID);
      }

      if (RADIO_MODE_TX == rangeTest.radioMode) {
        sprintf(buff,
                "Sent, Actual:%u, Max:%u, IdS:%u, IdR:%u\n",
                rangeTestBuffered.pktsSent,
                rangeTestBuffered.pktsReq,
                rangeTestBuffered.srcID,
                rangeTestBuffered.destID);
      }
      UARTDRV_Transmit(UARTHandle, (uint8_t *) buff, strlen(buff), NULL);

      pktLog = false;
    }
  }
}

/**************************************************************************//**
 * @brief  Function to execute Range Test functions, depending on the
 *         selected mode (TX or RX).
 *
 * @return None.
 *****************************************************************************/
bool runDemo()
{
  // Range Test runner
  uint8_t retVal = false;

  if (rangeTest.isRunning) {
    // Started

    switch (rangeTest.radioMode) {
      case RADIO_MODE_RX:
        if (pktRx) {
          // Refresh screen
          pktRx = false;

          retVal = true;
        }
        break;

      case RADIO_MODE_TX:
      {
        // Buffering volatile field
        uint16_t pktsSent = rangeTest.pktsSent;
        uint16_t pktsReq = rangeTest.pktsReq;

        if (pktsSent < pktsReq) {
          // Need to send more packets
          if (txReady) {
            rangeTestPacket_t * pTxPacket;

            txReady = false;

            // Send next packet
            pTxPacket = (rangeTestPacket_t*)txBuffer;

            rangeTest.pktsSent++;
            if (rangeTest.pktsSent > 50000u) {
              rangeTest.pktsSent = 1u;
            }
            pTxPacket->pktCounter = rangeTest.pktsSent;
            pTxPacket->destID = rangeTest.destID;
            pTxPacket->srcID = rangeTest.srcID;

            uint16_t txLength = rangeTest.payload;
            rangeTestGeneratePayload(txBuffer, txLength);
            RAIL_SetTxFifo(railHandle, txBuffer, txLength, txLength);

            RAIL_StartTx(railHandle,
                         rangeTest.channel,
                         RAIL_TX_OPTIONS_DEFAULT,
                         NULL);

            // Refresh screen
            retVal = true;
          }
        } else {
          // Requested amount of packets has been sent
          rangeTest.isRunning = false;

          // Refresh screen
          retVal = true;
        }
        break;
      }
      case RADIO_MODE_TRX:
        break;

      default:
        //assert!
        break;
    }
  } else {
    // Stopped

    if (RAIL_RF_STATE_IDLE != RAIL_GetRadioState(railHandle)) {
      RAIL_Idle(railHandle, RAIL_IDLE, true);
    }

    pktRx = false;
    txReady = true;

    if (RADIO_MODE_RX == rangeTest.radioMode) {
      // Can't stop RX
      rangeTest.isRunning = true;

      // Kick-start RX
      RAIL_StartRx(railHandle, rangeTest.channel, NULL);
    }
  }

  return retVal;
}
