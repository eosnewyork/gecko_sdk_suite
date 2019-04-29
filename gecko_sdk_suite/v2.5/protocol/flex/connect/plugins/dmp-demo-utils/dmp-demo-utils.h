/***************************************************************************//**
 * @brief DMP Demo Helper Functions
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

#ifndef _DMP_DEMO_UTILS_H_
#define _DMP_DEMO_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "rail.h"
#include "rail_types.h"
#include "em_device.h"

/**
 * @addtogroup Header_helpers
 * @{
 */

typedef enum {
  DEMO_CONTROL_CMD_LIGHT_TOGGLE = 1,
  DEMO_CONTROL_CMD_LIGHT_STATE_REPORT = 2,
  DEMO_CONTROL_CMD_LIGHT_STATE_GET = 3,
} DemoControlCommandType;

typedef enum {
  DEMO_CONTROL_ROLE_LIGHT = 0,
  DEMO_CONTROL_ROLE_SWITCH = 1,
} DemoControlRole;

// Byte index for Light Control
// bit 7   - sender's role, value 0 - light, 1 - switch
// bit 6-4 - command
// bit 3-0 - data
#define DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT              (0x80)
#define DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT_SHIFT        (7)
#define DEMO_CONTROL_PAYLOAD_CMD_MASK                  (0x70)
#define DEMO_CONTROL_PAYLOAD_CMD_MASK_SHIFT            (4)
#define DEMO_CONTROL_PAYLOAD_CMD_DATA                  (0x0F)

bool getLightStatus(uint8_t * rxLightData, uint16_t rxLen);
void timerDelay(uint32_t delay);
void setRole(uint8_t * payload, DemoControlRole role);
DemoControlRole getRole(uint8_t payload);
void setCommandType(uint8_t * payload, DemoControlCommandType type);
DemoControlCommandType getCommandType(uint8_t payload);
void setLightState(uint8_t * payload, bool state);
bool getLightState(uint8_t payload);
void printReceivedPackets(uint8_t * pktPtr, uint16_t pktCount);
void printBuffer(uint8_t * buffer, const uint16_t bufferLength);
void setEUI(uint8_t * payload);
void getEUI(uint8_t * dst, uint8_t * src);

/**
 * @}
 */

#endif
