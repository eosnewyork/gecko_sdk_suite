/***************************************************************************//**
 * @file main.c
 * @brief Simple Multi-PHY TRX Example
 * @copyright Copyright 2017 Silicon Laboratories, Inc. http://www.silabs.com
 ******************************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

#include "rail.h"
#include "hal_common.h"
#include "rail_config.h"

#include "em_chip.h"
#include "em_prs.h"
#include "em_cmu.h"
#include "em_core.h"

#if defined(HAL_CONFIG)
#include "hal-config.h"
#endif

#include "bsp.h"
#include "bsphalconfig.h"
#include "gpiointerrupt.h"
#include "command_interpreter.h"
#include "retargetserial.h"

#define TX_FIFO_SIZE 64
#define DEFAULT_CHANNEL 0

#define ACTIVE_CHANNEL_CONFIG_NUMS 2
#define PREAMBLE_LENGTH_TO_SCAN 32

#define ENABLE_PRS

/* Prototypes */

void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events);
void RAILCb_Timer(RAIL_Handle_t railHandle);
void gpioCallback(uint8_t pin);
static void RAILCb_RadioConfigChanged(RAIL_Handle_t railHandle,
                                      const RAIL_ChannelConfigEntry_t *entry);
void RAILCb_AssertFailed(RAIL_Handle_t railHandle, uint32_t errorCode);

typedef enum state {
  RX_SCAN_START, RX_SCAN,
  RX_START, RX, RX_PACKET, RX_PRINT,
  TX_START, TX, TX_PRINT,
  RX_RECEIVE
} state_t;

typedef struct ButtonArray {
  GPIO_Port_TypeDef port;
  unsigned int pin;
} ButtonArray_t;

static const ButtonArray_t buttonArray[BSP_NO_OF_BUTTONS] =
  BSP_GPIO_BUTTONARRAY_INIT;

/* Default */

static RAIL_Handle_t railHandle;
static RAIL_Config_t railCfg = { .eventsCallback = &RAILCb_Generic, };
static const RAIL_Events_t events = (
  RAIL_EVENTS_RX_COMPLETION | RAIL_EVENTS_TX_COMPLETION | RAIL_EVENT_CAL_NEEDED
  | RAIL_EVENT_RX_PREAMBLE_DETECT | RAIL_EVENT_RX_PREAMBLE_LOST);

/* App related */

/* Program flow variables: flags and the state holder */
static bool scanModeEnabled = true;
static volatile int16_t txRequestedOnChannel = -1;

static volatile state_t state = RX_START;

/* Channel Selector */
static uint16_t activeChannel = DEFAULT_CHANNEL;

/* Scan times on the predefined channels */
static RAIL_Time_t scanTimes[ACTIVE_CHANNEL_CONFIG_NUMS];

/* Memory allocation for transmit buffer */
static uint8_t txBuffer[16] = {
  0x00,
  0x11,
  0x22,
  0x33,
  0x44,
  0x55,
  0x66,
  0x77,
  0x88,
  0x99,
  0xaa,
  0xbb,
  0xcc,
  0xdd,
  0xee,
  0xff
};

/* Command Line Interface function definitions and variables */

void CLI_TestRx(int argc, char **argv);
void CLI_TestTx(int argc, char **argv);
void CLI_TestRxScan(int argc, char **argv);

static CommandState_t commandState;

static char ciBuffer[256];
static CommandEntry_t commands[] = {
  COMMAND_ENTRY("rx", "u", CLI_TestRx, "Switch Radio to RX mode. [arg: channel selector]"),
  COMMAND_ENTRY("tx", "u", CLI_TestTx, "Send a Packet on the given channel. [arg: channel selector]"),
  COMMAND_ENTRY("rx-scan", "", CLI_TestRxScan, "Switch mode to the default, channel scanner mode."),
  COMMAND_ENTRY(NULL, NULL, NULL, NULL)
};

void initRadio()
{
  halInit();
  railHandle = RAIL_Init(&railCfg, NULL);
  if (railHandle == NULL) {
    while (1)
      ;
  }
  RAIL_ConfigCal(railHandle, RAIL_CAL_ALL);
  RAIL_ConfigChannels(railHandle, channelConfigs[0], RAILCb_RadioConfigChanged);

  RAIL_TxPowerConfig_t txPowerConfig = {
#if HAL_PA_2P4_LOWPOWER
    .mode = RAIL_TX_POWER_MODE_2P4_LP,
#else
    .mode = RAIL_TX_POWER_MODE_2P4_HP,
#endif
    .voltage = BSP_PA_VOLTAGE, .rampTime = HAL_PA_RAMP,
  };
  if (channelConfigs[0]->configs[0].baseFrequency < 1000000000UL) {
    txPowerConfig.mode = RAIL_TX_POWER_MODE_SUBGIG;
  }
  if (RAIL_ConfigTxPower(railHandle, &txPowerConfig) != RAIL_STATUS_NO_ERROR) {
    while (1)
      ;
  }
  RAIL_SetTxPower(railHandle, HAL_PA_POWER);

  // Configure events and state transitions
  RAIL_ConfigEvents(railHandle, events, events);

  RAIL_StateTransitions_t defaultStateTransition = {
    .error = RAIL_RF_STATE_IDLE,
    .success = RAIL_RF_STATE_IDLE
  };
  RAIL_SetRxTransitions(railHandle, &defaultStateTransition);
}

/* Inlines */

static inline void setScanTimes(void)
{
  for (uint16_t i = 0; i < ACTIVE_CHANNEL_CONFIG_NUMS; ++i) {
    RAIL_PrepareChannel(railHandle, i);
    uint32_t bitrate = RAIL_GetBitRate(railHandle);
    scanTimes[i] = ((PREAMBLE_LENGTH_TO_SCAN * 1000000) / bitrate) + 90;
  }
}

static inline void sendPacket()
{
  RAIL_Idle(railHandle, RAIL_IDLE, true);
  RAIL_SetTxFifo(railHandle, txBuffer, 16, TX_FIFO_SIZE);
  RAIL_StartTx(railHandle, activeChannel, RAIL_TX_OPTIONS_DEFAULT, NULL);
}

void configPRS(void);

void initApp(void);

int main(void)
{
  CHIP_Init();
  initRadio();
  initApp();

  setScanTimes();

  printf("RAIL Simple Multi-PHY TRX Application\n");
  printf("Scantimes configured to:\n");
  uint32_t preambleUs = 0;
  for ( int i = 0; i < ACTIVE_CHANNEL_CONFIG_NUMS; i++) {
    printf("ch%d: %ldus; ", i, scanTimes[i]);
    preambleUs += scanTimes[i];
  }
  printf("\n");
  printf("> ");

  while (1) {
    switch (state) {
      case TX_START:
        sendPacket();
        state = TX;
        break;
      case TX_PRINT:
        printf("Tx on ch%d\n", activeChannel);
        state = RX_START;
        break;
      case RX_START:
        RAIL_CancelTimer(railHandle);
        RAIL_Idle(railHandle, RAIL_IDLE, true);
        if ( txRequestedOnChannel >= 0 ) { //handle pending tx events first
          activeChannel = txRequestedOnChannel;
          txRequestedOnChannel = -1;
          state = TX_START;
        } else if ( scanModeEnabled ) {
          activeChannel = (activeChannel + 1) % ACTIVE_CHANNEL_CONFIG_NUMS;

          //clear pending events - otherwise we might get an
          RAIL_ConfigEvents(railHandle, RAIL_EVENTS_ALL, RAIL_EVENTS_NONE);
          RAIL_ConfigEvents(railHandle, events, events);

          state = RX;
          RAIL_StartRx(railHandle, activeChannel, NULL);

          //set up timer for channel changing
          RAIL_SetTimer(railHandle, RAIL_GetTime() + scanTimes[activeChannel],
                        RAIL_TIME_ABSOLUTE, RAILCb_Timer);
        } else {
          state = RX;
          RAIL_StartRx(railHandle, activeChannel, NULL);
        }
        break;
      case RX_PRINT:
        BSP_LedToggle(activeChannel);
        printf("Packet received on channel %d!\n", activeChannel);
        if ( scanModeEnabled ) {
          state = RX_START;
        } else {
          state = RX; //auto state transition restarted the radio
        }
        break;
      default:
        //waiting for events
        break;
    }

    char input = getchar();
    if (input != '\0' && input != 0xFF) {
      if (ciProcessInput(&commandState, &input, 1) > 0) {
        printf("> ");
      }
    }
  }

  return 0;
}

void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events)
{
  if (events & RAIL_EVENTS_RX_COMPLETION) {
    if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
      state = RX_PRINT;
    } else if ( scanModeEnabled ) {
      state = RX_START;
    }
  }

  if (events & RAIL_EVENTS_TX_COMPLETION) {
    state = TX_PRINT;
  }

  if (events & RAIL_EVENT_RX_PREAMBLE_DETECT) {
    RAIL_CancelTimer(railHandle);
    state = RX_PACKET;
  }

  if (events & RAIL_EVENT_RX_PREAMBLE_LOST) {
    state = RX_START;
  }

  if (events & RAIL_EVENT_CAL_NEEDED) {
    RAIL_Calibrate(railHandle, NULL, RAIL_CAL_ALL_PENDING);
  }
}

void RAILCb_Timer(RAIL_Handle_t railHandle)
{
  if ( state == RX ) {
    state = RX_START;
  }
}

static void RAILCb_RadioConfigChanged(RAIL_Handle_t railHandle,
                                      const RAIL_ChannelConfigEntry_t *entry)
{
  // Ensure that the correct PA is configured for use. If it is correct,
  // we don't need to do anything as RAIL library takes care of setting
  // the power level according to channel limits. If the PA needs to change
  // however, the app needs to make that change explicitly and re-set the
  // power.
  RAIL_TxPowerConfig_t txConfig;
  RAIL_GetTxPowerConfig(railHandle, &txConfig);
  if ( entry->baseFrequency > 1000000000UL && txConfig.mode == RAIL_TX_POWER_MODE_SUBGIG ) {
#if HAL_PA_2P4_LOWPOWER
    txConfig.mode = RAIL_TX_POWER_MODE_2P4_LP;
#else
    txConfig.mode = RAIL_TX_POWER_MODE_2P4_HP;
#endif
    RAIL_ConfigTxPower(railHandle, &txConfig);
    RAIL_SetTxPower(railHandle, HAL_PA_POWER);
  } else if ( entry->baseFrequency < 1000000000UL && (txConfig.mode == RAIL_TX_POWER_MODE_2P4_HP
                                                      || txConfig.mode == RAIL_TX_POWER_MODE_2P4_LP)) {
    txConfig.mode = RAIL_TX_POWER_MODE_SUBGIG;
    RAIL_ConfigTxPower(railHandle, &txConfig);
    RAIL_SetTxPower(railHandle, HAL_PA_POWER);
  }
}

void initApp(void)
{
  // Default serial end sequence is CR + LF (\r\n)
  RETARGET_SerialCrLf(1);

  BSP_LedsInit();

#ifdef ENABLE_PRS
  // Active PRS channels are RX (EXP_HEADER 15) and TX (EXP_HEADER 16)
  configPRS();
#endif

  // Initialize button interrupt on WSTK pushbuttons
  for (int i = 0; i < BSP_NO_OF_BUTTONS; i++) {
    GPIO_PinModeSet(buttonArray[i].port, buttonArray[i].pin, gpioModeInputPull,
                    1);
  }

  GPIOINT_Init();
  GPIOINT_CallbackRegister(buttonArray[0].pin, gpioCallback);
  GPIOINT_CallbackRegister(buttonArray[1].pin, gpioCallback);
  GPIO_IntConfig(buttonArray[0].port, buttonArray[0].pin, false, true, true);
  GPIO_IntConfig(buttonArray[1].port, buttonArray[1].pin, false, true, true);

  // Initialize command line interface
  ciInitState(&commandState, ciBuffer, sizeof(ciBuffer), commands);
}

void gpioCallback(uint8_t pin)
{
  BSP_LedToggle(pin & 0x01);
  if ( !scanModeEnabled ) {
    activeChannel = pin & 0x01;
    state = TX_START;
  } else {
    txRequestedOnChannel = pin & 0x01;
  }
}

void cli_waitForRxStateThenStop()
{
  while ( state != RX )
    ;
  RAIL_CancelTimer(railHandle);
  RAIL_Idle(railHandle, RAIL_IDLE, true);
}

void CLI_TestRx(int argc, char **argv)
{
  if (argc < 2) {
    printf("Please select channel in argument\n");
    return;
  }

  if ( RAIL_IsValidChannel(railHandle, ciGetUnsigned(argv[1])) != RAIL_STATUS_NO_ERROR ) {
    printf("Channel is not valid\n");
    return;
  }

  //it's only safe to change the mode in RX state
  cli_waitForRxStateThenStop();
  state = RX_START;
  RAIL_StateTransitions_t defaultStateTransition = {
    .error = RAIL_RF_STATE_RX,
    .success = RAIL_RF_STATE_RX
  };
  RAIL_SetRxTransitions(railHandle, &defaultStateTransition);
  scanModeEnabled = false;

  activeChannel = ciGetUnsigned(argv[1]);

  printf("Rx on ch%d\n", activeChannel);
}

void CLI_TestTx(int argc, char **argv)
{
  if (argc < 2) {
    printf("Please select channel in argument\n");
    return;
  }

  if ( RAIL_IsValidChannel(railHandle, ciGetUnsigned(argv[1])) != RAIL_STATUS_NO_ERROR ) {
    printf("Channel is not valid\n");
    return;
  }

  if ( !scanModeEnabled ) {
    activeChannel = ciGetUnsigned(argv[1]);
    state = TX_START;
  } else {
    txRequestedOnChannel = ciGetUnsigned(argv[1]);
  }
}

void CLI_TestRxScan(int argc, char **argv)
{
  cli_waitForRxStateThenStop();

  state = RX_START;
  RAIL_StateTransitions_t defaultStateTransition = {
    .error = RAIL_RF_STATE_IDLE,
    .success = RAIL_RF_STATE_IDLE
  };
  RAIL_SetRxTransitions(railHandle, &defaultStateTransition);
  scanModeEnabled = true;

  printf("Rx scan mode\n");
}

void configPRS()
{
  GPIO_PinModeSet(gpioPortC, 10, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortC, 11, gpioModePushPull, 0);

  halEnablePrs(9, 15, gpioPortC, 10, PRS_RAC_RX >> 8, PRS_RAC_RX & 0xFF);
  halEnablePrs(10, 5, gpioPortC, 11, PRS_RAC_TX >> 8, PRS_RAC_TX & 0xFF);
}

void RAILCb_AssertFailed(RAIL_Handle_t railHandle, uint32_t errorCode)
{
  static const char* railErrorMessages[] = RAIL_ASSERT_ERROR_MESSAGES;
  const char *errorMessage = "Unknown";
  // If this error code is within the range of known error messages then use
  // the appropriate error message.
  if (errorCode < (sizeof(railErrorMessages) / sizeof(char*))) {
    errorMessage = railErrorMessages[errorCode];
  }
  printf(errorMessage);
  // Reset the chip since an assert is a fatal error
  NVIC_SystemReset();
}
