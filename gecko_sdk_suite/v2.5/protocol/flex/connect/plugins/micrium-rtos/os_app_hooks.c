/***************************************************************************//**
 * @brief
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

#define   MICRIUM_SOURCE
#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "stack/include/ember.h"
#include "micrium-rtos/micrium-rtos-support.h"
#include "serial/serial.h"
#include <kernel/include/os.h>
#include <em_emu.h>

void  App_OS_IdleTaskHook (void);
#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
void  App_OS_RedzoneHitHook (OS_TCB  *p_tcb);
#endif
void  App_OS_StatTaskHook (void);
void  App_OS_TaskCreateHook (OS_TCB  *p_tcb);
void  App_OS_TaskDelHook (OS_TCB  *p_tcb);
void  App_OS_TaskReturnHook (OS_TCB  *p_tcb);
void  App_OS_TaskSwHook (void);
void  App_OS_TimeTickHook (void);

/*
 ************************************************************************************************************************
 *                                              SET ALL APPLICATION HOOKS
 *
 * Description: Set ALL application hooks.
 *
 * Arguments  : none.
 *
 * Note(s)    : none
 ************************************************************************************************************************
 */
void  App_OS_SetAllHooks(void)
{
#if OS_CFG_APP_HOOKS_EN > 0u
  CPU_SR_ALLOC();

  CPU_CRITICAL_ENTER();
  OS_AppIdleTaskHookPtr   = App_OS_IdleTaskHook;

#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
  OS_AppRedzoneHitHookPtr = App_OS_RedzoneHitHook;
#endif

  OS_AppStatTaskHookPtr   = App_OS_StatTaskHook;

  OS_AppTaskCreateHookPtr = App_OS_TaskCreateHook;

  OS_AppTaskDelHookPtr    = App_OS_TaskDelHook;

  OS_AppTaskReturnHookPtr = App_OS_TaskReturnHook;

  OS_AppTaskSwHookPtr     = App_OS_TaskSwHook;

  OS_AppTimeTickHookPtr   = App_OS_TimeTickHook;
  CPU_CRITICAL_EXIT();
#endif
}

/*
 ************************************************************************************************************************
 *                                             CLEAR ALL APPLICATION HOOKS
 *
 * Description: Clear ALL application hooks.
 *
 * Arguments  : none.
 *
 * Note(s)    : none
 ************************************************************************************************************************
 */
void  App_OS_ClrAllHooks(void)
{
#if OS_CFG_APP_HOOKS_EN > 0u
  CPU_SR_ALLOC();

  CPU_CRITICAL_ENTER();
  OS_AppIdleTaskHookPtr   = (OS_APP_HOOK_VOID)0;

#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
  OS_AppRedzoneHitHookPtr = (OS_APP_HOOK_TCB)0;
#endif

  OS_AppStatTaskHookPtr   = (OS_APP_HOOK_VOID)0;

  OS_AppTaskCreateHookPtr = (OS_APP_HOOK_TCB)0;

  OS_AppTaskDelHookPtr    = (OS_APP_HOOK_TCB)0;

  OS_AppTaskReturnHookPtr = (OS_APP_HOOK_TCB)0;

  OS_AppTaskSwHookPtr     = (OS_APP_HOOK_VOID)0;

  OS_AppTimeTickHookPtr   = (OS_APP_HOOK_VOID)0;
  CPU_CRITICAL_EXIT();
#endif
}

/*
 ************************************************************************************************************************
 *                                              APPLICATION IDLE TASK HOOK
 *
 * Description: This function is called by the idle task.  This hook has been added to allow you to do such things as
 *              STOP the CPU to conserve power.
 *
 * Arguments  : none
 *
 * Note(s)    : none
 ************************************************************************************************************************
 */
void  App_OS_IdleTaskHook(void)
{
#ifndef DISABLE_WATCHDOG
  halResetWatchdog();
#endif

#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_SLEEP_ENABLE)
  emAfPluginMicriumRtosSleepHandler();
#endif
}

/*
 ************************************************************************************************************************
 *                                             APPLICATION REDZONE HIT HOOK
 *
 * Description: This function is called when a task's stack overflowed.
 *
 * Arguments  : p_tcb   is a pointer to the task control block of the offending task. NULL if ISR.
 *
 * Note(s)    : None.
 ************************************************************************************************************************
 */
#if (OS_CFG_TASK_STK_REDZONE_EN == DEF_ENABLED)
void  App_OS_RedzoneHitHook(OS_TCB  *p_tcb)
{
  while (DEF_ON) {
#ifndef DISABLE_WATCHDOG
    halResetWatchdog();
#endif

#if (OS_CFG_DBG_EN == DEF_ENABLED)
    emberSerialGuaranteedPrintf(APP_SERIAL, "stack overflow: %s\n",
                                ((p_tcb) ? p_tcb->NamePtr : "isr"));
#else
    emberSerialGuaranteedPrintf(APP_SERIAL, "stack overflow: p_tcb=%4x\n", p_tcb);
#endif
  }
}
#endif

/*
 ************************************************************************************************************************
 *                                           APPLICATION STATISTIC TASK HOOK
 *
 * Description: This function is called every second by uC/OS-III's statistics task.  This allows your application to add
 *              functionality to the statistics task.
 *
 * Arguments  : none
 *
 * Note(s)    : none
 ************************************************************************************************************************
 */
void  App_OS_StatTaskHook(void)
{
}

/*
 ************************************************************************************************************************
 *                                            APPLICATION TASK CREATION HOOK
 *
 * Description: This function is called when a task is created.
 *
 * Arguments  : p_tcb   is a pointer to the task control block of the task being created.
 *
 * Note(s)    : none
 ************************************************************************************************************************
 */
void  App_OS_TaskCreateHook(OS_TCB  *p_tcb)
{
  (void)&p_tcb;
}

/*
 ************************************************************************************************************************
 *                                            APPLICATION TASK DELETION HOOK
 *
 * Description: This function is called when a task is deleted.
 *
 * Arguments  : p_tcb   is a pointer to the task control block of the task being deleted.
 *
 * Note(s)    : none
 ************************************************************************************************************************
 */
void  App_OS_TaskDelHook(OS_TCB  *p_tcb)
{
  (void)&p_tcb;
}

/*
 ************************************************************************************************************************
 *                                             APPLICATION TASK RETURN HOOK
 *
 * Description: This function is called if a task accidentally returns.  In other words, a task should either be an
 *              infinite loop or delete itself when done.
 *
 * Arguments  : p_tcb     is a pointer to the OS_TCB of the task that is returning.
 *
 * Note(s)    : none
 ************************************************************************************************************************
 */
void  App_OS_TaskReturnHook(OS_TCB  *p_tcb)
{
  (void)&p_tcb;
}

/*
 ************************************************************************************************************************
 *                                             APPLICATION TASK SWITCH HOOK
 *
 * Description: This function is called when a task switch is performed.  This allows you to perform other operations
 *              during a context switch.
 *
 * Arguments  : none
 *
 * Note(s)    : 1) Interrupts are disabled during this call.
 *              2) It is assumed that the global pointer 'OSTCBHighRdyPtr' points to the TCB of the task that will be
 *                 'switched in' (i.e. the highest priority task) and, 'OSTCBCurPtr' points to the task being switched out
 *                 (i.e. the preempted task).
 ************************************************************************************************************************
 */
void  App_OS_TaskSwHook(void)
{
}

/*
 ************************************************************************************************************************
 *                                                APPLICATION TICK HOOK
 *
 * Description: This function is called every tick.
 *
 * Arguments  : none
 *
 * Note(s)    : 1) This function is assumed to be called from the Tick ISR.
 ************************************************************************************************************************
 */
void  App_OS_TimeTickHook(void)
{
}
