/***************************************************************************//**
 * @file
 * @brief Preamp example project definitions.
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

#ifndef PREAMP_H
#define PREAMP_H

#define USE_GUI           /* Use emWin library to make nice gui. */
#define BALANCE_MAX       10
#define BALANCE_CENTER    5
#define TONE_MAX          10
#define TONE_CENTER       5

extern int volume, balance, bass, treble;
extern const float volumeTable[];

#ifdef USE_GUI
extern const float volumeTable_dB[];
extern const float balanceTable_dB[];
#endif

#endif
