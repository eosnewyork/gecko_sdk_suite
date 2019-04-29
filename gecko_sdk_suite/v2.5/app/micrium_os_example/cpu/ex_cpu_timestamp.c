/***************************************************************************//**
 * @file
 * @brief CPU Timestamp Counter Example
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/common.h>
#include  <kernel/include/os.h>

#include  <common/include/lib_def.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/toolchains.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                  EXAMPLE CONFIGURATION REQUIREMENTS
 ********************************************************************************************************
 *******************************************************************************************************/

#if  CPU_CFG_TS_TMR_EN != DEF_ENABLED
#error "This example requires either CPU_CFG_TS_32_EN or CPU_CFG_TS_64_EN to be defined."
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               LOGGING
 *
 * Note(s) : (1) This example outputs information to the console via the function printf() via a macro
 *               called EX_TRACE(). This can be modified or disabled if printf() is not supported.
 *******************************************************************************************************/

#ifndef  EX_TRACE
#include  <stdio.h>
#define  EX_TRACE(...)                                      printf(__VA_ARGS__)
#endif

/********************************************************************************************************
 *                                 EXAMPLE CPU IMESTAMP COUNTER SETTINGS
 *******************************************************************************************************/

//                                                                 Iterations per single loop.
#define  EX_CPU_TS_ITERATIONS_PER_LOOP                    50u
//                                                                 Number of single loops to simulate slow algo.
#define  EX_CPU_TS_SLOW_ALGO                              40u
//                                                                 Number of single loops to simulate fast algo.
#define  EX_CPU_TS_FAST_ALGO                               1u

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void Ex_CPU_TimestampSimulateAlgorithm(CPU_INT32U cycles);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          Ex_CPU_Timestamp()
 *
 * @brief  Illustrates the usage of CPU's Timestamp feature.
 *******************************************************************************************************/
void Ex_CPU_Timestamp(void)
{
  RTOS_ERR        err;
  CPU_TS32        ts;
  CPU_INT64U      usecs;
  CPU_TS_TMR_FREQ freq;

  //                                                               Get Timestamp frequency.
  freq = CPU_TS_TmrFreqGet(&err);
  //                                                               Check error code.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE), 0u);
  EX_TRACE("CPU Timestamp: the timestamp counter runs at %u Hz\r\n", freq);

  //                                                               Profile Algorithm version 1.
  ts = CPU_TS_Get32();
  Ex_CPU_TimestampSimulateAlgorithm(EX_CPU_TS_SLOW_ALGO);
  ts = CPU_TS_Get32() - ts;
  //                                                               Get delta in microseconds.
  usecs = CPU_TS32_to_uSec(ts);
  EX_TRACE("CPU Timestamp: algorithm version 1 took %u timestamp ticks, or %llu microseconds\r\n", ts, usecs);

  //                                                               Profile Algorithm version 2.
  ts = CPU_TS_Get32();
  Ex_CPU_TimestampSimulateAlgorithm(EX_CPU_TS_FAST_ALGO);
  ts = CPU_TS_Get32() - ts;
  //                                                               Get delta in microseconds.
  usecs = CPU_TS32_to_uSec(ts);
  EX_TRACE("CPU Timestamp: algorithm version 2 took %u timestamp ticks, or %llu microseconds\r\n", ts, usecs);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                  Ex_CPU_TimestampSimulateAlgorithm()
 *
 * @brief  Simulates the processing time of an algorithm.
 *
 * @param  cycles  Number of iterations of the tight loop.
 *******************************************************************************************************/
static void Ex_CPU_TimestampSimulateAlgorithm(CPU_INT32U cycles)
{
  CPU_INT64U counter;
  CPU_INT32U i;

  counter = 0u;
  for (i = 0; i < cycles * EX_CPU_TS_ITERATIONS_PER_LOOP; ++i) {
    ++counter;
  }
}
