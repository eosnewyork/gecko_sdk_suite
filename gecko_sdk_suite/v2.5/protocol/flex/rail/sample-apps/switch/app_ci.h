/***************************************************************************//**
 * @file
 * @brief This is the header file for common RAIL test application commands.
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

#ifndef __APP_CI_H__
#define __APP_CI_H__

/******************************************************************************
 * Application commands.
 *****************************************************************************/
#ifdef INTERNAL_COMMAND_HEADER
  #include INTERNAL_COMMAND_HEADER
#endif
#ifndef INTERNAL_COMMANDS
  #define INTERNAL_COMMANDS COMMAND_ENTRY(NULL, NULL, NULL, NULL)
#endif

// Base set of Command Interface commands
// <command> <args> <help text>
// u=uint8, v=uint16, w=uint32, s=int32, b=string, ?=Anything, *=0 or more of previous

#define APP_CI_COMMANDS                                                                                                                                                                         \
  COMMAND_SEPARATOR("Application Configuration"),                                                                                                                                               \
  COMMAND_ENTRY("lightToggle", "", lightToggle, "Toggle light."),                                                                                                                               \
  COMMAND_ENTRY("setPower", "sb*", setPower, "[power raw] Set the current transmit power in deci dBm, or raw units if 'raw' is specified"),                                                     \
  COMMAND_ENTRY("getPower", "", getPower, "Get the current transmit power in deci dBm"),                                                                                                        \
  COMMAND_ENTRY("setchannel", "u", setChannel, "[channel] Set the current radio channel"),                                                                                                      \
  COMMAND_ENTRY("getchannel", "", getChannel, "Get the current radio channel"),                                                                                                                 \
  COMMAND_ENTRY("getRssi", "w*", getRssi, "[wait] Get RSSI in dBm if the receiver is turned on. Optionally specify whether or not to wait for a valid value in case it is initially invalid."), \
  COMMAND_ENTRY("getVersion", "", getVersion, "Get version information."),                                                                                                                      \
  INTERNAL_COMMANDS

/******************************************************************************
 * Externed Prototypes
 *****************************************************************************/
// Command callback function prototypes
void lightToggle(int argc, char **argv);

void setPower(int argc, char **argv);
void getPower(int argc, char **argv);
void setChannel(int argc, char **argv);
void getChannel(int argc, char **argv);
void getRssi(int argc, char **argv);
void getVersion(int argc, char **argv);

#endif // __APP_CI_H__
