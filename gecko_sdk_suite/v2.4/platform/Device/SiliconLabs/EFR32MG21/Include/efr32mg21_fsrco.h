/**************************************************************************//**
 * @file efr32mg21_fsrco.h
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
#ifndef EFR32MG21_FSRCO_H
#define EFR32MG21_FSRCO_H
#define FSRCO_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32MG21_FSRCO FSRCO
 * @{
 * @brief EFR32MG21 FSRCO Register Declaration.
 *****************************************************************************/

/** FSRCO Register Declaration. */
typedef struct {
  __IM uint32_t IPVERSION;                      /**< IP Version                                         */
  uint32_t      RESERVED0[1023U];               /**< Reserved for future use*/
  __IM uint32_t IPVERSION_SET;                  /**< IP Version                                         */
  uint32_t      RESERVED1[1023U];               /**< Reserved for future use*/
  __IM uint32_t IPVERSION_CLR;                  /**< IP Version                                         */
  uint32_t      RESERVED2[1023U];               /**< Reserved for future use*/
  __IM uint32_t IPVERSION_TGL;                  /**< IP Version                                         */
} FSRCO_TypeDef;
/** @} End of group EFR32MG21_FSRCO */

/**************************************************************************//**
 * @addtogroup EFR32MG21_FSRCO
 * @{
 * @defgroup EFR32MG21_FSRCO_BitFields FSRCO Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for FSRCO IPVERSION */
#define _FSRCO_IPVERSION_RESETVALUE           0x00000000UL                              /**< Default value for FSRCO_IPVERSION           */
#define _FSRCO_IPVERSION_MASK                 0xFFFFFFFFUL                              /**< Mask for FSRCO_IPVERSION                    */
#define _FSRCO_IPVERSION_IPVERSION_SHIFT      0                                         /**< Shift value for FSRCO_IPVERSION             */
#define _FSRCO_IPVERSION_IPVERSION_MASK       0xFFFFFFFFUL                              /**< Bit mask for FSRCO_IPVERSION                */
#define _FSRCO_IPVERSION_IPVERSION_DEFAULT    0x00000000UL                              /**< Mode DEFAULT for FSRCO_IPVERSION            */
#define FSRCO_IPVERSION_IPVERSION_DEFAULT     (_FSRCO_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for FSRCO_IPVERSION    */

/** @} End of group EFR32MG21_FSRCO_BitFields */
/** @} End of group EFR32MG21_FSRCO */
/** @} End of group Parts */
#endif /* EFR32MG21_FSRCO_H */
