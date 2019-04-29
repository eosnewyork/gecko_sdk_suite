/***************************************************************************//**
 * @file
 * @brief Common Clk Example
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

#include  <rtos_description.h>

#ifdef RTOS_MODULE_COMMON_CLK_AVAIL

#include  <cpu/include/cpu.h>
#include  <common/include/clk.h>
#include  <common/include/lib_def.h>
#include  <common/include/rtos_utils.h>

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
#define  EX_TRACE(...)                                  printf(__VA_ARGS__)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                            Ex_CommonClkGet()
 *
 * @brief  Provides example on how to use the Clock sub-module of Common to get time.
 *******************************************************************************************************/
void Ex_CommonClkGet(void)
{
  CLK_DATE_TIME       date_time_set;
  CLK_DATE_TIME       date_time_get;
  CLK_DAY             day_of_wk;
  CLK_DAY             day_of_yr;
  CPU_BOOLEAN         is_ok;
  volatile CPU_INT32U i = 0u;

  is_ok = Clk_DateTimeMake(&date_time_set,
                           2000u,
                           CLK_MONTH_JAN,
                           01u,
                           7u,
                           8u,
                           9u,
                           CLK_TZ_SEC_FROM_UTC_GET(0));
  APP_RTOS_ASSERT_CRITICAL(is_ok == DEF_OK,; );

  is_ok = Clk_SetDateTime(&date_time_set);
  APP_RTOS_ASSERT_CRITICAL(is_ok == DEF_OK,; );

  day_of_wk = Clk_GetDayOfWk(2000u,
                             CLK_MONTH_JAN,
                             01u);
  EX_TRACE("Day of week for Jan 1st 2000 is %u.\r\n", day_of_wk);

  day_of_yr = Clk_GetDayOfYr(2000u,
                             CLK_MONTH_AUG,
                             01u);
  EX_TRACE("Day of year for Aug 1st 2000 is %u.\r\n", day_of_yr);

  while (i < 100000000u) {                                       // Let some time pass.
    i++;
  }

  is_ok = Clk_GetDateTime(&date_time_get);
  APP_RTOS_ASSERT_CRITICAL(is_ok == DEF_OK,; );

  if ((date_time_get.Hr == date_time_set.Hr)
      && (date_time_get.Min == date_time_set.Min)
      && (date_time_get.Sec == date_time_set.Sec)) {
    EX_TRACE("Clk did not see any time pass. Try increasing the previous delay and/or see if the signaling to Clk is done correctly, either in Clk task or via Clk_SignalClk() calls.\r\n");
  } else {
    EX_TRACE("Time set was: %uh%02u, %02u seconds. Time is now: %uh%02u, %02u seconds.",
             date_time_set.Hr, date_time_set.Min, date_time_set.Sec,
             date_time_get.Hr, date_time_get.Min, date_time_get.Sec);
  }
}

#endif // RTOS_MODULE_COMMON_CLK_AVAIL
