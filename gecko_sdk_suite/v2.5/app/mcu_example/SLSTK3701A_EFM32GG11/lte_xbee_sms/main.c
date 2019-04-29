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

#include <stdio.h>
#include <string.h>
#include "em_device.h"
#include "em_cmu.h"
#include "em_chip.h"
#include "em_emu.h"

#include "bsp.h"
#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"

#include "xbee/platform.h"
#include "xbee/device.h"
#include "xbee/sms.h"
#include "xbee/atcmd.h"

/* Defined in project settings, configures the XBee at runtime */
#ifdef XBEE_DEMO_CONFIG
  #include "xbee_config.h"
#endif

#define XBEE_TARGET_BAUD 115200

xbee_serial_t ser1;
xbee_dev_t myXbee;

/* Max PN length is 20, + 1 guarantees null termination */
char destPhoneNum[XBEE_SMS_MAX_PHONE_LENGTH + 1] = { 0 };   // Where to send
char sourcePhoneNum[XBEE_SMS_MAX_PHONE_LENGTH + 1] = { 0 }; // This XBee's number
char sendBuf[XBEE_SMS_MAX_MESSAGE_LENGTH + 1];
int messageCount = 0;

/* Local prototypes */
int xbeeSmsReceiveHandler(xbee_dev_t *xbee,
                          const void FAR *raw,
                          uint16_t length,
                          void FAR *context);
void promptUser(void);

/* Attaches our handler in the dispatch table */
#define XBEE_FRAME_HANDLE_SMS \
  { XBEE_FRAME_RECEIVE_SMS, 0, xbeeSmsReceiveHandler, NULL }

/* Table used to dispatch received frames */
const xbee_dispatch_table_entry_t xbee_frame_handlers[] =
{
  XBEE_FRAME_HANDLE_LOCAL_AT,
  XBEE_FRAME_HANDLE_SMS,
  XBEE_FRAME_TABLE_END
};

/***************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 ******************************************************************************/
void delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = xbee_millisecond_timer();
  while ((xbee_millisecond_timer() - curTicks) < dlyTicks) ;
}

/***************************************************************************//**
 * @brief Gets basic device information, only calls init once
 ******************************************************************************/
void initCommandLayer(void)
{
  xbee_cmd_init_device(&myXbee);
  /* Wait for XBee to respond */
  while (!(xbee_cmd_query_status(&myXbee) == XBEE_COMMAND_LIST_DONE
           || xbee_cmd_query_status(&myXbee) == XBEE_COMMAND_LIST_TIMEOUT)) {
    xbee_dev_tick(&myXbee);
    printf(".");
    delay(100);
  }
}

void printDeviceDescription(void)
{
  printf("\fHardware Version:\n%x\nFirmware Version:\n%x\nBaud Rate:\n%u\n",
         (unsigned int) myXbee.hardware_version,
         (unsigned int) myXbee.firmware_version,
         (unsigned int) myXbee.serport.baudrate);
}

/***************************************************************************//**
 * @brief This handler gets called when an SMS is received,
 * it then responds to the sender and sets the destination phone number
 ******************************************************************************/
int xbeeSmsReceiveHandler(xbee_dev_t *xbee,
                          const void FAR *raw,
                          uint16_t length,
                          void FAR *context)
{
  (void) xbee;
  (void) context;

  char plural[] = "s";
  const xbee_frame_receive_sms_t FAR *sms = raw;
  char message[64] = "[EMPTY MESSAGE]";
  uint16_t message_len;

  /* If this is a new phone number, save the number and send a reply. */
  if (memcmp(destPhoneNum, sms->phone, 20) != 0) {
    memcpy(destPhoneNum, sms->phone, 20);
    xbee_sms_send(&myXbee, destPhoneNum, "You are linked to this XBee", 0);
  }

  promptUser(); // Clear last message
  messageCount++;

  if (messageCount < 2) {
    plural[0] = '\0';
  }

  /* The SMS message is not null-terminate, but we can determine its length */
  /* by subtracting the length of frame_type and phone from length. */
  message_len = (length > 21) ? (length - 21) : 0;

  /* Copy sms->message into message. */
  if (message_len > 0) {
    if (message_len < sizeof(message)) {
      memcpy(message, sms->message, message_len);
      message[message_len] = '\0';
    } else {
      memcpy(message, sms->message, sizeof(message) - 1);
      message[sizeof(message) - 1] = '\0';
    }
  }
  printf("\n\nReceived %d message%s."
         "\n\nLast message:\n%s"
         "\n\nWill send to:\n%s\nOn BTN0 press",
         messageCount,
         plural,
         message,
         destPhoneNum);

  return 0;
}

/***************************************************************************//**
 * @brief If XBee sends its phone number this is called records it for our use
 * @param response The AT command response from PH issued by getPhoneNum()
 ******************************************************************************/
int xbeePhCallback(const xbee_cmd_response_t FAR *response)
{
  if (((response->flags & XBEE_CMD_RESP_MASK_STATUS) == XBEE_AT_RESP_SUCCESS)
      && response->value_length > 0) {
    memcpy(sourcePhoneNum, response->value_bytes, response->value_length);
    sourcePhoneNum[response->value_length] = 0; // Null terminate PN string
  }
  return 0;
}

/***************************************************************************//**
 * @brief If XBee sends an AI command this updates the connection status
 * @param response The AT command response from AI issued by checkConnection()
 ******************************************************************************/
int xbeeConnectCallback(const xbee_cmd_response_t FAR *response)
{
  if (((response->flags & XBEE_CMD_RESP_MASK_STATUS) == XBEE_AT_RESP_SUCCESS)
      && response->value_length == 1) {
    if (response->value_bytes[0] == 0) {
      myXbee.wpan_dev.flags |= WPAN_FLAG_JOINED;
    } else {
      myXbee.wpan_dev.flags &= ~(WPAN_FLAG_JOINED);
    }
  }
  return 0;
}

/***************************************************************************//**
 * @brief Helper function for sending AT commands
 ******************************************************************************/
void sendAtRequest(const char *command,
                   const char *parameters,
                   xbee_cmd_callback_fn callback,
                   void FAR *context)
{
  int16_t request;

  request = xbee_cmd_create(&myXbee, command);

  if (request > 0) {
    xbee_cmd_set_callback(request, callback, context);
    if (parameters != NULL) {
      xbee_cmd_set_param_bytes(request, parameters, strlen(parameters));
    }
    xbee_cmd_send(request);
    xbee_cmd_request_t *thisReq = _xbee_cmd_handle_to_address(request);
    /* Checks if the command has responded or timed-out */
    while (thisReq->device != NULL) {
      xbee_dev_tick(&myXbee);
      xbee_cmd_tick();
      delay(100);
    }
  }
}

/***************************************************************************//**
 * @brief Blocks, gets the AI (connection status) of the XBee
 ******************************************************************************/
void checkConnection(void)
{
  while (!(WPAN_DEV_IS_JOINED(&(myXbee.wpan_dev)))) {
    sendAtRequest("AI", NULL, xbeeConnectCallback, NULL);
    /* Print a dot for every attempt */
    printf(".");
    /* Wait for XBee to respond */
    delay(100);
  }
}

/***************************************************************************//**
 * @brief Blocks, gets the phone number of the XBee
 ******************************************************************************/
void getPhoneNum(void)
{
  while (sourcePhoneNum[0] == 0) {
    sendAtRequest("PH", NULL, xbeePhCallback, NULL);
    /* Print a dot for every attempt */
    printf(".");
    /* Wait for XBee to respond */
    delay(100);
  }
}

/***************************************************************************//**
 * @brief Clears the screen and displays a prompt with the XBee's phone number
 ******************************************************************************/
void promptUser(void)
{
  printf("\f");  //clear the screen
#ifdef XBEE_DEMO_HOLOGRAM_SIM
  printf("Send a text to the \nphone number of the \nSIM card.");
#else
  printf("Send a text to:\n%.*s", TEXTDISPLAY_DEVICE_0_COLUMNS, sourcePhoneNum);
#endif
}

/***************************************************************************//**
 * @brief Sets the pin-mode of push button 0 to an input with a pull up
 ******************************************************************************/
void initButton(void)
{
  // The library already starts the GPIO clock, no need to call CMU_ClockEnable
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
}

/***************************************************************************//**
 * @brief Checks the state of push button 0
 * @ret 1       if the button is pressed
 * @ret 0       if the button is not pressed
 ******************************************************************************/
int checkButton(void)
{
  return (0 == GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN));
}

#if defined(XBEE_DEMO_CONFIG) && defined(XBEE_DEMO_HOLOGRAM_SIM)
/***************************************************************************//**
 * @brief Custom configuration of XBee to work correctly with Hologram SIM card
 * @retval 0  Successfully configured XBee
 * @retval -EIO Could not configure XBeee
 ******************************************************************************/
int configureXbeeForHologramSim(void)
{
  char buff[64];
  uint8_t i;
  uint32_t rxTimeout;
  int ch;

  if (configureXBee(&myXbee,
                    XBEE_TARGET_BAUD,
                    XBEE_CONFIG_MODE_BYPASS,
                    XBEE_CONFIG_CARRIER_AUTO_DETECT,
                    TRUE)) {
    return -EIO;
  } else {
    /* Disable echo so that response to AT+URAT=7 command will be predictable */
    delay(8000);
    xbee_ser_write(&myXbee.serport, "ATE0\r", 10);
    delay(1000);
    xbee_ser_rx_flush(&myXbee.serport);

    /* Disable NB-IoT */
    xbee_ser_write(&myXbee.serport, "AT+URAT=7\r", 10);

    /* Wait to receive "OK" response */
    rxTimeout = xbee_millisecond_timer();
    i = 0;
    while (xbee_millisecond_timer() - rxTimeout < 2000) {
      ch = xbee_ser_getchar(&myXbee.serport);
      if (ch >= 0) {
        buff[i] = ch;
        i++;
        if (i >= strlen("\r\nOK\r\n")) {
          buff[i] = '\0';
          break;
        }
      }
    }
  }
  /* Make sure the device responded to the AT+URAT=7 command with "OK" */
  if (strncmp(buff, "\r\nOK\r\n", strlen("\r\nOK\r\n"))) {
    return -EIO;
  }
  /* Place the XBee into API mode */
  if (configureXBee(&myXbee,
                    XBEE_TARGET_BAUD,
                    XBEE_CONFIG_MODE_API,
                    XBEE_CONFIG_CARRIER_ATT,
                    TRUE)) {
    return -EIO;
  }
  /* Reinitialize with correct settings */
  myXbee.flags &= ~XBEE_DEV_FLAG_CMD_INIT;
  myXbee.flags &= ~XBEE_DEV_FLAG_QUERY_BEGIN;

  return 0;
}
#endif

/***************************************************************************//**
 * @brief A top level function to call all init. and config. functions for XBee
 * @retval 0    Successfully established and configured XBee connection
 * @retval -EIO Could not establish connection to XBeee
 *
 * @note Specific error messages are printed to the display
 ******************************************************************************/
int initXBee(void)
{
  xbee_serial_t ser1;
  ser1.baudrate = XBEE_TARGET_BAUD;
  /* Initialize the XBee device (identifies device) */
  if (xbee_dev_init(&myXbee, &ser1, NULL, NULL)) {
    printf("\fCould not open communication with XBee\n");
    return -EIO;
  }

#if defined(XBEE_DEMO_CONFIG) && defined(XBEE_DEMO_HOLOGRAM_SIM)
  printf("\fConfigure XBee for \nHologram SIM");
  int retval = configureXbeeForHologramSim();
  if (retval) {
    printf("\fFailed to config XBee\n"
           "for Hologram SIM.\n"
           "\nCheck that the XBee\n"
           "is powered and is\n"
           "connected properly");
    return retval;
  }
#endif

  /* Command layer initialization (AT commands) */
  printf("\fInitializing");
  initCommandLayer();

  /* Check for successful communication with the XBee */
  if (myXbee.hardware_version == 0) {
    printf("\fFailed to initialize\nattempting to\nreconfigure the XBee\n");
#if defined(XBEE_DEMO_CONFIG) && !defined(XBEE_DEMO_HOLOGRAM_SIM)
    if (configureXBee(&myXbee,
                      XBEE_TARGET_BAUD,
                      XBEE_CONFIG_MODE_API,
                      XBEE_CONFIG_CARRIER_ATT,
                      TRUE)) {
      printf("\fFailed to config\n"
             "check that the XBee\n"
             "is powered and is\n"
             "connected properly");
      return -EIO;
    }
    /* Reinitialize with correct settings */
    myXbee.flags &= ~XBEE_DEV_FLAG_CMD_INIT;
    myXbee.flags &= ~XBEE_DEV_FLAG_QUERY_BEGIN;
    printf("\fInitializing");
    initCommandLayer();
#else
    /* Without the config code, we cannot continue */
    printf("\nCould not reconfig,\ninclude config code");
    return -EIO;
#endif
  }

#if defined(XBEE_DEMO_CONFIG) && defined(XBEE_CHANGE_APN)
  /* Attempt to Apply the correct APN as defined by XBEE_TARGET_APN */
  printf("\fAttempting to apply\nthe new APN\n");
  int ret = configureAPN(&myXbee);
  if (ret < 0) {
    printf("\fFailed to apply\nthe new APN\n");
  } else if (ret == 1) {
    printf("The APN was already\nset correctly!\n");
  } else {
    printf("The new APN has been\nset correctly!\n");
  }
#endif

  return 0;
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator with kit specific parameters */
  EMU_DCDCInit(&dcdcInit);

  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
    while (1) ;
  }

  /* Initialize the display module. */
  DISPLAY_Init();

  /* Retarget stdio to a text display. */
  if (RETARGET_TextDisplayInit() != TEXTDISPLAY_EMSTATUS_OK) {
    while (1) ;
  }

  initButton();
  int buttonCD = 0;

  /* Calls all init. code related to the XBee */
  if (initXBee() != 0) {
    while (1) ; /* Blocks if there was an error with opening communications */
  }

  delay(1000);

  /* Prints out device information of the XBee */
  printDeviceDescription();

  delay(4000);
  /* Wait for cell connection */
  printf("\fWaiting for cell signal");
  checkConnection();

  /* Wait, Request the phone number of XBee */
#if !defined(XBEE_DEMO_HOLOGRAM_SIM)
  printf("\fGetting phone number");
  getPhoneNum();
#endif

  /* Display XBee's phone number with prompt */
  promptUser();

  /* Update Memory LCD display forever */
  while (1) {
    /* Check for button press */
    if (checkButton()) {
      /* Do we have a destination PN and haven't pressed the button recently */
      if (destPhoneNum[0] != 0 && buttonCD <= 0) {
        /* This is purely for formatting */

        sprintf(sendBuf, "Thanks for pressing the button :)\n"
                         "I have received %d %s so far",
                messageCount,
                (messageCount == 1) ? "message" : "messages");

        xbee_sms_send(&myXbee, destPhoneNum, sendBuf, 0);
        /* Make sure you only send up to 1 SMS a second */
        buttonCD = 100;
      }
    } else {
      /* Make sure you only send up to 1 SMS a second */
      if (buttonCD > 0) {
        buttonCD--;
      }
    }

    /* Check for any incoming data */
    xbee_dev_tick(&myXbee);
    /* We don't need to check for new data from XBee that often */
    delay(10); // Delays 10 ms
  }
}
