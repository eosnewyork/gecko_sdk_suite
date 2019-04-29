// *******************************************************************
// * micrium-rtos-sleep.c
// *
// *
// * Copyright 2018 Silicon Laboratories, Inc.                              *80*
// *******************************************************************

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "stack/include/api-rename.h"
#include "stack/include/ember.h"

#include "hal.h"

#include "micrium-rtos-support.h"

//------------------------------------------------------------------------------
// Static variables.

static bool sleepAllowed = true;

//------------------------------------------------------------------------------
// Sleep handler - invoked from the idle task hook.

void emAfPluginMicriumRtosSleepHandler(void)
{
  uint32_t sleepTimeMs, stackTaskNextWakeUpTimeMs, afTaskNextWakeUpTimeMs,
           callTimeMs;

  INTERRUPTS_OFF();

  callTimeMs = halCommonGetInt32uMillisecondTick();
  stackTaskNextWakeUpTimeMs =
    emAfPluginMicriumRtosStackTaskGetNextWakeUpTimeMs();
  afTaskNextWakeUpTimeMs =
    emAfPluginMicriumRtosAfTaskGetNextWakeUpTimeMs();

  if (timeGTorEqualInt32u(callTimeMs, stackTaskNextWakeUpTimeMs)
      || timeGTorEqualInt32u(callTimeMs, afTaskNextWakeUpTimeMs)) {
    INTERRUPTS_ON();
    return;
  } else {
    sleepTimeMs = elapsedTimeInt32u(callTimeMs, stackTaskNextWakeUpTimeMs);
    if (sleepTimeMs > elapsedTimeInt32u(callTimeMs, afTaskNextWakeUpTimeMs)) {
      sleepTimeMs = elapsedTimeInt32u(callTimeMs, afTaskNextWakeUpTimeMs);
    }
  }

  if (sleepAllowed
      && emAfPluginMicriumRtosStackTaskIsDeepSleepAllowed()
      && emAfPluginMicriumRtosAfTaskIsDeepSleepAllowed()) {
    halPowerDown();
    halSleepForMilliseconds(&sleepTimeMs);
    halPowerUp();
  } else {
    halCommonIdleForMilliseconds(&sleepTimeMs);
  }
}

 #ifdef EMBER_AF_PLUGIN_MICRIUM_RTOS_USE_BUTTONS
void halButtonIsr(uint8_t button, uint8_t state)
{
  if (state == BUTTON_PRESSED) {
    sleepAllowed = (button == BUTTON1);
  }
}
 #endif
