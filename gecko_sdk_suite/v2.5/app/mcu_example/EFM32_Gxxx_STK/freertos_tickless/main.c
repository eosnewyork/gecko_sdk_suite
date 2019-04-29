/***************************************************************************//**
 * @file
 * @brief FreeRTOS Tickless Demo for Energy Micro EFM32-Gxxx-STK Development Kit
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

#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "croutine.h"

#include "em_chip.h"
#include "bsp_trace.h"
#include "segmentlcd.h"

#include "sleep.h"

#define STACK_SIZE_FOR_TASK    (configMINIMAL_STACK_SIZE + 10)
#define TASK_PRIORITY          (tskIDLE_PRIORITY + 1)

/* Semaphores used by application to synchronize two tasks */
xSemaphoreHandle sem;
/* Text to display */
char text[8];
/* Counter start value*/
int count = 0;

/***************************************************************************//**
 * @brief LcdPrint task which is showing numbers on the display
 * @param *pParameters pointer to parameters passed to the function
 ******************************************************************************/
static void LcdPrint(void *pParameters)
{
  (void) pParameters;   /* to quiet warnings */

  for (;; ) {
    /* Wait for semaphore, then display next number */
    if (pdTRUE == xSemaphoreTake(sem, portMAX_DELAY)) {
      SegmentLCD_Write(text);
    }
  }
}

/***************************************************************************//**
 * @brief Count task which is preparing next number to display
 * @param *pParameters pointer to parameters passed to the function
 ******************************************************************************/
static void Count(void *pParameters)
{
  (void) pParameters;   /* to quiet warnings */

  for (;; ) {
    count = (count + 1) & 0xF;
    text[0] = count >= 10 ? '1' : '0';
    text[1] = count % 10 + '0';
    text[2] = '\0';
    xSemaphoreGive(sem);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* If first word of user data page is non-zero, enable Energy Profiler trace */
  BSP_TraceProfilerSetup();

  /* Initialize SLEEP driver, no calbacks are used */
  SLEEP_Init(NULL, NULL);
#if (configSLEEP_MODE < 3)
  /* do not let to sleep deeper than define */
  SLEEP_SleepBlockBegin((SLEEP_EnergyMode_t)(configSLEEP_MODE + 1));
#endif

  /* Initialize the LCD driver */
  SegmentLCD_Init(false);

  /* Create standard binary semaphore */
  vSemaphoreCreateBinary(sem);

  /* Create two task to show numbers from 0 to 15 */
  xTaskCreate(Count, (const char *) "Count", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
  xTaskCreate(LcdPrint, (const char *) "LcdPrint", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);

  /* Start FreeRTOS Scheduler */
  vTaskStartScheduler();

  return 0;
}
