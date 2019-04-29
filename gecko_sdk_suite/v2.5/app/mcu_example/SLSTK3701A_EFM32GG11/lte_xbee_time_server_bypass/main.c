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
#include "xbee/atcmd.h"
#include "xbee/byteorder.h"
/* Defined in project settings, configures the XBee at runtime */
#ifdef XBEE_DEMO_CONFIG
  #include "xbee_config.h"
#endif

#define XBEE_TARGET_BAUD        115200
#define TIME_SERVER_URL         "time.nist.gov"
#define TIME_PROTOCOL_PORT      37
/*
 * Frequency to request the time from the server, shouldn't be less than 10000,
 * as the server might think it's a DDOS attack and will then deny service
 * @note See documentation for the given modem for AT commands and settings
 */
#define TIME_UPDATE_FREQ_MS     10000

#define TIME_PROTOCOL_PORT_STR  STR(TIME_PROTOCOL_PORT)
#define STR(A)                  _STR(A)
#define _STR(A)                 #A
#define MAX_SOCKETS             7     /* Number of sockets available in modem */
#define MODEM_STATUS_UPDATE     2000  /* How often to check connection status */
#define POLL_DATA_TIMEOUT       10000 /* Timeout for time server to send data */
#define READLINE_TIMEOUT_MS     2     /* Maximum ms in between chars from modem */

void updateDisplay(void);
void configModem(void);
xbee_dev_t myXbee;
uint32_t serverAddrBE = 0;  /* The IPV4 address of the server */

union sT {
  uint8_t bytes[4];
  uint32_t dWord;
} serverTime = { .dWord = 0 }; /* Last server time fetched */

uint32_t serverTimeStampMs; /* Local msTicks time when serverTime was fetched */
struct tm t;
int connStatus = 0;
bool_t isConfiguring = 0;
int statusRet = 0;

/* Table used to dispatch received frames
 * Table is empty because we only need to define it to support XBee
 * configuration. (i.e. this table is not actually used by the Timer Server
 * application.
 */
const xbee_dispatch_table_entry_t xbee_frame_handlers[] =
{
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

/***************************************************************************//**
 * @brief Wrapper function to send AT commands to the modem
 * @retval -EINVAL      If the passed AT command was too long
 * @retval -EBUSY       If there is an error writing to the modem
 ******************************************************************************/
int issueAT(char *cmd, char *param)
{
  char cmdBuff[256] = { 0 };
  int thisSize = strlen(cmd) + strlen(param) + 3;
  if (thisSize > 256) {
    return -EINVAL;
  }
  strcpy(cmdBuff, "AT");
  strcat(cmdBuff, cmd);
  strcat(cmdBuff, param);
  strcat(cmdBuff, "\r");
  if (thisSize != xbee_ser_write(&myXbee.serport, cmdBuff, thisSize)) {
    return -EBUSY;
  }
  return 0;
}

/***************************************************************************//**
 * @brief Reads a single line from the modem, timeouts to check for modem reset
 * @return Amount of data available in the buffer
 ******************************************************************************/
int readLine(char *buff, const size_t buffSize, size_t *length)
{
  size_t pos = 0;
  int ch;
  uint32_t emptyReads = xbee_millisecond_timer() + READLINE_TIMEOUT_MS;
  ch = xbee_ser_getchar(&myXbee.serport);
  while (ch != '\r' && ch != '\n') {
    if (ch >= 0) {
      if (pos < buffSize) {
        buff[pos++] = (char) ch;
      } else {
        pos = 0;
        break;
      }
    }
    ch = xbee_ser_getchar(&myXbee.serport);

    if (ch == -ENODATA) {
      if (xbee_millisecond_timer() > emptyReads) {
        pos = 0;
        break;  // Catches timeout if modem is reset mid stream
      }
    } else {
      emptyReads = xbee_millisecond_timer() + READLINE_TIMEOUT_MS;
    }
  }
  buff[pos] = '\0';
  *length = pos;
  return 0;
}

/***************************************************************************//**
 * @brief Gets the modems response to a previously given AT command
 * @retval >0     Length of read AT command response
 * @retval -ETIMEDOUT   The modem didn't respond within timeout
 * @retval -EINVAL      The buffer passed was NULL
 * @retval -EIO         The response seen is invalid
 ******************************************************************************/
int getATResponse(char *buff, size_t size, char * resp, uint32_t msTimeout)
{
  size_t length = 0;
  size_t respLen = strlen(resp);
  uint32_t startTime = xbee_millisecond_timer();
  if (buff == NULL) {
    return -EINVAL;
  }
  do {
    updateDisplay();
    if (xbee_millisecond_timer() >= startTime + msTimeout) {
      return -ETIMEDOUT;
    }
    if (xbee_ser_rx_used(&myXbee.serport) > 0) {
      readLine(buff, size, &length);
    }
    if (length > 0) {
      if (strstr(buff, "OK") != NULL) {
        length = 0;
      } else if (strstr(buff, "+CIND: ") != NULL) {
        sscanf(buff, "+CIND: %*d, %*d,%d", &connStatus);
        length = 0;
      } else if ((strstr(buff, "CME ERROR") != NULL)) {
        return -EIO;
      } else if (strstr(buff, "AT") == buff) {
        /* Should never see the modem respond with an AT, reset/reconfigure */
        if (!isConfiguring) {
          configModem();
        }
        length = 0; // Drop echoed data
        return -EIO;
      }
    }
  } while (length == 0 || strncmp(resp, buff, respLen));

  return length;
}

/***************************************************************************//**
 * @brief Waits specifically for an OK response from the modem
 * @retval 0            An OK was read from the modem
 * @retval -ETIMEDOUT   The modem didn't respond within timeout
 * @retval -EINVAL      The buffer passed was NULL
 * @retval -EIO         The response seen is invalid
 ******************************************************************************/
int getOk(uint32_t msTimeout)
{
  char statusBuffer[64];
  size_t length = 0;
  int retval = -ETIMEDOUT;
  uint32_t startTime = xbee_millisecond_timer();

  while (xbee_millisecond_timer() < startTime + msTimeout) {
    updateDisplay();

    if (xbee_ser_rx_used(&myXbee.serport) > 0) {
      readLine(statusBuffer, 64, &length);
    }

    if (length > 0) { /* Ignore blank lines */
      if (strstr(statusBuffer, "+CIND: ") != NULL) {
        sscanf(statusBuffer, "+CIND: %*d, %*d,%d", &connStatus);
      } else if (strstr(statusBuffer, "AT") == statusBuffer) {
        /* We shouldn't see an AT prefix with ATE0 */
        if (!isConfiguring) {
          configModem();
        }
        retval = -EIO;
        break;
      } else if (strstr(statusBuffer, "OK") != NULL) {
        retval = 0;
        break;
      } else if (length == 5 && (strncmp(statusBuffer, "ERROR", length) == 0)) {
        retval = -EIO;
        break;
      } else if ((strstr(statusBuffer, "CME ERROR") != NULL)) {
        retval = -EIO;
        break;
      } else {
        retval = -EIO;
        break;
      }
    }
  }

  return retval;
}

/***************************************************************************//**
 * @brief Checks if the modem is registered to the network
 * @retval 0            Successfully got updated connection status of the modem
 * @retval <0           Errors due to failed communication with modem
 ******************************************************************************/
int checkConnection(void)
{
  int ret;
  ret = issueAT("+CIND", "?");
  if (ret != 0) {
    return ret;
  }
  ret = getOk(1000);
  if (ret != 0) {
    return ret;
  }
  return 0;
}

/***************************************************************************//**
 * @brief Opens a TCP socket with the time server (no payload)
 * @retval 0            Successfully got time from the time server
 * @retval <0           Errors due to failed communication with modem or server
 ******************************************************************************/
int sendTimeRequest(void)
{
  char cmdBuff[128] = { 0 };
  char rcvBuff[64];
  unsigned int socketId;
  int ret;
  int dataAvail = 0;
  int i;
  int isOpen;
  char *dat;
  uint32_t pollStart;

  /* Create socket */
  ret = issueAT("+USOCR", "=6");
  if (ret != 0) {
    goto ERR_EXIT;
  }
  ret = getATResponse(rcvBuff, 64, "+USOCR", 2000);
  if (ret == -2) { // Out of sockets
    /* Close all sockets */
    for (i = 0; i < MAX_SOCKETS; i++) {
      sprintf(cmdBuff, "=%c", ('0' + i));
      issueAT("+USOCL", cmdBuff);
      getOk(200);
    }
    goto ERR_EXIT;
  }
  if (ret < 0) {
    goto ERR_EXIT;
  }
  /* Successful in creating socket */
  if (ret == 9) {
    ret = sscanf(rcvBuff, "+USOCR: %u", &socketId);
    if (ret <= 0) {
      ret = -EIO;
      goto ERR_EXIT;
    }
  } else {
    ret = -EIO;
    goto ERR_EXIT;
  }
  ret = getOk(200);
  if (ret != 0) {
    goto ERR_CLOSE;
  }

  /* Connect socket */
  sprintf(cmdBuff, "=%u,\"%s\",%u", socketId, TIME_SERVER_URL, TIME_PROTOCOL_PORT);
  ret = issueAT("+USOCO", cmdBuff);
  if (ret != 0) {
    goto ERR_CLOSE;
  }
  ret = getOk(20000); /* Connect timeout is 20 seconds */
  if (ret != 0) {
    goto ERR_CLOSE;
  }

  /* Polling wait for incoming data */
  pollStart = xbee_millisecond_timer();
  while (dataAvail < 4) {
    if (xbee_millisecond_timer() >= POLL_DATA_TIMEOUT + pollStart) {
      ret = -ETIMEDOUT;
      goto ERR_CLOSE;
    }
    sprintf(cmdBuff, "=%u,0", socketId);
    ret = issueAT("+USORD", cmdBuff); // Check the # of bytes available
    if (ret != 0) {
      goto ERR_CLOSE;
    }
    ret = getATResponse(rcvBuff, 64, "+USORD", 1000);
    if (ret < 0) {
      goto ERR_CLOSE;
    }
    ret = sscanf(rcvBuff, "+USORD: %*d,%d", &dataAvail);
    if (ret <= 0) {
      ret = -EIO;
      goto ERR_CLOSE;
    }
    ret = getOk(500);
    if (ret != 0) {
      goto ERR_CLOSE;
    }
    delay(30); // We don't need to constantly poll
  }

  sprintf(cmdBuff, "=%u,4", socketId);
  ret = issueAT("+USORD", cmdBuff); // Read from socket
  if (ret != 0) {
    goto ERR_CLOSE;
  }
  ret = getATResponse(rcvBuff, 64, "+USORD", 1000);
  if (ret < 0) {
    goto ERR_CLOSE;
  }
  ret = getOk(400);
  if (ret != 0) {
    goto ERR_CLOSE;
  }

  dat = strchr(rcvBuff, '\"');
  if (dat == NULL) {
    ret = -EIO;
    goto ERR_CLOSE;
  }
  dat++;
  /* Convert the incoming data from hex to bytes */
  for (i = 0; i < 4; i++) {
    ret = hexstrtobyte(dat + (i * 2)); /* Converts two Hex digits to a bytes */
    if (ret < 0) {
      /* The data was not hex, (modem was probably reset, reconfig.) */
      configModem();
      goto ERR_CLOSE;
    }
    serverTime.bytes[i] = ret;
  }
  /* Convert from network byte order to host platform's byte order */
  serverTime.dWord = be32toh(serverTime.dWord);

  /* Make sure socket is closed */
  sprintf(cmdBuff, "=%u,10", socketId);
  ret = issueAT("+USOCTL", cmdBuff); // Check the status of socket
  if (ret != 0) {
    goto ERR_CLOSE;
  }
  ret = getATResponse(rcvBuff, 64, "+USOCTL", 2000);
  if (ret < 0) {
    goto ERR_CLOSE;
  }
  ret = sscanf(rcvBuff, "+USOCTL: %*d,%*d,%d", &isOpen);
  if (ret <= 0) {
    goto ERR_CLOSE;
  }
  ret = getOk(400);
  if (ret != 0) {
    goto ERR_CLOSE;
  }
  /* If the socket is not closed, close it */
  if (isOpen != 0) {
    sprintf(cmdBuff, "=%u", socketId);
    issueAT("+USOCL", cmdBuff);
  }

  return 0;

  /* We saw an error, close the socket, flush buffers and exit function */
  ERR_CLOSE:
  sprintf(cmdBuff, "=%u", socketId);
  issueAT("+USOCL", cmdBuff);
  ERR_EXIT:
  xbee_ser_rx_flush(&myXbee.serport);
  return ret;
}

/***************************************************************************//**
* @brief Configures the modem to not echo data and to respond with data in hex
*******************************************************************************/
void configModem(void)
{
  char tempBuff[32];
  int ret;
  int i;

  isConfiguring = 1;
  printf("\fDevice reset detected\nReconfiguring modem");
  issueAT("E", "1");

  delay(1000);

  xbee_ser_rx_flush(&myXbee.serport);

  /* Turns off local echo */
  issueAT("E", "0");

  delay(1000);

  xbee_ser_rx_flush(&myXbee.serport);
  delay(1000);
  /* Put modem in HEX mode (all received data is in HEX) */
  issueAT("+UDCONF", "=1,1");
  ret = getOk(1000);
  if (ret != 0) {
    goto ERR_FAIL;
  }
  /* Attempt to close all sockets */
  for (i = 0; i < MAX_SOCKETS; i++) {
    sprintf(tempBuff, "=%c", ('0' + i));
    issueAT("+USOCL", tempBuff);
    getOk(200);
  }
  printf("\f");
  isConfiguring = 0;
  return;

  /* Failure to configure modem could be due to wrong modem being attached */
  ERR_FAIL:
  isConfiguring = 0;
  printf("\fCan't config device\nCheck baudrate and\nCheck API\n"
         "Also check that this\nis the correct modem\nfor this demo");
  while (1) {
  }
}

/***************************************************************************//**
* @brief Updates the display every second
*******************************************************************************/
void updateDisplay(void)
{
  static uint32_t delayer = 0;

  if (delayer == 0) {
    delayer = xbee_millisecond_timer();
  }

  /* Checks if we have received a time to display and updates seconds */
  if (serverTime.dWord && (xbee_millisecond_timer() - delayer) >= 1000) {
    delayer = xbee_millisecond_timer();
    /*  The - 2208988800ul is to convert from seconds since 1900 to 1970 */
    time_t temp = ++(serverTime.dWord) - 2208988800ul;
    /* Convert raw seconds into a usable date */
    t = *localtime(&temp);
    /* Display formatted time and connection status */
    printTime();
    printf("\033[H" "\n\nConnected: %.1d", connStatus);
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

#if defined(XBEE_DEMO_CONFIG) && !defined(XBEE_DEMO_HOLOGRAM_SIM)
  if (configureXBee(&myXbee,
                    XBEE_TARGET_BAUD,
                    XBEE_CONFIG_MODE_BYPASS,
                    XBEE_CONFIG_CARRIER_AUTO_DETECT,
                    TRUE)) {
    printf("\fFailed to init\n"
           "check that the XBee\n"
           "is powered and is\n"
           "connected properly");
    return -EIO;
  }
#endif
#if defined(XBEE_DEMO_CONFIG) && defined(XBEE_CHANGE_APN)
  /* Attempt to Apply the correct APN as defined by XBEE_TARGET_APN */
  printf("\fAttempting to apply\nthe new APN\n\n");
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
  uint32_t requestTimeStamp = 0;
  uint32_t statusTimeStamp = 0;
  uint32_t thisMs;
  int ret;
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator with kit specific parameters */
  EMU_DCDCInit(&dcdcInit);

  /* Initialize the display module. */
  DISPLAY_Init();

  /* Retarget stdio to a text display. */
  if (RETARGET_TextDisplayInit() != TEXTDISPLAY_EMSTATUS_OK) {
    while (1) ;
  }

  printf("\fInitializing modem");

  /* Calls all init. code related to the XBee */
  if (initXBee() != 0) {
    while (1) ; /* Blocks if there was an error with opening communications */
  }

  /* Give the modem time to start */
  delay(8000);
  configModem();

  /* Wait for cell connection */
  printf("\fWaiting for signal");
  while (connStatus != 1) {
    checkConnection();
    printf(".");
    delay(1000);
  }

  printf("\fAcquired signal");
  delay(1000);
  printf("\fFetching the time\nfrom server\n");

  while (1) {
    thisMs = xbee_millisecond_timer();
    /* Update connection status */
    if ((thisMs - statusTimeStamp) >= MODEM_STATUS_UPDATE) {
      statusRet = checkConnection();
      statusTimeStamp = thisMs;
    }

    /* Get new clock data */
    if ((thisMs - requestTimeStamp) >= TIME_UPDATE_FREQ_MS) {
      if ((ret = sendTimeRequest()) == 0) {
        requestTimeStamp = xbee_millisecond_timer();
        printf("\033[H" "Last request took:  \n"
                        "%6" PRIu32 " ms   ", xbee_millisecond_timer() - thisMs);
      } else {
        printf("\033[H" "Failed last request: \nStatus: %2d  ", ret);
      }
    }
    updateDisplay();
    delay(200);
  }
}
