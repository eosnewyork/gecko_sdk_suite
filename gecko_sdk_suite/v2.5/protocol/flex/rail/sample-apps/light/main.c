/***************************************************************************//**
 * @file
 * @brief Application specific overrides of weak functions defined as part of
 * the test application.
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
#include "command_interpreter.h"
#include "response_print.h"
#include "app_ci.h"
#define  APP_COMMAND_INTERFACE_BUFFER_SIZE 256

#include "plugin-common/dmp-demo-ui/dmp-ui.h"
#include "dmp-demo-utils/dmp-demo-utils.h"

// Allow local echo to be turned on/off for the command prompt
#ifdef DISABLE_LOCAL_ECHO
  #define localEcho 0
#else
  #define localEcho 1
#endif

#define SECOND_IN_MILLISECONDS (1000000)

//Command line variables
static CommandEntry_t commands[] = {
  APP_CI_COMMANDS,
  COMMAND_ENTRY(NULL, NULL, NULL, NULL)
};
static CommandState_t ciState;
static char ciBuffer[APP_COMMAND_INTERFACE_BUFFER_SIZE];

// Memory manager configuration
#define MAX_BUFFER_SIZE  256

// Minimum allowed size of the TX FIFO
#define RAIL_TRX_FIFO_SIZE 64

// General application memory sizes
#define APP_MAX_PACKET_LENGTH  (MAX_BUFFER_SIZE - 12) /* sizeof(RAIL_RxPacketInfo_t) == 12) */

#define DEMO_CONTROL_PAYLOAD_BYTE (15)

#define DEMO_LIGHT_STATUS_BROADCAST_INTERVAL (1000000)

#define PACKET_HEADER_LEN (2)

#define DEMO_CONTROL_CMD_ADVERTISE 0

// Prototypes
void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events);
void radioInit();
void gpioCallback(uint8_t pin);
void processInputCharacters(void);
void updateLed(bool lightState);
void broadcast_TimerExpired(RAIL_Handle_t rail);
static void scheduleBroadcasts();

RAIL_Handle_t railHandle = NULL;

typedef struct ButtonArray{
  GPIO_Port_TypeDef   port;
  unsigned int        pin;
} ButtonArray_t;

static const ButtonArray_t buttonArray[BSP_NO_OF_BUTTONS] = BSP_GPIO_BUTTONARRAY_INIT;

uint8_t channel = 0;
volatile bool lightStateToggle = false; //go into transfer mode
volatile bool lightStateBroadcast = false; //go into transfer mode
volatile bool packetRx = true;  //go into receive mode

static uint8_t txData[RAIL_TRX_FIFO_SIZE] = {
  0x0F, 0x16, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
  0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0x00,
};
static uint8_t rxData[RAIL_TRX_FIFO_SIZE] = { 0 };

static RAIL_Config_t railCfg = {
  .eventsCallback = &RAILCb_Generic,
};

#define LED_0 (0)
#define LED_1 (1)

typedef enum {
  LIGHT_MODE_ADVERTISE = 0,
  LIGHT_MODE_READY     = 1
} LightMode;

typedef struct {
  uint8_t addr[8];
  LightMode mode;
  char* modeText[2];
  char modeTextBuf[10];
  bool state;
} Light;

static Light light = {
  .addr = { 0 },
  .mode = LIGHT_MODE_ADVERTISE,
  .modeText = { "ADVERT", "READY" },
  .modeTextBuf = { 0 },
  .state = false
};

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

  // Initialize Radio
  radioInit();

  // Configure RAIL callbacks
  RAIL_ConfigEvents(railHandle,
                    RAIL_EVENTS_ALL,
                    (RAIL_EVENTS_RX_COMPLETION
                     | RAIL_EVENTS_TX_COMPLETION));

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

  printf("DMP Demo %s:\n", DEVICE_TYPE);
  printf("> ");

  setEUI(light.addr);
  dmpUiInit();
  light.mode = LIGHT_MODE_ADVERTISE;
  dmpUiClearMainScreen((uint8_t *)DEVICE_TYPE, true, false);
  dmpUiDisplayId(DMP_UI_PROTOCOL1, (uint8_t*)light.modeText[light.mode]);
  snprintf(light.modeTextBuf,
           sizeof(light.modeTextBuf),
           "ID:%04X", *((uint16_t*)light.addr));
  dmpUiDisplayId(DMP_UI_PROTOCOL2, (uint8_t*)light.modeTextBuf);
  ciInitState(&ciState, ciBuffer, sizeof(ciBuffer), commands);
  scheduleBroadcasts();
  uint16_t dataLength = 0;

  while (1) {
    processInputCharacters();

    if (lightStateToggle || lightStateBroadcast) {
      if (lightStateToggle) {
        light.state = !light.state;
        dmpUiDisplayLight(light.state);
        dmpUiDisplayId(DMP_UI_PROTOCOL1, (uint8_t*)light.modeText[light.mode]);
        snprintf(light.modeTextBuf,
                 sizeof(light.modeTextBuf),
                 "ID:%04X", *((uint16_t*)light.addr));
        dmpUiDisplayId(DMP_UI_PROTOCOL2, (uint8_t*)light.modeTextBuf);
        updateLed(light.state);
      }

      lightStateToggle = false;
      lightStateBroadcast = false;

      // address of light
      memcpy((void*)&txData[PACKET_HEADER_LEN],
             (void*)light.addr,
             sizeof(light.addr));
      // Set current light state.
      setRole(&txData[DEMO_CONTROL_PAYLOAD_BYTE], DEMO_CONTROL_ROLE_LIGHT);
      // advertisement packet
      if (LIGHT_MODE_ADVERTISE == light.mode) {
        setCommandType(&txData[DEMO_CONTROL_PAYLOAD_BYTE], DEMO_CONTROL_CMD_ADVERTISE);
        // status packet
      } else {
        setCommandType(&txData[DEMO_CONTROL_PAYLOAD_BYTE], DEMO_CONTROL_CMD_LIGHT_STATE_REPORT);
        setLightState(&txData[DEMO_CONTROL_PAYLOAD_BYTE], light.state);
      }

      RAIL_Idle(railHandle, RAIL_IDLE, true);
      dataLength = sizeof(txData);
      RAIL_SetTxFifo(railHandle, &txData[0], dataLength, dataLength);
      RAIL_StartTx(railHandle, 0, RAIL_TX_OPTIONS_DEFAULT, NULL);
    }

    if (packetRx) {
      packetRx = false;
      RAIL_Idle(railHandle, RAIL_IDLE, true);
      RAIL_StartRx(railHandle, 0, NULL);
    }
  }
}

/******************************************************************************
 * Utility Functions
 *****************************************************************************/
void radioInit()
{
  railHandle = RAIL_Init(&railCfg, NULL);
  if (railHandle == NULL) {
    while (1) ;
  }
  RAIL_ConfigCal(railHandle, RAIL_CAL_ALL);

  // Set us to a valid channel for this config and force an update in the main
  // loop to restart whatever action was going on
  RAIL_ConfigChannels(railHandle, channelConfigs[0], NULL);
}

void gpioCallback(uint8_t pin)
{
  // button 0: toggle light
  if (BSP_BUTTON0_PIN == pin) {
    lightStateToggle = true;
    // button 1: toggle mode
  } else if (BSP_BUTTON1_PIN == pin) {
    if (LIGHT_MODE_ADVERTISE == light.mode) {
      light.mode = LIGHT_MODE_READY;
    } else {
      light.mode = LIGHT_MODE_ADVERTISE;
    }
    dmpUiClearMainScreen((uint8_t *)DEVICE_TYPE, true, false);
    dmpUiDisplayLight(light.state);
    dmpUiDisplayId(DMP_UI_PROTOCOL1, (uint8_t*)light.modeText[light.mode]);
    snprintf(light.modeTextBuf,
             sizeof(light.modeTextBuf),
             "ID:%04X", *((uint16_t*)light.addr));
    dmpUiDisplayId(DMP_UI_PROTOCOL2, (uint8_t*)light.modeTextBuf);
  }
}

void lightToggle(int argc, char **argv)
{
  printf("Sending light toggle command.\n");
  lightStateToggle = true;
}

void updateLed(bool lightState)
{
  if (lightState) {
    BSP_LedSet(LED_0);
    BSP_LedSet(LED_1);
  } else {
    BSP_LedClear(LED_0);
    BSP_LedClear(LED_1);
  }
}

/******************************************************************************
 * RAIL Callback Implementation
 *****************************************************************************/
void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events)
{
  (void)railHandle;

  if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
    uint8_t payload = 0;
    uint16_t bytesAvailable = RAIL_GetRxFifoBytesAvailable(railHandle);
    uint16_t rxReceived = RAIL_ReadRxFifo(railHandle, rxData, bytesAvailable);
    printReceivedPackets(rxData + PACKET_HEADER_LEN, rxReceived - PACKET_HEADER_LEN);
    // accept packets in ready mode only
    if (LIGHT_MODE_READY == light.mode) {
      payload = rxData[DEMO_CONTROL_PAYLOAD_BYTE];
      // packet sent by Switch
      if (getRole(payload) == DEMO_CONTROL_ROLE_SWITCH) {
        // handle only those packets that include the Light's address
        if (!memcmp((void*)light.addr,
                    (void*)&rxData[PACKET_HEADER_LEN],
                    sizeof(light.addr))) {
          if (getCommandType(payload) == DEMO_CONTROL_CMD_LIGHT_TOGGLE) {
            light.state = !light.state;
            dmpUiDisplayLight(light.state);
            dmpUiDisplayId(DMP_UI_PROTOCOL1, (uint8_t*)light.modeText[light.mode]);
            snprintf(light.modeTextBuf,
                     sizeof(light.modeTextBuf),
                     "ID:%04X", *((uint16_t*)light.addr));
            dmpUiDisplayId(DMP_UI_PROTOCOL2, (uint8_t*)light.modeTextBuf);
            updateLed(light.state);
            lightStateBroadcast = true;
          }
        }
      }
    }
  }
  packetRx = true;
}

void processInputCharacters()
{
  char input = getchar();
  while (input != '\0' && input != 0xFF) {
    if (localEcho) {
      if (input != '\n') {
        printf("%c", input);
      }
    }
    if (ciProcessInput(&ciState, &input, 1) > 0) {
      printf("> ");
    }
    input = getchar();
  }
}

/******************************************************************************
 * Command Interpreter Override Functions
 *****************************************************************************/

// Error callback function using responsePrint
void ciErrorCallback(char* command, CommandError_t error)
{
  if (error == CI_UNKNOWN_COMMAND) {
    responsePrintError("ci",
                       CI_UNKNOWN_COMMAND,
                       "Unknown command '%s'",
                       command);
  } else if (error == CI_MAX_ARGUMENTS) {
    responsePrintError("ci",
                       CI_MAX_ARGUMENTS,
                       "CI_MAX_ARGUMENTS '%s'",
                       command);
  } else if (error == CI_INVALID_ARGUMENTS) {
    responsePrintError("ci",
                       CI_INVALID_ARGUMENTS,
                       "Invalid arguments for '%s'",
                       command);
  }
}

#ifdef USE_RESPONSE_HELP
// Help callback function using responsePrint
bool ciPrintHelp(CommandEntry_t *commands)
{
  responsePrintHeader("help", "command:%s,args:%s,help:%s");
  while (commands != NULL && commands->command != NULL) {
    char *args = commands->arguments;
    char *helpStr = commands->helpStr;

    if (commands->callback != NULL) {
      // Make sure that we print something for NULL pointers
      if (args == NULL) {
        args = "";
      }
      if (helpStr == NULL) {
        helpStr = "";
      }

      // Print the multiline response for this entry
      responsePrintMulti("command:%s,args:%s,help:%s",
                         commands->command,
                         args,
                         helpStr);
    }
    commands++;
  }
  return true;
}
#endif //USE_RESPONSE_HELP

void broadcast_TimerExpired(RAIL_Handle_t rail)
{
  lightStateBroadcast = true;
  scheduleBroadcasts();
}

static void scheduleBroadcasts()
{
  RAIL_SetTimer(railHandle,
                DEMO_LIGHT_STATUS_BROADCAST_INTERVAL,
                RAIL_TIME_DELAY,
                &broadcast_TimerExpired);
}
