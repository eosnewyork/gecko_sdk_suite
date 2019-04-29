/***************************************************************************//**
 * @file
 * @brief Gecko Bootloader interface API
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

#ifndef BTLAPI_H
#define BTLAPI_H

#include <inttypes.h>

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

void BTLAPI_Init(void);
void BTLAPI_validateAndReinstall(void);
void BTLAPI_reprogramBtlStorage(uint8_t *data, uint16_t dataLen);

#endif /* BTLAPI_H */
