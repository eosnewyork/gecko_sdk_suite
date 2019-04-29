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

// select timers to use for 100us timestamp
#define HRM_TIMER1      TIMER2
#define HRM_TIMER2      TIMER3
#define CLK_HRM_TIMER1  cmuClock_TIMER2
#define CLK_HRM_TIMER2  cmuClock_TIMER3

#define enableGreenLED() do {       \
    GPIO_PinOutClear(gpioPortC, 3); \
    GPIO_PinOutSet(gpioPortE, 2);   \
    GPIO_PinOutSet(gpioPortE, 3);   \
} while (0)

#define disableGreenLED() do {    \
    GPIO_PinOutSet(gpioPortC, 3); \
} while (0)

#define enableRedLED() do {         \
    GPIO_PinOutClear(gpioPortC, 0); \
    GPIO_PinOutSet(gpioPortE, 2);   \
    GPIO_PinOutSet(gpioPortE, 3);   \
} while (0)

#define disableRedLED() do {        \
    GPIO_PinOutSet(gpioPortC, 0);   \
    GPIO_PinOutClear(gpioPortE, 2); \
    GPIO_PinOutClear(gpioPortE, 3); \
} while (0)

#endif /* __HEARTRATEMONITOR_CFG_H */
