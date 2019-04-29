/***************************************************************************//**
 * @file
 * @brief CPU Interrupt Disabled Measurement Example
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

#ifndef CPU_CFG_INT_DIS_MEAS_EN
#error "This example requires CPU_CFG_INT_DIS_MEAS_EN to be defined."
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
 *                          EXAMPLE CPU INTERRUPT DISABLED MEASUREMENT SETTINGS
 *******************************************************************************************************/

//                                                                 Iterations per single loop.
#define  EX_CPU_INT_ITERATIONS_PER_LOOP                   50u
//                                                                 Number of single loops for an 'extra-long' delay.
#define  EX_CPU_INT_XLONG_LOOP                            40u
//                                                                 Number of single loops for a  'medium'     delay.
#define  EX_CPU_INT_MEDIUM_LOOP                            1u

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

static void Ex_CPU_IntDelay(CPU_INT32U cycles);

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
 *                                          Ex_CPU_Interrupt()
 *
 * @brief  Illustrates the usage of CPU's Interrupts Disabled Measurement feature.
 *******************************************************************************************************/
void Ex_CPU_Interrupt(void)
{
  CPU_TS_TMR count;

  //                                                               Get Current maximum interrupts disabled time.
  count = CPU_IntDisMeasMaxCurGet();
  EX_TRACE("CPU Interrupts Disabled: Current maximum interrupts disabled time: %u\r\n",
           count);

  //                                                               Get Global maximum interrupts disabled time.
  count = CPU_IntDisMeasMaxGet();
  EX_TRACE("CPU Interrupts Disabled: Global maximum interrupts disabled time: %u\r\n",
           count);

  //                                                               Reset the current maximum interrupts disabled time.
  CPU_IntDisMeasMaxCurReset();
  EX_TRACE("CPU Interrupts Disabled: Resetting current maximum interrupts disabled time\r\n");

  //                                                               Simulate a medium critical section.
  EX_TRACE("CPU Interrupts Disabled: Simulating medium critical section\r\n");
  Ex_CPU_IntDelay(EX_CPU_INT_MEDIUM_LOOP);

  //                                                               Get Current maximum interrupts disabled time.
  count = CPU_IntDisMeasMaxCurGet();
  EX_TRACE("CPU Interrupts Disabled: Current maximum interrupts disabled time: %u\r\n",
           count);

  //                                                               Simulate a extra-long critical section.
  EX_TRACE("CPU Interrupts Disabled: Simulating extra-long critical section\r\n");
  Ex_CPU_IntDelay(EX_CPU_INT_XLONG_LOOP);

  //                                                               Get Current maximum interrupts disabled time.
  count = CPU_IntDisMeasMaxCurGet();
  EX_TRACE("CPU Interrupts Disabled: Current maximum interrupts disabled time: %u\r\n",
           count);

  //                                                               Get Global maximum interrupts disabled time.
  count = CPU_IntDisMeasMaxGet();
  EX_TRACE("CPU Interrupts Disabled: Global maximum interrupts disabled time: %u\r\n",
           count);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           Ex_CPU_IntDelay()
 *
 * @brief  Creates a vriable length (in time) critical section.
 *
 * @param  cycles  Number of iterations of the tight loop.
 *******************************************************************************************************/
static void Ex_CPU_IntDelay(CPU_INT32U cycles)
{
  CPU_INT64U counter;
  CPU_INT32U i;
  CPU_SR_ALLOC();

  counter = 0u;
  CPU_CRITICAL_ENTER();
  for (i = 0; i < cycles * EX_CPU_INT_ITERATIONS_PER_LOOP; ++i) {
    ++counter;
  }
  CPU_CRITICAL_EXIT();
}
