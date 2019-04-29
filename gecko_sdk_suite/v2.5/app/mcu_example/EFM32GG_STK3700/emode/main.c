/***************************************************************************//**
 * @file
 * @brief Demo for energy mode current consumption testing for Giant Gecko.
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
  /* Use default settings for EM23, EM4 */
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Initialize EM23 and EM4 with default parameters */
  EMU_EM23Init(&em23Init);
  EMU_EM4Init(&em4Init);

  /* Sets "mode" variable to user desired energy mode enum,
   * and reconfigures chip to initial state.
   * See files for "lcd_setup" for more information. */
  Energy_Mode_TypeDef mode = LCD_SelectMode();

  /* Start the selected energy mode setup (See Data Sheet Table 3.4).
   * See files for "emodes" for more information.
   * Copy and paste function definition to replicate setup. */
  switch (mode) {
    case EM0_HFXO_48MHZ:
      em_EM0_Hfxo();
      break;
    case EM0_HFRCO_28MHZ:
      em_EM0_Hfrco(cmuHFRCOBand_28MHz);
      break;
    case EM0_HFRCO_21MHZ:
      em_EM0_Hfrco(cmuHFRCOBand_21MHz);
      break;
    case EM0_HFRCO_14MHZ:
      em_EM0_Hfrco(cmuHFRCOBand_14MHz);
      break;
    case EM0_HFRCO_11MHZ:
      em_EM0_Hfrco(cmuHFRCOBand_11MHz);
      break;
    case EM0_HFRCO_7MHZ:
      em_EM0_Hfrco(cmuHFRCOBand_7MHz);
      break;
    case EM0_HFRCO_1MHZ:
      em_EM0_Hfrco(cmuHFRCOBand_1MHz);
      break;
    case EM1_HFXO_48MHZ:
      em_EM1_Hfxo();
      break;
    case EM1_HFRCO_28MHZ:
      em_EM1_Hfrco(cmuHFRCOBand_28MHz);
      break;
    case EM1_HFRCO_21MHZ:
      em_EM1_Hfrco(cmuHFRCOBand_21MHz);
      break;
    case EM1_HFRCO_14MHZ:
      em_EM1_Hfrco(cmuHFRCOBand_14MHz);
      break;
    case EM1_HFRCO_11MHZ:
      em_EM1_Hfrco(cmuHFRCOBand_11MHz);
      break;
    case EM1_HFRCO_7MHZ:
      em_EM1_Hfrco(cmuHFRCOBand_7MHz);
      break;
    case EM1_HFRCO_1MHZ:
      em_EM1_Hfrco(cmuHFRCOBand_1MHz);
      break;
    case EM2_LFRCO_RTC:
      em_EM2_LfrcoRTC();
      break;
    case EM3_ULFRCO:
      em_EM3();
      break;
    case EM4:
      em_EM4();
      break;
    default:
      EFM_ASSERT(false);
      break;
  }

  // Should not be reached.
  EFM_ASSERT(false);
}
