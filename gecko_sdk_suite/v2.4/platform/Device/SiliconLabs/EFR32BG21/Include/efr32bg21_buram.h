/**************************************************************************//**
 * @file efr32bg21_buram.h
 * @brief CMSIS Cortex-M Peripheral Access Layer Header File
 *        for EFR32BG21
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
#ifndef EFR32BG21_BURAM_H
#define EFR32BG21_BURAM_H
#define BURAM_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32BG21_BURAM BURAM
 * @{
 * @brief EFR32BG21 BURAM Register Declaration.
 *****************************************************************************/

/** BURAM RET Register Group Declaration. */
typedef struct {
  __IOM uint32_t REG;                                /**< Retention Register                                 */
} BURAM_RET_TypeDef;

/** BURAM Register Declaration. */
typedef struct {
  BURAM_RET_TypeDef RET[32U];                   /**< RetentionReg                                      */
  uint32_t          RESERVED0[992U];            /**< Reserved for future use*/
  BURAM_RET_TypeDef RET_SET[32U];               /**< RetentionReg                                      */
  uint32_t          RESERVED1[992U];            /**< Reserved for future use*/
  BURAM_RET_TypeDef RET_CLR[32U];               /**< RetentionReg                                      */
  uint32_t          RESERVED2[992U];            /**< Reserved for future use*/
  BURAM_RET_TypeDef RET_TGL[32U];               /**< RetentionReg                                      */
} BURAM_TypeDef;
/** @} End of group EFR32BG21_BURAM */

/**************************************************************************//**
 * @addtogroup EFR32BG21_BURAM
 * @{
 * @defgroup EFR32BG21_BURAM_BitFields BURAM Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for BURAM RET_REG */
#define _BURAM_RET_REG_RESETVALUE        0x00000000UL                                   /**< Default value for BURAM_RET_REG             */
#define _BURAM_RET_REG_MASK              0xFFFFFFFFUL                                   /**< Mask for BURAM_RET_REG                      */
#define _BURAM_RET_REG_RETREG_SHIFT      0                                              /**< Shift value for BURAM_RETREG                */
#define _BURAM_RET_REG_RETREG_MASK       0xFFFFFFFFUL                                   /**< Bit mask for BURAM_RETREG                   */
#define _BURAM_RET_REG_RETREG_DEFAULT    0x00000000UL                                   /**< Mode DEFAULT for BURAM_RET_REG              */
#define BURAM_RET_REG_RETREG_DEFAULT     (_BURAM_RET_REG_RETREG_DEFAULT << 0)           /**< Shifted mode DEFAULT for BURAM_RET_REG      */

/** @} End of group EFR32BG21_BURAM_BitFields */
/** @} End of group EFR32BG21_BURAM */
/** @} End of group Parts */
#endif /* EFR32BG21_BURAM_H */
