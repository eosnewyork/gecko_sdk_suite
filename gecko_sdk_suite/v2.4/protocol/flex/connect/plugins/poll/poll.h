// Copyright 2015 Silicon Laboratories, Inc.

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
