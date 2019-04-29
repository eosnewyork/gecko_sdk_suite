/***************************************************************************//**
 * @brief APIs for the poll plugin.
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

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "hal/hal.h"
#include "stack/include/ember.h"

#include "poll.h"

static uint8_t shortPollIntervalQS = EMBER_AF_PLUGIN_POLL_SHORT_POLL_INTERVAL_QS;
static uint16_t longPollIntervalS = EMBER_AF_PLUGIN_POLL_LONG_POLL_INTERVAL_S;
static bool shortPolling = FALSE;
static uint32_t lastPollMs;

EmberEventControl emberAfPluginPollEventControl;

static void scheduleNextPoll(void)
{
  emberEventControlSetDelayQS(emberAfPluginPollEventControl,
                              ((shortPolling)
                               ? shortPollIntervalQS
                               : (longPollIntervalS << 2)));
}

void emberAfPluginPollEventHandler(void)
{
  uint32_t nowMs = halCommonGetInt32uMillisecondTick();
  emberEventControlSetInactive(emberAfPluginPollEventControl);

  if (emberStackIsUp()
      && (emberGetNodeType() == EMBER_STAR_END_DEVICE
          || emberGetNodeType() == EMBER_STAR_SLEEPY_END_DEVICE
          || emberGetNodeType() == EMBER_MAC_MODE_DEVICE
          || emberGetNodeType() == EMBER_MAC_MODE_SLEEPY_DEVICE)) {
    lastPollMs = nowMs;
    scheduleNextPoll();
    emberPollForData();
  }
}

void emberAfPluginPollStackStatusCallback(EmberStatus status)
{
  if (emberStackIsUp()
      && (emberGetNodeType() == EMBER_STAR_END_DEVICE
          || emberGetNodeType() == EMBER_STAR_SLEEPY_END_DEVICE
          || emberGetNodeType() == EMBER_MAC_MODE_DEVICE
          || emberGetNodeType() == EMBER_MAC_MODE_SLEEPY_DEVICE)) {
    scheduleNextPoll();
  }
}

//------------------------------------------------------------------------------
// Public APIs

void emberAfPluginPollSetShortPollInterval(uint8_t intervalQS)
{
  shortPollIntervalQS = intervalQS;
  scheduleNextPoll();
}

void emberAfPluginPollSetLongPollInterval(uint16_t intervalS)
{
  longPollIntervalS = intervalS;
  scheduleNextPoll();
}

void emberAfPluginPollEnableShortPolling(bool enable)
{
  uint32_t nowMs = halCommonGetInt32uMillisecondTick();
  uint32_t pollIntervalMs;

  // Already short polling or already long polling, nothing to do.
  if ((enable && shortPolling)
      || (!enable && !shortPolling)) {
    return;
  }

  shortPolling = enable;

  if (emberEventControlGetActive(emberAfPluginPollEventControl)) {
    pollIntervalMs = ((shortPolling)
                      ? shortPollIntervalQS * MILLISECOND_TICKS_PER_QUARTERSECOND
                      : longPollIntervalS * MILLISECOND_TICKS_PER_SECOND);

    if (elapsedTimeInt32u(lastPollMs, nowMs) >= pollIntervalMs) {
      emberEventControlSetActive(emberAfPluginPollEventControl);
    } else {
      emberEventControlSetDelayMS(emberAfPluginPollEventControl,
                                  pollIntervalMs
                                  - elapsedTimeInt32u(lastPollMs, nowMs));
    }
  }
}
