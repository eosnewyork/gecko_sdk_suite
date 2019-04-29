/**************************************************************************//**
 * @file efr32bg21_dpll.h
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
#ifndef EFR32BG21_DPLL_H
#define EFR32BG21_DPLL_H
#define DPLL_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32BG21_DPLL DPLL
 * @{
 * @brief EFR32BG21 DPLL Register Declaration.
 *****************************************************************************/

/** DPLL Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< New Register                                       */
  __IOM uint32_t EN;                            /**< New Register                                       */
  __IOM uint32_t CFG;                           /**< New Register                                       */
  __IOM uint32_t CFG1;                          /**< New Register                                       */
  __IOM uint32_t IF;                            /**< New Register                                       */
  __IOM uint32_t IEN;                           /**< New Register                                       */
  __IM uint32_t  STATUS;                        /**< New Register                                       */
  uint32_t       RESERVED0[2U];                 /**< Reserved for future use                      */
  __IOM uint32_t LOCK;                          /**< New Register                                       */
  uint32_t       RESERVED1[1014U];              /**< Reserved for future use*/
  __IM uint32_t  IPVERSION_SET;                 /**< New Register                                       */
  __IOM uint32_t EN_SET;                        /**< New Register                                       */
  __IOM uint32_t CFG_SET;                       /**< New Register                                       */
  __IOM uint32_t CFG1_SET;                      /**< New Register                                       */
  __IOM uint32_t IF_SET;                        /**< New Register                                       */
  __IOM uint32_t IEN_SET;                       /**< New Register                                       */
  __IM uint32_t  STATUS_SET;                    /**< New Register                                       */
  uint32_t       RESERVED2[2U];                 /**< Reserved for future use                      */
  __IOM uint32_t LOCK_SET;                      /**< New Register                                       */
  uint32_t       RESERVED3[1014U];              /**< Reserved for future use*/
  __IM uint32_t  IPVERSION_CLR;                 /**< New Register                                       */
  __IOM uint32_t EN_CLR;                        /**< New Register                                       */
  __IOM uint32_t CFG_CLR;                       /**< New Register                                       */
  __IOM uint32_t CFG1_CLR;                      /**< New Register                                       */
  __IOM uint32_t IF_CLR;                        /**< New Register                                       */
  __IOM uint32_t IEN_CLR;                       /**< New Register                                       */
  __IM uint32_t  STATUS_CLR;                    /**< New Register                                       */
  uint32_t       RESERVED4[2U];                 /**< Reserved for future use                      */
  __IOM uint32_t LOCK_CLR;                      /**< New Register                                       */
  uint32_t       RESERVED5[1014U];              /**< Reserved for future use*/
  __IM uint32_t  IPVERSION_TGL;                 /**< New Register                                       */
  __IOM uint32_t EN_TGL;                        /**< New Register                                       */
  __IOM uint32_t CFG_TGL;                       /**< New Register                                       */
  __IOM uint32_t CFG1_TGL;                      /**< New Register                                       */
  __IOM uint32_t IF_TGL;                        /**< New Register                                       */
  __IOM uint32_t IEN_TGL;                       /**< New Register                                       */
  __IM uint32_t  STATUS_TGL;                    /**< New Register                                       */
  uint32_t       RESERVED6[2U];                 /**< Reserved for future use                      */
  __IOM uint32_t LOCK_TGL;                      /**< New Register                                       */
} DPLL_TypeDef;
/** @} End of group EFR32BG21_DPLL */

/**************************************************************************//**
 * @addtogroup EFR32BG21_DPLL
 * @{
 * @defgroup EFR32BG21_DPLL_BitFields DPLL Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for DPLL IPVERSION */
#define _DPLL_IPVERSION_RESETVALUE           0x00000000UL                               /**< Default value for DPLL_IPVERSION            */
#define _DPLL_IPVERSION_MASK                 0xFFFFFFFFUL                               /**< Mask for DPLL_IPVERSION                     */
#define _DPLL_IPVERSION_IPVERSION_SHIFT      0                                          /**< Shift value for DPLL_IPVERSION              */
#define _DPLL_IPVERSION_IPVERSION_MASK       0xFFFFFFFFUL                               /**< Bit mask for DPLL_IPVERSION                 */
#define _DPLL_IPVERSION_IPVERSION_DEFAULT    0x00000000UL                               /**< Mode DEFAULT for DPLL_IPVERSION             */
#define DPLL_IPVERSION_IPVERSION_DEFAULT     (_DPLL_IPVERSION_IPVERSION_DEFAULT << 0)   /**< Shifted mode DEFAULT for DPLL_IPVERSION     */

/* Bit fields for DPLL EN */
#define _DPLL_EN_RESETVALUE                  0x00000000UL                               /**< Default value for DPLL_EN                   */
#define _DPLL_EN_MASK                        0x00000001UL                               /**< Mask for DPLL_EN                            */
#define DPLL_EN_EN                           (0x1UL << 0)                               /**< Module Enable                               */
#define _DPLL_EN_EN_SHIFT                    0                                          /**< Shift value for DPLL_EN                     */
#define _DPLL_EN_EN_MASK                     0x1UL                                      /**< Bit mask for DPLL_EN                        */
#define _DPLL_EN_EN_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for DPLL_EN                    */
#define DPLL_EN_EN_DEFAULT                   (_DPLL_EN_EN_DEFAULT << 0)                 /**< Shifted mode DEFAULT for DPLL_EN            */

/* Bit fields for DPLL CFG */
#define _DPLL_CFG_RESETVALUE                 0x00000000UL                               /**< Default value for DPLL_CFG                  */
#define _DPLL_CFG_MASK                       0x00000047UL                               /**< Mask for DPLL_CFG                           */
#define DPLL_CFG_MODE                        (0x1UL << 0)                               /**< Operating Mode Control                      */
#define _DPLL_CFG_MODE_SHIFT                 0                                          /**< Shift value for DPLL_MODE                   */
#define _DPLL_CFG_MODE_MASK                  0x1UL                                      /**< Bit mask for DPLL_MODE                      */
#define _DPLL_CFG_MODE_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for DPLL_CFG                   */
#define _DPLL_CFG_MODE_FLL                   0x00000000UL                               /**< Mode FLL for DPLL_CFG                        */
#define _DPLL_CFG_MODE_PLL                   0x00000001UL                               /**< Mode PLL for DPLL_CFG                        */
#define DPLL_CFG_MODE_DEFAULT                (_DPLL_CFG_MODE_DEFAULT << 0)              /**< Shifted mode DEFAULT for DPLL_CFG           */
#define DPLL_CFG_MODE_FLL                    (_DPLL_CFG_MODE_FLL << 0)                  /**< Shifted mode FLL for DPLL_CFG                */
#define DPLL_CFG_MODE_PLL                    (_DPLL_CFG_MODE_PLL << 0)                  /**< Shifted mode PLL for DPLL_CFG                */
#define DPLL_CFG_EDGESEL                     (0x1UL << 1)                               /**< Reference Edge Select                       */
#define _DPLL_CFG_EDGESEL_SHIFT              1                                          /**< Shift value for DPLL_EDGESEL                */
#define _DPLL_CFG_EDGESEL_MASK               0x2UL                                      /**< Bit mask for DPLL_EDGESEL                   */
#define _DPLL_CFG_EDGESEL_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for DPLL_CFG                   */
#define DPLL_CFG_EDGESEL_DEFAULT             (_DPLL_CFG_EDGESEL_DEFAULT << 1)           /**< Shifted mode DEFAULT for DPLL_CFG           */
#define DPLL_CFG_AUTORECOVER                 (0x1UL << 2)                               /**< Automatic Recovery Control                  */
#define _DPLL_CFG_AUTORECOVER_SHIFT          2                                          /**< Shift value for DPLL_AUTORECOVER            */
#define _DPLL_CFG_AUTORECOVER_MASK           0x4UL                                      /**< Bit mask for DPLL_AUTORECOVER               */
#define _DPLL_CFG_AUTORECOVER_DEFAULT        0x00000000UL                               /**< Mode DEFAULT for DPLL_CFG                   */
#define DPLL_CFG_AUTORECOVER_DEFAULT         (_DPLL_CFG_AUTORECOVER_DEFAULT << 2)       /**< Shifted mode DEFAULT for DPLL_CFG           */
#define DPLL_CFG_DITHEN                      (0x1UL << 6)                               /**< Dither Enable Control                       */
#define _DPLL_CFG_DITHEN_SHIFT               6                                          /**< Shift value for DPLL_DITHEN                 */
#define _DPLL_CFG_DITHEN_MASK                0x40UL                                     /**< Bit mask for DPLL_DITHEN                    */
#define _DPLL_CFG_DITHEN_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for DPLL_CFG                   */
#define DPLL_CFG_DITHEN_DEFAULT              (_DPLL_CFG_DITHEN_DEFAULT << 6)            /**< Shifted mode DEFAULT for DPLL_CFG           */

/* Bit fields for DPLL CFG1 */
#define _DPLL_CFG1_RESETVALUE                0x00000000UL                               /**< Default value for DPLL_CFG1                 */
#define _DPLL_CFG1_MASK                      0x0FFF0FFFUL                               /**< Mask for DPLL_CFG1                          */
#define _DPLL_CFG1_M_SHIFT                   0                                          /**< Shift value for DPLL_M                      */
#define _DPLL_CFG1_M_MASK                    0xFFFUL                                    /**< Bit mask for DPLL_M                         */
#define _DPLL_CFG1_M_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for DPLL_CFG1                  */
#define DPLL_CFG1_M_DEFAULT                  (_DPLL_CFG1_M_DEFAULT << 0)                /**< Shifted mode DEFAULT for DPLL_CFG1          */
#define _DPLL_CFG1_N_SHIFT                   16                                         /**< Shift value for DPLL_N                      */
#define _DPLL_CFG1_N_MASK                    0xFFF0000UL                                /**< Bit mask for DPLL_N                         */
#define _DPLL_CFG1_N_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for DPLL_CFG1                  */
#define DPLL_CFG1_N_DEFAULT                  (_DPLL_CFG1_N_DEFAULT << 16)               /**< Shifted mode DEFAULT for DPLL_CFG1          */

/* Bit fields for DPLL IF */
#define _DPLL_IF_RESETVALUE                  0x00000000UL                               /**< Default value for DPLL_IF                   */
#define _DPLL_IF_MASK                        0x00000007UL                               /**< Mask for DPLL_IF                            */
#define DPLL_IF_LOCK                         (0x1UL << 0)                               /**< Lock Interrupt Flag                         */
#define _DPLL_IF_LOCK_SHIFT                  0                                          /**< Shift value for DPLL_LOCK                   */
#define _DPLL_IF_LOCK_MASK                   0x1UL                                      /**< Bit mask for DPLL_LOCK                      */
#define _DPLL_IF_LOCK_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for DPLL_IF                    */
#define DPLL_IF_LOCK_DEFAULT                 (_DPLL_IF_LOCK_DEFAULT << 0)               /**< Shifted mode DEFAULT for DPLL_IF            */
#define DPLL_IF_LOCKFAILLOW                  (0x1UL << 1)                               /**< Lock Failure Low Interrupt Flag             */
#define _DPLL_IF_LOCKFAILLOW_SHIFT           1                                          /**< Shift value for DPLL_LOCKFAILLOW            */
#define _DPLL_IF_LOCKFAILLOW_MASK            0x2UL                                      /**< Bit mask for DPLL_LOCKFAILLOW               */
#define _DPLL_IF_LOCKFAILLOW_DEFAULT         0x00000000UL                               /**< Mode DEFAULT for DPLL_IF                    */
#define DPLL_IF_LOCKFAILLOW_DEFAULT          (_DPLL_IF_LOCKFAILLOW_DEFAULT << 1)        /**< Shifted mode DEFAULT for DPLL_IF            */
#define DPLL_IF_LOCKFAILHIGH                 (0x1UL << 2)                               /**< Lock Failure High Interrupt Flag            */
#define _DPLL_IF_LOCKFAILHIGH_SHIFT          2                                          /**< Shift value for DPLL_LOCKFAILHIGH           */
#define _DPLL_IF_LOCKFAILHIGH_MASK           0x4UL                                      /**< Bit mask for DPLL_LOCKFAILHIGH              */
#define _DPLL_IF_LOCKFAILHIGH_DEFAULT        0x00000000UL                               /**< Mode DEFAULT for DPLL_IF                    */
#define DPLL_IF_LOCKFAILHIGH_DEFAULT         (_DPLL_IF_LOCKFAILHIGH_DEFAULT << 2)       /**< Shifted mode DEFAULT for DPLL_IF            */

/* Bit fields for DPLL IEN */
#define _DPLL_IEN_RESETVALUE                 0x00000000UL                               /**< Default value for DPLL_IEN                  */
#define _DPLL_IEN_MASK                       0x00000007UL                               /**< Mask for DPLL_IEN                           */
#define DPLL_IEN_LOCK                        (0x1UL << 0)                               /**< New BitField                                */
#define _DPLL_IEN_LOCK_SHIFT                 0                                          /**< Shift value for DPLL_LOCK                   */
#define _DPLL_IEN_LOCK_MASK                  0x1UL                                      /**< Bit mask for DPLL_LOCK                      */
#define _DPLL_IEN_LOCK_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for DPLL_IEN                   */
#define DPLL_IEN_LOCK_DEFAULT                (_DPLL_IEN_LOCK_DEFAULT << 0)              /**< Shifted mode DEFAULT for DPLL_IEN           */
#define DPLL_IEN_LOCKFAILLOW                 (0x1UL << 1)                               /**< New BitField                                */
#define _DPLL_IEN_LOCKFAILLOW_SHIFT          1                                          /**< Shift value for DPLL_LOCKFAILLOW            */
#define _DPLL_IEN_LOCKFAILLOW_MASK           0x2UL                                      /**< Bit mask for DPLL_LOCKFAILLOW               */
#define _DPLL_IEN_LOCKFAILLOW_DEFAULT        0x00000000UL                               /**< Mode DEFAULT for DPLL_IEN                   */
#define DPLL_IEN_LOCKFAILLOW_DEFAULT         (_DPLL_IEN_LOCKFAILLOW_DEFAULT << 1)       /**< Shifted mode DEFAULT for DPLL_IEN           */
#define DPLL_IEN_LOCKFAILHIGH                (0x1UL << 2)                               /**< New BitField                                */
#define _DPLL_IEN_LOCKFAILHIGH_SHIFT         2                                          /**< Shift value for DPLL_LOCKFAILHIGH           */
#define _DPLL_IEN_LOCKFAILHIGH_MASK          0x4UL                                      /**< Bit mask for DPLL_LOCKFAILHIGH              */
#define _DPLL_IEN_LOCKFAILHIGH_DEFAULT       0x00000000UL                               /**< Mode DEFAULT for DPLL_IEN                   */
#define DPLL_IEN_LOCKFAILHIGH_DEFAULT        (_DPLL_IEN_LOCKFAILHIGH_DEFAULT << 2)      /**< Shifted mode DEFAULT for DPLL_IEN           */

/* Bit fields for DPLL STATUS */
#define _DPLL_STATUS_RESETVALUE              0x00000000UL                               /**< Default value for DPLL_STATUS               */
#define _DPLL_STATUS_MASK                    0x80000003UL                               /**< Mask for DPLL_STATUS                        */
#define DPLL_STATUS_RDY                      (0x1UL << 0)                               /**< Ready Status                                */
#define _DPLL_STATUS_RDY_SHIFT               0                                          /**< Shift value for DPLL_RDY                    */
#define _DPLL_STATUS_RDY_MASK                0x1UL                                      /**< Bit mask for DPLL_RDY                       */
#define _DPLL_STATUS_RDY_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for DPLL_STATUS                */
#define DPLL_STATUS_RDY_DEFAULT              (_DPLL_STATUS_RDY_DEFAULT << 0)            /**< Shifted mode DEFAULT for DPLL_STATUS        */
#define DPLL_STATUS_ENS                      (0x1UL << 1)                               /**< Enable Status                               */
#define _DPLL_STATUS_ENS_SHIFT               1                                          /**< Shift value for DPLL_ENS                    */
#define _DPLL_STATUS_ENS_MASK                0x2UL                                      /**< Bit mask for DPLL_ENS                       */
#define _DPLL_STATUS_ENS_DEFAULT             0x00000000UL                               /**< Mode DEFAULT for DPLL_STATUS                */
#define DPLL_STATUS_ENS_DEFAULT              (_DPLL_STATUS_ENS_DEFAULT << 1)            /**< Shifted mode DEFAULT for DPLL_STATUS        */
#define DPLL_STATUS_LOCK                     (0x1UL << 31)                              /**< Lock Status                                 */
#define _DPLL_STATUS_LOCK_SHIFT              31                                         /**< Shift value for DPLL_LOCK                   */
#define _DPLL_STATUS_LOCK_MASK               0x80000000UL                               /**< Bit mask for DPLL_LOCK                      */
#define _DPLL_STATUS_LOCK_DEFAULT            0x00000000UL                               /**< Mode DEFAULT for DPLL_STATUS                */
#define _DPLL_STATUS_LOCK_UNLOCKED           0x00000000UL                               /**< Mode UNLOCKED for DPLL_STATUS                */
#define _DPLL_STATUS_LOCK_LOCKED             0x00000001UL                               /**< Mode LOCKED for DPLL_STATUS                  */
#define DPLL_STATUS_LOCK_DEFAULT             (_DPLL_STATUS_LOCK_DEFAULT << 31)          /**< Shifted mode DEFAULT for DPLL_STATUS        */
#define DPLL_STATUS_LOCK_UNLOCKED            (_DPLL_STATUS_LOCK_UNLOCKED << 31)         /**< Shifted mode UNLOCKED for DPLL_STATUS        */
#define DPLL_STATUS_LOCK_LOCKED              (_DPLL_STATUS_LOCK_LOCKED << 31)           /**< Shifted mode LOCKED for DPLL_STATUS          */

/* Bit fields for DPLL LOCK */
#define _DPLL_LOCK_RESETVALUE                0x00007102UL                               /**< Default value for DPLL_LOCK                 */
#define _DPLL_LOCK_MASK                      0x0000FFFFUL                               /**< Mask for DPLL_LOCK                          */
#define _DPLL_LOCK_LOCKKEY_SHIFT             0                                          /**< Shift value for DPLL_LOCKKEY                */
#define _DPLL_LOCK_LOCKKEY_MASK              0xFFFFUL                                   /**< Bit mask for DPLL_LOCKKEY                   */
#define _DPLL_LOCK_LOCKKEY_DEFAULT           0x00007102UL                               /**< Mode DEFAULT for DPLL_LOCK                  */
#define _DPLL_LOCK_LOCKKEY_UNLOCK            0x00007102UL                               /**< Mode UNLOCK for DPLL_LOCK                    */
#define DPLL_LOCK_LOCKKEY_DEFAULT            (_DPLL_LOCK_LOCKKEY_DEFAULT << 0)          /**< Shifted mode DEFAULT for DPLL_LOCK          */
#define DPLL_LOCK_LOCKKEY_UNLOCK             (_DPLL_LOCK_LOCKKEY_UNLOCK << 0)           /**< Shifted mode UNLOCK for DPLL_LOCK            */

/** @} End of group EFR32BG21_DPLL_BitFields */
/** @} End of group EFR32BG21_DPLL */
/** @} End of group Parts */
#endif /* EFR32BG21_DPLL_H */
