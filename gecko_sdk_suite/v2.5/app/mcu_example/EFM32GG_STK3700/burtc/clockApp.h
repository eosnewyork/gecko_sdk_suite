/***************************************************************************//**
 * @file
 * @brief CALENDAR header file
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

#ifndef __CLOCKAPP_H
#define __CLOCKAPP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Function prototypes */
void clockAppInit(void);
void clockAppDisplay(void);
void clockAppBackup(void);
void clockAppRestore(uint32_t);
void clockAppUpdate(void);
void clockAppPrintWakeupStatus(uint32_t);
void clockAppPrintRamWErr(void);
void clockAppPrintNoTimestamp(void);
void clockAppPrintResetCause(uint32_t);
void clockAppOverflow(void);

void gpioIrqInit(void);

#ifdef __cplusplus
}
#endif

#endif
