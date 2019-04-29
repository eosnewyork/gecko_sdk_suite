/***************************************************************************//**
 * @file
 * @brief Selftest command for the shell.
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
#include <stdio.h>

#include "cmd_declarations.h"
#include "em_device.h"
#include "shell_util.h"

// -----------------------------------------------------------------------------
// Local defines

#define IRQBUFLEN  128
#define VTOROFFSET  16 // Offset number of VTOR vector

// -----------------------------------------------------------------------------
// Local global variables

// Buffer used to print index and address of unknown IRQ
static char irqNameBuf[IRQBUFLEN];

static const char *irqNames[] = {
  [EMU_IRQn] = "EMU_IRQn",
  [WDOG0_IRQn] = "WDOG0_IRQn",
  [LDMA_IRQn] = "LDMA_IRQn",
  [GPIO_EVEN_IRQn] = "GPIO_EVEN_IRQn",
  [SMU_IRQn] = "SMU_IRQn",
  [TIMER0_IRQn] = "TIMER0_IRQn",
  [USART0_RX_IRQn] = "USART0_RX_IRQn",
  [USART0_TX_IRQn] = "USART0_TX_IRQn",
  [ACMP0_IRQn] = "ACMP0_IRQn",
  [ADC0_IRQn] = "ADC0_IRQn",
  [IDAC0_IRQn] = "IDAC0_IRQn",
  [I2C0_IRQn] = "I2C0_IRQn",
  [I2C1_IRQn] = "I2C1_IRQn",
  [GPIO_ODD_IRQn] = "GPIO_ODD_IRQn",
  [TIMER1_IRQn] = "TIMER1_IRQn",
  [TIMER2_IRQn] = "TIMER2_IRQn",
  [TIMER3_IRQn] = "TIMER3_IRQn",
  [USART1_RX_IRQn] = "USART1_RX_IRQn",
  [USART1_TX_IRQn] = "USART1_TX_IRQn",
  [USART2_RX_IRQn] = "USART2_RX_IRQn",
  [USART2_TX_IRQn] = "USART2_TX_IRQn",
  [UART0_RX_IRQn] = "UART0_RX_IRQn",
  [UART0_TX_IRQn] = "UART0_TX_IRQn",
  [UART1_RX_IRQn] = "UART1_RX_IRQn",
  [UART1_TX_IRQn] = "UART1_TX_IRQn",
  [LEUART0_IRQn] = "LEUART0_IRQn",
  [LEUART1_IRQn] = "LEUART1_IRQn",
  [LETIMER0_IRQn] = "LETIMER0_IRQn",
  [PCNT0_IRQn] = "PCNT0_IRQn",
  [PCNT1_IRQn] = "PCNT1_IRQn",
  [PCNT2_IRQn] = "PCNT2_IRQn",
  [RTCC_IRQn] = "RTCC_IRQn",
  [CMU_IRQn] = "CMU_IRQn",
  [MSC_IRQn] = "MSC_IRQn",
  [CRYPTO0_IRQn] = "CRYPTO0_IRQn",
  [CRYOTIMER_IRQn] = "CRYOTIMER_IRQn",
  [FPUEH_IRQn] = "FPUEH_IRQn",
  [USART3_RX_IRQn] = "USART3_RX_IRQn",
  [USART3_TX_IRQn] = "USART3_TX_IRQn",
  [USART4_RX_IRQn] = "USART4_RX_IRQn",
  [USART4_TX_IRQn] = "USART4_TX_IRQn",
  [WTIMER0_IRQn] = "WTIMER0_IRQn",
  [WTIMER1_IRQn] = "WTIMER1_IRQn",
  [WTIMER2_IRQn] = "WTIMER2_IRQn",
  [WTIMER3_IRQn] = "WTIMER3_IRQn",
  [I2C2_IRQn] = "I2C2_IRQn",
  [VDAC0_IRQn] = "VDAC0_IRQn",
  [TIMER4_IRQn] = "TIMER4_IRQn",
  [TIMER5_IRQn] = "TIMER5_IRQn",
  [TIMER6_IRQn] = "TIMER6_IRQn",
  [USART5_RX_IRQn] = "USART5_RX_IRQn",
  [USART5_TX_IRQn] = "USART5_TX_IRQn",
  [CSEN_IRQn] = "CSEN_IRQn",
  [LESENSE_IRQn] = "LESENSE_IRQn",
  [EBI_IRQn] = "EBI_IRQn",
  [ACMP2_IRQn] = "ACMP2_IRQn",
  [ADC1_IRQn] = "ADC1_IRQn",
  [LCD_IRQn] = "LCD_IRQn",
  [SDIO_IRQn] = "SDIO_IRQn",
  [ETH_IRQn] = "ETH_IRQn",
  [CAN0_IRQn] = "CAN0_IRQn",
  [CAN1_IRQn] = "CAN1_IRQn",
  [USB_IRQn] = "USB_IRQn",
  [RTC_IRQn] = "RTC_IRQn",
  [WDOG1_IRQn] = "WDOG1_IRQn",
  [LETIMER1_IRQn] = "LETIMER1_IRQn",
  [TRNG0_IRQn] = "TRNG0_IRQn",
  [QSPI0_IRQn] = "QSPI0_IRQn",
};

// -----------------------------------------------------------------------------
// Local function prototype

static const char* getIrqName(IRQn_Type type);


// -----------------------------------------------------------------------------
// Global functions

/***************************************************************************//**
 * @brief
 *   Test executed at bootup. May also be executed on command.
 *
 * @param argc
 *   Number of arguments
 *
 * @param argv
 *   Array of pointers to strings which are the arguments.
 *
 * @param out_fnct
 *   Not used.
 *
 * @param pcmd_param
 *   Not used.
 *
 * @return
 *   SHELL_EXEC_ERR on error, SHELL_EXEC_ERR_NONE otherwise.
 ******************************************************************************/
CPU_INT16S selftestCmd(CPU_INT16U argc,
                       CPU_CHAR *argv[],
                       SHELL_OUT_FNCT outFunc,
                       SHELL_CMD_PARAM *cmdParam)
{
  int errorCounter = 0;
  int i;

  (void)cmdParam; // Deliberately unused arguments

  for (i = 1; i < argc; i++) {
    if (!Str_Cmp(argv[i], "-help")) {
      shellPrint(outFunc, "selftest - Checks if interrupt vector table is within program memory.\n");
      return SHELL_EXEC_ERR_NONE;
    } else {
      shellPrintf(outFunc, "selftestCmd: Invalid argument '%s'.\n", argv[i]);
      return SHELL_EXEC_ERR;
    }
  }

  // Are all interrupt pointers within program memory?
  for (i = VTOROFFSET; i < EXT_IRQ_COUNT-2 + VTOROFFSET; i++) {
    if ( (uint32_t)(((uint32_t*)SCB->VTOR)[i] - FLASH_BASE) > FLASH_SIZE ) {
      shellPrintf(outFunc, "IRQ %s out of program memory\n", getIrqName((IRQn_Type)(i - VTOROFFSET)));
      errorCounter++;
    }
  }

  if (errorCounter == 0) {
    shellPrint(outFunc, "All the interrupt pointers are within the program memory\n");
  }

  return SHELL_EXEC_ERR_NONE;
}


// -----------------------------------------------------------------------------
// Local functions

/***************************************************************************//**
 * @brief
 *   Converts IRQn_Type to String.
 *
 * @param type
 *   Interrupt number.
 *
 * @return
 *   String containing name of IRQ.
 ******************************************************************************/
static const char* getIrqName(IRQn_Type type)
{
  if (type < EMU_IRQn || QSPI0_IRQn < type) { // Check if within known IRQ range
    Str_FmtPrint(irqNameBuf,
                 IRQBUFLEN,
                 "Unknown index=%d address=%x",
                 type,
                 (unsigned int)((uint32_t *)SCB->VTOR)[type]);
    return irqNameBuf;
  }

  return irqNames[type];
}
