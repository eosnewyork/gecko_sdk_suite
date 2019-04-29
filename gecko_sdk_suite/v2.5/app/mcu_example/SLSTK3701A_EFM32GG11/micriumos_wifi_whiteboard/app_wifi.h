/***************************************************************************//**
 * @file
 * @brief Wireless Whiteboard's WiFi Application Header File
 * WGM110 and GLIB demo for the SLSTK3701A running on MicOS.
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

#ifndef APP_WIFI_H
#define APP_WIFI_H

// -----------------------------------------------------------------------------
// Externs

#ifdef APP_WIFI_MODULE
#define APP_WIFI_MODULE_EXT
#else
#define APP_WIFI_MODULE_EXT extern
#endif

// -----------------------------------------------------------------------------
// Global Variables

/// WGM110 MAC address.
APP_WIFI_MODULE_EXT char APP_WIFI_MacAddr[16];

/// WGM110 IP address.
APP_WIFI_MODULE_EXT char APP_WIFI_DeviceIpAddr[16];

/// Access Point SSID name.
APP_WIFI_MODULE_EXT char APP_WIFI_ApSsid[32];

/// Server's IP address.
APP_WIFI_MODULE_EXT char APP_WIFI_TcpServerIpAddr[16];

// -----------------------------------------------------------------------------
// Function Prototypes

void APP_WIFI_Init(void);

#endif  // APP_WIFI_H
