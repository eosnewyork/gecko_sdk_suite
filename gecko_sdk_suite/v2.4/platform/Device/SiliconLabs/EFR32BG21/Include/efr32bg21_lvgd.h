/**************************************************************************//**
 * @file efr32bg21_lvgd.h
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
#ifndef EFR32BG21_LVGD_H
#define EFR32BG21_LVGD_H
#define LVGD_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32BG21_LVGD LVGD
 * @{
 * @brief EFR32BG21 LVGD Register Declaration.
 *****************************************************************************/

/** LVGD Register Declaration. */
typedef struct {
  uint32_t RESERVED0[1U];                       /**< Reserved for future use                      */
  uint32_t RESERVED1[1023U];                    /**< Reserved for future use*/
  uint32_t RESERVED2[1U];                       /**< Reserved for future use                      */
  uint32_t RESERVED3[1023U];                    /**< Reserved for future use*/
  uint32_t RESERVED4[1U];                       /**< Reserved for future use                      */
  uint32_t RESERVED5[1023U];                    /**< Reserved for future use*/
  uint32_t RESERVED6[1U];                       /**< Reserved for future use                      */
} LVGD_TypeDef;
/** @} End of group EFR32BG21_LVGD */

/**************************************************************************//**
 * @addtogroup EFR32BG21_LVGD
 * @{
 * @defgroup EFR32BG21_LVGD_BitFields LVGD Bit Fields
 * @{
 *****************************************************************************/

/** @} End of group EFR32BG21_LVGD_BitFields */
/** @} End of group EFR32BG21_LVGD */
/** @} End of group Parts */
#endif /* EFR32BG21_LVGD_H */
