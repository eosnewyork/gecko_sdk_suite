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
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                         Ex_CommonClkSetManual()
 *
 * @brief  Provides example on how to use the Clock sub-module to manually set the time.
 *******************************************************************************************************/
void Ex_CommonClkSetManual(void)
{
  CLK_DATE_TIME date_time_set;
  CPU_BOOLEAN   is_ok;

  //                                                               Obtain date and time data from user-input or ...
  is_ok = Clk_DateTimeMake(&date_time_set,                      // RTC, use it to set clk's internal date and time.
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
}

#endif // RTOS_MODULE_COMMON_CLK_AVAIL
