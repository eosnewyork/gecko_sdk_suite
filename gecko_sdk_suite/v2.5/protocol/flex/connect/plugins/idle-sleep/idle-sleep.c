/***************************************************************************//**
 * @brief Callbacks for the idle-sleep plugin.
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

// *******************************************************************
// * idle-sleep.c
// *
// *
// * Copyright 2018 Silicon Laboratories, Inc.                                 *80*
// *******************************************************************

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"
#include "flex-callbacks.h"

#ifdef EMBER_AF_PLUGIN_SERIAL
#include "serial/serial.h"
#endif

#ifdef EMBER_AF_PLUGIN_FREE_RTOS
#include "FreeRTOS.h"
#include "task.h"
#include "rtcdriver.h"
#endif

#include "idle-sleep.h"

extern const EmberEventData emAppEvents[];
extern EmberTaskId emAppTask;
static bool sleepAllowed = true;

// In testing, we cannot sleep or idle if the simulator is transmitting on any
// of the serial ports.
#ifdef EMBER_TEST
extern bool doingSerialTx[];
  #define simulatorDoingSerialTx(port) doingSerialTx[port]
#else
  #define simulatorDoingSerialTx(port) false
#endif

static bool okToIdleOrSleep(void)
{
#ifdef EMBER_AF_PLUGIN_SERIAL
#ifdef EM_NUM_SERIAL_PORTS
  // With numbered serial ports, if any port has pending incoming or outgoing
  // data or if the simulator is transmitting, we do not sleep.
  uint8_t i;
  for (i = 0; i < EM_NUM_SERIAL_PORTS; i++) {
    if (!emberSerialUnused(i)
        && (emberSerialReadAvailable(i) != 0
            || emberSerialWriteUsed(i) != 0
            || simulatorDoingSerialTx(i))) {
      return false;
    }
  }
#else // EM_NUM_SERIAL_PORTS
  // Without numbered ports, if there is pending incoming or outgoing data, we
  // do not sleep.
  if (!emberSerialUnused(APP_SERIAL)
      && (emberSerialReadAvailable(APP_SERIAL) != 0
          || emberSerialWriteUsed(APP_SERIAL) != 0
          || simulatorDoingSerialTx(APP_SERIAL))) {
    return false;
  }
#endif // EM_NUM_SERIAL_PORTS
#endif // EMBER_AF_PLUGIN_SERIAL
  return true;
}

void emberAfPluginIdleSleepTickCallback(void)
{
  // Turn interrupts off so that we can safely determine whether we can sleep
  // or idle.  Both halSleepForMilliseconds and emberMarkTaskIdle forcibly
  // enable interrupts.  We can call neither function or exactly one of them
  // without risking a race condition.  Note that if we don't call either, we
  // have to enable interrupts ourselves.
  INTERRUPTS_OFF();

  // We can idle or sleep if some basic conditions are satisfied.  If not, we
  // stay awake and active.  Otherwise, we try to sleep or idle, in that order.
  if (okToIdleOrSleep()) {
    // If the stack says we can nap, it means that we may sleep for some amount
    // of time.  Otherwise, we can't sleep at all, although we can try to idle.
    if (emberOkToNap()) {
      // If the stack says we can hiberate, it means we can sleep as long as we
      // want.  Otherwise, we cannot sleep longer than the duration to its next
      // event.  In either case, we will never sleep longer than the duration
      // to our next event.
      uint32_t durationMs = (emberOkToHibernate()
                             ? MAX_INT32U_VALUE
                             : emberMsToNextStackEvent());
      durationMs = emberMsToNextEvent(emAppEvents, durationMs);

      // If the sleep duration is below our minimum threshold, we don't bother
      // sleeping.  It takes time to shut everything down and bring everything
      // back up and, at some point, it becomes useless to sleep.  We also give
      // the application one last chance to stay awake if it wants.  If the
      // duration is long enough and the application says okay, we will sleep.
      // Otherwise, we will try to idle instead.
      if (EMBER_AF_PLUGIN_IDLE_SLEEP_MINIMUM_SLEEP_DURATION_MS <= durationMs
          && emberAfPluginIdleSleepOkToSleepCallback(durationMs)
          && sleepAllowed) {
        uint32_t attemptedDurationMs = durationMs;
        emberStackPowerDown();
        // WARNING: The following function enables interrupts, so
        // we will cease to be atomic by the time it returns.
        // One of emberAfPluginIdleSleepRtosCallback() or
        // halSleepForMilliseconds() will enable interrupts.
        if ( !emberAfPluginIdleSleepRtosCallback(&durationMs, true) ) {
          emberAfPluginIdleSleepPowerModeChangedCallback(
            EMBER_AF_PLUGIN_IDLE_SLEEP_CPU_MODE_SLEEP, START_EVENT);
          halPowerDown();
          halSleepForMilliseconds(&durationMs);
          halPowerUp();
          emberAfPluginIdleSleepPowerModeChangedCallback(
            EMBER_AF_PLUGIN_IDLE_SLEEP_CPU_MODE_SLEEP, END_EVENT);
        }
        emberStackPowerUp();
        emberAfPluginIdleSleepWakeUpCallback(attemptedDurationMs - durationMs);
        return;
      }
    }

#ifndef EMBER_NO_IDLE_SUPPORT
    // If we are here, it means we could not sleep, so we will try to idle
    // instead.  We give the application one last chance to stay active if it
    // wants.  Note that interrupts are still disabled at this point.
    if (emberAfPluginIdleSleepOkToIdleCallback() && sleepAllowed) {
      // We are going to try to idle.  Setting our task to idle will trigger a
      // check of every other task.  If the others are also idle, the device
      // will idle.  If not, it won't.  In either case, we set our task back to
      // active again.  This ensures that we are in control of idling and will
      // prevent the device from idling without our knowledge.
      // WARNING: The following function enables interrupts, so we will cease
      // to be atomic by the time it returns.
      emberAfPluginIdleSleepPowerModeChangedCallback(
        EMBER_AF_PLUGIN_IDLE_SLEEP_CPU_MODE_IDLE, START_EVENT);
      bool idled = emberMarkTaskIdle(emAppTask);
      emberMarkTaskActive(emAppTask);
      emberAfPluginIdleSleepPowerModeChangedCallback(
        EMBER_AF_PLUGIN_IDLE_SLEEP_CPU_MODE_IDLE, END_EVENT);
      if (idled) {
        emberAfPluginIdleSleepActiveCallback();
      }
      return;
    }
#endif // !EMBER_NO_IDLE_SUPPORT
  } // emAfOkToIdleOrSleep

  // If we are here, it means we did not sleep or idle.  Interrupts are still
  // disabled at this point, so we have to enable them again.
  INTERRUPTS_ON();
}

#ifdef EMBER_AF_PLUGIN_IDLE_SLEEP_USE_BUTTONS
void halButtonIsr(uint8_t button, uint8_t state)
{
  if (state == BUTTON_PRESSED) {
    sleepAllowed = (button == BUTTON1);
  }
}
#endif
