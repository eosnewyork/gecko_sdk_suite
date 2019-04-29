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

//This function is called to record the initial CPU active time when
//the device boots up.
void emberAfPluginCpuTracingInit(void);

//This is the function to be called when the device needs to start or stop
//counting the amount of time that the device spent in a certain CPU mode
//(CPU active, EM1, or EM2).
void emberAfPluginIdleSleepPowerModeChangedCallback
  (uint8_t cpuMode, bool startOrEnd);

//This function returns (but doesn't print out) the amount of time that
//the device spent in a certain CPU mode (CPU active, EM1, or EM2)
uint32_t emberAfPluginCpuTracingGetCpuModeTimeMs(uint8_t cpuMode);

//This functions prints out the total up time for CPU, EM1 mode, and EM2 mode.
//Also prints out the percentage of CPU active time (up to two digits afer the
//decimal point)
void emAfPluginCpuTracingPrintStatsCommand(void);

//This callback always returns false, disabling the EM2 mode.
//The device would never enter EM2 mode when the CPU-Tracing plugin is
//included.
bool emberAfPluginIdleSleepOkToSleepCallback(uint32_t durationMs);
