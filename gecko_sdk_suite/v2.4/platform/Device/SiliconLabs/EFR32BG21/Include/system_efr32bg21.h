/**************************************************************************//**
 * @file system_efr32bg21.h
 * @brief CMSIS system header file for EFR32BG21
 * @version 5.6.0
 ******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories, Inc. http://www.silabs.com</b>
 ******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.@n
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.@n
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Laboratories, Inc.
 * has no obligation to support this Software. Silicon Laboratories, Inc. is
 * providing the Software 'AS IS', with no express or implied warranties of any
 * kind, including, but not limited to, any implied warranties of
 * merchantability or fitness for any particular purpose or warranties against
 * infringement of any proprietary rights of a third party.
 *
 * Silicon Laboratories, Inc. will not be liable for any consequential,
 * incidental, or special damages, or any other relief, or for any claim by
 * any third party, arising from your use of this Software.
 *
 *****************************************************************************/

#ifndef SYSTEM_EFR32BG21_H
#define SYSTEM_EFR32BG21_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*******************************************************************************
 **************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

#if !defined(SYSTEM_NO_STATIC_MEMORY)
extern uint32_t SystemCoreClock;     /**< System Clock Frequency (Core Clock) */
extern uint32_t SystemHfrcoFreq;     /**< System HFRCO frequency */
#endif

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

void Reset_Handler(void);
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

/* Part Specific Interrupts */
void SMU_SECURE_IRQHandler(void);
void SMU_PRIVILEGED_IRQHandler(void);
void EMU_IRQHandler(void);
void TIMER0_IRQHandler(void);
void TIMER1_IRQHandler(void);
void TIMER2_IRQHandler(void);
void TIMER3_IRQHandler(void);
void RTCC_IRQHandler(void);
void USART0_RX_IRQHandler(void);
void USART0_TX_IRQHandler(void);
void USART1_RX_IRQHandler(void);
void USART1_TX_IRQHandler(void);
void USART2_RX_IRQHandler(void);
void USART2_TX_IRQHandler(void);
void ICACHE0_IRQHandler(void);
void BURTC_IRQHandler(void);
void LETIMER0_IRQHandler(void);
void SYSCFG_IRQHandler(void);
void LDMA_IRQHandler(void);
void LFXO_IRQHandler(void);
void LFRCO_IRQHandler(void);
void ULFRCO_IRQHandler(void);
void GPIO_ODD_IRQHandler(void);
void GPIO_EVEN_IRQHandler(void);
void I2C0_IRQHandler(void);
void I2C1_IRQHandler(void);
void EMUDG_IRQHandler(void);
void EMUSE_IRQHandler(void);
void AGC_IRQHandler(void);
void BUFC_IRQHandler(void);
void FRC_PRI_IRQHandler(void);
void FRC_IRQHandler(void);
void MODEM_IRQHandler(void);
void PROTIMER_IRQHandler(void);
void RAC_RSM_IRQHandler(void);
void RAC_SEQ_IRQHandler(void);
void SYNTH_IRQHandler(void);
void ACMP0_IRQHandler(void);
void ACMP1_IRQHandler(void);
void WDOG0_IRQHandler(void);
void WDOG1_IRQHandler(void);
void HFXO00_IRQHandler(void);
void HFRCO0_IRQHandler(void);
void HFRCOEM23_IRQHandler(void);
void CMU_IRQHandler(void);
void AES_IRQHandler(void);
void IADC_IRQHandler(void);
void MSC_IRQHandler(void);
void DPLL0_IRQHandler(void);
void SW0_IRQHandler(void);
void SW1_IRQHandler(void);
void SW2_IRQHandler(void);
void SW3_IRQHandler(void);
void KERNEL0_IRQHandler(void);
void KERNEL1_IRQHandler(void);
void M33CTI0_IRQHandler(void);
void M33CTI1_IRQHandler(void);

#if (__FPU_PRESENT == 1)
void FPUEH_IRQHandler(void);
#endif

uint32_t SystemHCLKGet(void);

/**************************************************************************//**
 * @brief
 *   Update CMSIS SystemCoreClock variable.
 *
 * @details
 *   CMSIS defines a global variable SystemCoreClock that shall hold the
 *   core frequency in Hz. If the core frequency is dynamically changed, the
 *   variable must be kept updated in order to be CMSIS compliant.
 *
 *   Notice that only if changing the core clock frequency through the EMLIB
 *   CMU API, this variable will be kept updated. This function is only
 *   provided for CMSIS compliance and if a user modifies the the core clock
 *   outside the EMLIB CMU API.
 *****************************************************************************/
static __INLINE uint32_t SystemCoreClockGet(void)
{
  return SystemHCLKGet();
}

/**************************************************************************//**
 * @brief
 *   Update CMSIS SystemCoreClock variable.
 *
 * @details
 *   CMSIS defines a global variable SystemCoreClock that shall hold the
 *   core frequency in Hz. If the core frequency is dynamically changed, the
 *   variable must be kept updated in order to be CMSIS compliant.
 *
 *   Notice that only if changing the core clock frequency through the EMLIB
 *   CMU API, this variable will be kept updated. This function is only
 *   provided for CMSIS compliance and if a user modifies the the core clock
 *   outside the EMLIB CMU API.
 *****************************************************************************/
static __INLINE void SystemCoreClockUpdate(void)
{
  SystemHCLKGet();
}

void     SystemInit(void);
uint32_t SystemSYSCLKGet(void);
uint32_t SystemMaxCoreClockGet(void);
uint32_t SystemFSRCOClockGet(void);
uint32_t SystemHFXOClockGet(void);
void     SystemHFXOClockSet(uint32_t freq);
uint32_t SystemHFRCODPLLClockGet(void);
void     SystemHFRCODPLLClockSet(uint32_t freq);
uint32_t SystemCLKIN0Get(void);
#if defined(HFRCOEM23_PRESENT)
uint32_t SystemHFRCOEM23ClockGet(void);
#endif
uint32_t SystemLFXOClockGet(void);
void     SystemLFXOClockSet(uint32_t freq);
uint32_t SystemLFRCOClockGet(void);
uint32_t SystemULFRCOClockGet(void);

#ifdef __cplusplus
}
#endif
#endif /* SYSTEM_EFR32BG21_H */
