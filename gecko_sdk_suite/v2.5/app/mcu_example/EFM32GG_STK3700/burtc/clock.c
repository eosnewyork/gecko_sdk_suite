/***************************************************************************//**
 * @file
 * @brief System CLOCK for EFM32 Backup Power Domain Application Note
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

#include <time.h>
#include "em_device.h"
#include "clock.h"

/* Clock Defines */
static time_t rtcStartTime = 0;

/* Time defines */
static uint32_t rtcOverflowCounter = 0;
static uint32_t overflow_interval;
static uint32_t overflow_interval_r;

/* This variables must reflect BURTC frequency */
#define COUNTS_PER_SEC (32768 / 128)

/******************************************************************************
* @brief Returns the current system time
*
* @param timer
*   If not a null pointer, time is copied to this
*
* @return
*   Current system time. Should, but does not, return -1 if system time is not available
*
******************************************************************************/
#if defined (__ICCARM__)
time_t __time32(time_t * timer)
#elif defined (__CC_ARM)
time_t time(time_t * timer)
#elif defined (__GNUC__)
time_t time(time_t * timer)
#else
#error Undefined toolkit, need to define alignment
#endif
{
  time_t t;

  /* Add the time offset */
  t = rtcStartTime;

  /* Add time based on number of counter overflows*/
  t += rtcOverflowCounter * overflow_interval;

  /* Correct if overflow interval is not an integer*/
  if ( overflow_interval_r != 0 ) {
    t += rtcOverflowCounter * overflow_interval_r / COUNTS_PER_SEC;
  }

  /* Add the number of seconds for BURTC */
  t += (BURTC->CNT / COUNTS_PER_SEC);

  /* Copy system time to timer if not NULL*/
  if ( !timer ) {
    timer = &t;
  }

  return t;
}

/***************************************************************************//**
 * @brief Set the epoch offset
 *
 * @param[in] timeptr
 *   Calendar struct which is converted to unix time and used as new epoch
 *   offset
 *
 ******************************************************************************/
void clockSetCal(struct tm * timeptr)
{
  rtcStartTime = mktime(timeptr);
}

/***************************************************************************//**
 * @brief Set the epoch offset
 *
 * @param[in] offset
 *   unix time when the counter was started
 *
 ******************************************************************************/
void clockSetStartTime(time_t offset)
{
  rtcStartTime = offset;
}

/***************************************************************************//**
 * @brief Get the epoch offset
 *
 * @return
 *   unix time when the counter was started
 *
 ******************************************************************************/
time_t clockGetStartTime(void)
{
  return rtcStartTime;
}

/***************************************************************************//**
 * @brief Initialize system CLOCK
 *
 * @param[in] timeptr
 *   Calendar struct that is used to set the start time of the counter.
 *
 ******************************************************************************/
void clockInit(struct tm * timeptr)
{
  /* Reset variables */
  rtcOverflowCounter = 0;

  /* Set overflow interval based on counter width and frequency */
  overflow_interval  =  ((uint64_t)UINT32_MAX + 1) / COUNTS_PER_SEC; /* in seconds */
  overflow_interval_r = ((uint64_t)UINT32_MAX + 1) % COUNTS_PER_SEC; /* division remainder */

  /* Set epoch offset */
  clockSetCal(timeptr);
}

/***************************************************************************//**
 * @brief Call this function on counter overflow to let CLOCK know how many
 *        overflows has occured since start time
 *
 ******************************************************************************/
uint32_t clockOverflow(void)
{
  rtcOverflowCounter++;
  return rtcOverflowCounter;
}

/***************************************************************************//**
 * @brief Call this function on counter overflow to let CLOCK know how many
 *        overflows has occured since start time
 *
 ******************************************************************************/
void clockSetOverflowCounter(uint32_t of)
{
  rtcOverflowCounter = of;
}

/***************************************************************************//**
 * @brief Call this function on counter overflow to let CLOCK know how many
 *        overflows has occured since start time
 *
 ******************************************************************************/
uint32_t clockGetOverflowCounter(void)
{
  return rtcOverflowCounter;
}
