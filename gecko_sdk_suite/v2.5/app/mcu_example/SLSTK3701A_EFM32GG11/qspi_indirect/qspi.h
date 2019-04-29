/***************************************************************************//**
 * @file
 * @brief QSPI indirect example
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

#ifndef QSPI_H
#define QSPI_H

#include <stdint.h>
#include <stddef.h>

void QSPI_InitIndirect(void);
int QSPI_WriteIndirect(const void * buffer, uint32_t addr, size_t n);
int QSPI_ReadIndirect(void * buffer, uint32_t addr, size_t n);
void QSPI_WaitRead(int id);
void QSPI_WaitWrite(int id);
void QSPI_Wait(void);

#endif
