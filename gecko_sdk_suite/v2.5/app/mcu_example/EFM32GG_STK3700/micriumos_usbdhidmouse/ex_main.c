/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/*
*********************************************************************************************************
*
*                                             EXAMPLE MAIN
*
* File : ex_main.c
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <bsp_os.h>
#include  "bsp.h"

#include  <cpu/include/cpu.h>
#include  <kernel/include/os.h>
#include  <kernel/include/os_trace.h>
#include  <common/include/common.h>
#include  <common/include/lib_def.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/toolchains.h>

#include  "ex_description.h"

#include  "common/common/ex_common.h"

#include  "usb/device/ex_usbd.h"

#include "retargetserial.h"
#include  <stdio.h>

/*
*********************************************************************************************************
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

#define  EX_MAIN_START_TASK_PRIO              21u
#define  EX_MAIN_START_TASK_STK_SIZE         512u


/*
*********************************************************************************************************
*********************************************************************************************************
*                                        LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/

                                                                /* Start Task Stack.                                    */
static  CPU_STK  Ex_MainStartTaskStk[EX_MAIN_START_TASK_STK_SIZE];
                                                                /* Start Task TCB.                                      */
static  OS_TCB   Ex_MainStartTaskTCB;


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

static  void  Ex_MainStartTask (void  *p_arg);


/*
*********************************************************************************************************
*********************************************************************************************************
*                                          GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

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

int main(void)
{
  RTOS_ERR  err;

  BSP_SystemInit();                                           /* Initialize System.                                   */
  CPU_Init();                                                 /* Initialize CPU.                                      */

  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);
  printf("\n\n*** MicriumOS USB HID mouse example. ***\n");

  OS_TRACE_INIT();
  OSInit(&err);                                               /* Initialize the Kernel.                               */
                                                              /*   Check error code.                                  */
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

  OSTaskCreate(&Ex_MainStartTaskTCB,                          /* Create the Start Task.                               */
               "Ex Main Start Task",
                Ex_MainStartTask,
                DEF_NULL,
                EX_MAIN_START_TASK_PRIO,
               &Ex_MainStartTaskStk[0],
               (EX_MAIN_START_TASK_STK_SIZE / 10u),
                EX_MAIN_START_TASK_STK_SIZE,
                0u,
                0u,
                DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
                                                              /*   Check error code.                                  */
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

  OSStart(&err);                                              /* Start the kernel.                                    */
                                                              /*   Check error code.                                  */
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), 1);

  return (1);
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          Ex_MainStartTask()
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

static void Ex_MainStartTask(void *p_arg)
{
  RTOS_ERR  err;

  PP_UNUSED_PARAM(p_arg);                                     /* Prevent compiler warning.                            */

  BSP_TickInit();                                             /* Initialize Kernel tick source.                       */

#if (OS_CFG_STAT_TASK_EN == DEF_ENABLED)
  OSStatTaskCPUUsageInit(&err);                               /* Initialize CPU Usage.                                */
                                                              /*   Check error code.                                  */
  APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
  CPU_IntDisMeasMaxCurReset();                                /* Initialize interrupts disabled measurement.          */
#endif

  Ex_CommonInit();                                            /* Call common module initialization example.           */

  BSP_OS_Init();                                              /* Initialize the BSP. It is expected that the BSP ...  */
                                                              /* ... will register all the hardware controller to ... */
                                                              /* ... the platform manager at this moment.             */

  Ex_USBD_Init();                                             /* Call USB device module initialization example.       */
  Ex_USBD_Start();                                            /* Call USB device controller start.                    */

  while (DEF_ON) {
    BSP_LedToggle(0);
                                                              /* Delay Start Task execution for                       */
    OSTimeDly(1000,                                           /*   1000 OS Ticks                                      */
              OS_OPT_TIME_DLY,                                /*   from now.                                          */
             &err);
                                                              /*   Check error code.                                  */
    APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
  }
}
