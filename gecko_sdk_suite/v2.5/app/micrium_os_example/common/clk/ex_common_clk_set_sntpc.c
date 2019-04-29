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

#if defined(RTOS_MODULE_COMMON_CLK_AVAIL) \
  && defined(RTOS_MODULE_NET_AVAIL)       \
  && defined(RTOS_MODULE_NET_SNTP_CLIENT_AVAIL)

#include  <cpu/include/cpu.h>
#include  <common/include/clk.h>
#include  <common/include/lib_def.h>
#include  <common/include/rtos_utils.h>
#include  <net/include/sntp_client.h>
#include  <kernel/include/os.h>

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
 *                                         Ex_CommonClkSetSNTPc()
 *
 * @brief  Provides example on how to use the Clock sub-module to set the time using SNTPc.
 *
 * @note   (1) This examples assumes that SNTPc has previously been initialized successfully.
 *******************************************************************************************************/
void Ex_CommonClkSetSNTPc(void)
{
  SNTP_PKT      pkt;
  SNTP_TS       ts;
  CLK_DATE_TIME date_time;
  CLK_DATE_TIME date_time_from_convert_ts;
  CLK_DATE_TIME date_time_from_convert_ntp;
  CLK_TS_SEC    ts_sec;
  CLK_TS_SEC    ts_sec_ntp_from_clk;
  CPU_BOOLEAN   ok;
  CPU_CHAR      clk_str_buf[CLK_STR_FMT_DAY_MONTH_DD_HH_MM_SS_YYYY_LEN];
  CPU_INT08U    retry_ctr = 0;
  RTOS_ERR      err;

  for (retry_ctr = 0; retry_ctr < 3; retry_ctr++) {
    RTOS_ERR err_local;

    SNTPc_ReqRemoteTime("0.pool.ntp.org", &pkt, &err);          // Send SNTP request.
    if (err.Code == RTOS_ERR_NONE) {
      break;
    } else {
      OSTimeDlyHMSM(0, 0, 0, 200, OS_OPT_TIME_HMSM_NON_STRICT, &err_local);
    }
  }

  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  ts = SNTPc_GetRemoteTime(&pkt, &err);                         // Retrieve current time.
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  ok = Clk_SetTS_NTP(ts.Sec);
  APP_RTOS_ASSERT_DBG((ok == DEF_OK),; );

  ok = Clk_GetTS_NTP(&ts_sec_ntp_from_clk);
  APP_RTOS_ASSERT_DBG((ok == DEF_OK),; );

  ts_sec = Clk_GetTS();
  ok = Clk_TS_ToDateTime(ts_sec, 0, &date_time_from_convert_ts);
  APP_RTOS_ASSERT_DBG((ok == DEF_OK),; );

  ok = Clk_TS_NTP_ToDateTime(ts.Sec, 0, &date_time_from_convert_ntp);
  APP_RTOS_ASSERT_DBG((ok == DEF_OK),; );

  ok = Clk_GetDateTime(&date_time);
  APP_RTOS_ASSERT_DBG((ok == DEF_OK),; );

  EX_TRACE("ts.sec = %u | ts_sec_ntp_from_clk = %u\r\n", ts.Sec, ts_sec_ntp_from_clk);

  ok = Clk_DateTimeToStr(&date_time, CLK_STR_FMT_DAY_MONTH_DD_HH_MM_SS_YYYY, &clk_str_buf[0u], CLK_STR_FMT_DAY_MONTH_DD_HH_MM_SS_YYYY_LEN);
  APP_RTOS_ASSERT_DBG((ok == DEF_OK),; );

  EX_TRACE("Clk: We are %s.\r\n", (CPU_CHAR *)&clk_str_buf);
}

#endif // RTOS_MODULE_COMMON_CLK_AVAIL
