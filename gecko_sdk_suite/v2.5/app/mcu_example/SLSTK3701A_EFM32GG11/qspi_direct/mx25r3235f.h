/***************************************************************************//**
 * @file
 * @brief Functions for the MX25 flash
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
#ifndef MX25R3235F_H
#define MX25R3235F_H

#include <stdint.h>

#define MX25_MANUFACTURER_ID 0xC2

#define MX25_CMD_WRSR   0x01
#define MX25_CMD_RDSR   0x05
#define MX25_CMD_WREN   0x06
#define MX25_CMD_SE     0x20
#define MX25_CMD_RDID   0x9F

#define MX25_STATUS_SRWD (1 << 7)
#define MX25_STATUS_QE   (1 << 6)
#define MX25_STATUS_WEL  (1 << 1)
#define MX25_STATUS_WIP  (1 << 0)

#define MX25_CONFIG2_LH (1 << 1) // Config register no2, (set for high performance)

#define MX25_PAGE_SIZE   256
#define MX25_FLASH_SIZE  0x400000
#define MX25_LBLOCK_SIZE 0x10000
#define MX25_SBLOCK_SIZE 0x8000
#define MX25_SECTOR_SIZE 0x1000

void MX25_WREN(void);
uint32_t MX25_RDID(void);
uint8_t MX25_RDSR(void);
void MX25_WRSR(uint8_t status, uint8_t conf1, uint8_t conf2);
void MX25_WriteEnable(void);
void MX25_WriteComplete(void);
void MX25_SectorErase(uint32_t sector);
void MX25_SetQuadMode(void);
void MX25_Init(void);
void MX25_ChipErase(void);

#endif
