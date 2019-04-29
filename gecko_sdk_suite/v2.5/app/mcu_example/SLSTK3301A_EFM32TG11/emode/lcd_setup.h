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

// Energy mode enumerations
typedef enum {
  EM0_HFXO_48MHZ_WHILE,          /**< EM0 with HFXO at 48MHz (while loop)                    */
  EM0_HFRCO_48MHZ_WHILE,         /**< EM0 with HFRCO at 48MHz (while loop)                   */
  EM0_HFRCO_48MHZ_PRIME,         /**< EM0 with HFRCO at 48MHz (prime calculations)           */
  EM0_HFRCO_48MHZ_COREMARK,      /**< EM0 with HFRCO at 48MHz (Coremark)                     */
  EM0_HFRCO_19MHZ_WHILE,         /**< EM0 with HFRCO at 19MHz (while loop)                   */
  EM0_HFRCO_1MHZ_WHILE,          /**< EM0 with HFRCO at 1MHz (while loop)                    */

  EM1_HFXO_48MHZ,                /**< EM1 with HFXO at 48MHz                                 */
  EM1_HFRCO_48MHZ,               /**< EM1 with HFRCO at 48MHz                                */
  EM1_HFRCO_19MHZ,               /**< EM1 with HFRCO at 19MHz                                */
  EM1_HFRCO_1MHZ,                /**< EM1 with HFRCO at 1MHz                                 */

  EM2_LFXO_RTCC,                 /**< EM2 with LFXO RTCC                                     */
  EM2_LFRCO_RTCC,                /**< EM2 with LFRCO RTCC                                    */

  EM3_ULFRCO_CRYO,               /**< EM3 with ULFRCO CRYOTIMER                              */

  EM4H_LFXO_RTCC,                /**< EM4H with RTCC                                         */
  EM4H_ULFRCO_CRYO,              /**< EM4H with ULFRCO CRYOTIMER                             */
  EM4H,                          /**< EM4H                                                   */
  EM4S,                          /**< EM4S                                                   */

  EM0_HFXO_48MHZ_WHILE_DCDC,     /**< EM0 with HFXO at 48MHz with DCDC (while loop)          */
  EM0_HFRCO_48MHZ_WHILE_DCDC,    /**< EM0 with HFRCO at 48MHz with DCDC (while loop)         */
  EM0_HFRCO_48MHZ_PRIME_DCDC,    /**< EM0 with HFRCO at 48MHz with DCDC (prime calculations) */
  EM0_HFRCO_48MHZ_COREMARK_DCDC, /**< EM0 with HFRCO at 48MHz with DCDC (Coremark)           */
  EM0_HFRCO_19MHZ_WHILE_DCDC,    /**< EM0 with HFRCO at 19MHz with DCDC (while loop)         */
  EM0_HFRCO_1MHZ_WHILE_DCDC,     /**< EM0 with HFRCO at 1MHz with DCDC (while loop)         */

  EM1_HFXO_48MHZ_DCDC,           /**< EM1 with HFXO at 48MHz with DCDC                       */
  EM1_HFRCO_48MHZ_DCDC,          /**< EM1 with HFRCO at 48MHz with DCDC                      */
  EM1_HFRCO_19MHZ_DCDC,          /**< EM1 with HFRCO at 19MHz with DCDC                      */
  EM1_HFRCO_1MHZ_DCDC,           /**< EM1 with HFRCO at 1MHz with DCDC                       */

  EM2_LFXO_RTCC_DCDC,            /**< EM2 with LFXO RTCC with DCDC                           */
  EM2_LFRCO_RTCC_DCDC,           /**< EM2 with LFRCO RTCC with DCDC                          */

  EM3_ULFRCO_CRYO_DCDC,          /**< EM3 with ULFRCO CRYOTIMER with DCDC                    */

  EM4H_LFXO_RTCC_DCDC,           /**< EM4H with RTCC with DCDC                               */
  EM4H_ULFRCO_CRYO_DCDC,         /**< EM4H with ULFRCO CRYOTIMER with DCDC                   */
  EM4H_DCDC,                     /**< EM4H with DCDC                                         */
  EM4S_DCDC,                     /**< EM4S with DCDC                                         */
  NUM_EMODES                     /**< Number of Energy Modes                                 */
} Energy_Mode_TypeDef;

// Energy mode select using LCD and buttons
Energy_Mode_TypeDef LCD_SelectMode(void);

#endif // LCD_SETUP_H
