/***************************************************************************//**
 * @brief Set of APIs/callbacks for the cpu-tracing plugin.
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
#include "hal/hal.h"
#include "serial/serial.h"
#include "flex-callbacks.h"
#include "cpu-tracing.h"
#include "idle-sleep/idle-sleep.h"

static uint32_t totalCPUisActiveTimeMs = 0;
static uint32_t totalCPUisIdleTimeMs = 0;

static uint32_t lastTimeCPUIsActiveMs = 0;
static uint32_t lastTimeCPUIsIdleMs = 0;

void emberAfPluginCpuTracingInit(void)
{
  lastTimeCPUIsActiveMs = halCommonGetInt32uMillisecondTick();
}

void emberAfPluginIdleSleepPowerModeChangedCallback(uint8_t cpuMode,
                                                    bool startOrEnd)
{
  assert(cpuMode == EMBER_AF_PLUGIN_IDLE_SLEEP_CPU_MODE_IDLE);
  uint32_t currTimeMs = halCommonGetInt32uMillisecondTick();
  if (startOrEnd == START_EVENT) {
    totalCPUisActiveTimeMs += (currTimeMs
                               - lastTimeCPUIsActiveMs);
    lastTimeCPUIsIdleMs = currTimeMs;
  } else {
    totalCPUisIdleTimeMs += (currTimeMs
                             - lastTimeCPUIsIdleMs);
    lastTimeCPUIsActiveMs = currTimeMs;
  }
}

uint32_t emberAfPluginCpuTracingGetCpuModeTimeMs(uint8_t cpuMode)
{
  assert(cpuMode == EMBER_AF_PLUGIN_IDLE_SLEEP_CPU_MODE_ACTIVE
         || cpuMode == EMBER_AF_PLUGIN_IDLE_SLEEP_CPU_MODE_IDLE);
  if (cpuMode == EMBER_AF_PLUGIN_IDLE_SLEEP_CPU_MODE_ACTIVE) {
    return totalCPUisActiveTimeMs;
  } else {
    return totalCPUisIdleTimeMs;
  }
}

void emAfPluginCpuTracingPrintStatsCommand(void)
{
  emberSerialPrintfLine(APP_SERIAL, "CPU Active Time: %d",
                        totalCPUisActiveTimeMs);
  emberSerialPrintfLine(APP_SERIAL, "CPU Idle Time: %d",
                        totalCPUisIdleTimeMs);
  uint32_t totalTimeMs = totalCPUisActiveTimeMs + totalCPUisIdleTimeMs;
  uint64_t temp = ((uint64_t)totalCPUisActiveTimeMs) * 100000;
  temp = temp / totalTimeMs;
  if (temp % 10 >= 5) {
    temp = temp / 10 + 1;
  } else {
    temp = temp / 10;
  }
  if ((uint8_t)(temp % 100) < 10) {
    emberSerialPrintfLine(APP_SERIAL, "CPU Active Time Percentage: %d.0%d%%",
                          (uint8_t)(temp / 100), (uint8_t)(temp % 100));
  } else {
    emberSerialPrintfLine(APP_SERIAL, "CPU Active Time Percentage: %d.%d%%",
                          (uint8_t)(temp / 100), (uint8_t)(temp % 100));
  }
}

bool emberAfPluginIdleSleepOkToSleepCallback(uint32_t durationMs)
{
  return false;
}
