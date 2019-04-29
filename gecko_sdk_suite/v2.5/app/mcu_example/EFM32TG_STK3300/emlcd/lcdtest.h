/***************************************************************************//**
 * @file
 * @brief Energy Mode LCD demo and test, prototypes and definitions
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

#ifndef __LCDTEST_H
#define __LCDTEST_H

#ifdef __cplusplus
extern "C" {
#endif

/* Test patterns */
void ScrollText(char *scrolltext);
void BlinkTest(void);
void AnimateTest(void);

/* Utility functions */
void RtcTrigger(RTCDRV_TimerID_t id, void *user);
void EM1Sleep(uint32_t msec);
void EM2Sleep(uint32_t msec);
void EM3Sleep(void);
void EM4Sleep(void);

/* Main test iteration routine */
void Test(void);

#ifdef __cplusplus
}
#endif

#endif
