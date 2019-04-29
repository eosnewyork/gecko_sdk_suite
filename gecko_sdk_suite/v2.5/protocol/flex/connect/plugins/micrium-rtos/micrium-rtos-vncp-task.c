/***************************************************************************//**
 * @brief Micrium RTOS vncp code.
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
#include "stack/include/api-rename-undef.h"

#include "hal.h"
#include <kernel/include/os.h>

#include "micrium-rtos-support.h"

// Some large value still manageable by the OS and the BSP tick code (in case
// sleep is enabled).
#define MAX_VNCP_YIELD_TIME_MS (1000000)

//------------------------------------------------------------------------------
// Static variables.

static bool stackTaskDeepSleepAllowed = false;
static uint32_t stackTaskNextWakeUpTimeMs;

//------------------------------------------------------------------------------
// Forward declarations and external declarations

static void connectStackTaskYield(void);

extern OS_FLAG_GRP emAfPluginMicriumRtosFlags;

//------------------------------------------------------------------------------
// Internal APIs

// The stack task main loop.
void emAfPluginMicriumRtosStackTask(void *p_arg)
{
  // To save RAM and avoid a "system start" task, we use the VNCP task to
  // initialize all the other tasks.
  emAfPluginMicriumRtosInitTasks();

  // Initialize the radio and the stack.  If this fails, we have to assert
  // because something is wrong.
  assert(emApiInit() == EMBER_SUCCESS);

  while (true) {
    // Pet the watchdog.
    halResetWatchdog();
    // Call the stack tick API.
    emApiTick();
    // Process IPC commands from application tasks.
    emAfPluginMicriumRtosProcessIncomingApiCommand();
    // Yield the Connect stack task if possible.
    connectStackTaskYield();
  }
}

// This can be called from ISR.
void emAfPluginMicriumRtosWakeUpConnectStackTask(void)
{
  RTOS_ERR err;
  OSFlagPost(&emAfPluginMicriumRtosFlags,
             FLAG_STACK_ACTION_PENDING,
             OS_OPT_POST_FLAG_SET,
             &err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);
}

bool emAfPluginMicriumRtosStackTaskIsDeepSleepAllowed(void)
{
  return stackTaskDeepSleepAllowed;
}

uint32_t emAfPluginMicriumRtosStackTaskGetNextWakeUpTimeMs(void)
{
  return stackTaskNextWakeUpTimeMs;
}

//------------------------------------------------------------------------------
// Static functions

static void connectStackTaskYield(void)
{
  uint16_t currentStackTasks;
  uint32_t idleTimeMs = emApiStackIdleTimeMs(&currentStackTasks);

  if (idleTimeMs > 0) {
    RTOS_ERR err;

    if (idleTimeMs > MAX_VNCP_YIELD_TIME_MS) {
      idleTimeMs = MAX_VNCP_YIELD_TIME_MS;
    }

    OS_TICK yieldTimeTicks = (OSCfg_TickRate_Hz * idleTimeMs) / 1000;

    stackTaskDeepSleepAllowed =
      ((currentStackTasks & EMBER_HIGH_PRIORITY_TASKS) == 0);
    stackTaskNextWakeUpTimeMs
      = halCommonGetInt32uMillisecondTick() + idleTimeMs;

    // Pend on a stack action.
    OSFlagPend(&emAfPluginMicriumRtosFlags,
               FLAG_STACK_ACTION_PENDING,
               yieldTimeTicks,
               OS_OPT_PEND_BLOCKING
               + OS_OPT_PEND_FLAG_SET_ANY
               + OS_OPT_PEND_FLAG_CONSUME,
               NULL,
               &err);

    stackTaskDeepSleepAllowed = false;
  }
}
