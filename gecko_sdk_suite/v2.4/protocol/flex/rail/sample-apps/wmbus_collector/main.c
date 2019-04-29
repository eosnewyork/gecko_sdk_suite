/***************************************************************************//**
 * @file main.c
 * @brief Wireless M-Bus Collector Application
 * @copyright Copyright 2017 Silicon Laboratories, Inc. http://www.silabs.com
 ******************************************************************************/
#include "rail.h"
#include "hal_common.h"
#include "rail_config.h"

#include "em_chip.h"
#include "em_emu.h"

#include "retargetserial.h"
#include <stdio.h>
#include <string.h>

#include "wmbus/wmbus.h"
#include "bsp.h"

typedef enum {
  S_STARTRX,
  S_PRINTRX,
  S_IDLE,
} state_t;
static volatile state_t state;
static volatile RAIL_RxPacketHandle_t handleToPrint;
static uint8_t packetBuffer[256];

static const uint8_t cryptoKey[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                                     0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };

static RAIL_Handle_t railHandle;

static void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events);
static void RAILCb_RfReady(RAIL_Handle_t railHandle);

static RAIL_Config_t railCfg = {
  .eventsCallback = &RAILCb_Generic,
};

void initRadio()
{
  halInit();
  railHandle = RAIL_Init(&railCfg, &RAILCb_RfReady);

  RAIL_Idle(railHandle, RAIL_IDLE, true);

  RAIL_ConfigCal(railHandle, RAIL_CAL_ALL);

  RAIL_ConfigChannels(railHandle, channelConfigs[0], NULL);

  // Initialize the PA now that the HFXO is up and the timing is correct
  RAIL_TxPowerConfig_t txPowerConfig = {
#if HAL_PA_2P4_LOWPOWER
    .mode = RAIL_TX_POWER_MODE_2P4_LP,
#else
    .mode = RAIL_TX_POWER_MODE_2P4_HP,
#endif
    .voltage = BSP_PA_VOLTAGE,
    .rampTime = HAL_PA_RAMP,
  };
  if (channelConfigs[0]->configs[0].baseFrequency < 1000000000UL) {
    // Use the Sub-GHz PA if required
    txPowerConfig.mode = RAIL_TX_POWER_MODE_SUBGIG;
  }
  if (RAIL_ConfigTxPower(railHandle, &txPowerConfig) != RAIL_STATUS_NO_ERROR) {
    // Error: The PA could not be initialized due to an improper configuration.
    // Please ensure your configuration is valid for the selected part.
    while (1) ;
  }
  RAIL_SetTxPower(railHandle, HAL_PA_POWER);

  RAIL_ConfigRxOptions(railHandle, RAIL_RX_OPTIONS_ALL, RAIL_RX_OPTIONS_DEFAULT);

  RAIL_ConfigEvents(railHandle, RAIL_EVENTS_ALL,
                    RAIL_EVENT_CAL_NEEDED | RAIL_EVENT_RX_PACKET_RECEIVED);
}

void printBlocks(uint8_t *buffer, uint16_t length)
{
  for (uint8_t i = 0; i < length; i++) {
    if (  i % 16 == 0 ) {
      printf("[");
    } else if ( i % 16 == 8) {
      printf("| ");
    }
    printf("0x%02X", buffer[i]);
    if ( i % 16 == 15 || i == length - 1 ) {
      printf("]\n");
    } else {
      printf(" ");
    }
  }
}

void printRxPacket()
{
  RAIL_RxPacketInfo_t packetInfo;
  RAIL_GetRxPacketInfo(railHandle, handleToPrint, &packetInfo);

  RAIL_RxPacketDetails_t packetDetails;
  packetDetails.timeReceived.totalPacketBytes = packetInfo.packetBytes;
  packetDetails.timeReceived.timePosition = RAIL_PACKET_TIME_AT_SYNC_END;
  RAIL_GetRxPacketDetails(railHandle, handleToPrint, &packetDetails);

  memcpy(packetBuffer, packetInfo.firstPortionData, packetInfo.firstPortionBytes);
  if ( packetInfo.firstPortionBytes < packetInfo.packetBytes ) {
    memcpy(packetBuffer + packetInfo.firstPortionBytes, packetInfo.lastPortionData, packetInfo.packetBytes - packetInfo.firstPortionBytes);
  }
  RAIL_ReleaseRxPacket(railHandle, handleToPrint);

  WMBUS_dll_header_t *dllHeader = (WMBUS_dll_header_t*)packetBuffer;
  WMBUS_stl_header_t *stlHeader = (WMBUS_stl_header_t*)(packetBuffer + sizeof(WMBUS_dll_header_t));

  char mField[3];
  WMBUSframe_MField2Chars(dllHeader->address.detailed.manufacturer, mField);
  printf("RX:[Time:%lu]\n", packetDetails.timeReceived.packetTime);
  printf("Block-1:[L:%d,C:0x%02X,M:%c%c%c,ID:%08X,Version:0x%02X,devType:0x%02X]\n",
         dllHeader->lField,
         dllHeader->cField.raw,
         mField[0], mField[1], mField[2],
         (unsigned int)dllHeader->address.detailed.id,
         (unsigned int)dllHeader->address.detailed.version,
         (unsigned int)dllHeader->address.detailed.deviceType);
  if ( stlHeader->ciField == WMBUS_CI_EN13757_3_APPLICATION_SHORT ) {
    uint8_t *payloadStart = packetBuffer + sizeof(WMBUS_dll_header_t) + sizeof(WMBUS_stl_header_t);
    uint16_t payloadLen = dllHeader->lField - sizeof(WMBUS_dll_header_t) - sizeof(WMBUS_stl_header_t) + 1;
    printf("AppHeader:[CI:0x%02X,AccessNr:%d,Status:0x%02X,encMode:%d,Accessibility:%02X,encBlocks:%d,sync:%d]\n",
           stlHeader->ciField,
           stlHeader->accessNumber,
           stlHeader->status,
           stlHeader->confWord.mode_0_5.mode,
           stlHeader->confWord.mode_0_5.accessibility,
           stlHeader->confWord.mode_0_5.numOfEncBlocks,
           stlHeader->confWord.mode_0_5.synchronized);
    if ( stlHeader->confWord.mode_0_5.mode == 5 ) {
      uint8_t iv[16];
      //with long transport layer header, the address from the header should be used
      memcpy(iv, &(dllHeader->address.raw), 8);
      memset(iv + 8, stlHeader->accessNumber, 8);
      WMBUSframe_crypto5decrypt(payloadStart, payloadStart, iv, payloadLen);
    }
    printBlocks(payloadStart, payloadLen);
  } else {
    printBlocks(packetBuffer + sizeof(WMBUS_dll_header_t), dllHeader->lField - sizeof(WMBUS_dll_header_t) + 1);
  }
}

int main(void)
{
  CHIP_Init();
  state = S_IDLE;
  initRadio();
  WMBUSframe_crypto5Init();
  WMBUSframe_crypto5SetKey(cryptoKey);

  RETARGET_SerialCrLf(true);

  while (1) {
    switch (state) {
      case S_PRINTRX:
        printRxPacket();
        state = S_STARTRX;
        break;
      case S_STARTRX:
        RAIL_Idle(railHandle, RAIL_IDLE, true);
        RAIL_StartRx(railHandle, 0, NULL);
        state = S_IDLE;
        break;
      default:
        EMU_EnterEM1();
        break;
    }
  }
}

static void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events)
{
  if ( events & RAIL_EVENT_CAL_NEEDED ) {
    RAIL_Calibrate(railHandle, (RAIL_CalValues_t*)channelConfigs[0]->configs[0].attr->calValues, RAIL_CAL_ALL_PENDING);
  }

  if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
    BSP_LedToggle(1);
    //since we disable the receiver while printing, we can keep the packet in RAIL's buffer until that.
    handleToPrint = RAIL_HoldRxPacket(railHandle);
    state = S_PRINTRX;
  }
}

static void RAILCb_RfReady(RAIL_Handle_t railHandle)
{
  state = S_STARTRX;
}
