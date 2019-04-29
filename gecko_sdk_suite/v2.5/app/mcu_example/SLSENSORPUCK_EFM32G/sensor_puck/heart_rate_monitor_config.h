/***************************************************************************//**
 * @file
 * @brief Heart Rate peripheral config
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

#ifndef __HEARTRATEMONITOR_CFG_H
#define __HEARTRATEMONITOR_CFG_H

#define HRM_INCLUDE_SI1147PS

// select timers to use for 100us timestamp
#define HRM_TIMER1 TIMER0
#define HRM_TIMER2 TIMER1
#define CLK_HRM_TIMER1  cmuClock_TIMER0
#define CLK_HRM_TIMER2  cmuClock_TIMER1

#endif /* __HEARTRATEMONITOR_CFG_H */
