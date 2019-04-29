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
#include "mx25r3235f.h"
#include "em_qspi.h"

/***************************************************************************//**
 * @brief Set write enable latch
 ******************************************************************************/
void MX25_WREN(void)
{
  QSPI_StigCmd_TypeDef stigCmd = { 0 };
  stigCmd.cmdOpcode = MX25_CMD_WREN;
  QSPI_ExecStigCmd(QSPI0, &stigCmd);
}

/***************************************************************************//**
 * @brief Read identification tag
 ******************************************************************************/
uint32_t MX25_RDID(void)
{
  uint8_t buffer[3];
  QSPI_StigCmd_TypeDef stigCmd = { 0 };
  stigCmd.cmdOpcode = MX25_CMD_RDID;
  stigCmd.readDataSize = 3;
  stigCmd.readBuffer = &buffer;
  QSPI_ExecStigCmd(QSPI0, &stigCmd);
  return (buffer[0] << 16) | (buffer[1] << 8) | buffer[2];
}

/***************************************************************************//**
 * @brief Read status register
 ******************************************************************************/
uint8_t MX25_RDSR(void)
{
  uint8_t status;
  QSPI_StigCmd_TypeDef stigCmd = { 0 };
  stigCmd.cmdOpcode = MX25_CMD_RDSR;
  stigCmd.readDataSize = 1;
  stigCmd.readBuffer = &status;
  QSPI_ExecStigCmd(QSPI0, &stigCmd);
  return status;
}

/***************************************************************************//**
 * @brief Write status register (and the two MX25 FLASH config registers if needed)
 ******************************************************************************/
void MX25_WRSR(uint8_t status, uint8_t conf1, uint8_t conf2)
{
  uint8_t bytes[] = { status, conf1, conf2 };
  QSPI_StigCmd_TypeDef stigCmd = { 0 };
  stigCmd.cmdOpcode = MX25_CMD_WRSR;
  stigCmd.writeDataSize = 1;
  stigCmd.writeBuffer = &bytes;
  QSPI_ExecStigCmd(QSPI0, &stigCmd);
}

/***************************************************************************//**
 * @brief Sets write enable latch and waits for it to verify
 ******************************************************************************/
void MX25_WriteEnable(void)
{
  MX25_WREN();
  uint32_t status = MX25_RDSR();
  while (!(status & MX25_STATUS_WEL)) {
    status = MX25_RDSR();
  }
}

/***************************************************************************//**
 * @brief Wait for write in progres to complete
 ******************************************************************************/
void MX25_WriteComplete(void)
{
  uint32_t status = MX25_RDSR();

  while (status & MX25_STATUS_WIP) {
    status = MX25_RDSR();
  }
}

/***************************************************************************//**
 * @brief Erase a sector of the flash
 ******************************************************************************/
void MX25_SectorErase(uint32_t sector)
{
  QSPI_StigCmd_TypeDef stigCmd = { 0 };
  stigCmd.cmdOpcode = MX25_CMD_SE;
  stigCmd.addrSize = 3;
  stigCmd.address = sector;

  MX25_WriteEnable();
  QSPI_ExecStigCmd(QSPI0, &stigCmd);
  MX25_WriteComplete();
}

/***************************************************************************//**
 * @brief Enable quad mode and high performance mode
 ******************************************************************************/
void MX25_SetQuadMode(void)
{
  uint8_t status;

  MX25_WREN();
  status = MX25_RDSR();
  while (!(status & MX25_STATUS_WEL)) {
    status = MX25_RDSR();
  }

  MX25_WRSR(status | MX25_STATUS_QE, 0, MX25_CONFIG2_LH);

  status = MX25_RDSR();
  while (!(status & MX25_STATUS_QE)) {
    status = MX25_RDSR();
  }
}

/***************************************************************************//**
 * @brief Wait for ready device and set to quad mode
 ******************************************************************************/
void MX25_Init(void)
{
  uint32_t id = 0;
  // Check ID to make sure device is responding
  while (((id & 0xFF0000) >> 16) != MX25_MANUFACTURER_ID) {
    id = MX25_RDID();
  }

  // Switch to Quad mode on the MX25
  MX25_SetQuadMode();
}

/***************************************************************************//**
 * @brief. Issue a chip-wide erase command,
 * the flash will be unresponsive for around 1 min.
 ******************************************************************************/
void MX25_ChipErase(void)
{
  QSPI_StigCmd_TypeDef stigCmd = { 0 };
  stigCmd.cmdOpcode = 0x60;
  stigCmd.addrSize = 0;
  stigCmd.address = 0x000000;

  MX25_WriteEnable();
  QSPI_ExecStigCmd(QSPI0, &stigCmd);
  MX25_WriteComplete();
}
