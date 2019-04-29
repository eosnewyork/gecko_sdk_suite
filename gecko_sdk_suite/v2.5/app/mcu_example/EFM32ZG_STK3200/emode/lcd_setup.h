/***************************************************************************//**
 * @file
 * @brief Setup LCD for energy mode demo, header file
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

#ifndef LCD_SETUP_H
#define LCD_SETUP_H

#include <stdint.h>

/* Energy mode enumerations */
typedef enum {
  EM0_HFXO_24MHZ,  /**< EM0 with HFXO at 24MHz   */
  EM0_HFRCO_21MHZ, /**< EM0 with HFRCO at 21MHz  */
  EM0_HFRCO_14MHZ, /**< EM0 with HFRCO at 14MHz  */
  EM0_HFRCO_11MHZ, /**< EM0 with HFRCO at 11MHz  */
  EM0_HFRCO_7MHZ,  /**< EM0 with HFRCO at 7MHz   */
  EM0_HFRCO_1MHZ,  /**< EM0 with HFRCO at 1MHz   */
  EM1_HFXO_24MHZ,  /**< EM1 with HFXO at 24MHz   */
  EM1_HFRCO_21MHZ, /**< EM1 with HFRCO at 21MHz  */
  EM1_HFRCO_14MHZ, /**< EM1 with HFRCO at 14MHz  */
  EM1_HFRCO_11MHZ, /**< EM1 with HFRCO at 11MHz  */
  EM1_HFRCO_7MHZ,  /**< EM1 with HFRCO at 7MHz   */
  EM1_HFRCO_1MHZ,  /**< EM1 with HFRCO at 1MHz   */
  EM2_LFRCO_RTC,   /**< EM2 with LFRCO RTC       */
  EM3_ULFRCO,      /**< EM3 with ULFRCO          */
  EM4,             /**< EM4                      */
  NUM_EMODES,      /**< Number of Energy Modes   */
} Energy_Mode_TypeDef;

/* Energy mode select using LCD and buttons */
Energy_Mode_TypeDef LCD_SelectMode(void);

#endif // LCD_SETUP_H
