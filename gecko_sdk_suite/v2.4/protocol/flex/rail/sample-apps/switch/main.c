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

#define PACKET_HEADER_LEN (2)

#define DEMO_CONTROL_CMD_ADVERTISE 0

// Prototypes
void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events);
void radioInit();
void gpioCallback(uint8_t pin);
void processInputCharacters(void);

RAIL_Handle_t railHandle = NULL;

typedef struct ButtonArray{
  GPIO_Port_TypeDef   port;
  unsigned int        pin;
} ButtonArray_t;

static const ButtonArray_t buttonArray[BSP_NO_OF_BUTTONS] = BSP_GPIO_BUTTONARRAY_INIT;

uint8_t channel = 0;
volatile bool packetTx = false; //go into transfer mode
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
static bool lightState = false;

typedef enum {
  SWITCH_MODE_SCAN     = 0,
  SWITCH_MODE_LINKED   = 1
} SwitchState;

static char switchTextBuf[10];
static char* switchText[2] = { "SCAN:", "LINK:" };
static SwitchState switchMode = SWITCH_MODE_SCAN;

typedef struct {
  uint8_t addr[8];
  int8_t rssi;
} Light;

static Light light = {
  .addr = { 0 },
  .rssi = -128
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

  dmpUiInit();
  dmpUiClearMainScreen((uint8_t *)DEVICE_TYPE, true, false);
  dmpUiDisplayId(DMP_UI_PROTOCOL1, (uint8_t*)switchText[SWITCH_MODE_SCAN]);

  ciInitState(&ciState, ciBuffer, sizeof(ciBuffer), commands);

  setEUI(&txData[PACKET_HEADER_LEN]);

  switchMode = SWITCH_MODE_SCAN;
  while (1) {
    processInputCharacters();

    if (packetTx) {
      packetTx = false;
      RAIL_Idle(railHandle, RAIL_IDLE, true);
      uint16_t dataLength = sizeof(txData);
      // Set current light state.
      setRole(&txData[DEMO_CONTROL_PAYLOAD_BYTE], DEMO_CONTROL_ROLE_SWITCH);
      // TODO: put address of light into packet
      memcpy((void*)&txData[PACKET_HEADER_LEN],
             (void*)light.addr,
             sizeof(light.addr));
      setCommandType(&txData[DEMO_CONTROL_PAYLOAD_BYTE], DEMO_CONTROL_CMD_LIGHT_TOGGLE);
      RAIL_SetTxFifo(railHandle, &txData[0], dataLength, dataLength);
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
  uint8_t blankAddr[8] = { 0 };
  // button 0: send packet to Light
  if (buttonArray[0].pin == pin) {
    if (SWITCH_MODE_LINKED == switchMode) {
      // address of light stored, allow packets to be sent
      if (memcmp((void*)light.addr, (void*)blankAddr, sizeof(light.addr))) {
        packetTx = true;
      }
    }
    // button 1: toggle mode of the Switch
  } else if (buttonArray[1].pin == pin) {
    if (SWITCH_MODE_SCAN == switchMode) {
      dmpUiClearMainScreen((uint8_t *)DEVICE_TYPE, true, false);
      dmpUiDisplayLight(lightState);
      if (memcmp((void*)light.addr, (void*)blankAddr, sizeof(light.addr))) {
        snprintf(switchTextBuf,
                 sizeof(switchTextBuf),
                 "%s%04X",
                 switchText[SWITCH_MODE_LINKED],
                 *((uint16_t*)light.addr));
      } else {
        snprintf(switchTextBuf,
                 sizeof(switchTextBuf),
                 "%s",
                 switchText[SWITCH_MODE_LINKED]);
      }
      dmpUiDisplayId(DMP_UI_PROTOCOL1, (uint8_t*)switchTextBuf);
      switchMode = SWITCH_MODE_LINKED;
    } else {
      dmpUiClearMainScreen((uint8_t *)DEVICE_TYPE, true, false);
      dmpUiDisplayLight(lightState);
      if (memcmp((void*)light.addr, (void*)blankAddr, sizeof(light.addr))) {
        snprintf(switchTextBuf,
                 sizeof(switchTextBuf),
                 "%s%04X",
                 switchText[SWITCH_MODE_SCAN],
                 *((uint16_t*)light.addr));
      } else {
        snprintf(switchTextBuf,
                 sizeof(switchTextBuf),
                 "%s",
                 switchText[SWITCH_MODE_SCAN]);
      }
      dmpUiDisplayId(DMP_UI_PROTOCOL1, (uint8_t*)switchTextBuf);
      switchMode = SWITCH_MODE_SCAN;
    }
  }
}

void lightToggle(int argc, char **argv)
{
  printf("Sending light toggle command.\n");
  packetTx = true;
}

/******************************************************************************
 * RAIL Callback Implementation
 *****************************************************************************/
void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events)
{
  (void)railHandle;
  RAIL_RxPacketDetails_t rxPacketDetails;
  DemoControlCommandType pktType;
  // RX events
  if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
    uint16_t bytesAvailable = RAIL_GetRxFifoBytesAvailable(railHandle);
    uint16_t rxReceived = RAIL_ReadRxFifo(railHandle, rxData, bytesAvailable);
    printReceivedPackets(rxData + PACKET_HEADER_LEN, rxReceived - PACKET_HEADER_LEN);
    // packet from Light
    if (getRole(rxData[DEMO_CONTROL_PAYLOAD_BYTE]) == DEMO_CONTROL_ROLE_LIGHT) {
      pktType = (DemoControlCommandType)(((rxData[DEMO_CONTROL_PAYLOAD_BYTE]) \
                                          & DEMO_CONTROL_PAYLOAD_CMD_MASK) >> DEMO_CONTROL_PAYLOAD_CMD_MASK_SHIFT);
      // RAIL advertisement packet received in RAIL SCAN mode
      if (DEMO_CONTROL_CMD_ADVERTISE == pktType \
          && SWITCH_MODE_SCAN == switchMode) {
        // save address of light and RSSI
        RAIL_GetRxPacketDetails(railHandle,
                                RAIL_RX_PACKET_HANDLE_NEWEST,
                                &rxPacketDetails);
        // same Light; update RSSI value
        if (!memcmp((void*)light.addr, (void*)&rxData[2], sizeof(light.addr))) {
          light.rssi = rxPacketDetails.rssi;
        } else {
          // other Light with stronger signal: save ID and RSSI
          if (rxPacketDetails.rssi > light.rssi) {
            memcpy((void*)light.addr, (void*)&rxData[2], sizeof(light.addr));
            light.rssi = rxPacketDetails.rssi;
          }
        }
        dmpUiClearMainScreen((uint8_t *)DEVICE_TYPE, true, false);
        dmpUiDisplayLight(lightState);
        if (*((uint16_t*)light.addr)) {
          snprintf(switchTextBuf,
                   sizeof(switchTextBuf),
                   "%s%04X",
                   switchText[SWITCH_MODE_SCAN],
                   *((uint16_t*)light.addr));
        } else {
          snprintf(switchTextBuf,
                   sizeof(switchTextBuf),
                   "%s",
                   switchText[SWITCH_MODE_SCAN]);
        }
        dmpUiDisplayId(DMP_UI_PROTOCOL1, (uint8_t*)switchTextBuf);
        // RAIL status packet matches saved address in control mode
      } else if (DEMO_CONTROL_CMD_LIGHT_STATE_REPORT == pktType \
                 && SWITCH_MODE_LINKED == switchMode
                 && !memcmp((void*)light.addr,
                            (void*)&rxData[2],
                            sizeof(light.addr))) {
        lightState = getLightState(rxData[DEMO_CONTROL_PAYLOAD_BYTE]);
        dmpUiDisplayLight(lightState);
        snprintf(switchTextBuf,
                 sizeof(switchTextBuf),
                 "%s%04X",
                 switchText[SWITCH_MODE_LINKED],
                 *((uint16_t*)light.addr));
        dmpUiDisplayId(DMP_UI_PROTOCOL1, (uint8_t*)switchTextBuf);
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
