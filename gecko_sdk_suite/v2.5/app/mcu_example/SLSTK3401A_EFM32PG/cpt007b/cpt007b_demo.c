/***************************************************************************//**
 * @file
 * @brief CPT007B Demo
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
/***************************************************************************//**
 *
 * Program Description:
 *
 * This demo demonstrates the capacitive button function of CPT007B fixed
 * function board. This demo runs on the EFM32PG STK, and communicates
 * with CPT007B through the GPIO pins on the EXP header. Virtual buttons
 * on STK's LCD screen indicate whether the capacitive sensing buttons on
 * CPT007B are pressed or released.
 * This demo supports up to 7 capacitive buttons on CPT007B
 *
 * ----------------------------------------------------------------------------
 * How To Test: EFM32PG STK
 * ----------------------------------------------------------------------------
 * 1) Place the SW104 switch in "AEM" mode.
 * 2) Attach CPT007B to STK through EXP headers
 * 3) Connect the EFM32PG STK board to a PC using a mini USB cable.
 * 4) Compile and download code to the EFM32PG STK board.
 *    In Simplicity Studio IDE, select Run -> Debug from the menu bar,
 *    click the Debug button in the quick menu, or press F11.
 * 5) Run the code.
 *    In Simplicity Studio IDE, select Run -> Resume from the menu bar,
 *    click the Resume button in the quick menu, or press F8.
 * 6) The demo program should start with a set of circles on display, by
 *    pressing/releasing capacitive buttons on CPT007B, the circles turns
 *    ON/OFF.
 *
 * Release 0.1 (MR)
 *    - Initial Revision
 *    - 03 Nov 2015
 *
 ******************************************************************************/

#include "gpio.h"
#include "lcd.h"
#include "em_chip.h"
#include "em_cmu.h"

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // initializations
  initLCD();
  initGPIO();

  // Enable HFXO as the main clock
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  updateLCD();

  // Infinite loop
  while (1) {
    updateGPIO();
    // update LCD if detects new cap-sense action
    if ( getCapsensePrevious() != getCapsenseCurrent()) {
      updateLCD();
    }
  }
}
