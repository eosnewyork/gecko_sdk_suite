/***************************************************************************//**
 * @file
 * @brief Wireless Whiteboard Configuration File
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

#ifndef APP_WIFI_CFG_H
#define APP_WIFI_CFG_H

// -----------------------------------------------------------------------------
// Global Variables

// WiFi Access Point Credentials
#define  APP_WIFI_AP_SSID                "Your_SSID"
#define  APP_WIFI_AP_PWD                 "Your_Password"

#endif  // APP_WIFI_CFG_H
