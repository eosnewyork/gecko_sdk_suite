/***************************************************************************//**
 * @brief
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

#include "stack/include/ember.h"
#include "serial/serial.h"
#include <kernel/include/os.h>

#include "micrium-rtos-support.h"
#include "flex-bookkeeping.h"
#include "flex-callbacks.h"

// Some large value still manageable by the OS and the BSP tick code (in case
// sleep is enabled).
#define MAX_AF_TASK_YIELD_TIME_MS (1000000)

//------------------------------------------------------------------------------
// Static variables.

static uint32_t afTaskNextWakeUpTimeMs;

//------------------------------------------------------------------------------
// Forward and external declarations.

static void appFrameworkTaskYield(void);

extern EmberTaskId emAppTask;
extern const EmberEventData emAppEvents[];

extern OS_FLAG_GRP emAfPluginMicriumRtosFlags;

//------------------------------------------------------------------------------
// Internal APIs.

void emAfPluginMicriumRtosAppFrameworkTask(void *p_arg)
{
  // Init and register the application events.
  emAppTask = emberTaskInit(emAppEvents);

  // Call the init callback of plugins that subscribed to it.
  emberAfInit();
  // Call the application init callback.
  emberAfMainInitCallback();

  while (true) {
    // Pet the watchdog.
    halResetWatchdog();
    // Call the application tick callback.
    emberAfMainTickCallback();
    // Call the tick callback of plugins that subscribed to it.
    emberAfTick();
    // Run application events,
    emberRunTask(emAppTask);
    // Process incoming callback commands from the vNCP.
    while (emAfPluginMicriumRtosProcessIncomingCallbackCommand()) {
    }
    // Yield the Application Framework task if possible.
    appFrameworkTaskYield();
  }
}

// As far as the application framework task is concerned, EM2 is always allowed.
bool emAfPluginMicriumRtosAfTaskIsDeepSleepAllowed(void)
{
  return true;
}

uint32_t emAfPluginMicriumRtosAfTaskGetNextWakeUpTimeMs(void)
{
  return afTaskNextWakeUpTimeMs;
}

//------------------------------------------------------------------------------
// Static functions.

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
            || emberSerialWriteUsed(i) != 0)) {
      return false;
    }
  }
#else // EM_NUM_SERIAL_PORTS
  // Without numbered ports, if there is pending incoming or outgoing data, we
  // do not sleep.
  if (!emberSerialUnused(APP_SERIAL)
      && (emberSerialReadAvailable(APP_SERIAL) != 0
          || emberSerialWriteUsed(APP_SERIAL) != 0)) {
    return false;
  }
#endif // EM_NUM_SERIAL_PORTS
#endif // EMBER_AF_PLUGIN_SERIAL
  return true;
}

static void appFrameworkTaskYield(void)
{
  uint32_t idleTimeMs = emberMsToNextEvent(emAppEvents,
                                           MAX_AF_TASK_YIELD_TIME_MS);

  if (okToIdleOrSleep() && idleTimeMs > 0) {
    RTOS_ERR err;
    OS_TICK yieldTimeTicks;

#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_POLL_CLI)
    if (idleTimeMs > POLL_CLI_PERIOD_MS) {
      idleTimeMs = POLL_CLI_PERIOD_MS;
    }
#endif

    yieldTimeTicks = (OSCfg_TickRate_Hz * idleTimeMs) / 1000;

    afTaskNextWakeUpTimeMs = halCommonGetInt32uMillisecondTick() + idleTimeMs;

    // Pend on a callback coming from the stack.
    OSFlagPend(&emAfPluginMicriumRtosFlags,
               FLAG_STACK_CALLBACK_PENDING,
               yieldTimeTicks,
               OS_OPT_PEND_BLOCKING
               + OS_OPT_PEND_FLAG_SET_ANY
               + OS_OPT_PEND_FLAG_CONSUME,
               NULL,
               &err);
  }
}
