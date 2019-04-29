/**************************************************************************//**
 * @file efr32mg21_ldmaxbar.h
 * @brief CMSIS Cortex-M Peripheral Access Layer Header File
 *        for EFR32MG21
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
#ifndef EFR32MG21_LDMAXBAR_H
#define EFR32MG21_LDMAXBAR_H
#define LDMAXBAR_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32MG21_LDMAXBAR LDMAXBAR
 * @{
 * @brief EFR32MG21 LDMAXBAR Register Declaration.
 *****************************************************************************/

/** LDMAXBAR CH Register Group Declaration. */
typedef struct {
  __IOM uint32_t REQSEL;                             /**< Channel Peripheral Request Select Reg...           */
} LDMAXBAR_CH_TypeDef;

/** LDMAXBAR Register Declaration. */
typedef struct {
  LDMAXBAR_CH_TypeDef CH[8U];                   /**< DMA Channel Registers                             */
  uint32_t            RESERVED0[1016U];         /**< Reserved for future use*/
  LDMAXBAR_CH_TypeDef CH_SET[8U];               /**< DMA Channel Registers                             */
  uint32_t            RESERVED1[1016U];         /**< Reserved for future use*/
  LDMAXBAR_CH_TypeDef CH_CLR[8U];               /**< DMA Channel Registers                             */
  uint32_t            RESERVED2[1016U];         /**< Reserved for future use*/
  LDMAXBAR_CH_TypeDef CH_TGL[8U];               /**< DMA Channel Registers                             */
} LDMAXBAR_TypeDef;
/** @} End of group EFR32MG21_LDMAXBAR */

/**************************************************************************//**
 * @addtogroup EFR32MG21_LDMAXBAR
 * @{
 * @defgroup EFR32MG21_LDMAXBAR_BitFields LDMAXBAR Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for LDMAXBAR CH_REQSEL */
#define _LDMAXBAR_CH_REQSEL_RESETVALUE           0x00000000UL                                  /**< Default value for LDMAXBAR_CH_REQSEL        */
#define _LDMAXBAR_CH_REQSEL_MASK                 0x003F000FUL                                  /**< Mask for LDMAXBAR_CH_REQSEL                 */
#define _LDMAXBAR_CH_REQSEL_SIGSEL_SHIFT         0                                             /**< Shift value for LDMAXBAR_SIGSEL             */
#define _LDMAXBAR_CH_REQSEL_SIGSEL_MASK          0xFUL                                         /**< Bit mask for LDMAXBAR_SIGSEL                */
#define _LDMAXBAR_CH_REQSEL_SIGSEL_DEFAULT       0x00000000UL                                  /**< Mode DEFAULT for LDMAXBAR_CH_REQSEL         */
#define LDMAXBAR_CH_REQSEL_SIGSEL_DEFAULT        (_LDMAXBAR_CH_REQSEL_SIGSEL_DEFAULT << 0)     /**< Shifted mode DEFAULT for LDMAXBAR_CH_REQSEL */
#define _LDMAXBAR_CH_REQSEL_SOURCESEL_SHIFT      16                                            /**< Shift value for LDMAXBAR_SOURCESEL          */
#define _LDMAXBAR_CH_REQSEL_SOURCESEL_MASK       0x3F0000UL                                    /**< Bit mask for LDMAXBAR_SOURCESEL             */
#define _LDMAXBAR_CH_REQSEL_SOURCESEL_DEFAULT    0x00000000UL                                  /**< Mode DEFAULT for LDMAXBAR_CH_REQSEL         */
#define LDMAXBAR_CH_REQSEL_SOURCESEL_DEFAULT     (_LDMAXBAR_CH_REQSEL_SOURCESEL_DEFAULT << 16) /**< Shifted mode DEFAULT for LDMAXBAR_CH_REQSEL */

/** @} End of group EFR32MG21_LDMAXBAR_BitFields */
/** @} End of group EFR32MG21_LDMAXBAR */
/** @} End of group Parts */
#endif /* EFR32MG21_LDMAXBAR_H */
