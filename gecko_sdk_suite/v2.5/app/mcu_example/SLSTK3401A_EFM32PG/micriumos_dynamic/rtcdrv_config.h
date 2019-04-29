/***************************************************************************//**
 * @file
 * @brief RTCDRV configuration file.
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
#ifndef RTCDRV_CONFIG_H
#define RTCDRV_CONFIG_H

// Define how many timers RTCDRV will provide.
#define EMDRV_RTCDRV_NUM_TIMERS     (1)

// Uncomment the following line to include the wallclock functionality.
#define EMDRV_RTCDRV_WALLCLOCK_CONFIG

// Uncomment the following line to enable integration with SLEEP driver.
//#define EMDRV_RTCDRV_SLEEPDRV_INTEGRATION

#endif /* RTCDRV_CONFIG_H */
