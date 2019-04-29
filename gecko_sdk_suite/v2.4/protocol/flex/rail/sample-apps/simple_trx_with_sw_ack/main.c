/***************************************************************************//**
 * @file main.c
 * @brief Application specific overrides of weak functions defined as part of
 * the test application.
 * @copyright Copyright 2015 Silicon Laboratories, Inc. http://www.silabs.com
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rail.h"
#include "rail_types.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_emu.h"
#include "bsp.h"
#include "gpiointerrupt.h"
#include "rail_config.h"
#include "hal_common.h"

// Memory manager configuration
#define MAX_BUFFER_SIZE  (256)

// Minimum allowed size of the TX FIFO
#define RAIL_TX_FIFO_SIZE (64)

// Prototypes
void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events);
void radioInit();
void gpioCallback(uint8_t pin);
bool compareArray(uint8_t *array1, uint8_t *array2, uint8_t length);

typedef struct ButtonArray{
  GPIO_Port_TypeDef   port;
  unsigned int        pin;
} ButtonArray_t;

static const ButtonArray_t buttonArray[BSP_NO_OF_BUTTONS] = BSP_GPIO_BUTTONARRAY_INIT;

RAIL_Handle_t railHandle = NULL;

uint8_t channel = 0;
volatile bool dataTx  = false; //send a data packet
volatile bool ackTx    = false; //send an ack
volatile bool packetRx = true;  //go into receive mode
uint8_t receiveBuffer[MAX_BUFFER_SIZE];

static uint8_t txFifo[RAIL_TX_FIFO_SIZE];

static uint8_t data[] = {
  0x0F, 0x16, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
  0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE,
};

static uint8_t ack[] = {
  0x0F, 0x16, 0x00, 0x11, 0x00, 0x11, 0x00, 0x11,
  0x00, 0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x11,
};

static RAIL_Config_t railCfg = {
  .eventsCallback = &RAILCb_Generic,
};

#define LED_RX_DATA (0)
#define LED_RX_ACK  (1)

int main(void)
{
  // Initialize the chip
  CHIP_Init();

  // Initialize the system clocks and other HAL components
  halInit();

  CMU_ClockEnable(cmuClock_GPIO, true);

  // Initialize the BSP
  BSP_Init(BSP_INIT_BCC);

  // Initialize the LEDs on the board
  BSP_LedsInit();

  // Enable the buttons on the board
  for (int i = 0; i < BSP_NO_OF_BUTTONS; i++) {
    GPIO_PinModeSet(buttonArray[i].port, buttonArray[i].pin, gpioModeInputPull, 1);
  }

  // Button Interrupt Config
  GPIOINT_Init();
  GPIOINT_CallbackRegister(buttonArray[0].pin, gpioCallback);
  GPIOINT_CallbackRegister(buttonArray[1].pin, gpioCallback);
  GPIO_IntConfig(buttonArray[0].port, buttonArray[0].pin, false, true, true);
  GPIO_IntConfig(buttonArray[1].port, buttonArray[1].pin, false, true, true);

  // Initilize Radio
  radioInit();

  // Configure RAIL callbacks, with buffer error callbacks
  RAIL_ConfigEvents(railHandle,
                    RAIL_EVENTS_ALL,
                    (RAIL_EVENT_RX_PACKET_RECEIVED
                     | RAIL_EVENT_TX_PACKET_SENT
                     | RAIL_EVENT_TX_BLOCKED
                     | RAIL_EVENT_TX_ABORTED
                     | RAIL_EVENT_TX_UNDERFLOW));

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
    printf("Error: The PA could not be initialized due to an improper configuration.\n");
    printf("       Please ensure your configuration is valid for the selected part.\n");
    while (1) ;
  }
  RAIL_SetTxPower(railHandle, HAL_PA_POWER);
  RAIL_SetTxFifo(railHandle, txFifo, 0, sizeof(txFifo));

  while (1) {
    if (dataTx || ackTx) {
      RAIL_Idle(railHandle, RAIL_IDLE, true);
      if (ackTx) {
        RAIL_WriteTxFifo(railHandle, ack, sizeof(ack), true);
      } else if (dataTx) {
        RAIL_WriteTxFifo(railHandle, data, sizeof(data), true);
      }

      dataTx = false;
      ackTx = false;
      RAIL_StartTx(railHandle, channel, RAIL_TX_OPTIONS_DEFAULT, NULL);
    }

    if (packetRx) {
      packetRx = false;
      RAIL_Idle(railHandle, RAIL_IDLE, true);
      RAIL_StartRx(railHandle, channel, NULL);
    }
  }
}

/******************************************************************************
 * Configuration Utility Functions
 *****************************************************************************/
void radioInit()
{
  railHandle = RAIL_Init(&railCfg, NULL);
  if (railHandle == NULL) {
    printf("Error: Unable to initializes RAIL.\n");
    while (1) ;
  }
  RAIL_Idle(railHandle, RAIL_IDLE, true);
  RAIL_ConfigCal(railHandle, RAIL_CAL_ALL);

  // Set us to a valid channel for this config and force an update in the main
  // loop to restart whatever action was going on
  RAIL_ConfigChannels(railHandle, channelConfigs[0], NULL);
}

void gpioCallback(uint8_t pin)
{
  dataTx = true;
}

// Compares two arrays of equal length up to 255 bytes
bool compareArray(uint8_t *array1, uint8_t *array2, uint8_t length)
{
  for (int i = 0; i < length; i++) {
    if (array1[i] != array2[i]) {
      return false;
    }
  }
  return true;
}

/******************************************************************************
 * RAIL Callback Implementation
 *****************************************************************************/
void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events)
{
  if (events & RAIL_EVENT_TX_PACKET_SENT) {
    packetRx = true;
  }
  if (events & (RAIL_EVENT_TX_UNDERFLOW
                | RAIL_EVENT_TX_BLOCKED
                | RAIL_EVENT_TX_ABORTED)) {
    packetRx = true;
  }
  if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
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

    // Read packet data into our packet structure
    uint16_t length = packetInfo.packetBytes;
    memcpy(receiveBuffer,
           packetInfo.firstPortionData,
           packetInfo.firstPortionBytes);
    memcpy(receiveBuffer + packetInfo.firstPortionBytes,
           packetInfo.lastPortionData,
           length - packetInfo.firstPortionBytes);

    if ((length == sizeof(data))
        && compareArray(receiveBuffer, (uint8_t *)data, length)) {
      ackTx = true; //send an ack
      BSP_LedToggle(LED_RX_DATA); //packet match toggle the led
    } else if ((length == sizeof(ack))
               && compareArray(receiveBuffer, (uint8_t *)ack, length)) {
      BSP_LedToggle(LED_RX_ACK);
      packetRx = true;
    } else {
      packetRx = true;
    }
  }
}
