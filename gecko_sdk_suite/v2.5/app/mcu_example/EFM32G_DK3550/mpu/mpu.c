/***************************************************************************//**
 * @file
 * @brief MPU example for EFM32G_DK3550
 *        Connect a terminal application with baudrate 9600-8-N-1
 *        on the RS232 port of the kit to run the demo.
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
#include <inttypes.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_emu.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "retargetserial.h"

/***************************************************************************//**
 *
 *  This example project demonstrate use of the memory protection unit (MPU).
 *  The cpu will normally run in Priviledged state, but upon reception
 *  of a lowercase 'x' on the UART it will switch into User state and
 *  intentionally generate two access violation exceptions.
 *
 *  The first violation is by accessing the UART, the second by accessing
 *  internal SRAM.
 *  To be able to execute in User state at all, User state access must be
 *  granted to flash and SRAM memory. We use two MPU regions for this.
 *  Additionally one MPU section is used to deny User state access to a small
 *  part of SRAM. This section overlaps the section used to grant general
 *  general SRAM access, but it will take precedence because its section number
 *  is higher.
 *  A fourth MPU section is used to deny User state access to UART registers.
 *
 *  Entry into User state is performed by writing to the CONTROL cpu core
 *  register. Before exiting the memory protection fault handler the cpu
 *  is switched back to Priviledged state. (It is impossible to switch from
 *  User state to Priviledged state in Thread mode).
 *
 ******************************************************************************/

// This table contains 4 MPU region definitions.
static const ARM_MPU_Region_t mpuTable[] = {
  // *INDENT-OFF*
  //            RegionNo    BaseAddr
  //            DisableExec AccessPermission TypeExtField IsShareable IsCacheable IsBufferable SubRegionDisable Size
  // Flash memory
  { ARM_MPU_RBAR(0UL,       FLASH_MEM_BASE),
    ARM_MPU_RASR(0UL,       ARM_MPU_AP_FULL, 0UL,         0UL,        1UL,        0UL,         0x00UL,          ARM_MPU_REGION_SIZE_1MB) },

  // SRAM
  { ARM_MPU_RBAR(1UL,       RAM_MEM_BASE),
    ARM_MPU_RASR(0UL,       ARM_MPU_AP_FULL, 0UL,         1UL,        1UL,        0UL,         0x00UL,          ARM_MPU_REGION_SIZE_128KB) },

  // SRAM, a 4k part with priviledged only access, this regions settings will override those of the previous region
  { ARM_MPU_RBAR(2UL,       RAM_MEM_BASE + 0x2000),
    ARM_MPU_RASR(0UL,       ARM_MPU_AP_PRIV, 0UL,         1UL,        1UL,        0UL,         0x00UL,          ARM_MPU_REGION_SIZE_4KB) },

  // LEUART, priviledged only access
  { ARM_MPU_RBAR(3UL,       LEUART1_BASE),
    ARM_MPU_RASR(1UL,       ARM_MPU_AP_PRIV, 0UL,         1UL,        0UL,        1UL,         0x00UL,          ARM_MPU_REGION_SIZE_128B) }
  // *INDENT-ON*
};

/***************************************************************************//**
 * @brief  Hard fault exception handler.
 ******************************************************************************/
void HardFault_Handler(void)      /* We should never end up here !           */
{
  static uint32_t shcsr, hfsr, bfar;

  bfar  = SCB->BFAR;            /* Bus fault address register                */
  hfsr  = SCB->HFSR;            /* Hard fault status register                */
  shcsr = SCB->SHCSR;           /* System Handler Control and State Register */

  // *INDENT-OFF*
  printf("\nHard fault !\n"
         "  System Control Block (SCB) registers: \n"
         "    SCB->SHCSR = 0x%" PRIX32 "\n"
         "    SCB->HFSR  = 0x%" PRIX32 "\n"
         "    SCB->BFAR  = 0x%" PRIX32 "\n",
         shcsr, hfsr, bfar);
  // *INDENT-ON*

  for (;; ) {
    ;
  }
}

/***************************************************************************//**
 * @brief  Memory protection fault first level exception handler.
 ******************************************************************************/
#ifdef __CC_ARM  /* MDK-ARM compiler */

__asm void MemManage_Handler(void)
{
  EXTERN MemManage_HandlerC
  TST   LR, #4
  ITE   EQ
  MRSEQ R0, MSP
  MRSNE R0, PSP
  B MemManage_HandlerC
}
#else

#if defined(__GNUC__)
void MemManage_Handler(void) __attribute__ ((naked));
#endif
void MemManage_Handler(void)
{
  /*
   * Get the appropriate stack pointer, depending on our mode,
   * and use it as a parameter to a C handler.
   */
  __asm("TST   LR, #4         \n"
        "ITE   EQ             \n"
        "MRSEQ R0, MSP        \n"
        "MRSNE R0, PSP        \n"
        "B MemManage_HandlerC \n");
}
#endif

/***************************************************************************//**
 * @brief  Memory protection fault second level exception handler.
 ******************************************************************************/
void MemManage_HandlerC(uint32_t *stack)
{
  static uint32_t mmfar, pc, shcsr, cfsr;

  mmfar = SCB->MMFAR;           /* Memory Management Fault Address Register  */
  cfsr  = SCB->CFSR;            /* Configurable Fault Status Register        */
  shcsr = SCB->SHCSR;           /* System Handler Control and State Register */
  pc = stack[6];                /* Get stacked return address                */

  // *INDENT-OFF*
  printf("\nMPU fault !\n"
         "  Violation memory address  : 0x%" PRIX32 "\n"
         "  Violation program counter : 0x%" PRIX32 "\n"
         "  System Control Block (SCB) registers: \n"
         "    SCB->SHCSR = 0x%" PRIX32 "\n"
         "    SCB->CFSR  = 0x%" PRIX32 "\n"
         "    SCB->MMFAR = 0x%" PRIX32 "\n",
         mmfar, pc, shcsr, cfsr, mmfar);
  // *INDENT-ON*

  SCB->CFSR |= 0xFF;              /* Clear all status bits in the            */
                                  /* MMFSR part of CFSR                      */
  __set_CONTROL(0);               /* Enter Priviledged state before exit     */
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  int c;

  /* Chip revision alignment and errata fixes */
  CHIP_Init();

  /* Initialize DK board register access */
  BSP_Init(BSP_INIT_DK_SPI);

  /* If first word of user data page is non-zero, enable Energy Profiler trace */
  BSP_TraceProfilerSetup();

  /* Enable printf on RS232 port - this example only supports LEUART */
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

  printf("\nEFM32 MPU access violation example.\n"
         "Hit lowercase 'x' to force access violations.\n");

  /* Set up the MPU. */
  ARM_MPU_Disable();
  ARM_MPU_Load(mpuTable, 4);        /* Load all MPU settings from the table */
  ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk); /* Full access to default memory */
                                           /* map in priviledged state      */

  while (1) {
    EMU_EnterEM2(true);

    /* Retrieve new character */
    c = getchar();
    if (c > 0) {
      if ( c == 'x' ) {
        /* Generate an access violation in LEUART1 peripheral     */
        __set_CONTROL(1);     /* Enter User (unpriviledged) state */
        putchar(c);

        /* MemManage_Handler() will set back to priviledged state */

        /* Generate an access violation in internal SRAM          */
        __set_CONTROL(1);     /* Enter User (unpriviledged) state */
        *(volatile uint32_t *)(RAM_MEM_BASE + 0x2000) = 1;

        /* MemManage_Handler() will set back to priviledged state */
      } else {
        /* Echo character */
        putchar(c);
      }
    }

    /* Most terminals issue CR when pressing enter, add LF */
    if (c == '\r') {
      putchar('\n');
    }
  }
}
