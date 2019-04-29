/***************************************************************************//**
 * @file
 * @brief Wireless Whiteboard's WiFi Application Module
 * WGM110 and GLIB demo for the SLSTK3401A running on MicOS
 * This module besides initializing the UART and BGLIB, it also
 * implements the WiFi state machine, responsible for sending
 * commands and processing the corresponding responses and events.
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

#define APP_WIFI_MODULE

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "wifi_bglib.h"

#include <kernel/include/os.h>
#include <common/include/rtos_utils.h>

#include <app_cfg.h>

#include <app_wifi_cfg.h>

#include "app_wifi.h"
#include "app_uart.h"
#include "app_lcd.h"

// -----------------------------------------------------------------------------
// Data Structures

// Define BGLIB as indicated in wifi_bglib.h
BGLIB_DEFINE()

// -----------------------------------------------------------------------------
// IoT Application Settings

// Server hosting the whiteboard's web page and web services
#define  APP_WIFI_TCP_SERVER_HOSTNAME    "demo.silabs.com"

// Server's IP Address in case the DNS fails to resolve the hostname
#define  APP_WIFI_TCP_SERVER_IP_ADDR     "54.202.154.71"

// Web service port number
#define  APP_WIFI_TCP_SERVER_PORT        80

// TCP request response buffer size
#define  APP_WIFI_TCP_REQ_RX_BUF_SIZE   3000

// Maximum time to wait for a response from the WiFi module before resetting it
#define  APP_WIFI_CMD_TIMEOUT_MS        15000

// -----------------------------------------------------------------------------
// Data Types

// Types of TCP requests
typedef enum
{
  APP_TCP_REQ_TYPE_PEEK,     ///< TCP Request to get the latest image timestamp.
  APP_TCP_REQ_TYPE_GET       ///< TCP Request to get the latest image.
}  AppWiFiTcpReqType_t;

// States of the internal state machine.
typedef enum
{
  APP_WIFI_STATE_HELLO,                ///< Send a Hello command.
  APP_WIFI_STATE_RESET,                ///< Reset the WiFi module.
  APP_WIFI_STATE_GET_MAC_ADDR,         ///< Get the WiFi module's MAC address.
  APP_WIFI_STATE_SME_SET_TO_CLIENT,    ///< Set SME to Client Mode.
  APP_WIFI_STATE_WIFI_ON,              ///< Switch WiFi on.
  APP_WIFI_STATE_TCPIP_CONFIGURE,      ///< Configure TCPIP.
  APP_WIFI_STATE_SME_SET_PASSWORD,     ///< Set the AP's password.
  APP_WIFI_STATE_SME_SET_SCAN_CHS,     ///< Set the AP's channels list to scan.
  APP_WIFI_STATE_SME_CONNECT_SSID,     ///< Connect to WiFi Access Point.
  APP_WIFI_STATE_DNS_RES_HOSTNAME,     ///< Resolve the IP addr of a host name.
  APP_WIFI_STATE_TCP_CONNECT,          ///< Open a TCP connection.
  APP_WIFI_STATE_TCP_SEND,             ///< Send an HTTP GET request.
  APP_WIFI_STATE_TCP_CLOSE,            ///< Close a TCP endpoint.
  APP_WIFI_STATE_DISCONNECT            ///< Disconnect from the WiFi AP.
} AppWiFiState_t;

// States of the packet parsing state machine
typedef enum
{
  APP_WIFI_UART_RX_STATE_HDR_HILEN,      ///< Message type: Response/Event.
  APP_WIFI_UART_RX_STATE_HDR_LOLEN,      ///< Minimum payload length.
  APP_WIFI_UART_RX_STATE_HDR_CLASS,      ///< Message class: Configuration.
  APP_WIFI_UART_RX_STATE_HDR_METHOD,     ///< Message ID.
  APP_WIFI_UART_RX_STATE_PAYLOAD_LEN,    ///< Number of bytes in data.
  APP_WIFI_UART_RX_STATE_PAYLOAD_DATA    ///< Up to 2047 bytes of data.
} AppWiFiUartRxState_t;

// -----------------------------------------------------------------------------
// Global Variables

/// WiFi task control block
static OS_TCB appWiFiTaskTCB;

/// WiFi task stack
static CPU_STK appWiFiTaskStk[APP_CFG_TASK_WIFI_STK_SIZE];

/// List of WiFi channels to scan.
static const uint8_t appWiFiScanChannels[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

/// Current state of the WiFi module.
static AppWiFiState_t appWiFiState;

/// Current state of the packet parsing state machine.
static AppWiFiUartRxState_t appWiFiUartRxState;

/// Current index in the receive buffer for the latest received byte.
static uint16_t appWiFiUartRxBufIx;

/// Receive buffer to store the responses and events from the WiFi module.
static uint8_t appWiFiUartRxBuf[BGLIB_MSG_MAXLEN] = {0};

/// Receive buffer pointer.
static uint32_t *appWiFiUartRxBufPtr = (uint32_t *)&appWiFiUartRxBuf[0];

/// Payload length
static uint16_t appWiFiUartRxPayloadLen;

/// TCP endpoint number.
static uint8_t appWiFiTcpEndpoint;

/// TCP response buffer for received data.
static uint8_t appWiFiTcpReqRxBuf[APP_WIFI_TCP_REQ_RX_BUF_SIZE];

/// TCP response buffer index.
static uint16_t appWiFiTcpReqRxBufIx;

/// TCP request buffer to store the payload to be sent to the server.
static char appWiFiTcpReqHeader[128];

/// TCP request type: peek or get.
static AppWiFiTcpReqType_t appWiFiTcpReqType;

/// TCP requests statistics counters.
static uint32_t appWiFiTcpReqPeekOkCtr;
static uint32_t appWiFiTcpReqGetOkCtr;

/// Previous whiteboard's image timestamp.
static char appWiFiLastImgTS[36];

/// Current  whiteboard's image timestamp.
static char appWiFiCurImgTimeStamp[36];

/// There are not any images associated with this WiFi module.
static bool appWiFiFirstTime;

// -----------------------------------------------------------------------------
// Function Prototypes

static void AppWiFiTask(void *pArg);
static void AppWiFiUartRxCallback(void);
static void AppWiFiGoToState(AppWiFiState_t state);
static void AppWiFiProcessRxByte(uint8 rxData);
static void AppWiFiProcessPacket(void);
static bool AppWiFiParseTcpReqGet(void);
static bool AppWiFiParseTcpReqPeek(void);
static uint32_t AppWiFiConvertIpAddr2Dec(char* value);

/**************************************************************************//**
 * @brief Reset global variables
 * Function to reset the application variables.
 * @param reset_stats Reset or maintain the current statistics.
 * @returns Void.
 *****************************************************************************/
void AppWiFiResetVars(bool resetStats)
{
  CPU_SR_ALLOC();

  CPU_CRITICAL_ENTER();

  snprintf(&APP_WIFI_ApSsid[0],
           sizeof(APP_WIFI_ApSsid),
           APP_WIFI_AP_SSID);
  memset(&APP_WIFI_MacAddr[0], ' ',
         sizeof(APP_WIFI_MacAddr) - 1);
  APP_WIFI_MacAddr[sizeof(APP_WIFI_MacAddr) - 1] = '\0';
  memset(&APP_WIFI_DeviceIpAddr[0], ' ',
         sizeof(APP_WIFI_DeviceIpAddr) - 1);
  APP_WIFI_DeviceIpAddr[sizeof(APP_WIFI_DeviceIpAddr) - 1] = '\0';
  memset(&APP_WIFI_TcpServerIpAddr[0], ' ',
         sizeof(APP_WIFI_TcpServerIpAddr) - 1);
  APP_WIFI_TcpServerIpAddr[sizeof(APP_WIFI_TcpServerIpAddr) - 1] = '\0';

  CPU_CRITICAL_EXIT();

  memset(&appWiFiLastImgTS[0], ' ',
         sizeof(appWiFiLastImgTS) - 1);
  appWiFiLastImgTS[sizeof(appWiFiLastImgTS) - 1] = '\0';
  memset(&appWiFiCurImgTimeStamp[0], ' ',
         sizeof(appWiFiCurImgTimeStamp) - 1);
  appWiFiCurImgTimeStamp[sizeof(appWiFiCurImgTimeStamp) - 1] = '\0';

  appWiFiTcpReqRxBufIx = 0;

  if (resetStats) {
    appWiFiTcpReqPeekOkCtr = 0;
    appWiFiTcpReqGetOkCtr  = 0;
  }

  appWiFiTcpReqType  = APP_TCP_REQ_TYPE_PEEK;

  appWiFiUartRxState = APP_WIFI_UART_RX_STATE_HDR_HILEN;
}

/**************************************************************************//**
 * @brief Initialize module.
 * Function to initialize the module.
 * @returns Void.
 *****************************************************************************/
void APP_WIFI_Init(void)
{
  RTOS_ERR osErr;

  // Initialize the LCD via SPI.
  APP_LCD_Init();

  // Register the UART callback function.
  APP_UART_RegisterCallback(AppWiFiUartRxCallback);

  // Initialize the UART to interface with the WiFI module.
  APP_UART_Init();

  // Initialize the WiFi BGLib by passing the callback function pointer.
  BGLIB_INITIALIZE(APP_UART_Write);

  // Create the WiFi task
  OSTaskCreate(&appWiFiTaskTCB,
              "WiFi Task",
               AppWiFiTask,
               0u,
               APP_CFG_TASK_WIFI_PRIO,
              &appWiFiTaskStk[0u],
               appWiFiTaskStk[APP_CFG_TASK_WIFI_STK_SIZE / 10u],
               APP_CFG_TASK_WIFI_STK_SIZE,
               0u,
               0u,
               0u,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
              &osErr);

  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(osErr) == RTOS_ERR_NONE), 1);

  // Reset the WiFi module to start from a known state
  AppWiFiGoToState(APP_WIFI_STATE_RESET);
}

/**************************************************************************//**
 * @brief WiFi task handler
 * This task waits for a signal from the USART interrupt handler
 * to parse each byte and process the packet.
 * @param p_arg Kernel task optional argument pointer.
 * @returns Void.
 *****************************************************************************/
static void AppWiFiTask(void *pArg)
{
  RTOS_ERR osErr;
  CPU_TS ts;
  uint8  rxData;

  // Prevent 'variable unused' compiler warning.
  (void)&pArg;

  while (1) {
    // Wait for a signal from the USART interrupt handler
    OSTaskSemPend( APP_WIFI_CMD_TIMEOUT_MS,
                   OS_OPT_PEND_BLOCKING,
                  &ts,
                  &osErr);
    if (RTOS_ERR_CODE_GET(osErr) == RTOS_ERR_NONE) {
      // Read the entire buffer
      if (APP_UART_RxCount > 0) {
        // Read the data one byte at a time
        rxData = APP_UART_Read();
        // Make sure each byte conforms to the WiFi module's packet
        // format and parse the packet once it is fully received.
        AppWiFiProcessRxByte(rxData);
      }
    } else if (RTOS_ERR_CODE_GET(osErr) == RTOS_ERR_TIMEOUT) {
      // None of the events or responses from the WiFi module should take
      // more than APP_WIFI_CMD_TIMEOUT_MS. If so, then reset the WiFi module.
      AppWiFiGoToState(APP_WIFI_STATE_RESET);
    }
  }
}

/**************************************************************************//**
 * @brief WiFi state Go-To
 * This function is used to switch between the different WiFi states.
 * @param state WiFi state to set the module to.
 * @returns Void.
 *****************************************************************************/
static void AppWiFiGoToState(AppWiFiState_t state)
{
  switch (state) {
    case APP_WIFI_STATE_HELLO:
      wifi_cmd_system_hello();
      APP_PRINTF(("wifi_cmd_system_hello()\n"));
      break;

    case APP_WIFI_STATE_RESET:
      APP_UART_Stop();
      APP_UART_ClearBuffer();
      AppWiFiResetVars(false);
      APP_UART_Start();
      wifi_cmd_system_reset(0);
      APP_PRINTF(("wifi_cmd_system_reset(0)\n"));
      break;

    case APP_WIFI_STATE_GET_MAC_ADDR:
      wifi_cmd_config_get_mac(0);
      APP_PRINTF(("wifi_cmd_config_get_mac(0)\n"));
      break;

    case APP_WIFI_STATE_SME_SET_TO_CLIENT:
      wifi_cmd_sme_set_operating_mode(1);
      APP_PRINTF(("wifi_cmd_sme_set_operating_mode(1)\n"));
      break;

    case APP_WIFI_STATE_WIFI_ON:
      wifi_cmd_sme_wifi_on();
      APP_PRINTF(("wifi_cmd_sme_wifi_on()\n"));
      break;

    case APP_WIFI_STATE_TCPIP_CONFIGURE:
      wifi_cmd_tcpip_configure(0, 0, 0, 1);
      APP_PRINTF(("wifi_cmd_tcpip_configure()\n"));
      break;

    case APP_WIFI_STATE_SME_SET_PASSWORD:
      wifi_cmd_sme_set_password(sizeof(APP_WIFI_AP_PWD)-1, APP_WIFI_AP_PWD);
      APP_PRINTF(("wifi_cmd_sme_set_password(%d, \"%s\")\n",
                  sizeof(APP_WIFI_AP_PWD)-1,
                  APP_WIFI_AP_PWD));
      break;

    case APP_WIFI_STATE_SME_SET_SCAN_CHS:
      wifi_cmd_sme_set_scan_channels(0,
                                     sizeof(appWiFiScanChannels),
                                     &appWiFiScanChannels[0]);
      APP_PRINTF(("wifi_cmd_sme_set_scan_channels(0, %d, {%d,...,%d})\n",
                  sizeof(appWiFiScanChannels),
                  appWiFiScanChannels[0],
                  appWiFiScanChannels[sizeof(appWiFiScanChannels)-1]));
      break;

    case APP_WIFI_STATE_SME_CONNECT_SSID:
      wifi_cmd_sme_connect_ssid(sizeof(APP_WIFI_AP_SSID) - 1,
                                APP_WIFI_AP_SSID);
      APP_PRINTF(("wifi_cmd_sme_connect_ssid(%d, \"%s\")\n",
                  sizeof(APP_WIFI_AP_SSID) - 1,
                  APP_WIFI_AP_SSID));
      break;

    case APP_WIFI_STATE_DNS_RES_HOSTNAME:
      wifi_cmd_tcpip_dns_gethostbyname(sizeof(APP_WIFI_TCP_SERVER_HOSTNAME)-1,
                                       APP_WIFI_TCP_SERVER_HOSTNAME);
      APP_PRINTF(("wifi_cmd_tcpip_dns_gethostbyname(%d, \"%s\")\n",
                  sizeof(APP_WIFI_TCP_SERVER_HOSTNAME) - 1,
                  APP_WIFI_TCP_SERVER_HOSTNAME));
      break;

    case APP_WIFI_STATE_TCP_CONNECT:
      wifi_cmd_tcpip_tcp_connect(AppWiFiConvertIpAddr2Dec(APP_WIFI_TcpServerIpAddr),
                                 APP_WIFI_TCP_SERVER_PORT, -1);
      APP_PRINTF(("wifi_cmd_tcpip_tcp_connect(%ld, %d, -1)\n",
                  AppWiFiConvertIpAddr2Dec(APP_WIFI_TcpServerIpAddr),
                  APP_WIFI_TCP_SERVER_PORT));
      break;

    case APP_WIFI_STATE_TCP_SEND:
      if (appWiFiTcpReqType == APP_TCP_REQ_TYPE_PEEK) {
        snprintf(&appWiFiTcpReqHeader[0], sizeof(appWiFiTcpReqHeader),
                 "GET /whiteboard/imgget.html?action=peek&id=%s&bpp=1 "
                 "HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
                 &APP_WIFI_MacAddr[0], &APP_WIFI_TcpServerIpAddr[0]);
      } else {
        snprintf(&appWiFiTcpReqHeader[0], sizeof(appWiFiTcpReqHeader),
                 "GET /whiteboard/imgget.html?action=get&id=%s&bpp=1 "
                 "HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
                 &APP_WIFI_MacAddr[0], &APP_WIFI_TcpServerIpAddr[0]);
      }
      wifi_cmd_endpoint_send(appWiFiTcpEndpoint,
                             strlen(appWiFiTcpReqHeader),
                             appWiFiTcpReqHeader);
      APP_PRINTF(("wifi_cmd_endpoint_send(%d, %d, \"%s\")\n",
                  appWiFiTcpEndpoint,
                  strlen(appWiFiTcpReqHeader),
                  appWiFiTcpReqHeader));
      break;

    case APP_WIFI_STATE_TCP_CLOSE:
      wifi_cmd_endpoint_close(appWiFiTcpEndpoint);
      APP_PRINTF(("wifi_cmd_endpoint_close(%d)\n", appWiFiTcpEndpoint));
      break;

    case APP_WIFI_STATE_DISCONNECT:
      wifi_cmd_sme_disconnect();
      APP_PRINTF(("wifi_cmd_sme_disconnect()\n"));
      break;

    default:
      break;
  }
  appWiFiState = state;
}

/**************************************************************************//**
 * @brief Process received byte.
 * This function is used to process each byte received
 * from the WiFi module via the UART.
 * @param rx_data The received byte.
 * @returns Void.
 *****************************************************************************/
void AppWiFiProcessRxByte(uint8 rxData)
{
  switch (appWiFiUartRxState) {
    case APP_WIFI_UART_RX_STATE_HDR_HILEN:
      if (rxData == 0x88 || rxData == 0x08) {
        appWiFiUartRxBufIx = 0;
        appWiFiUartRxBuf[appWiFiUartRxBufIx] = rxData;
        appWiFiUartRxBufIx++;
        appWiFiUartRxState = APP_WIFI_UART_RX_STATE_HDR_LOLEN;
      } else {
        appWiFiUartRxBufIx = 0;
      }
      break;

    case APP_WIFI_UART_RX_STATE_HDR_LOLEN:
      appWiFiUartRxBuf[appWiFiUartRxBufIx] = rxData;
      appWiFiUartRxBufIx++;
      appWiFiUartRxState = APP_WIFI_UART_RX_STATE_HDR_CLASS;
      break;

    case APP_WIFI_UART_RX_STATE_HDR_CLASS:
      appWiFiUartRxBuf[appWiFiUartRxBufIx] = rxData;
      appWiFiUartRxBufIx++;
      appWiFiUartRxState = APP_WIFI_UART_RX_STATE_HDR_METHOD;
      break;

    case APP_WIFI_UART_RX_STATE_HDR_METHOD:
      appWiFiUartRxBuf[appWiFiUartRxBufIx] = rxData;
      appWiFiUartRxBufIx++;
      appWiFiUartRxState = APP_WIFI_UART_RX_STATE_PAYLOAD_LEN;
      break;

    case APP_WIFI_UART_RX_STATE_PAYLOAD_LEN:
      appWiFiUartRxPayloadLen = BGLIB_MSG_LEN(appWiFiUartRxBufPtr);
      if (appWiFiUartRxPayloadLen > 0) {
        appWiFiUartRxBuf[appWiFiUartRxBufIx] = rxData;
        appWiFiUartRxBufIx++;
        if (appWiFiUartRxBufIx == (appWiFiUartRxPayloadLen
                                   + BGLIB_MSG_HEADER_LEN)) {
          appWiFiUartRxState = APP_WIFI_UART_RX_STATE_HDR_HILEN;
          AppWiFiProcessPacket();
        } else {
          appWiFiUartRxState = APP_WIFI_UART_RX_STATE_PAYLOAD_DATA;
        }
      } else {
        appWiFiUartRxBufIx = 0;
        appWiFiUartRxState = APP_WIFI_UART_RX_STATE_HDR_HILEN;
      }
      break;

    case APP_WIFI_UART_RX_STATE_PAYLOAD_DATA:
      appWiFiUartRxBuf[appWiFiUartRxBufIx] = rxData;
      appWiFiUartRxBufIx++;
      if (appWiFiUartRxBufIx == (appWiFiUartRxPayloadLen
                                 + BGLIB_MSG_HEADER_LEN)) {
        appWiFiUartRxState = APP_WIFI_UART_RX_STATE_HDR_HILEN;
        AppWiFiProcessPacket();
      } else if (appWiFiUartRxBufIx > BGLIB_MSG_MAXLEN) {
        appWiFiUartRxBufIx = 0;
        appWiFiUartRxState = APP_WIFI_UART_RX_STATE_HDR_HILEN;
      }
      break;

    default:
      break;
  }
}

/**************************************************************************//**
 * @brief Process packet
 * This function is used to process the WiFi module's
 * packets including responses and events.
 * @returns Void.
 *****************************************************************************/
static void AppWiFiProcessPacket(void)
{
  uint8 i;
  uint8 *address;
  hw_addr mac;
  bool dataValid;
  struct wifi_cmd_packet* pck;
  CPU_SR_ALLOC();

  pck = BGLIB_MSG(appWiFiUartRxBuf);

  // Parse the Message ID and process the event/response accordingly.
  switch (BGLIB_MSG_ID(appWiFiUartRxBufPtr)) {
    // Cases are shown in the sequence as the command responses and events
    // should be received from the WiFi module
    case wifi_rsp_system_hello_id:
      APP_PRINTF(("on_wifi_rsp_system_hello()\n"));
      if(appWiFiState == APP_WIFI_STATE_HELLO) {
        AppWiFiGoToState(APP_WIFI_STATE_RESET);
      }
      break;

    case wifi_evt_system_boot_id:
      APP_PRINTF(("on_wifi_evt_system_boot()\n"));
      APP_LCD_ShowScreen(APP_LCD_SCREEN_CONN_AP);
      APP_UART_ClearBuffer();
      AppWiFiGoToState(APP_WIFI_STATE_GET_MAC_ADDR);
      break;

    case wifi_evt_system_power_saving_state_id:
      break;

    case wifi_rsp_config_get_mac_id:
      APP_PRINTF(("on_wifi_rsp_config_get_mac(), result: %d\n",
                  pck->rsp_config_get_mac.result));
      if (pck->rsp_config_get_mac.result != wifi_err_success) {
        AppWiFiGoToState(APP_WIFI_STATE_GET_MAC_ADDR);
      }
      break;

    case wifi_evt_config_mac_address_id:
      mac = pck->evt_config_mac_address.mac;
      address = mac.addr;
      for (i = 0; i < sizeof(mac); i++) {
        CPU_CRITICAL_ENTER();
        snprintf(&APP_WIFI_MacAddr[0],
                 sizeof(APP_WIFI_MacAddr),
                 "%02X%02X%02X%02X%02X%02X",
                 address[0], address[1], address[2],
                 address[3], address[4], address[5]);
        CPU_CRITICAL_EXIT();
      }
      APP_PRINTF(("on_wifi_evt_config_mac_address(), MAC: %s\n", APP_WIFI_MacAddr));
      AppWiFiGoToState(APP_WIFI_STATE_SME_SET_TO_CLIENT);
      break;

    case wifi_rsp_sme_set_operating_mode_id:
      APP_PRINTF(("on_wifi_rsp_sme_set_operating_mode(), result: %d\n",
                  pck->rsp_sme_set_operating_mode.result));
      if (pck->rsp_sme_set_operating_mode.result != wifi_err_success) {
        AppWiFiGoToState(APP_WIFI_STATE_SME_SET_TO_CLIENT);
      } else {
        AppWiFiGoToState(APP_WIFI_STATE_WIFI_ON);
      }
      break;

    case wifi_rsp_sme_set_11n_mode_id:
      if (pck->rsp_sme_set_11n_mode.result != wifi_err_success) {
        wifi_cmd_sme_set_11n_mode(0);
      } else {
        AppWiFiGoToState(APP_WIFI_STATE_WIFI_ON);
      }
      break;

    case wifi_rsp_sme_wifi_on_id:
      APP_PRINTF(("on_wifi_rsp_sme_wifi_on(), result: %d\n",
                  pck->rsp_sme_wifi_on.result));
      if (pck->rsp_sme_wifi_on.result != wifi_err_success) {
        AppWiFiGoToState(APP_WIFI_STATE_WIFI_ON);
      }
      break;

    case wifi_evt_sme_wifi_is_on_id:
      APP_PRINTF(("on_wifi_evt_sme_wifi_is_on(), result: %d\n",
                  pck->evt_sme_wifi_is_on.result));
      if (pck->evt_sme_wifi_is_on.result != wifi_err_success) {
        AppWiFiGoToState(APP_WIFI_STATE_WIFI_ON);
      } else {
        AppWiFiGoToState(APP_WIFI_STATE_TCPIP_CONFIGURE);
      }
      break;

    case wifi_rsp_tcpip_configure_id:
      APP_PRINTF(("on_wifi_rsp_tcpip_configure(), result: %d\n",
                  pck->rsp_tcpip_configure.result));
      if (pck->rsp_tcpip_configure.result != wifi_err_success) {
        AppWiFiGoToState(APP_WIFI_STATE_TCPIP_CONFIGURE);
      } else {
        AppWiFiGoToState(APP_WIFI_STATE_SME_SET_PASSWORD);
      }
      break;

    case wifi_rsp_sme_set_password_id:
      APP_PRINTF(("on_wifi_rsp_sme_set_password(), status: %d\n",
                  pck->rsp_sme_set_password.status));
      if (pck->rsp_sme_set_password.status != wifi_err_success) {
        AppWiFiGoToState(APP_WIFI_STATE_SME_SET_PASSWORD);
      } else {
        AppWiFiGoToState(APP_WIFI_STATE_SME_SET_SCAN_CHS);
      }
      break;

    case wifi_rsp_sme_set_scan_channels_id:
      APP_PRINTF(("on_wifi_rsp_sme_set_scan_channels(), result: %d\n",
                  pck->rsp_sme_set_scan_channels.result));
      if (pck->rsp_sme_set_scan_channels.result != wifi_err_success) {
        AppWiFiGoToState(APP_WIFI_STATE_SME_SET_SCAN_CHS);
      } else {
        AppWiFiGoToState(APP_WIFI_STATE_SME_CONNECT_SSID);
      }
      break;

    case wifi_rsp_sme_connect_ssid_id:
      APP_PRINTF(("on_wifi_rsp_sme_connect_ssid(), result: %d\n",
                  pck->rsp_sme_connect_ssid.result));
      if (pck->rsp_sme_connect_ssid.result != wifi_err_success) {
        AppWiFiGoToState(APP_WIFI_STATE_SME_CONNECT_SSID);
      }
      break;

    case wifi_evt_sme_connect_retry_id:
      APP_PRINTF(("on_wifi_evt_sme_connect_retry()\n"));
      break;

    case wifi_evt_sme_connected_id:
      APP_PRINTF(("on_wifi_evt_sme_connected(), status: %d\n",
                  pck->evt_sme_connected.status));
      if (pck->evt_sme_connected.status) {
        AppWiFiGoToState(APP_WIFI_STATE_RESET);
      }
      // WiFi connection has been established, waiting for an Interface Status
      // event to indicate that an IP address has been configured.
      break;

    case wifi_evt_sme_connect_failed_id:
      APP_PRINTF(("on_wifi_evt_sme_connect_failed(), reason: %d\n",
                  pck->evt_sme_connect_failed.reason));
      if (pck->evt_sme_connect_failed.reason != wifi_err_success) {
        if (pck->evt_sme_connect_failed.reason == wifi_err_ap_not_in_scanlist) {
          APP_LCD_ShowScreen(APP_LCD_SCREEN_CONN_AP_NOT_FOUND);
        }        
        AppWiFiGoToState(APP_WIFI_STATE_RESET);
      }
      break;

    case wifi_evt_tcpip_configuration_id:
      CPU_CRITICAL_ENTER();
      snprintf(&APP_WIFI_DeviceIpAddr[0],
               sizeof(APP_WIFI_DeviceIpAddr),
               "%d.%d.%d.%d",
               pck->evt_tcpip_configuration.address.a[0],
               pck->evt_tcpip_configuration.address.a[1],
               pck->evt_tcpip_configuration.address.a[2],
               pck->evt_tcpip_configuration.address.a[3]);
      CPU_CRITICAL_EXIT();
      APP_PRINTF(("on_wifi_evt_tcpip_configuration(), "
                  "Device's Local IP %d.%d.%d.%d\n",
                  pck->evt_tcpip_configuration.address.a[0],
                  pck->evt_tcpip_configuration.address.a[1],
                  pck->evt_tcpip_configuration.address.a[2],
                  pck->evt_tcpip_configuration.address.a[3]));
      break;

    case wifi_evt_tcpip_endpoint_status_id:
      if (pck->evt_tcpip_endpoint_status.local_ip.u != 0) {
        APP_PRINTF(("wifi_evt_tcpip_endpoint_status(), "
                    "Remote IP %d.%d.%d.%d on endpoint %d\n",
                    pck->evt_tcpip_endpoint_status.remote_ip.a[0],
                    pck->evt_tcpip_endpoint_status.remote_ip.a[1],
                    pck->evt_tcpip_endpoint_status.remote_ip.a[2],
                    pck->evt_tcpip_endpoint_status.remote_ip.a[3],
                    pck->evt_tcpip_endpoint_status.endpoint));
      }
      break;

    case wifi_evt_tcpip_dns_configuration_id:
      APP_PRINTF(("on_wifi_evt_tcpip_dns_configuration(), "
                  "DNS[%d] IP %d.%d.%d.%d\n",
                  pck->evt_tcpip_dns_configuration.index,
                  pck->evt_tcpip_dns_configuration.address.a[0],
                  pck->evt_tcpip_dns_configuration.address.a[1],
                  pck->evt_tcpip_dns_configuration.address.a[2],
                  pck->evt_tcpip_dns_configuration.address.a[3]));
      break;

    case wifi_evt_sme_interface_status_id:
      APP_PRINTF(("on_wifi_evt_sme_interface_status(), status: %d\n",
                  pck->evt_sme_interface_status.status));
      if (pck->evt_sme_interface_status.status) {
        // Network is up. Initiate a TCP connection.
        AppWiFiGoToState(APP_WIFI_STATE_DNS_RES_HOSTNAME);
      }
      break;

    case wifi_rsp_tcpip_dns_gethostbyname_id:
      APP_PRINTF(("on_wifi_rsp_tcpip_dns_gethostbyname(), result: %d\n",
                  pck->rsp_tcpip_dns_gethostbyname.result));
      break;

    case wifi_evt_tcpip_dns_gethostbyname_result_id:
      APP_PRINTF(("on_wifi_evt_tcpip_dns_gethostbyname_result(), "
                  "Result: %d, Hostname's IP %d.%d.%d.%d\n",
                  pck->evt_tcpip_dns_gethostbyname_result.result,
                  pck->evt_tcpip_dns_gethostbyname_result.address.a[0],
                  pck->evt_tcpip_dns_gethostbyname_result.address.a[1],
                  pck->evt_tcpip_dns_gethostbyname_result.address.a[2],
                  pck->evt_tcpip_dns_gethostbyname_result.address.a[3]));
      if (pck->evt_tcpip_dns_gethostbyname_result.result == wifi_err_success) {
        CPU_CRITICAL_ENTER();
        snprintf(&APP_WIFI_TcpServerIpAddr[0],
                 sizeof(APP_WIFI_TcpServerIpAddr),
                 "%d.%d.%d.%d",
                 pck->evt_tcpip_dns_gethostbyname_result.address.a[0],
                 pck->evt_tcpip_dns_gethostbyname_result.address.a[1],
                 pck->evt_tcpip_dns_gethostbyname_result.address.a[2],
                 pck->evt_tcpip_dns_gethostbyname_result.address.a[3]);
        CPU_CRITICAL_EXIT();
      } else {
        CPU_CRITICAL_ENTER();
        snprintf(&APP_WIFI_TcpServerIpAddr[0],
                 sizeof(APP_WIFI_TcpServerIpAddr),
                 APP_WIFI_TCP_SERVER_IP_ADDR);
        CPU_CRITICAL_EXIT();
      }
      APP_LCD_ShowScreen(APP_LCD_SCREEN_CONN_SERVER);
      AppWiFiGoToState(APP_WIFI_STATE_TCP_CONNECT);
      break;

    case wifi_rsp_tcpip_tcp_connect_id:
      APP_PRINTF(("on_wifi_rsp_tcpip_tcp_connect(), endpoint: %d, result: %d\n",
                  pck->rsp_tcpip_tcp_connect.endpoint,
                  pck->rsp_tcpip_tcp_connect.result));
      if (pck->rsp_tcpip_tcp_connect.result != wifi_err_success) {
        AppWiFiGoToState(APP_WIFI_STATE_RESET);
      }
      appWiFiTcpEndpoint = pck->rsp_tcpip_tcp_connect.endpoint;
      // Waiting for an endpoint status event that indicates the endpoint is
      // now active until sending the HTTP GET request.
      break;

    case wifi_evt_endpoint_status_id:
      APP_PRINTF(("on_wifi_evt_endpoint_status(), endpoint: %d, "
                  "is_streaming: %d, dest_endpoint: %d, is_active: %d\n",
                  pck->evt_endpoint_status.endpoint,
                  pck->evt_endpoint_status.streaming,
                  pck->evt_endpoint_status.destination,
                  pck->evt_endpoint_status.active));
      // If this is the endpoint assigned to the TCP connection opened
      // earlier and it's active, then send the HTTP GET request.
      if (pck->evt_endpoint_status.endpoint == appWiFiTcpEndpoint
          && pck->evt_endpoint_status.active
          && appWiFiState == APP_WIFI_STATE_TCP_CONNECT) {
        AppWiFiGoToState(APP_WIFI_STATE_TCP_SEND);
      } else if (pck->evt_endpoint_status.endpoint == appWiFiTcpEndpoint
                 && appWiFiState == APP_WIFI_STATE_TCP_CLOSE) {
        AppWiFiGoToState(APP_WIFI_STATE_TCP_CONNECT);
      }
      break;

    case wifi_rsp_endpoint_send_id:
      APP_PRINTF(("on_wifi_rsp_endpoint_send(), endpoint: %d, result: %d\n",
                  pck->rsp_endpoint_send.endpoint,
                  pck->rsp_endpoint_send.result));
      if (pck->rsp_endpoint_send.endpoint == appWiFiTcpEndpoint
          && appWiFiState == APP_WIFI_STATE_TCP_SEND
          && pck->rsp_endpoint_send.result != wifi_err_success) {
        AppWiFiGoToState(APP_WIFI_STATE_RESET);
      }
      break;

    case wifi_evt_endpoint_data_id:
      APP_PRINTF(("on_wifi_evt_endpoint_data(), endpoint: %d, data_len: %d\n",
                  pck->evt_endpoint_data.endpoint,
                  pck->evt_endpoint_data.data.len));
      // Instead of parsing the data as it's received, it's simply copied
      // to a buffer and handled when the server closes the connection.
      if ((appWiFiTcpReqRxBufIx + pck->evt_endpoint_data.data.len) 
          < APP_WIFI_TCP_REQ_RX_BUF_SIZE) {
        memcpy(&appWiFiTcpReqRxBuf[appWiFiTcpReqRxBufIx],
               pck->evt_endpoint_data.data.data,
               pck->evt_endpoint_data.data.len);
        appWiFiTcpReqRxBufIx += pck->evt_endpoint_data.data.len;
      } else {
        AppWiFiGoToState(APP_WIFI_STATE_RESET);
      }
      break;

    case wifi_evt_endpoint_closing_id:
      APP_PRINTF(("on_wifi_evt_endpoint_closing(), endpoint: %d, "
                  "reason: %d, AppWiFiTcpReqRxBufIx: %d\n",
                  pck->evt_endpoint_closing.endpoint,
                  pck->evt_endpoint_closing.reason,
                  appWiFiTcpReqRxBufIx));
      if (appWiFiState == APP_WIFI_STATE_TCP_SEND) {
        appWiFiTcpReqRxBuf[appWiFiTcpReqRxBufIx] = '\0';
        appWiFiTcpReqRxBufIx = 0;
        if (appWiFiTcpReqType == APP_TCP_REQ_TYPE_PEEK) {
          dataValid = AppWiFiParseTcpReqPeek();
          if (dataValid == true) {
            appWiFiTcpReqPeekOkCtr++;
            if (strncmp(&appWiFiLastImgTS[0], &appWiFiCurImgTimeStamp[0], 23)) {
              appWiFiTcpReqType = APP_TCP_REQ_TYPE_GET;
            } else {
              appWiFiTcpReqType = APP_TCP_REQ_TYPE_PEEK;
            }
          }
        } else if (appWiFiTcpReqType == APP_TCP_REQ_TYPE_GET) {
          dataValid = AppWiFiParseTcpReqGet();
          if (dataValid == true) {
            appWiFiTcpReqGetOkCtr++;
            snprintf(&appWiFiLastImgTS[0],
                     sizeof(appWiFiLastImgTS),
                     "%s",
                     &appWiFiCurImgTimeStamp[0]);
            appWiFiTcpReqType = APP_TCP_REQ_TYPE_PEEK;
            if (appWiFiFirstTime || (appWiFiTcpReqGetOkCtr > 1)) {
              APP_LCD_ShowScreen(APP_LCD_SCREEN_WHITEBOARD);
              appWiFiFirstTime = false;
            } else {
              APP_LCD_ShowScreen(APP_LCD_SCREEN_NETWORK_INFO);
            }
          }
        }
        // Disconnect from the TCP server.
        AppWiFiGoToState(APP_WIFI_STATE_TCP_CLOSE);
      } else {
        // TCP connection was closed unexpectedly.
        AppWiFiGoToState(APP_WIFI_STATE_RESET);
      }
      break;

    case wifi_rsp_endpoint_close_id:
      APP_PRINTF(("on_wifi_rsp_endpoint_close(), result: %d\n",
                  pck->rsp_endpoint_close.result));
      if (pck->rsp_endpoint_close.result != wifi_err_success) {
        AppWiFiGoToState(APP_WIFI_STATE_TCP_CONNECT);
      }
      break;

    case wifi_rsp_sme_disconnect_id:
      APP_PRINTF(("on_wifi_rsp_sme_disconnect(), result: %d\n",
                  pck->rsp_sme_disconnect.result));
      if (pck->rsp_sme_disconnect.result != wifi_err_success) {
         AppWiFiGoToState(APP_WIFI_STATE_RESET);
      }
      break;

    case wifi_evt_sme_disconnected_id:
      APP_PRINTF(("on_wifi_evt_sme_disconnected(), reason: %d\n",
                  pck->evt_sme_disconnected.reason));
      if (appWiFiState == APP_WIFI_STATE_DISCONNECT) {
        AppWiFiGoToState(APP_WIFI_STATE_SME_CONNECT_SSID);
      } else {
        // An unexpected disconnection from the WiFi access point.
        AppWiFiGoToState(APP_WIFI_STATE_RESET);
      }
      break;

    default:
      APP_PRINTF(("on_unhandled_rsp_evt(), ID %lu\n",
                  BGLIB_MSG_ID(appWiFiUartRxBufPtr)));
      break;
  }
}

/**************************************************************************//**
 * @brief Parse response from a TCP request (peek)
 * This function is used to parse the response from the TCP server
 * to a TCP peek request.
 * @returns bool Success true or false.
 * @note The peek request is meant to return the timestamp of
 *       the last time an image was drawn on the whiteboard.
 *****************************************************************************/
static bool AppWiFiParseTcpReqPeek(void)
{
  char *token = NULL;
  int dataLen;
  bool dataValid;
  int retVal;

  dataValid = false;

  // Parse response and check the date
  token = strstr((char *)&appWiFiTcpReqRxBuf[0], "200 OK");

  if (token) {
    token = strstr(token, "Content-Length: ");
    token += 16;
    if (token) {
      retVal = sscanf(token, "%d\r\n", &dataLen);
      if (token && retVal == 1) {
        token = strstr(token, "\r\n\r\n");
        if (token) {
          token += 4;
          if (token) {
            if (strstr(token, "20")) {
              retVal = snprintf(&appWiFiCurImgTimeStamp[0],
                                 sizeof(appWiFiCurImgTimeStamp),
                                 "%s",
                                 token);
              if (retVal == dataLen) {
                // This is a valid timestamp, there is an image.
                dataValid = true;
              }
            } else if (strstr(token, "none")) {
              // This means that the image has never been drawn yet.
              if (appWiFiTcpReqPeekOkCtr == 0) {
                appWiFiFirstTime = true;
                APP_LCD_ShowScreen(APP_LCD_SCREEN_NETWORK_INFO);
              }
              dataValid = true;
            }
          }
        } 
      }
    }
  }
  return(dataValid);
}

/**************************************************************************//**
 * @brief Parse response from a TCP request (get)
 * This function is used to parse the response from the TCP server
 * to a TCP get request.
 * @returns bool Success true or false.
 * @note The get request is meant to return the latest image
 *       that was drawn on the whiteboard.
 *****************************************************************************/
static bool AppWiFiParseTcpReqGet(void)
{
  char *token = NULL;
  int dataLen;
  bool dataValid;
  int retVal;
  CPU_SR_ALLOC();

  dataValid = false;

  // Parse the response and display on LCD
  token = strstr((char *)&appWiFiTcpReqRxBuf[0], "200 OK");

  if (token) {
    token = strstr(token, "Content-Length: 2048");
    token += 16;
    if (token) {
      retVal = sscanf(token, "%d\r\n", &dataLen);
      if (token && retVal == 1 && dataLen == 2048) {
        token = strstr(token, "\r\n\r\n");
        if (token) {
          token += 4;
          if (token) {
            CPU_CRITICAL_ENTER();
            memcpy(&APP_LCD_PixValues[0], token, dataLen);
            CPU_CRITICAL_EXIT();
            dataValid = true;
          }
        }
      }
    }
  }
  return(dataValid);
}

/**************************************************************************//**
 * @brief Convert the IP address string to an integer.
 * @param value Value to parse.
 * @return 0 if failure, corresponding integer value otherwise.
 *****************************************************************************/
static uint32_t AppWiFiConvertIpAddr2Dec(char* value)
{
  int ip[4];

  if (sscanf(value, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]) != 4) {
      return 0;
  }

  return (uint32_t)(ip[3] << 24 |
                    ip[2] << 16 |
                    ip[1] <<  8 |
                    ip[0]);
}

/**************************************************************************//**
 * @brief UART receive byte callback.
 * Callback function called from the UART module when
 * a byte has been received.
 * @return Void.
 *****************************************************************************/
static void AppWiFiUartRxCallback(void)
{
  RTOS_ERR osErr;

  OSTaskSemPost(&appWiFiTaskTCB,
                 OS_OPT_POST_NONE,
                &osErr);
}
