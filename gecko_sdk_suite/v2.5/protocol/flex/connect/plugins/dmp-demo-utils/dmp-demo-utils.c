/***************************************************************************//**
 * @brief wMBus helper plugin
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

#ifndef _DMP_DEMO_UTILS_C_
#define _DMP_DEMO_UTILS_C_

#include "dmp-demo-utils.h"

bool getLightState(uint8_t payload)
{
  return !!(payload & DEMO_CONTROL_PAYLOAD_CMD_DATA);
}

void setLightState(uint8_t * payload, bool state)
{
  *payload &= ~DEMO_CONTROL_PAYLOAD_CMD_DATA;
  *payload |= state;
}

DemoControlCommandType getCommandType(uint8_t payload)
{
  return (DemoControlCommandType)
         ((payload & DEMO_CONTROL_PAYLOAD_CMD_MASK) >> DEMO_CONTROL_PAYLOAD_CMD_MASK_SHIFT);
}

void setCommandType(uint8_t * payload, DemoControlCommandType type)
{
  *payload &= ~DEMO_CONTROL_PAYLOAD_CMD_MASK;
  *payload |= (type << DEMO_CONTROL_PAYLOAD_CMD_MASK_SHIFT) & DEMO_CONTROL_PAYLOAD_CMD_MASK;
}

DemoControlRole getRole(uint8_t payload)
{
  return (DemoControlRole) !!(payload & DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT);
}

void setRole(uint8_t * payload, DemoControlRole role)
{
  if (role == DEMO_CONTROL_ROLE_LIGHT) {
    *payload  &= ~DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT;
  } else { // role == DEMO_CONTROL_ROLE_SWITCH
    *payload  |= DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT;
  }
}

void setEUI(uint8_t * payload)
{
  uint32_t low = (DEVINFO->UNIQUEL);
  uint32_t high = (DEVINFO->UNIQUEH);
  uint8_t i = 0U;
  while (i < 4U) {
    payload[i] = low & 0xFFU;
    low >>= 8;
    i++;
  }
  while (i < 8U) {
    payload[i] = high & 0xFFU;
    high >>= 8;
    i++;
  }
}

void getEUI(uint8_t * dst, uint8_t * src)
{
  uint8_t i = 0U;
  for (i = 0; i < 8; i++) {
    dst[i] = src[i];
  }
}

void timerDelay(uint32_t delay)
{
  uint32_t startTime = RAIL_GetTime();
  while ((RAIL_GetTime() - startTime) < delay) {
  }
}

/******************************************************************************
 *
 * Common Printing Utilities
 *
 *****************************************************************************/
void printReceivedPackets(uint8_t * pktPtr, uint16_t pktCount)
{
  printf("RX(%d): ", pktCount);
  printBuffer(pktPtr, pktCount);
  printf("\n");
}

void printBuffer(uint8_t * buffer, const uint16_t bufferLength)
{
  for (uint16_t i = 0; i < bufferLength; ++i) {
    printf("0x%02X, ", buffer[i]);
  }
}

#endif // #ifndef _DMP_DEMO_UTILS_C_
