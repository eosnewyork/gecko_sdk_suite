/***************************************************************************//**
 * @file
 * @brief Defines to configure the main test application for this specific use
 * use case. These defines set things like channels, packet configuration, etc.
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

#ifndef __APP_DEFS_H_
#define __APP_DEFS_H_

/******************************************************************************
 * Application Memory Defintions
 *****************************************************************************/
// Memory manager configuration
#define BUFFER_POOL_SIZE 15
#define MAX_BUFFER_SIZE  256

// General application memory sizes
#define APP_MAX_PACKET_LENGTH  (MAX_BUFFER_SIZE - 12) /* sizeof(RAIL_RxPacketInfo_t) == 12) */

/******************************************************************************
 * Application User Interface
 *****************************************************************************/
#define APP_DEMO_STRING_INIT "RAIL Range Test App"

#endif
