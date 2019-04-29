/***************************************************************************//**
 * @file
 * @brief Wireless M-Bus Meter Application
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
#include "rail.h"
#include "rail_types.h"
#include "hal_common.h"
#include "rail_config.h"

#include <stdlib.h>
#include <string.h>
#include "em_chip.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_rtcc.h"
#include "bsp.h"

#if _SILICON_LABS_32B_SERIES_1_CONFIG < 3
#include "rtcdriver.h"
#endif

#include "wmbus/wmbus.h"
#include "wmbus_sample_frame.h"

static const uint8_t cryptoKey[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                                     0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };

//start the rx window earlier and close it later by this amount
static const uint32_t responseDelaySafetyMargin = 200;

static const WMBUS_Mode_t mode = WMBUS_MODE_T_METER;
#define TX_CHANNEL 0
//rx channel will be TX_CHANNEL+1 in modes T and C, and TX_CHANNEL in all other modes
static uint16_t rxChannel;

#define RAIL_WAKEUP_TIME 1000 //time in us the MCU takes to get from the IRQ to the RAIL_Wake API (usually the time it takes to start up the clocks)

/*
 * 256 would be enough for an uncoded payload, but T mode does need 435B
 * However, EFR32 can handle 2 powers only.
 */
#define BUFFER_LENGTH 512

uint8_t gAccessNumber = 0;
uint8_t gTxBuffer[BUFFER_LENGTH];
const uint32_t gWmbusAppPeriodNom = 10e6; //10s
uint32_t gWmbusAppPeriodAcc = 500e3; //the first delay does not matter, so send it early, 500ms after boot
RAIL_Time_t gLastTxStartTime, gLastTxEndTime;
uint16_t gLastTxLength;
WMBUS_accessibility_t gAccessibility = WMBUS_ACCESSIBILITY_LIMITED_ACCESS;
static RAIL_Handle_t railHandle;

typedef enum {
  S_SCHEDULE_TX,
  S_RESPONSE_DELAY,
  S_STARTRX,
  S_RX,
  S_SLEEP,
  S_TX_PREPARE,
  S_TX_DONE,
} state_t;

static volatile state_t state;

//callback protos
static void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events);

static RAIL_Config_t railCfg = {
  .eventsCallback = &RAILCb_Generic,
};

void initRadio()
{
  halInit();
  railHandle = RAIL_Init(&railCfg, NULL);

  RAIL_Idle(railHandle, RAIL_IDLE, true);

  RAIL_ConfigCal(railHandle, RAIL_CAL_ALL - RAIL_CAL_ONETIME_IRCAL); //we calibrate IR manually, only on the Rx channel

  RAIL_ConfigChannels(railHandle, channelConfigs[0], NULL);

  // Initialize the PA now that the HFXO is up and the timing is correct
  RAIL_TxPowerConfig_t txPowerConfig = {
    .mode = RAIL_TX_POWER_MODE_SUBGIG,
    .voltage = BSP_PA_VOLTAGE,
    .rampTime = HAL_PA_RAMP,
  };
  if (RAIL_ConfigTxPower(railHandle, &txPowerConfig) != RAIL_STATUS_NO_ERROR) {
    // Error: The PA could not be initialized due to an improper configuration.
    // Please ensure your configuration is valid for the selected part.
    while (1) ;
  }
  RAIL_SetTxPower(railHandle, HAL_PA_POWER);

  RAIL_ConfigRxOptions(railHandle, RAIL_RX_OPTIONS_ALL, RAIL_RX_OPTIONS_DEFAULT);

  RAIL_ConfigEvents(railHandle, RAIL_EVENTS_ALL,
                    RAIL_EVENTS_TX_COMPLETION | RAIL_EVENTS_RX_COMPLETION
                    | RAIL_EVENT_CAL_NEEDED | RAIL_EVENT_RX_SCHEDULED_RX_END);

  //LE clocks and PRS clock (and PRS channel 7) is required for RAIL timer sync
  CMU_ClockEnable(cmuClock_PRS, true);
  CMU_ClockEnable(cmuClock_HFLE, true);
#if _SILICON_LABS_32B_SERIES_1_CONFIG < 3
  //on xG1 and xG12 we also need RTCC clock running (and compare channel 0 available). RTCDRV will do that for us
  RTCDRV_Init();
#endif
  RAIL_ConfigSleep(railHandle, RAIL_SLEEP_CONFIG_TIMERSYNC_ENABLED);
}

void sendPacketAt(uint16_t length, uint32_t sendAt)
{
  gLastTxLength = WMBUS_phy_software(gTxBuffer, length, BUFFER_LENGTH);
  RAIL_SetTxFifo(railHandle, gTxBuffer, gLastTxLength, BUFFER_LENGTH);
  if ( gLastTxLength != length ) {
    RAIL_SetFixedLength(railHandle, gLastTxLength);
  } else {
    RAIL_SetFixedLength(railHandle, RAIL_SETFIXEDLENGTH_INVALID);
  }
  RAIL_ScheduleTxConfig_t schedule = {
    .mode = RAIL_TIME_ABSOLUTE,
    .when = sendAt,
  };
  RAIL_StartScheduledTx(railHandle, TX_CHANNEL, RAIL_TX_OPTIONS_DEFAULT, &schedule, NULL);
}

/*
 * The application flow is the following for various accesibility modes:
 * WMBUS_ACCESSIBILITY_NO_ACCESS/WMBUS_ACCESSIBILITY_TEMPORARY_NO_ACCESS:
 *  -S_SCHEDULE_TX
 *  -S_SLEEP
 *  -S_TX_PREPARE
 *  -tx_completion event
 *  -S_TX_DONE (calculating t_period_acc)
 *  -S_SCHEDULE_TX
 * WMBUS_ACCESSIBILITY_UNLIMITED_ACCESS:
 *  -S_SCHEDULE_TX
 *  -S_RX
 *  -tx_completion event (auto state transition to RX)
 *  -S_TX_DONE (calculating t_period_acc)
 *  -S_SCHEDULE_TX
 *
 *  On Rx: Auto state transition back to rx.
 * WMBUS_ACCESSIBILITY_LIMITED_ACCESS:
 *  -S_SCHEDULE_TX
 *  -S_SLEEP
 *  -S_TX_PREPARE
 *  -tx_completion event
 *  -S_TX_DONE (calculating t_period_acc)
 *  -S_RESPONSE_DELAY
 *  -S_RX
 *  -rx_completion or rx_schedule_end event
 *  -S_SCHEDULE_TX
 */
void enablePrs(const char *debugPinName, uint16_t prsSig)
{
  uint32_t count;
  uint32_t pin = 0;
  const debugPin_t *debugPins = halGetDebugPins(&count);
  while ( pin < count && strcmp(debugPins[pin].name, debugPinName) ) {
    pin++;
  }
  if (pin < count) {
    CMU_ClockEnable(cmuClock_GPIO, true);
    GPIO_PinModeSet(debugPins[pin].gpioPort, debugPins[pin].gpioPin, gpioModePushPull, 0);
    halEnablePrs(debugPins[pin].prsChannel, debugPins[pin].prsLocation, debugPins[pin].gpioPort, debugPins[pin].gpioPin, prsSig >> 8, prsSig & 0xff);
  }
}

int main(void)
{
  CHIP_Init();
  initRadio();
  enablePrs("PC10", PRS_RAC_RX);
  enablePrs("PC11", PRS_RAC_TX);

  if ( gAccessibility == WMBUS_ACCESSIBILITY_UNLIMITED_ACCESS ) {
    RAIL_StateTransitions_t transitions = {
      .error = RAIL_RF_STATE_RX,
      .success = RAIL_RF_STATE_RX,
    };
    RAIL_SetTxTransitions(railHandle, &transitions);
    RAIL_SetRxTransitions(railHandle, &transitions);
  }

  if ( mode == WMBUS_MODE_T_METER || mode == WMBUS_MODE_T_COLLECTOR || mode == WMBUS_MODE_C ) {
    rxChannel = TX_CHANNEL + 1; //asymmetric config, rx will be on a different channel
  } else {
    rxChannel = TX_CHANNEL;
  }

  //calibrate IR on rx channel before starting the main loop, since it takes longer than most rx operation
  RAIL_PrepareChannel(railHandle, rxChannel);
  RAIL_CalibrateIr(railHandle, NULL);

  WMBUS_SetMode(mode);

  WMBUSframe_crypto5Init();
  WMBUSframe_crypto5SetKey(cryptoKey);
  gAccessNumber = rand() % 256;

  //send the first frame gWmbusAppPeriodAcc from now
  state = S_SCHEDULE_TX;
  gLastTxStartTime = RAIL_GetTime();

  uint16_t length;
  while (1) {
    switch (state) {
      case S_SCHEDULE_TX:
        length = WMBUS_SAMPLE_setupFrame(gTxBuffer, gAccessNumber, gAccessibility, 12345, 123, true, true);
        sendPacketAt(length, gLastTxStartTime + gWmbusAppPeriodAcc);
        if ( gAccessibility == WMBUS_ACCESSIBILITY_UNLIMITED_ACCESS ) {
          state = S_RX;
        } else {
          state = S_SLEEP;
        }
        break;
      case S_SLEEP:
      {
        bool canDeepSleep = false;
        if ( RAIL_Sleep(RAIL_WAKEUP_TIME, &canDeepSleep) != RAIL_STATUS_NO_ERROR) {
          canDeepSleep = false;
        }
        if ( canDeepSleep ) {
          EMU_EnterEM2(true);
          //interrupt will wake us up, and once it's handled we'll continue from here
          RAIL_Wake(0);
          state = S_TX_PREPARE; //make sure we don't re-enter sleep mode
        } else {
          EMU_EnterEM1();
        }
      }
      break;
      case S_TX_DONE:
        BSP_LedToggle(0);
        //let's setup the time for the next tx.
        /*
         * from EN13757-4:
         * tACC=(1+(|nACC-128|-64)/2048*tNOM
         * a bit easier to calculate:
         * tACC=(2048+|nACC-128|-64)*tNOM/2048
         */

        gAccessNumber++;
        gWmbusAppPeriodAcc = gAccessNumber > 128 ? gAccessNumber - 128 : 128 - gAccessNumber; //abs(gAccessNumber-128)
        gWmbusAppPeriodAcc = (2048 + gWmbusAppPeriodAcc - 64) * (gWmbusAppPeriodNom / 2048);
        if ( gAccessibility == WMBUS_ACCESSIBILITY_LIMITED_ACCESS ) {
          state = S_RESPONSE_DELAY;
        } else {
          state = S_SCHEDULE_TX;
        }
        break;
      case S_RESPONSE_DELAY:
      {
        RAIL_ScheduleRxConfig_t schedule = {
          .start = gLastTxEndTime + WMBUS_getMeterLimitedAccRxStart(false) - responseDelaySafetyMargin,
          .startMode = RAIL_TIME_ABSOLUTE,
          .end = gLastTxEndTime + WMBUS_getMeterLimitedAccRxStop(false) + responseDelaySafetyMargin,
          .endMode = RAIL_TIME_ABSOLUTE,
          .hardWindowEnd = 0,   //extend rx window if we're receiving
        };
        RAIL_Idle(railHandle, RAIL_IDLE, true);
        RAIL_ScheduleRx(railHandle, rxChannel, &schedule, NULL);
        state = S_RX;
      }
      break;
      default:
        //in idle states (like S_RX, we need the main oscillator, but waiting for an interrupt. EM1 is safe
        EMU_EnterEM1();
        break;
    }
  }
}

static void RAILCb_Generic(RAIL_Handle_t railHandle, RAIL_Events_t events)
{
  if ( events & RAIL_EVENTS_TX_COMPLETION ) {
    if ( events & RAIL_EVENT_TX_PACKET_SENT ) {
      RAIL_GetTxPacketDetailsAlt(railHandle, false, &gLastTxEndTime);
      gLastTxStartTime = gLastTxEndTime;
      RAIL_GetTxTimePreambleStart(railHandle, gLastTxLength, &gLastTxStartTime);
      RAIL_GetTxTimeFrameEnd(railHandle, gLastTxLength, &gLastTxEndTime);
    }
    state = S_TX_DONE;
  }
  if ( events & RAIL_EVENTS_RX_COMPLETION ) {
    if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
      BSP_LedToggle(1);
    }
    if ( gAccessibility == WMBUS_ACCESSIBILITY_LIMITED_ACCESS ) {
      state = S_SCHEDULE_TX;
    } else {
      state = S_RX;
    }
  }
  if ( events & RAIL_EVENT_RX_SCHEDULED_RX_END ) {
    state = S_SCHEDULE_TX;
  }
  if ( events & RAIL_EVENT_CAL_NEEDED ) {
    RAIL_CalibrateTemp(railHandle);
  }
}
