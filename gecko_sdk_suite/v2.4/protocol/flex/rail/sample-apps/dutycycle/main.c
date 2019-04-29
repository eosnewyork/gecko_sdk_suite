/***************************************************************************//**
 * @file main.c
 * @brief This application allows the user to use Button 1 to switch between
 * modes (Duty Cycle, Master, Slave) and use Button 0 to transmit a packet
 * or blast packets using the RAIL library, where blast refers to a continuous
 * flow of packets. The current mode and application status is printed on the
 * LCD screen.
 * @copyright Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com
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
#include "em_core.h"

#include "rtcdriver.h"
#include "bsp.h"
#include "gpiointerrupt.h"
#include "graphics.h"

#include "rail_config.h"

#include "hal_common.h"

// Display buffer size
#ifndef APP_DISPLAY_BUFFER_SIZE
#define APP_DISPLAY_BUFFER_SIZE 64
#endif

// Memory manager configuration
#define MAX_BUFFER_SIZE 256

// Minimum allowed size of the TX FIFO
#define RAIL_TX_FIFO_SIZE 64

// General application memory sizes
#define APP_MAX_PACKET_LENGTH (MAX_BUFFER_SIZE - 12) /* sizeof(RAIL_RxPacketInfo_t) == 12) */

// buttons
typedef struct ButtonArray{
  GPIO_Port_TypeDef   port;
  unsigned int        pin;
} ButtonArray_t;
static const ButtonArray_t buttonArray[BSP_NO_OF_BUTTONS] = BSP_GPIO_BUTTONARRAY_INIT;

// Error codes
typedef enum {
  INVALID_APP_STATE,
  FAILED_TO_SET_RADIO_CONFIG
} ErrorCode;

char *errorCodeString[] =
{
  "Invalid App State         ",
  "Failed to set radio config"
};

// App states
typedef enum {
  IDLE,              // idle state
  PREAMBLE_RX,       // received a preamble
  SYNC_RX,           // received a sync
  PACKET_RX,         // listening for a packet
  PACKET_TX,         // sending a packet
  MASTER_BLAST_TX,   // blasting packets in master mode
  SLAVE_BLAST_RX,    // receiving a blast of packets in slave mode
} AppState;

// App modes
typedef enum {
  DUTY_CYCLE_MODE,  // alternate consistently between RX and IDLE
  MASTER_MODE,      // stay IDLE and blast many packets when button0 is pressed
  SLAVE_MODE,       // sleep; wake up occasionally to intercept master blast
} AppMode;

char *appModeString[] =
{
  "Duty cycle        ",
  "Master            ",
  "Slave             "
};

// Function prototypes
void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events);
void RAILCb_TimerExpired(RAIL_Handle_t railHandle);
static void lcdPrintError(int errorCode);
void gpioCallback(uint8_t pin);
static void serviceButtonPresses(void);
static void switchAppMode(AppMode mode);
static void switchDutyCycleAppState(void);
static void switchMasterAppState(void);
static void switchSlaveAppState(void);
static void updateDisplay(void);
void slaveTimerExpiredCallback(RTCDRV_TimerID_t id, void *incomingPacket);

/******************************************************************************
 * App constants and variables
 *****************************************************************************/

AppMode appMode = DUTY_CYCLE_MODE;      // Start in duty cycle mode
AppState appState = IDLE;               // Start in receive mode
AppState previousAppState = IDLE;       // Tracks previous state
bool forceStateChange = true;           // call state change irrespective of previous state
int channel = 0;                        // the channel used by the application
bool button0Pressed = false;            // is button0 pressed?
bool button1Pressed = false;            // is button1 pressed?
int packetReceived = 0;                 // RX packets count
int packetTransmitted = 0;              // TX packets count
int receiveBuffer[MAX_BUFFER_SIZE];     // buffer used for packet reception

// Parameters used in the initialization of rail
RAIL_Handle_t railHandle = NULL;

static RAIL_Config_t railCfg = {
  .eventsCallback = &RAILCb_Generic,
};

// Application payload data sent OTA with each transmission
static uint8_t data[RAIL_TX_FIFO_SIZE] = {
  0x0F, 0x16, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
  0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE,
};

/******************************************************************************
 * Duty Cycle mode constants and variables
 *****************************************************************************/

#define DUTY_CYCLE_ON_TIME          1600   // time to stay on (us)
#define DUTY_CYCLE_OFF_TIME         3400   // time to stay off (us)
#define DUTY_CYCLE_PREAMBLE_ON_TIME 10000  // time to stay on after preamble received (us)

/******************************************************************************
 * Master mode constants and variables
 *****************************************************************************/

#define MASTER_BLAST_TX_SIZE    150    // blast consecutive packets
#define MASTER_ACK_WAITING_TIME 150000 // how long to wait for Ack (us)

int masterTxBlastId = 0;            // ID for a blast
int masterTxBlastAcksReceived = 0;  // ID for ack of a blast
int masterTxBlastPacketsCount = 0;  // count of packets sent in a blast

/******************************************************************************
 * Slave mode constants and variables
 *****************************************************************************/

#define SLAVE_ON_TIME         15    // time to stay on (ms)
#define SLAVE_OFF_TIME        2500  // time to stay off (ms)
#define SLAVE_BLAST_RX_DELAY  30    // keep waiting for blast to end (ms)

int slaveRxBlastId = 0;       // blast ID of the blast received
int slaveRxBlastCount = 0;    // count of total blasts received
RTCDRV_TimerID_t slaveRtcId;  // RTC timer ID
bool slaveRtcRunning;         // boolean for RTC status

/******************************************************************************
 * App main
 *****************************************************************************/
int main(void)
{
  // Initialize the chip
  CHIP_Init();

  // Initialize the system clocks and other HAL components
  halInit();

  // RTC Init -- for system timekeeping and other useful things
  RTCDRV_Init();

  CMU_ClockEnable(cmuClock_GPIO, true);

  // Initialize the BSP
  BSP_Init(BSP_INIT_BCC);

  // Enable the buttons on the board
  for (int i = 0; i < BSP_NO_OF_BUTTONS; i++) {
    GPIO_PinModeSet(buttonArray[i].port, buttonArray[i].pin, gpioModeInputPull, 1);
  }

  // Initialize the LCD display
  GRAPHICS_Init();

  // Button Interrupt Config
  GPIOINT_Init();
  GPIOINT_CallbackRegister(buttonArray[0].pin, gpioCallback);
  GPIOINT_CallbackRegister(buttonArray[1].pin, gpioCallback);
  GPIO_IntConfig(buttonArray[0].port, buttonArray[0].pin, false, true, true);
  GPIO_IntConfig(buttonArray[1].port, buttonArray[1].pin, false, true, true);

  // Initialize Radio
  railHandle = RAIL_Init(&railCfg, NULL);
  if (railHandle == NULL) {
    while (1) ;
  }
  RAIL_Idle(railHandle, RAIL_IDLE, true);
  RAIL_ConfigCal(railHandle, RAIL_CAL_ALL);

  // Set us to a valid channel for this config and force an update in the main
  // loop to restart whatever action was going on
  RAIL_ConfigChannels(railHandle, channelConfigs[0], NULL);

  // Configure RAIL callbacks with no appended info
  RAIL_ConfigEvents(railHandle,
                    RAIL_EVENTS_ALL,
                    (RAIL_EVENT_RX_PACKET_RECEIVED
                     | RAIL_EVENT_RX_PREAMBLE_DETECT
                     | RAIL_EVENT_RX_FRAME_ERROR
                     | RAIL_EVENT_RX_SYNC1_DETECT
                     | RAIL_EVENT_RX_ADDRESS_FILTERED
                     | RAIL_EVENT_RX_FIFO_OVERFLOW
                     | RAIL_EVENT_TX_PACKET_SENT
                     | RAIL_EVENT_TX_CHANNEL_BUSY
                     | RAIL_EVENT_TX_ABORTED
                     | RAIL_EVENT_TX_BLOCKED
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
    // Error: The PA could not be initialized due to an improper configuration.
    // Please ensure your configuration is valid for the selected part.
    while (1) ;
  }
  RAIL_SetTxPower(railHandle, HAL_PA_POWER);

  updateDisplay();

  while (1) {
    // process button press behavior
    serviceButtonPresses();

    // call appropriate switch function if
    // 1) app state has changed or
    // 2) force change is requested
    if (previousAppState != appState
        || forceStateChange) {
      forceStateChange = false;
      if (appMode == DUTY_CYCLE_MODE) {
        switchDutyCycleAppState();
      } else if (appMode == SLAVE_MODE) {
        switchSlaveAppState();
      } else if (appMode == MASTER_MODE) {
        switchMasterAppState();
      }
    }
  }
}

/******************************************************************************
 * App specific functions
 *****************************************************************************/
/**************************************************************************//**
 * @brief   A function for managing what is shown on the LCD display in an
 *          error state.
 *****************************************************************************/
static void lcdPrintError(int errorCode)
{
  char textBuf[APP_DISPLAY_BUFFER_SIZE];
  // Clear what's currently on screen
  GRAPHICS_Clear();
  // Add the demo output strings
  GRAPHICS_AppendString("RAIL Duty Cycle Demo");
  GRAPHICS_AppendString("");
  snprintf(textBuf, APP_DISPLAY_BUFFER_SIZE, "ERROR: %d", errorCode);
  GRAPHICS_AppendString(textBuf);
  GRAPHICS_AppendString("");
  GRAPHICS_AppendString(errorCodeString[errorCode]);
  // Force a redraw
  GRAPHICS_Update();
}

/**************************************************************************//**
 * @brief   A callback to handle a button press on the WSTK
 *
 * @param[in] pin  The pin used to identify the button that has been pressed.
 *****************************************************************************/
void gpioCallback(uint8_t pin)
{
  // button press on 0 causes a transmit from any other state. When tx
  // is complete we'll go back into idle state.
  if (pin == buttonArray[0].pin) {
    button0Pressed = true;
  }
  // button 1 takes us into our next appState IF we are not
  // in a protected appState, like preparing to transmit or receive
  else if (pin == buttonArray[1].pin) {
    button1Pressed = true;
  }
}

/**************************************************************************//**
 * @brief  A function for handling button presses outside of interrupt
 *         context.
 *****************************************************************************/
static void serviceButtonPresses(void)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL(); // Disable interrupts so we can't be whacked while processing
  // button press on 0 causes a transmit from any other state. When tx
  // is complete we'll go back into our previous appState
  if (button0Pressed) {
    button0Pressed = false;
    if (appMode == DUTY_CYCLE_MODE
        && appState != PACKET_TX) {
      appState = PACKET_TX;
    } else if (appMode == MASTER_MODE
               && appState != MASTER_BLAST_TX) {
      masterTxBlastId++;
      appState = MASTER_BLAST_TX;
    } else if (appMode == SLAVE_MODE) {
      // not doing anything in slave mode.
    }
  }
  // button 1 takes us into our next appState IF we are not
  // in a protected appState, like preparing to transmit or receive
  else if (button1Pressed) {
    button1Pressed = false;
    // protect the transitional states and only switch if IDLE or PACKET_RX
    if (appState == PREAMBLE_RX
        || appState == SYNC_RX
        || appState == PACKET_TX
        || appState == MASTER_BLAST_TX
        || appState == SLAVE_BLAST_RX) {
      // oops nothing hapened we were busy, could print an error here
    } else if (appMode == DUTY_CYCLE_MODE) {
      switchAppMode(MASTER_MODE);
    } else if (appMode == MASTER_MODE) {
      switchAppMode(SLAVE_MODE);
    } else if (appMode == SLAVE_MODE) {
      switchAppMode(DUTY_CYCLE_MODE);
    } else {
      lcdPrintError(INVALID_APP_STATE);
      while (1) ; //should never get here
    }
  }
  CORE_EXIT_CRITICAL(); // Re-enable interrupts when we are done
}

/**************************************************************************//**
 * @brief   Function to switch application mode
 *****************************************************************************/
static void switchAppMode(AppMode mode)
{
  if (mode == SLAVE_MODE) {
    if (RAIL_IsTimerRunning(railHandle)) {
      RAIL_CancelTimer(railHandle);
    }
    RTCDRV_AllocateTimer(&slaveRtcId);   // Reserve a timer
  } else {
    RTCDRV_FreeTimer(slaveRtcId); // stop RTC timer
  }

  forceStateChange = true;
  appState = IDLE;
  appMode = mode;
  packetReceived = 0;
  slaveRxBlastId = 0;
  slaveRxBlastCount = 0;
  masterTxBlastId = 0;
  masterTxBlastAcksReceived = 0;
  masterTxBlastPacketsCount = 0;
  packetTransmitted = 0;

  updateDisplay();
}

/**************************************************************************//**
 * @brief   Function to switch application state in Duty Cycle mode
 *****************************************************************************/
static void switchDutyCycleAppState(void)
{
  previousAppState = appState;

  // cancel previous timer and set new delay below
  if (RAIL_IsTimerRunning(railHandle)) {
    RAIL_CancelTimer(railHandle);
  }

  switch (appState) {
    case IDLE:
      RAIL_SetTimer(railHandle,
                    DUTY_CYCLE_OFF_TIME,
                    RAIL_TIME_DELAY,
                    &RAILCb_TimerExpired);
      RAIL_Idle(railHandle, RAIL_IDLE, true);
      EMU_EnterEM1();
      break;
    case PREAMBLE_RX:
      RAIL_SetTimer(railHandle,
                    DUTY_CYCLE_PREAMBLE_ON_TIME,
                    RAIL_TIME_DELAY,
                    &RAILCb_TimerExpired);
      break;
    case SYNC_RX:
      // do nothing.. just wait.
      break;
    case PACKET_RX:
      RAIL_Idle(railHandle, RAIL_IDLE, true);
      RAIL_StartRx(railHandle, channel, NULL);
      RAIL_SetTimer(railHandle,
                    DUTY_CYCLE_ON_TIME,
                    RAIL_TIME_DELAY,
                    &RAILCb_TimerExpired);
      break;
    case PACKET_TX:
      RAIL_Idle(railHandle, RAIL_IDLE, true);
      data[0] = 0;
      RAIL_SetTxFifo(railHandle, data, sizeof(data), sizeof(data));
      RAIL_StartTx(railHandle, channel, RAIL_TX_OPTIONS_DEFAULT, NULL);
      break;
    default:
    {
      lcdPrintError(INVALID_APP_STATE);
      while (1) ; //should never get here
    }
  }
}

/**************************************************************************//**
 * @brief   Function to switch application state in Master mode
 *****************************************************************************/
static void switchMasterAppState(void)
{
  previousAppState = appState;

  // cancel previous timer
  if (RAIL_IsTimerRunning(railHandle)) {
    RAIL_CancelTimer(railHandle);
  }

  switch (appState) {
    case IDLE:
      RAIL_Idle(railHandle, RAIL_IDLE, true);
      break;
    case PACKET_RX:
      RAIL_SetTimer(railHandle,
                    MASTER_ACK_WAITING_TIME,
                    RAIL_TIME_DELAY,
                    &RAILCb_TimerExpired);
      RAIL_Idle(railHandle, RAIL_IDLE, true);
      RAIL_StartRx(railHandle, channel, NULL);
      break;
    case MASTER_BLAST_TX:
      data[0] = masterTxBlastId;
      RAIL_SetTxFifo(railHandle, data, sizeof(data), sizeof(data));
      RAIL_StartTx(railHandle, channel, RAIL_TX_OPTIONS_DEFAULT, NULL);
      break;
    default:
    {
      lcdPrintError(INVALID_APP_STATE);
      while (1) ; //should never get here
    }
  }

  updateDisplay();
}

/**************************************************************************//**
 * @brief   Function to switch application state in Slave mode
 *****************************************************************************/
static void switchSlaveAppState(void)
{
  previousAppState = appState;

  // cancel previous timer
  RTCDRV_IsRunning(slaveRtcId, &slaveRtcRunning);
  if (slaveRtcRunning) {
    RTCDRV_StopTimer(slaveRtcId);
  }

  switch (appState) {
    case IDLE:
      RTCDRV_StartTimer(slaveRtcId, rtcdrvTimerTypeOneshot, SLAVE_OFF_TIME, slaveTimerExpiredCallback, NULL);
      RAIL_Idle(railHandle, RAIL_IDLE, true);
      EMU_EnterEM2(true);
      break;
    case PACKET_RX:
      RAIL_Idle(railHandle, RAIL_IDLE, true);
      RAIL_StartRx(railHandle, channel, NULL);
      RTCDRV_StartTimer(slaveRtcId, rtcdrvTimerTypeOneshot, SLAVE_ON_TIME, slaveTimerExpiredCallback, NULL);
      break;
    case PACKET_TX:
      data[0] = slaveRxBlastId;
      RAIL_Idle(railHandle, RAIL_IDLE, true);
      RAIL_SetTxFifo(railHandle, data, sizeof(data), sizeof(data));
      RAIL_StartTx(railHandle, channel, RAIL_TX_OPTIONS_DEFAULT, NULL);
      break;
    case SLAVE_BLAST_RX:
      RAIL_Idle(railHandle, RAIL_IDLE, true);
      RAIL_StartRx(railHandle, channel, NULL);
      RTCDRV_StartTimer(slaveRtcId, rtcdrvTimerTypeOneshot, SLAVE_BLAST_RX_DELAY, slaveTimerExpiredCallback, NULL);
      break;
    default:
    {
      lcdPrintError(INVALID_APP_STATE);
      while (1) ; //should never get here
    }
  }

  updateDisplay();
}

/**************************************************************************//**
 * @brief   A function for managing what is shown on the LCD display under
 *          normal conditions.
 *****************************************************************************/
static void updateDisplay(void)
{
  char textBuf[APP_DISPLAY_BUFFER_SIZE];

  // Clear what's currently on screen
  GRAPHICS_Clear();

  // Add the demo output strings
  GRAPHICS_AppendString("RAIL Duty Cycle Demo");
  GRAPHICS_AppendString("");
  GRAPHICS_AppendString("----- Mode -----");
  GRAPHICS_AppendString(appModeString[appMode]);
  GRAPHICS_AppendString("");

  if (appMode == DUTY_CYCLE_MODE) {
    GRAPHICS_AppendString("----- RX -----");
    snprintf(textBuf, APP_DISPLAY_BUFFER_SIZE, "Packet Rx: %d", (int)packetReceived);
    GRAPHICS_AppendString(textBuf);
    GRAPHICS_AppendString("");
    GRAPHICS_AppendString("----- TX -----");
    snprintf(textBuf, APP_DISPLAY_BUFFER_SIZE, "Packet Tx: %d", (int)packetTransmitted);
    GRAPHICS_AppendString(textBuf);
    GRAPHICS_AppendString("");
  } else if (appMode == MASTER_MODE) {
    GRAPHICS_AppendString("----- RX -----");
    snprintf(textBuf, APP_DISPLAY_BUFFER_SIZE, "Blast Acks: %d", (int)masterTxBlastAcksReceived);
    GRAPHICS_AppendString(textBuf);
    GRAPHICS_AppendString("");
    GRAPHICS_AppendString("----- TX -----");
    snprintf(textBuf, APP_DISPLAY_BUFFER_SIZE, "Blast ID: %d", (int)masterTxBlastId);
    GRAPHICS_AppendString(textBuf);
    GRAPHICS_AppendString("");
  } else if (appMode == SLAVE_MODE) {
    GRAPHICS_AppendString("----- RX -----");
    snprintf(textBuf, APP_DISPLAY_BUFFER_SIZE, "Blast ID: %d", (int)slaveRxBlastId);
    GRAPHICS_AppendString(textBuf);
    snprintf(textBuf, APP_DISPLAY_BUFFER_SIZE, "Blasts Rx: %d", (int)slaveRxBlastCount);
    GRAPHICS_AppendString(textBuf);
    GRAPHICS_AppendString("");

    GRAPHICS_AppendString("----- TX -----");
    snprintf(textBuf, APP_DISPLAY_BUFFER_SIZE, "Acks Tx: %d", (int)packetTransmitted);
    GRAPHICS_AppendString(textBuf);
    GRAPHICS_AppendString("");
  }

  // Force a redraw
  GRAPHICS_Update();
}

/******************************************************************************
 * Callback Implementation
 *****************************************************************************/
void slaveTimerExpiredCallback(RTCDRV_TimerID_t id, void *incomingPacket)
{
  switch (appState) {
    case IDLE:  // set to RX mode, if we were in IDLE
      appState = PACKET_RX;
      break;
    case PACKET_RX: // set to IDLE, if we were in RX mode
      appState = IDLE;
      break;
    case SLAVE_BLAST_RX: // blast RX finished, so wait a bit and then ACK
      RTCDRV_Delay(2 * SLAVE_BLAST_RX_DELAY);
      appState = PACKET_TX;
      break;
    default:
    {
      lcdPrintError(INVALID_APP_STATE);
      while (1) ; //should never get here
    }
  }
}

void RAILCb_TimerExpired(RAIL_Handle_t railHandle)
{
  if (appMode == MASTER_MODE) {
    appState = IDLE;  // ACK not received, go back to IDLE
  } else if (appMode == DUTY_CYCLE_MODE) {
    switch (appState) {
      case IDLE:  // set to RX mode, if we were in IDLE
        appState = PACKET_RX;
        break;
      case PACKET_RX:   // set to IDLE, if we were in RX mode
      case PREAMBLE_RX: // set to IDLE, if we received a preamble but not sync
      case PACKET_TX:   // set to IDLE, if we were transmitting a packet
        appState = IDLE;
        break;
      default:
      {
        lcdPrintError(INVALID_APP_STATE);
        while (1) ; //should never get here
      }
    }
  }
}

void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events)
{
  // TX Radio Errors
  if (events & (RAIL_EVENT_TX_CHANNEL_BUSY
                | RAIL_EVENT_TX_ABORTED
                | RAIL_EVENT_TX_BLOCKED
                | RAIL_EVENT_TX_UNDERFLOW)) {
    if (appMode == MASTER_MODE) {
      if (masterTxBlastPacketsCount < MASTER_BLAST_TX_SIZE) {
        // blast going on
        appState = MASTER_BLAST_TX;
      }
    } else {
      // set to IDLE
      if (events & (RAIL_EVENT_TX_UNDERFLOW
                    | RAIL_EVENT_TX_CHANNEL_BUSY)) {
        appState = IDLE;
      }
    }
  }
  // Successful packet TX
  if (events & RAIL_EVENT_TX_PACKET_SENT) {
    if (appMode == DUTY_CYCLE_MODE) {
      // packet sent, go back to IDLE
      packetTransmitted++;
      appState = IDLE;
      updateDisplay();
    } else if (appMode == MASTER_MODE) {
      if (masterTxBlastPacketsCount < MASTER_BLAST_TX_SIZE) {
        // blast going on
        masterTxBlastPacketsCount++;
        appState = MASTER_BLAST_TX;
        forceStateChange = true;
      } else {
        // blast finished
        masterTxBlastPacketsCount = 0;
        appState = PACKET_RX;
      }
    } else if (appMode == SLAVE_MODE) {
      // ACK sent
      packetTransmitted++;
      slaveRxBlastCount++;
      appState = IDLE;
    }
  }
  // RX transition events
  if (events & (RAIL_EVENT_RX_PREAMBLE_DETECT
                | RAIL_EVENT_RX_SYNC1_DETECT
                | RAIL_EVENT_RX_FRAME_ERROR
                | RAIL_EVENT_RX_FIFO_OVERFLOW
                | RAIL_EVENT_RX_ADDRESS_FILTERED)) {
    if (appMode == DUTY_CYCLE_MODE) {
      // if preamble detected, continue listening for sync
      if (events & RAIL_EVENT_RX_PREAMBLE_DETECT) {
        forceStateChange = true;
        appState = PREAMBLE_RX;
      }

      // if Sync received, continue listening for packet
      if (events & RAIL_EVENT_RX_SYNC1_DETECT) {
        appState = SYNC_RX;
      }

      // if failed, switch back to idle
      if (events & (RAIL_EVENT_RX_FRAME_ERROR
                    | RAIL_EVENT_RX_FIFO_OVERFLOW
                    | RAIL_EVENT_RX_ADDRESS_FILTERED)) {
        appState = IDLE;
      }
    } else if (appMode == MASTER_MODE) {
      // ACK failed, set back to IDLE
      if (events & (RAIL_EVENT_RX_FRAME_ERROR
                    | RAIL_EVENT_RX_FIFO_OVERFLOW
                    | RAIL_EVENT_RX_ADDRESS_FILTERED)) {
        appState = IDLE;
      }
    } else if (appMode == SLAVE_MODE) {
      // continue listening for more packets in blast
      RTCDRV_StopTimer(slaveRtcId);
      appState = SLAVE_BLAST_RX;
      forceStateChange = true;
    }
  }
  // Successful packet RX
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

    if (appMode == DUTY_CYCLE_MODE) {
      // set back to idle and update display
      packetReceived++;
      appState = IDLE;
      updateDisplay();
    } else if (appMode == MASTER_MODE) {
      // increment ACK counter
      if (masterTxBlastId == packetInfo.firstPortionData[0]) {
        masterTxBlastAcksReceived++;
      }
      appState = IDLE;
    } else if (appMode == SLAVE_MODE) {
      // save the blast ID and wait for blast to end
      // after that, we will send the ACK
      if (slaveRxBlastId != packetInfo.firstPortionData[0]) {
        slaveRxBlastId = packetInfo.firstPortionData[0];
      }
      RTCDRV_StopTimer(slaveRtcId);
      appState = SLAVE_BLAST_RX;
      forceStateChange = true;
    }
  }
}
