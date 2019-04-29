/***************************************************************************//**
 * @file
 * @brief QSPI direct access example with PHY enabled for EFM32GG11
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
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_timer.h"
#include "em_qspi.h"
#include "em_gpio.h"
#include "em_prs.h"
#include "em_ldma.h"
#include "bsp.h"
#include "retargetserial.h"
#include "mx25r3235f.h"
#include "mx25flash_config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE              (256 * 1024) // 256 kB
#define MAX_DMA_TRANSFER                8192  // 2048 transfers of 4 bytes

static uint32_t buffer[BUFFER_SIZE / 4];
static LDMA_Descriptor_t desc;

/***************************************************************************//**
 * @brief Simple LDMA interrupt handler. Clear flags when channel is done.
 ******************************************************************************/
void LDMA_IRQHandler(void)
{
  uint32_t pending = LDMA_IntGetEnabled();
  LDMA_IntClear(pending);
}

/***************************************************************************//**
 * @brief Test the speed of random access single byte read from external flash.
 ******************************************************************************/
uint32_t testRead8(void)
{
  uint32_t i = 0;
  uint32_t addr = 0;
  uint8_t byte = 0;
  uint32_t timeStamp = 0;
  uint32_t time = 0;
  bool ok = true;
  volatile uint8_t * qspiMem = (volatile uint8_t *)QSPI0_MEM_BASE;

  for (i = 0; i < MX25_FLASH_SIZE; i++) {
    addr = rand() % MX25_FLASH_SIZE; // Random addres
    DWT->CYCCNT = 0; //Start timing
    byte = qspiMem[addr]; // Read takes usually 30~70 cycles
    timeStamp = DWT->CYCCNT; // Record time spent

    // Accumulate time
    time = time + timeStamp - 1;

    // Check if content is correct
    if (byte != (addr & 0x000000ff) ) {
      ok = false;
    }
  }

  if (!ok) {
    printf("Test Failed!\n");
  }
  return time;
}

/***************************************************************************//**
 * @brief Test the speed of random access 4 byte read from external flash.
 ******************************************************************************/
uint32_t testRead32(void)
{
  uint32_t i = 0;
  uint32_t j = 0;
  uint32_t addr = 0;
  uint32_t word = 0;
  uint32_t check = 0;
  uint32_t timeStamp = 0;
  uint32_t time = 0;
  bool ok = true;
  volatile uint32_t * qspiMem = (volatile uint32_t *)QSPI0_MEM_BASE;

  for (i = 0; i < (MX25_FLASH_SIZE / 4); i++) {
    addr = (rand() % (MX25_FLASH_SIZE / 4)); // Random addres
    DWT->CYCCNT = 0; //Start timing
    word = qspiMem[addr]; // Read takes usually 30~70 cycles
    timeStamp = DWT->CYCCNT; // Record time spent

    // Accumulate time
    time = time + timeStamp - 1;

    // Check if content is correct
    j = ((addr * 4) & 0x000000ff);
    check = ((j + 3) << 24) + ((j + 2) << 16) + ((j + 1) << 8) + (j);
    if (word != check) {
      ok = false;
    }
  }

  if (!ok) {
    printf("Test Failed!\n");
  }
  return time;
}

/***************************************************************************//**
 * @brief Test the speed of reading a block of data from external flash.
 ******************************************************************************/
uint32_t testReadBlock(uint32_t *dst, uint32_t len)
{
  uint32_t i = 0;
  uint32_t j = 0;
  uint32_t word = 0;
  uint32_t check = 0;
  uint32_t time = 0;
  bool ok = true;
  volatile uint32_t * qspiMem = (volatile uint32_t *)QSPI0_MEM_BASE;

  memset(dst, 0, len);
  DWT->CYCCNT = 0;
  for (i = 0; i < (len / 4); i++) {
    dst[i] = qspiMem[i];
  }
  time = DWT->CYCCNT;

  // Verify content of the block
  for (i = 0; i < (len / 4); i++) {
    word = dst[i];
    j = ((i * 4) & 0x000000ff);
    check = ((j + 3) << 24) + ((j + 2) << 16) + ((j + 1) << 8) + (j);
    if (word != check) {
      ok = false;
    }
  }

  if (!ok) {
    printf("Test Failed!\n");
  }
  return time;
}

/***************************************************************************//**
 * @brief Test the speed of reading a block of data using DMA from external flash.
 ******************************************************************************/
uint32_t testReadBlockDma(uint32_t *dst, uint32_t len)
{
  uint32_t i = 0;
  uint32_t j = 0;
  uint32_t word = 0;
  uint32_t check = 0;
  uint32_t time = 0;
  bool ok = true;
  volatile uint32_t * qspiMem = (volatile uint32_t *)QSPI0_MEM_BASE;
  int ch = 1;
  bool done = false;

  // Initialize LDMA
  CMU_ClockEnable(cmuClock_LDMA, true);
  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init(&init);

  // Configure LDMA for memory transfer
  LDMA_TransferCfg_t memoryTransfer = LDMA_TRANSFER_CFG_MEMORY();
  desc = (LDMA_Descriptor_t) LDMA_DESCRIPTOR_SINGLE_M2M_WORD(&qspiMem[0], dst, (len / 4));

  memset(dst, 0, len);
  DWT->CYCCNT = 0;
  LDMA_StartTransfer(ch, &memoryTransfer, &desc);
  while (!done) {
    done = LDMA_TransferDone(ch);
  }
  time = DWT->CYCCNT;

  // Verify content of the block
  for (i = 0; i < (len / 4); i++) {
    word = dst[i];
    j = ((i * 4) & 0x000000ff);
    check = ((j + 3) << 24) + ((j + 2) << 16) + ((j + 1) << 8) + (j);
    if (word != check) {
      ok = false;
    }
  }

  if (!ok) {
    printf("Test Failed!\n");
  }
  return time;
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

  // Interrupt mask register
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

  // Enable QSPI and DAC in config
  QSPI0->CONFIG |= (QSPI_CONFIG_PHYMODEENABLE | QSPI_CONFIG_ENBSPI
                    | QSPI_CONFIG_ENBDIRACCCTLR);
}

/***************************************************************************//**
 * @brief Using PRS to route QSPI lines to the EXP Header.
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

  GPIO_IntConfig(gpioPortG, 0, true, true, true);       // SCLK
  GPIO_IntConfig(gpioPortG, 1, true, true, true);       // DQ0
  GPIO_IntConfig(gpioPortG, 2, true, true, true);       // DQ1
  GPIO_IntConfig(gpioPortG, 3, true, true, true);       // DQ2
  GPIO_IntConfig(gpioPortG, 4, true, true, true);       // DQ3
  GPIO_IntConfig(gpioPortG, 9, true, true, true);       // CS0

  PRS_SourceSignalSet(1,
                      PRS_CH_CTRL_SOURCESEL_GPIOH,
                      PRS_CH_CTRL_SIGSEL_GPIOPIN9,
                      prsEdgeOff
                      );  // CS0
  PRS_SourceSignalSet(3,
                      PRS_CH_CTRL_SOURCESEL_GPIOL,
                      PRS_CH_CTRL_SIGSEL_GPIOPIN0,
                      prsEdgeOff
                      );  // CLK
  PRS_SourceSignalSet(2,
                      PRS_CH_CTRL_SOURCESEL_GPIOL,
                      PRS_CH_CTRL_SIGSEL_GPIOPIN1,
                      prsEdgeOff
                      );  // DQ0
  PRS_SourceSignalSet(21,
                      PRS_CH_CTRL_SOURCESEL_GPIOL,
                      PRS_CH_CTRL_SIGSEL_GPIOPIN2,
                      prsEdgeOff
                      );  // DQ1
  PRS_SourceSignalSet(13,
                      PRS_CH_CTRL_SOURCESEL_GPIOH,
                      PRS_CH_CTRL_SIGSEL_GPIOPIN9,
                      prsEdgeOff
                      );  // DQ2
  PRS_SourceSignalSet(8,
                      PRS_CH_CTRL_SOURCESEL_GPIOL,
                      PRS_CH_CTRL_SIGSEL_GPIOPIN4,
                      prsEdgeOff
                      );  // DQ3

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

/***************************************************************************//**
 * @brief Fill the whole flash with a pattern
 ******************************************************************************/
static void writePattern(void)
{
  volatile uint8_t * qspimem = (volatile uint8_t *) QSPI0_MEM_BASE;
  uint8_t value = 0;
  uint32_t addr = 0;

  for (addr = 0; addr < MX25_FLASH_SIZE; addr++) {
    qspimem[addr] = value;
    value++;
  }
}

/***************************************************************************//**
 * @brief Print info to virtual COM port regarding transfer tests
 ******************************************************************************/
static void printInfo(float sec, int bytes)
{
  float kbytes = (float)bytes / 1024.0f;
  float speed = (kbytes / sec);

  printf("read %d bytes (%0.1f kB)\n", bytes, kbytes);
  printf("total time = %0.4f s\n", sec);
  printf("speed %0.1f kB/s\n", speed);
  printf("-----------------------------------------------\n");
}

/**************************************************************************//**
 * @brief Setup QSPI and external flash and start read tests
 *****************************************************************************/
int main(void)
{
  uint32_t cycles = 0;
  float time = 0;
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
  printf("|       QSPI Direct Access Demo with PHY      |\n");
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
  printf("Erasing and writing may take several minutes\n");
  printf("Erasing entire flash (ca. 50 sec)...\n");
  MX25_ChipErase();
  printf("Writing pattern to entire flash (ca. 60 sec)...\n");
  writePattern();
  printf("Starting read tests:\n");
  printf("-----------------------------------------------\n");

  while (true) {
    // Random access using 8 bit reads
    printf("Running test with 8 bit random access reads:\n");
    cycles = testRead8();
    time = cycles / coreFreq;
    printInfo(time, MX25_FLASH_SIZE);

    // Random access using 32 bit reads
    printf("Running test with 32 bit random access reads:\n");
    cycles = testRead32();
    time = cycles / coreFreq;
    printInfo(time, MX25_FLASH_SIZE);

    // Sequential read of 256 kB using 32 bit reads
    printf("Running test with 32 bit sequential reads:\n");
    cycles = testReadBlock(buffer, sizeof(buffer));
    time = cycles / coreFreq;
    printInfo(time, sizeof(buffer));

    // Sequential dma read of 8 kB
    printf("Running test with DMA read operation:\n");
    cycles = testReadBlockDma(buffer, MAX_DMA_TRANSFER);
    time = cycles / coreFreq;
    printInfo(time, MAX_DMA_TRANSFER);
  }
}
