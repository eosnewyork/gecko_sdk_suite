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

#include <bsp_os.h>
#include "bsp.h"

#include <cpu/include/cpu.h>
#include <common/include/common.h>
#include <kernel/include/os.h>
#include <kernel/include/os_trace.h>

#include <common/include/lib_def.h>
#include <common/include/rtos_utils.h>
#include <common/include/toolchains.h>

#include <canopen/include/canopen_dict.h>
#include <canopen/include/canopen_obj.h>
#include <canopen/include/canopen_types.h>

#include "ex_canopen.h"
#include "retargetserial.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_chip.h"
#include "glib.h"
#include "display.h"
#include "gpiointerrupt.h"

#include <stdio.h>
#include <string.h>

/*
*********************************************************************************************************
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

#define EX_MAIN_START_TASK_PRIO              21u
#define EX_MAIN_START_TASK_STK_SIZE         512u

#define GLIB_FONT_WIDTH           (glibContext.font.fontWidth \
                                   + glibContext.font.charSpacing)
#define GLIB_FONT_HEIGHT          (glibContext.font.fontHeight)

#define MAX_STR_LEN               48



CANOPEN_NODE_HANDLE Ex_CANopen_Node1Handle;
CANOPEN_NODE_HANDLE Ex_CANopen_Node2Handle;

/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/

// Start Task Stack.
static CPU_STK  Ex_MainStartTaskStk[EX_MAIN_START_TASK_STK_SIZE];
// Start Task TCB.
static OS_TCB   Ex_MainStartTaskTCB;

// BTN0 interrupt flag
static volatile bool btn0Pressed = false;
// BTN1 interrupt flag
static volatile bool btn1Pressed = false;

// GLIB_Context
static GLIB_Context_t glibContext;

// String to be shown on the display
static char strOnDisplay[MAX_STR_LEN];

/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

static void Ex_MainStartTask (void  *p_arg);


/*
*********************************************************************************************************
*********************************************************************************************************
*                                          GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/***************************************************************************//**
 *  @brief
 *    Draw text function using GLIB example.
 *
 *  @param text
 *    Pointer to the string that is drawn
 *
 *  @param x0
 *    Start x-coordinate for the string (Upper left corner)
 *
 *  @param y0
 *    Start y-coordinate for the string (Upper left corner)
 ******************************************************************************/
static void writeTextOnTheDisplay(const char *text, int32_t x0, int32_t y0)
{
  // Print text on the display
  GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNormal8x8);
  GLIB_drawString(&glibContext,
                  text,
                  strlen(text),
                  x0,
                  y0,
                  0);
}


/***************************************************************************//**
 * @brief  Show CAN actions on the display
 *
 * @param	node1val	Value retrieved from node 1
 *
 * @param	node2val 	Value retrieved from node 2
 ******************************************************************************/
static void actionDisplay(CPU_INT32U node1val,
		                  CPU_INT32U node2val)
{
  // Clear screen
  GLIB_clear(&glibContext);

  snprintf(strOnDisplay, MAX_STR_LEN, "CANopen DEMO");
  writeTextOnTheDisplay(strOnDisplay, 5, 5);
  snprintf(strOnDisplay, MAX_STR_LEN, "Press a button\n"
  		                              "to send PDO msg\n"
  		                              "can0 -> can1");
  writeTextOnTheDisplay(strOnDisplay, 5, 5 + (2 * GLIB_FONT_HEIGHT));
  snprintf(strOnDisplay, MAX_STR_LEN, "can0 = %X", node1val);
  writeTextOnTheDisplay(strOnDisplay, 5, 5 + (7 * GLIB_FONT_HEIGHT));
  snprintf(strOnDisplay, MAX_STR_LEN, "can1 = %X", node2val);
  writeTextOnTheDisplay(strOnDisplay, 5, 5 + (9 * GLIB_FONT_HEIGHT));

  // Update display
  DMD_updateDisplay();
}

void btn_pressed_irq(uint8_t pin)
{
  GPIO_IntClear(0x1 << pin);
  if (pin == BSP_GPIO_PB0_PIN)
    btn0Pressed = true;
  else
    btn1Pressed = true;
}

/***************************************************************************//**
 *  @brief
 *    This is the standard entry point for C applications. It is assumed that
 *    your code will call main() once you have performed all necessary
 *    initialization.
 ******************************************************************************/

int  main (void)
{
    RTOS_ERR  err;


    BSP_SystemInit();                                           /* Initialize System.                                   */
    CPU_Init();                                                 /* Initialize CPU.                                      */
    RETARGET_SerialInit();
    RETARGET_SerialCrLf(1);

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


/***************************************************************************//**
 *  @brief
 *    This is the task that will be called by the Startup when all
 *    services are initializes successfully.
 *
 *  @param p_arg Argument passed from task creation. Unused, in this case.
 ******************************************************************************/

static  void  Ex_MainStartTask (void  *p_arg)
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

    //Ex_CommonInit();                                            /* Call common module initialization example.           */
    Common_Init(&err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

    BSP_OS_Init();                                              /* Initialize the BSP. It is expected that the BSP ...  */
                                                                /* ... will register all the hardware controller to ... */
                                                                /* ... the platform manager at this moment.             */

    Ex_CANopen_Init();                                          /* Call CANopen module initialization example.          */
    Ex_CANopen_NodeStart();                                     /* Call CANopen module interface start example.         */

    // Glib Init
    DMD_init(0);
    GLIB_contextInit(&glibContext);
    glibContext.backgroundColor = White;
    glibContext.foregroundColor = Black;
    GLIB_clear(&glibContext);

    printf("Micrium OS CANopen Example\n");

    CMU_ClockEnable(cmuClock_GPIO, true);
    GPIOINT_Init();
    GPIO_PinModeSet(gpioPortC, 8u, gpioModeInput, 0u);
    GPIO_PinModeSet(gpioPortC, 9u, gpioModeInput, 0u);
    GPIO_ExtIntConfig(gpioPortC, BSP_GPIO_PB0_PIN, BSP_GPIO_PB0_PIN, false, true, true);
    GPIO_ExtIntConfig(gpioPortC, BSP_GPIO_PB1_PIN, BSP_GPIO_PB1_PIN, false, true, true);

    GPIOINT_CallbackRegister(BSP_GPIO_PB0_PIN, btn_pressed_irq);
    GPIOINT_CallbackRegister(BSP_GPIO_PB1_PIN, btn_pressed_irq);


    															/* Read Object Dictionary value on each of the CANopen  */
    															/* nodes.												*/
    actionDisplay(Ex_CANopen_DictRd(1), Ex_CANopen_DictRd(2));

    while (DEF_ON) {
        if (btn0Pressed) {
            BSP_LedToggle(0);
        	CANopen_DictLongWr(Ex_CANopen_Node1Handle,
                               CANOPEN_DEV(0x2007, 0x00),
							   0xFEDCBA98,
                               &err);
        	btn0Pressed = false;
        }
        if (btn1Pressed) {
            BSP_LedToggle(0);
            CANopen_DictLongWr(Ex_CANopen_Node1Handle,
                               CANOPEN_DEV(0x2007, 0x00),
							   0x12345679,
                               &err);
        	btn1Pressed = false;
        }

        actionDisplay(Ex_CANopen_DictRd(1), Ex_CANopen_DictRd(2));

                                                                /* Delay Start Task execution for                       */
        OSTimeDly(100,                                          /*   100 OS Ticks                                       */
                  OS_OPT_TIME_DLY,                              /*   from now.                                          */
                  &err);
                                                                /*   Check error code.                                  */
        APP_RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), ;);
    }
}
