/***************************************************************************//**
 * @brief Types defined for mailbox.
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

#ifndef _MAILBOX_TYPES_H_
#define _MAILBOX_TYPES_H_

/**
 * @addtogroup mailbox_common
 * @{
 */

/**
 * @brief Mailbox return status codes.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberAfMailboxStatus
#else
typedef uint8_t EmberAfMailboxStatus;
enum
#endif
{
  EMBER_MAILBOX_STATUS_SUCCESS             = 0x00,
  EMBER_MAILBOX_STATUS_INVALID_CALL        = 0x01,
  EMBER_MAILBOX_STATUS_BUSY                = 0x02,
  EMBER_MAILBOX_STATUS_STACK_ERROR         = 0x03,
  EMBER_MAILBOX_STATUS_INVALID_ADDRESS     = 0x04,
  EMBER_MAILBOX_STATUS_MESSAGE_TOO_LONG    = 0x05,
  EMBER_MAILBOX_STATUS_MESSAGE_TABLE_FULL  = 0x06,
  EMBER_MAILBOX_STATUS_MESSAGE_NO_BUFFERS  = 0x07,
  EMBER_MAILBOX_STATUS_MESSAGE_NO_RESPONSE = 0x08,
  EMBER_MAILBOX_STATUS_MESSAGE_TIMED_OUT   = 0x09,
  EMBER_MAILBOX_STATUS_MESSAGE_NO_DATA     = 0x0A,
};

/** @} // END addtogroup
 */

#endif // _MAILBOX_TYPES_H_
