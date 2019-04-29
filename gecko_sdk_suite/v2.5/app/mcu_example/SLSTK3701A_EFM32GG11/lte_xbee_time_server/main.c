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
#include <time.h>

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
#include "xbee/ipv4.h"
#include "xbee/atcmd.h"
#include "xbee/byteorder.h"
/* Defined in project settings, configures the XBee at runtime */
#ifdef XBEE_DEMO_CONFIG
  #include "xbee_config.h"
#endif

/* Freq. to request the time from the server (shouldn't be less than 4000) */
#define TIME_UPDATE_FREQ_MS 10000

#define TIME_SERVER_URL     "time.nist.gov"
#define TIME_PROTOCOL_PORT  37
#define XBEE_TARGET_BAUD    115200

/* Local prototypes */
void delay(uint32_t dlyTicks);
int xbeeIpv4ReceiveHandler(xbee_dev_t *xbee,
                           const void FAR *raw,
                           uint16_t length,
                           void FAR *context);
/* Global variables */
xbee_serial_t ser1;
xbee_dev_t myXbee;
xbee_ipv4_envelope_t env;

uint32_t serverAddrBE = 0;  /* The IPV4 address of the server */
uint32_t serverTime = 0;    /* Last server time fetched */
uint32_t serverTimeStampMs; /* Local msTicks time when serverTime was fetched */
struct tm t;

/* Attaches our handler in the dispatch table */
#define XBEE_FRAME_HANDLE_IPV4 \
  { XBEE_FRAME_RECEIVE_IPV4, 0, xbeeIpv4ReceiveHandler, NULL }

/* Table used to dispatch received frames */
const xbee_dispatch_table_entry_t xbee_frame_handlers[] =
{
  XBEE_FRAME_HANDLE_LOCAL_AT,
  XBEE_FRAME_HANDLE_IPV4,
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
 * @brief Formats and prints timestamp
 ******************************************************************************/
int printTime(void)
{
  char timeBuffer[64];
  size_t ret;

  ret = strftime(timeBuffer, 64, "\n\n\n\n      %T\n      %F\n", &t);
  if (ret == 0) {
    return -EINVAL;
  }

  printf(TEXTDISPLAY_ESC_SEQ_CURSOR_HOME_VT100); // set to home
  printf(timeBuffer);
  return 0;
}

void printDeviceDescription(void)
{
  printf("\fHardware Version:\n%x\nFirmware Version:\n%x\nBaud Rate:\n%u\n",
         (unsigned int) myXbee.hardware_version,
         (unsigned int) myXbee.firmware_version,
         (unsigned int) myXbee.serport.baudrate);
}

/***************************************************************************//**
 * @brief Callback for response of time server, updates time
 ******************************************************************************/
int xbeeIpv4ReceiveHandler(xbee_dev_t *xbee,
                           const void FAR *raw,
                           uint16_t length,
                           void FAR *context)
{
  (void) xbee;
  (void) length;
  (void) context;

  const xbee_frame_receive_ipv4_t FAR *ipv4 = raw;
  /* If this is the first time receiving a time update, clear screen */
  if (serverTime == 0) {
    printf("\f");
  }
  /* Copy the 32-bit seconds from server to local */
  memcpy(&serverTime, ipv4->payload, 4);
  /* Convert from network order to host order */
  serverTime = be32toh(serverTime);
  /* To keep track of time since last update */
  serverTimeStampMs = xbee_millisecond_timer();

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
 * @brief Callback for LA command for resolving a fully qualified domain name
 ******************************************************************************/
int xbeeFqdnCallback(const xbee_cmd_response_t FAR *response)
{
  if (((response->flags & XBEE_CMD_RESP_MASK_STATUS) == XBEE_AT_RESP_SUCCESS)
      && response->value_length == 4) { /* Only copy if its a valid response */
    /* Copy the big-endian address */
    memcpy(&serverAddrBE, response->value_bytes, 4);
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
 * @brief Blocks, Sends request to the XBee using LA command to resolve a FQDN
 ******************************************************************************/
void resolveTimeServerFqdn(void)
{
  while (serverAddrBE == 0) {
    sendAtRequest("LA", TIME_SERVER_URL, xbeeFqdnCallback, NULL);
    /* Print a dot for every attempt */
    printf(".");
    /* Wait for XBee to respond */
    delay(100);
  }
}

/***************************************************************************//**
 * @brief Opens a TCP socket with the time server (no payload)
 ******************************************************************************/
void sendTimeRequest(void)
{
  env.xbee = &myXbee; /* The device to send it on */
  env.remote_addr_be = serverAddrBE; /* Big endian IPV4 address */
  env.local_port = 0; /* For a new socket connection, local port must be 0 */
  env.remote_port = TIME_PROTOCOL_PORT; /* The destination port */
  env.protocol = XBEE_IPV4_PROTOCOL_TCP; /* TCP protocol */
  env.options = 0; /* Options, see documentation (0 to leave socket open) */
  env.length = 0; /* The length of the payload */
  env.payload = NULL; /* Pointer to the payload */
  xbee_ipv4_envelope_send(&env); /* Finally send the packet */
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
                    XBEE_CONFIG_CARRIER_AUTO_DETECT,
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
 * @retval 0  Successfully established and configured XBee connection
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
                      XBEE_CONFIG_CARRIER_AUTO_DETECT,
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

  /* Calls all init. code related to the XBee */
  if (initXBee() != 0) {
    while (1) ; /* Blocks if there was an error with opening communications */
  }

  delay(1000);
  printDeviceDescription();
  delay(4000);

  /* Wait for cell connection */
  printf("\fWaiting for signal");
  checkConnection();

  /* Wait for domain name resolution */
  printf("\fResolving FQDN");
  resolveTimeServerFqdn();

  char stringAddr[16];
  xbee_ipv4_ntoa(stringAddr, serverAddrBE);
  printf("\fGetting Server Time\nFrom server:\n%s", stringAddr);

  uint32_t delayer = 0;
  uint32_t requestTimeStamp;
  uint32_t thisMs;

  /* Get the first reading from the time server */
  thisMs = xbee_millisecond_timer();
  sendTimeRequest();
  requestTimeStamp = thisMs;
  delay(1000);

  while (1) {
    thisMs = xbee_millisecond_timer();
    /* Process incoming data */
    xbee_dev_tick(&myXbee);

    /* Get new clock data */
    if ((thisMs - requestTimeStamp) >= TIME_UPDATE_FREQ_MS) {
      sendTimeRequest();
      requestTimeStamp = thisMs;
    }
    /* Checks if we have received a time to display and updates seconds */
    if (serverTime && (thisMs - delayer) >= 1000) {
      delayer = thisMs;
      /*  The - 2208988800ul is to convert from seconds since 1900 to 1970 */
      time_t temp = ++serverTime - 2208988800ul;
      t = *localtime(&temp);
      printTime();
    }
    delay(10);
  }
}
