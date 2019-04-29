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
#include "em_chip.h"
#include "em_qspi.h"
#include "em_prs.h"
#include "qspi.h"
#include "mx25r3235f.h"
#include "mx25flash_config.h"
#include "retargetserial.h"
#include "em_cmu.h"

#include <stdio.h>
#include <stdlib.h>

#define MX25_SECTOR_SIZE_WORDS  (MX25_SECTOR_SIZE / 4)
#define MX25_FLASH_SIZE_WORDS   (MX25_FLASH_SIZE / 4)

static uint32_t pattern[32000];
static uint32_t output[32000];

/***************************************************************************//**
 * @brief Hard Fault Handler
 ******************************************************************************/
void HardFault_Handler(void)
{
  printf("HardFault\n");
  while (true)
    ;
}

/***************************************************************************//**
 * @brief Print info to virtual COM port regarding transfer tests
 ******************************************************************************/
static void printInfo(uint32_t cycles, int bytes)
{
  float sec = (float)cycles / CMU_ClockFreqGet(cmuClock_CORE);
  float kbytes = bytes / 1024.0;
  float speed = kbytes / sec;

  printf("transferred %d bytes (%0.1f kB)\n", bytes, kbytes);
  printf("total time %0.4f s\n", sec);
  printf("speed %0.1f kB/s\n", speed);
  printf("-----------------------------------------------\n");
}

/***************************************************************************//**
 * @brief Read out the whole flash and measure the transfer speed.
 ******************************************************************************/
static void testReadFlash(void)
{
  int block = MX25_SECTOR_SIZE;
  uint32_t cycles;

  printf("Starting read flash test\n");
  DWT->CYCCNT = 0;
  for (int addr = 0; addr < MX25_FLASH_SIZE; addr += block) {
    QSPI_ReadIndirect(output, addr, block);
    QSPI_Wait();
  }
  cycles = DWT->CYCCNT;
  printInfo(cycles, MX25_FLASH_SIZE);
}

/***************************************************************************//**
 * @brief Write a pattern to the flash and read back to verify.
 ******************************************************************************/
static void testWriteFlash(void)
{
  int block = MX25_SECTOR_SIZE;
  bool ok = true;
  uint32_t cycles;

  printf("Starting write flash test\n");
  for (int i = 0; i < (block / 4); i++) {
    pattern[i] = i;
  }

  printf("Writing pattern to flash\n");
  DWT->CYCCNT = 0;
  for (int addr = 0; addr < MX25_FLASH_SIZE; addr += block) {
    QSPI_WriteIndirect(pattern, addr, block);
    printf(".");
    QSPI_Wait();
  }
  cycles = DWT->CYCCNT;
  printf("\n");
  printInfo(cycles, MX25_FLASH_SIZE);

  for (int addr = 0; addr < MX25_FLASH_SIZE; addr += block) {
    QSPI_ReadIndirect(output, addr, block);
    QSPI_Wait();
    for (int i = 0; i < (block / 4); i++) {
      if (pattern[i] != output[i]) {
        ok = false;
      }
    }
  }

  if (ok) {
    printf("Write flash test OK! \n");
  } else {
    printf("Write flash test FAIL! \n");
  }
}

/***************************************************************************//**
 * @brief Set up QSPI with PHY.
 ******************************************************************************/
static void setupQspi(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_QSPI0, true);

  // Make sure QSPI module is idle
  while (!(QSPI0->CONFIG & _QSPI_CONFIG_IDLE_MASK)) {
  }

  // Clear configuration register (disable QSPI)
  QSPI0->CONFIG = 0;

  // Config device read, address and data is transferred over 4 I/O lines
  QSPI_ReadConfig_TypeDef readConfig = QSPI_READCONFIG_DEFAULT;

  readConfig.dummyCycles  = 6;
  readConfig.opCode       = 0xEB;
  readConfig.instTransfer = qspiTransferSingle;
  readConfig.addrTransfer = qspiTransferQuad;
  readConfig.dataTransfer = qspiTransferQuad;

  QSPI_ReadConfig(QSPI0, &readConfig);

  // Config device write, address and data is transferred over 4 I/O lines
  QSPI_WriteConfig_TypeDef writeConfig = QSPI_WRITECONFIG_DEFAULT;

  writeConfig.dummyCycles              = 0;
  writeConfig.opCode                   = 0x38;
  writeConfig.addrTransfer             = qspiTransferQuad;
  writeConfig.dataTransfer             = qspiTransferQuad;
  writeConfig.autoWEL                  = true;

  QSPI_WriteConfig(QSPI0, &writeConfig);

  // Device size config
  QSPI0->DEVSIZECONFIG = 0x10 << _QSPI_DEVSIZECONFIG_BYTESPERSUBSECTOR_SHIFT;
  QSPI0->DEVSIZECONFIG |= (0x0100 << _QSPI_DEVSIZECONFIG_BYTESPERDEVICEPAGE_SHIFT);
  QSPI0->DEVSIZECONFIG |= (2 << _QSPI_DEVSIZECONFIG_NUMADDRBYTES_SHIFT); // 3 bytes adress (24 bit)

  // Device delay config
  // mx25 requires at least 30 ns deselect time
  QSPI0->DEVDELAY = 2 << _QSPI_DEVDELAY_DNSS_SHIFT;

  // Write protection registers
  QSPI0->WRPROTCTRL = 0; //disable writeprotection

  // Clear interrupt mask register
  QSPI0->IRQMASK = 0;

  // Route pins
  QSPI0->ROUTELOC0 = MX25_QSPI_LOC;
  QSPI0->ROUTEPEN  = (QSPI_ROUTEPEN_SCLKPEN
                      | MX25_QSPI_CSPEN
                      | QSPI_ROUTEPEN_DQ0PEN
                      | QSPI_ROUTEPEN_DQ1PEN
                      | QSPI_ROUTEPEN_DQ2PEN
                      | QSPI_ROUTEPEN_DQ3PEN);

  QSPI0->CONFIG |= MX25_QSPI_CSNUM << _QSPI_CONFIG_PERIPHCSLINES_SHIFT; //CS0

  // Set up output pins
  GPIO_PinModeSet(MX25_PORT_CS, MX25_PIN_CS, gpioModePushPull, 0);
  GPIO_PinModeSet(MX25_PORT_SCLK, MX25_PIN_SCLK, gpioModePushPull, 0);
  GPIO_PinModeSet(MX25_PORT_DQ0, MX25_PIN_DQ0, gpioModePushPull, 0);
  GPIO_PinModeSet(MX25_PORT_DQ1, MX25_PIN_DQ1, gpioModePushPull, 0);
  GPIO_PinModeSet(MX25_PORT_DQ2, MX25_PIN_DQ2, gpioModePushPull, 0);
  GPIO_PinModeSet(MX25_PORT_DQ3, MX25_PIN_DQ3, gpioModePushPull, 0);
  GPIO_PinModeSet(MX25_PORT_PWR_EN, MX25_PIN_PWR_EN, gpioModePushPull, 1);

  // Set up drive strength and slew rate on output port
  GPIO_SlewrateSet(MX25_PORT_SCLK, 6, 6);
  GPIO_DriveStrengthSet(MX25_PORT_SCLK, gpioDriveStrengthStrongAlternateStrong);

  // Enable PHY
  QSPI0->CONFIG |= QSPI_CONFIG_PHYMODEENABLE;

  // Set values for the DLL (Delayed Lock-Loop) used for timing in PHY mode
  // TX and RX DLL values copied from datasheet
  QSPI0->PHYCONFIGURATION = (79 << _QSPI_PHYCONFIGURATION_PHYCONFIGRXDLLDELAY_SHIFT)
                            | (37 << _QSPI_PHYCONFIGURATION_PHYCONFIGTXDLLDELAY_SHIFT);
  QSPI0->PHYCONFIGURATION |= (QSPI_PHYCONFIGURATION_PHYCONFIGRESYNC);

  // Select 32 MHz AUXHRCO as the QSPI reference clock
  CMU_AUXHFRCOBandSet(cmuAUXHFRCOFreq_32M0Hz);
  CMU_ClockSelectSet(cmuClock_QSPI0REF, cmuSelect_AUXHFRCO);

  NVIC_EnableIRQ(QSPI0_IRQn);

  // Enable QSPI and indirect access
  QSPI0->CONFIG = (QSPI0->CONFIG & ~_QSPI_CONFIG_ENBDIRACCCTLR_MASK)
                  | QSPI_CONFIG_ENBSPI;
}

/***************************************************************************//**
 * @brief Using PRS to route lines to the EXP Header.
 ******************************************************************************/
void setupDebugOutput(void)
{
  // PRS mapping:
  // CS0  EXP 8
  // SCLK EXP 6
  // DQ0  EXP 4
  // DQ1  EXP 11
  // DQ2  EXP 13
  // DQ3  EXP 14

  CMU_ClockEnable(cmuClock_PRS, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  GPIO_PinModeSet(gpioPortE, 12, gpioModePushPull, 0);  // CS0
  GPIO_PinModeSet(gpioPortE, 11, gpioModePushPull, 0);  // SCLK
  GPIO_PinModeSet(gpioPortE, 10, gpioModePushPull, 0);  // DQ0
  GPIO_PinModeSet(gpioPortB, 11, gpioModePushPull, 0);  // DQ1
  GPIO_PinModeSet(gpioPortB, 9, gpioModePushPull, 0);   // DQ2
  GPIO_PinModeSet(gpioPortE, 9, gpioModePushPull, 0);   // DQ3

  GPIO_IntConfig(gpioPortG, 0, true, true, true); // SCLK
  GPIO_IntConfig(gpioPortG, 1, true, true, true); // DQ0
  GPIO_IntConfig(gpioPortG, 2, true, true, true); // DQ1
  GPIO_IntConfig(gpioPortG, 3, true, true, true); // DQ2
  GPIO_IntConfig(gpioPortG, 4, true, true, true); // DQ3
  GPIO_IntConfig(gpioPortG, 9, true, true, true); // CS0

  PRS_SourceSignalSet(1,
                      PRS_CH_CTRL_SOURCESEL_GPIOH,
                      PRS_CH_CTRL_SIGSEL_GPIOPIN9,
                      prsEdgeOff
                      );  //CS0
  PRS_SourceSignalSet(3,
                      PRS_CH_CTRL_SOURCESEL_GPIOL,
                      PRS_CH_CTRL_SIGSEL_GPIOPIN0,
                      prsEdgeOff
                      );  //CLK
  PRS_SourceSignalSet(2,
                      PRS_CH_CTRL_SOURCESEL_GPIOL,
                      PRS_CH_CTRL_SIGSEL_GPIOPIN1,
                      prsEdgeOff
                      );  //DQ0
  PRS_SourceSignalSet(21,
                      PRS_CH_CTRL_SOURCESEL_GPIOL,
                      PRS_CH_CTRL_SIGSEL_GPIOPIN2,
                      prsEdgeOff
                      );  //DQ1
  PRS_SourceSignalSet(13,
                      PRS_CH_CTRL_SOURCESEL_GPIOH,
                      PRS_CH_CTRL_SIGSEL_GPIOPIN9,
                      prsEdgeOff
                      );  //DQ2
  PRS_SourceSignalSet(8,
                      PRS_CH_CTRL_SOURCESEL_GPIOL,
                      PRS_CH_CTRL_SIGSEL_GPIOPIN4,
                      prsEdgeOff
                      );  //DQ3

  PRS->ROUTELOC0 |= PRS_ROUTELOC0_CH1LOC_LOC3
                    | PRS_ROUTELOC0_CH2LOC_LOC2
                    | PRS_ROUTELOC0_CH3LOC_LOC2; //CLK, DQ0, CS
  PRS->ROUTELOC5 |= PRS_ROUTELOC5_CH21LOC_LOC2;  //DQ1
  PRS->ROUTELOC3 |= PRS_ROUTELOC3_CH13LOC_LOC1;  //DQ2
  PRS->ROUTELOC2 |= PRS_ROUTELOC2_CH8LOC_LOC2;   //DQ3

  PRS->ROUTEPEN |= PRS_ROUTEPEN_CH1PEN           //CS
                   | PRS_ROUTEPEN_CH2PEN         //DQ0
                   | PRS_ROUTEPEN_CH3PEN         //CLK
                   | PRS_ROUTEPEN_CH21PEN        //DQ1
                   | PRS_ROUTEPEN_CH13PEN        //DQ2
                   | PRS_ROUTEPEN_CH8PEN;        //DQ3
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  float coreFreq = 0;
  float qspiFreq = 0;

  // Chip errata
  CHIP_Init();
  // Set core frequency to max
  CMU_HFRCOBandSet(cmuHFRCOFreq_72M0Hz);

  // Seeding random generator
  srand(0x839FC60);

  // Initialize USART and map LF to CRLF
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

  printf("\n-----------------------------------------------\n");
  printf("|       QSPI Indirect Access                  |\n");
  printf("-----------------------------------------------\n");

  // Set up cycle counter to measure time
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= 1;

  // Set up debug to EXP
  setupDebugOutput();
  // Set up QSPI and PHY
  setupQspi();

  coreFreq = CMU_ClockFreqGet(cmuClock_CORE);
  qspiFreq = CMU_ClockFreqGet(cmuClock_QSPI0REF);
  printf("Running Core clock at %0.1f MHz \n", coreFreq  / 1000000.0f);
  printf("Running QSPI clock at %0.1f MHz in PHY mode \n", qspiFreq  / 1000000.0f);

  // Initialize flash and setup enable QSPI mode
  printf("Initializing MX25 Flash\n");
  MX25_Init();
  QSPI_InitIndirect();
  printf("Erasing entire flash (ca. 50 sec)...\n");
  MX25_ChipErase();
  testWriteFlash();
  testReadFlash();
  while (true)
    ;
}
