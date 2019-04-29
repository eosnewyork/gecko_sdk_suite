/***************************************************************************//**
 * @file
 * @brief Demo for energy mode current consumption testing.
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

#include "lcd_setup.h"
#include "emodes.h"
#include "coremark/core_main.h"
#include "bsp.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_assert.h"
#include <stdint.h>

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  /* Use default settings for DCDC, energy modes, and HFXO */
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
  EMU_EM01Init_TypeDef em01Init = EMU_EM01INIT_DEFAULT;
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Unlatch pins if previous mode was EM4 with pin retention. */
  EMU_UnlatchPinRetention();

  EMU_DCDCInit(&dcdcInit);
  CMU_HFXOInit(&hfxoInit);

  /* Enable downscaling in EM0 to 4. */
  em01Init.vScaleEM01LowPowerVoltageEnable = true;
  em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;
  em4Init.vScaleEM4HVoltage = emuVScaleEM4H_LowPower;

  /* Initialize EM0123 and HFXO with default parameters */
  EMU_EM01Init(&em01Init);
  EMU_EM23Init(&em23Init);
  EMU_EM4Init(&em4Init);

  /* Disable peripheral retention. */
  EMU_PeripheralRetention(emuPeripheralRetention_ALL, false);

  /* Sets "mode" variable to user desired energy mode enum,
   * and reconfigures chip to initial state.
   * See files for "lcd_setup" for more information. */
  Energy_Mode_TypeDef mode = LCD_SelectMode();

  /* Start the selected energy mode setup.
   * See files for "emodes" for more information.
   * Copy and paste function definition to replicate setup.*/
  switch (mode) {
    case EM0_HFXO_48MHZ_WHILE:
      EMU_DCDCModeSet(emuDcdcMode_Bypass);
      em_EM0_Hfxo();
      while (1) ;
    /* no break */
    case EM0_HFRCO_48MHZ_WHILE:
      EMU_DCDCModeSet(emuDcdcMode_Bypass);
      em_EM0_Hfrco(cmuHFRCOFreq_48M0Hz);
      while (1) ;
    /* no break */
    case EM0_HFRCO_48MHZ_PRIME:
      EMU_DCDCModeSet(emuDcdcMode_Bypass);
      em_EM0_Hfrco(cmuHFRCOFreq_48M0Hz);
      primeCalc();
      break;
    case EM0_HFRCO_48MHZ_COREMARK:
      EMU_DCDCModeSet(emuDcdcMode_Bypass);
      em_EM0_Hfrco(cmuHFRCOFreq_48M0Hz);
      CMU_ClockEnable(cmuClock_HFPER, true); // enable timer for CoreMark
      while (1) {
        CoreMark_Main();
      }
    /* no break */
    case EM0_HFRCO_19MHZ_WHILE:
      EMU_DCDCModeSet(emuDcdcMode_Bypass);
      em_EM0_Hfrco(cmuHFRCOFreq_19M0Hz);
      while (1) ;
    /* no break */
    case EM0_HFRCO_1MHZ_WHILE:
      EMU_DCDCModeSet(emuDcdcMode_Bypass);
      em_EM0_Hfrco(cmuHFRCOFreq_1M0Hz);
      while (1) ;
    /* no break */
    case EM1_HFXO_48MHZ:
      EMU_DCDCModeSet(emuDcdcMode_Bypass);
      em_EM1_Hfxo();
      break;
    case EM1_HFRCO_48MHZ:
      EMU_DCDCModeSet(emuDcdcMode_Bypass);
      em_EM1_Hfrco(cmuHFRCOFreq_48M0Hz);
      break;
    case EM1_HFRCO_19MHZ:
      EMU_DCDCModeSet(emuDcdcMode_Bypass);
      em_EM1_Hfrco(cmuHFRCOFreq_19M0Hz);
      break;
    case EM1_HFRCO_1MHZ:
      EMU_DCDCModeSet(emuDcdcMode_Bypass);
      em_EM1_Hfrco(cmuHFRCOFreq_1M0Hz);
      break;
    case EM2_LFXO_RTCC:
      EMU_DCDCModeSet(emuDcdcMode_Bypass);
      // Full RAM
      em_EM2_RTCC(cmuSelect_LFXO, false); // disable RAM powerdown
      break;
    case EM2_LFRCO_RTCC:
      EMU_DCDCModeSet(emuDcdcMode_Bypass);
      // 8kB RAM
      em_EM2_RTCC(cmuSelect_LFRCO, true); // enable RAM powerdown
      break;
    case EM3_ULFRCO_CRYO:
      EMU_DCDCModeSet(emuDcdcMode_Bypass);
      // Full RAM
      em_EM3_UlfrcoCRYO();
      break;
    case EM4H_LFXO_RTCC:
      EMU_DCDCModeSet(emuDcdcMode_Bypass);
      // 128b RAM
      em_EM4H_LfxoRTCC();
      break;
    case EM4H_ULFRCO_CRYO:
      EMU_DCDCModeSet(emuDcdcMode_Bypass);
      // 128b RAM
      em_EM4H_UlfrcoCRYO();
      break;
    case EM4H:
      EMU_DCDCModeSet(emuDcdcMode_Bypass);
      // 128b RAM
      em_EM4H();
      break;
    case EM4S:
      EMU_DCDCModeSet(emuDcdcMode_Bypass);
      // No RAM
      em_EM4S();
      break;
    case EM0_HFXO_48MHZ_WHILE_DCDC:
      em_EM0_Hfxo();
      while (1) ;
    /* no break */
    case EM0_HFRCO_48MHZ_WHILE_DCDC:
      em_EM0_Hfrco(cmuHFRCOFreq_48M0Hz);
      while (1) ;
    /* no break */
    case EM0_HFRCO_48MHZ_PRIME_DCDC:
      em_EM0_Hfrco(cmuHFRCOFreq_48M0Hz);
      primeCalc();
      break;
    case EM0_HFRCO_48MHZ_COREMARK_DCDC:
      em_EM0_Hfrco(cmuHFRCOFreq_48M0Hz);
      CMU_ClockEnable(cmuClock_HFPER, true); // enable timer for CoreMark
      while (1) {
        CoreMark_Main();
      }
    /* no break */
    case EM0_HFRCO_19MHZ_WHILE_DCDC:
      em_EM0_Hfrco(cmuHFRCOFreq_19M0Hz);
      while (1) ;
    /* no break */
    case EM0_HFRCO_1MHZ_WHILE_DCDC:
      em_EM0_Hfrco(cmuHFRCOFreq_1M0Hz);
      while (1) ;
    /* no break */
    case EM1_HFXO_48MHZ_DCDC:
      EMU_DCDCModeSet(emuDcdcMode_LowPower);
      em_EM1_Hfxo();
      break;
    case EM1_HFRCO_48MHZ_DCDC:
      EMU_DCDCModeSet(emuDcdcMode_LowPower);
      em_EM1_Hfrco(cmuHFRCOFreq_48M0Hz);
      break;
    case EM1_HFRCO_19MHZ_DCDC:
      EMU_DCDCModeSet(emuDcdcMode_LowPower);
      em_EM1_Hfrco(cmuHFRCOFreq_19M0Hz);
      break;
    case EM1_HFRCO_1MHZ_DCDC:
      EMU_DCDCModeSet(emuDcdcMode_LowPower);
      em_EM1_Hfrco(cmuHFRCOFreq_1M0Hz);
      break;
    case EM2_LFXO_RTCC_DCDC:
      // Full RAM
      em_EM2_RTCC(cmuSelect_LFXO, false); // disable RAM powerdown
      break;
    case EM2_LFRCO_RTCC_DCDC:
      // 8kB RAM
      em_EM2_RTCC(cmuSelect_LFRCO, true); // enable RAM powerdown
      break;
    case EM3_ULFRCO_CRYO_DCDC:
      // Full RAM
      em_EM3_UlfrcoCRYO();
      break;
    case EM4H_LFXO_RTCC_DCDC:
      // 128b RAM
      em_EM4H_LfxoRTCC();
      break;
    case EM4H_ULFRCO_CRYO_DCDC:
      // 128b RAM
      em_EM4H_UlfrcoCRYO();
      break;
    case EM4H_DCDC:
      // 128b RAM
      em_EM4H();
      break;
    case EM4S_DCDC:
      // No RAM
      em_EM4S();
      break;
    default:
      EFM_ASSERT(false);
      break;
  }

  // Should not be reached.
  EFM_ASSERT(false);
}
