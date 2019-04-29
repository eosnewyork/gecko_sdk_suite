/***************************************************************************//**
 * @brief MIcrium RTOS sleep code.
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
#include "stack/include/api-rename.h"
#include "stack/include/ember.h"
#include "hal.h"

#include "micrium-rtos-support.h"

#include "flex-callbacks.h"

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
      && emAfPluginMicriumRtosAfTaskIsDeepSleepAllowed()
      && emberAfPluginMicriumRtosOkToEnterLowPowerCallback(true, sleepTimeMs)) {
    halPowerDown();
    halSleepForMilliseconds(&sleepTimeMs);
    halPowerUp();
  } else if (emberAfPluginMicriumRtosOkToEnterLowPowerCallback(false,
                                                               sleepTimeMs)) {
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
