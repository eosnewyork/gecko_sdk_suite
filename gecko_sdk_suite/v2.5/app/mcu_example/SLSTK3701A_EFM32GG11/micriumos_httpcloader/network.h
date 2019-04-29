/***************************************************************************//**
 * @file
 * @brief Network core initialization
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

#ifndef  NETWORK_H
#define  NETWORK_H

void  Net_CoreInit       (void);

void  Net_CoreStartIF    (void);

void  Net_CoreStartEther (void);

void  Net_CoreStartWiFi  (void);

#endif  /* NETWORK_H */
