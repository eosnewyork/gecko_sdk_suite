/***************************************************************************//**
 * @brief host message streams
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

// Struct for buffering partial messages.
typedef struct {
  uint8_t buffer[4096];
  uint16_t index;
} Stream;

typedef enum {
  NCP_READ_DONE,          // There is no input pending.
  NCP_READ_PROGRESS,      // A command is executed but no input is pending.
  NCP_READ_PENDING,       // An input is pending.
  NCP_READ_FORMAT_ERROR,
  NCP_READ_IO_ERROR,
} ncpReadStatus;

//----------------------------------------------------------------
// Reading NCP messages.

// 'Type' is one of the UART_LINK_TYPE_... values.  'message' points
// to the first byte of message data, and 'length' is the number of
// message bytes.

typedef void (ncpMsgHandler)(SerialLinkMessageType type,
                             const uint8_t *message,
                             uint16_t length);

// Process NCP messages from 'stream', passing any complete messages
// to 'handler'.

ncpReadStatus processNcpInput(Stream *stream,
                              ncpMsgHandler *handler);

void emRemoveStreamBytes(Stream *stream, int count);
