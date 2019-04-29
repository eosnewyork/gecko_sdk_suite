/***************************************************************************//**
 * @brief APIs for the poll plugin.
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

/**
 * @addtogroup poll
 *
 * See poll.h for source code.
 * @{
 */

/** @brief Sets the short poll interval.
 *
 *  @param intervalQS The short poll interval in quarter seconds.
 */
void emberAfPluginPollSetShortPollInterval(uint8_t intervalQS);

/** @brief Sets the long poll interval.
 *
 *  @param intervalS The long poll interval in seconds.
 */
void emberAfPluginPollSetLongPollInterval(uint16_t intervalS);

/** @brief Enables/disables short polling.
 *
 *  @param enable If this parameter is true, short polling is enabled.
 *  Otherwise, the node switches back to long polling.
 */
void emberAfPluginPollEnableShortPolling(bool enable);

/** @} // END addtogroup
 */