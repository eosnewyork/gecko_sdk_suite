/***************************************************************************//**
 * @file
 * @brief micrium_httpcloader example
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

#include  <bsp_os.h>
#include  <stdio.h>

#include  "platform_time.h"
#include  "bsp.h"

#include  <rtos_cfg.h>

#include  <cpu/include/cpu.h>
#include  <config-ssl-httpcloader.h>
#include  <common/include/common.h>
#include  <kernel/include/os.h>
#include  <kernel/include/os_trace.h>

#include  <common/include/auth.h>
#include  <common/include/lib_def.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/toolchains.h>
#include  <common/include/rtos_err.h>
#include  <common/include/clk.h>

#include  <net/include/dhcp_client_types.h>
#include  <net/include/sntp_client.h>

#include  "description.h"
#include  "network.h"
#include  "httpclient.h"
#include  "retargetserial.h"
#include  "em_cmu.h"
#include  "em_emu.h"
#include  "em_chip.h"
#include  "btlapi.h"

// -----------------------------------------------------------------------------
// Defines

#define  MAIN_START_TASK_PRIO              21u

// -----------------------------------------------------------------------------
// Local variables

/* Start Task Stack.                                    */
static  CPU_STK  MainStartTaskStk[MAIN_START_TASK_STK_SIZE];
/* Start Task TCB.                                      */
static  OS_TCB   MainStartTaskTCB;
/* Time elapsed since 1970.01.01                        */
static  time_t   timeElapsed;

// -----------------------------------------------------------------------------
// Local function prototypes

static  void  mainStartTask (void  *p_arg);
static  void  getElapsedTime (void);

// -----------------------------------------------------------------------------
// Global functions

/*
 *********************************************************************************************************
 *                                             mbedtls_time()
 *
 * Description : This global function is used to estimate current time.
 *               This function is necessary for checking the certification expire date of CAs.
 *
 * Note(s)     : Current time has been calculated by using SNTP protocol.
 *********************************************************************************************************
 */
mbedtls_time_t mbedtls_time(mbedtls_time_t* time)
{
  (void)time;
  return timeElapsed;
}

/*
 *********************************************************************************************************
 *                                                main()
 *
 * Description : This is the standard entry point for C applications. It is assumed that your code will
 *               call main() once you have performed all necessary initialization.
 *
 * Argument(s) : None.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *********************************************************************************************************
 */
int  main(void)
{
  RTOS_ERR  err;

  BSP_SystemInit();                                             /* Initialize System.                                   */
  CPU_Init();                                                   /* Initialize CPU.                                      */

  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

  OS_TRACE_INIT();
  OSInit(&err);                                                 /* Initialize the Kernel.                               */
                                                                /*   Check error code.                                  */
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

  OSTaskCreate(&MainStartTaskTCB,                            /* Create the Start Task.                               */
               "Ex Main Start Task",
               mainStartTask,
               DEF_NULL,
               MAIN_START_TASK_PRIO,
               &MainStartTaskStk[0],
               (MAIN_START_TASK_STK_SIZE / 10u),
               MAIN_START_TASK_STK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  /*   Check error code.                                  */
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

  OSStart(&err);                                                /* Start the kernel.                                    */
                                                                /*   Check error code.                                  */
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

  return (1);
}

// -----------------------------------------------------------------------------
// Local functions

/*
 *********************************************************************************************************
 *                                          mainStartTask()
 *
 * Description : This is the task that will be called by the Startup when all services are initializes
 *               successfully.
 *
 * Argument(s) : p_arg   Argument passed from task creation. Unused, in this case.
 *
 * Return(s)   : None.
 *
 * Notes       : None.
 *********************************************************************************************************
 */
static  void  mainStartTask(void  *p_arg)
{
  RTOS_ERR  err;

  PP_UNUSED_PARAM(p_arg);                                       /* Prevent compiler warning.                            */

  BSP_TickInit();                                               /* Initialize Kernel tick source.                       */

#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)
  OSStatTaskCPUUsageInit(&err);                                 /* Initialize CPU Usage.                                */
                                                                /*   Check error code.                                  */
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE),; );
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
  CPU_IntDisMeasMaxCurReset();                                  /* Initialize interrupts disabled measurement.          */
#endif

  Common_Init(&err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

#ifdef  RTOS_MODULE_COMMON_CLK_AVAIL
  Clk_Init(&err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
#endif

  Auth_Init(&err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
                                                                /* ------------- INITIALIZE CLIENT SUITE -------------- */
  SNTPc_Init(&err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

  BSP_OS_Init();                                                /* Initialize the BSP. It is expected that the BSP ...  */
                                                                /* ... will register all the hardware controller to ... */
                                                                /* ... the platform manager at this moment.             */

  printf("\nMicriumOS HTTPc Loader Example\n");

  BTLAPI_Init();
  Net_CoreInit();                                               /* -------------- INITIALIZE CORE MODULE -------------- */
  Net_CoreStartIF();                                            /* Call network interface start example.                */
  getElapsedTime();
  HTTP_Client_Init();                                           /* ---------- INITIALIZE HTTP CLIENT MODULE ----------- */

  while (DEF_ON) {
    BSP_LedToggle(0);
    HTTP_Client_Get();                                          /* Send HTTP Get Request                                */
    BTLAPI_validateAndReinstall();
  }
}

/*
 *********************************************************************************************************
 *                                          getElapsedTime()
 *
 * Description : This is the function used to get seconds passed since 1970.01.01.
 *
 * Argument(s) : None.
 *
 * Return(s)   : None.
 *
 * Note(s)     : (1) The timestamp returned by SNTPc_GetRemoteTime() represents the quantity of seconds
 *                   since January 1, 1900. It also gives the fractions of second.
 *********************************************************************************************************
 */
static  void  getElapsedTime (void)
{
    SNTP_PKT    pkt;
    SNTP_TS     ts;
    RTOS_ERR    err;

    SNTPc_ReqRemoteTime("0.pool.ntp.org",                       /* Send SNTP request.                                   */
                        &pkt,
                        &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

    ts = SNTPc_GetRemoteTime(&pkt, &err);                       /* Retrieve current time.                               */
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

                                                                /* ts.Sec represents qty of seconds since Jan 1, 1900.  */
    timeElapsed  = ts.Sec-2208984820;                           /* Seconds passed since 1970  */
}
