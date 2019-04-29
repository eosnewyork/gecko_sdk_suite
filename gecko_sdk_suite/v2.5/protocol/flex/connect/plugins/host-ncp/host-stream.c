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

#include <stdlib.h>
#include <unistd.h>

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"
#include "uart-link-protocol.h"
#include "host-stream.h"

void emRemoveStreamBytes(Stream *stream, int count)
{
  assert(count <= stream->index);
  memmove(stream->buffer, stream->buffer + count, stream->index - count);
  stream->index -= count;

  // help the debugger
  MEMSET(stream->buffer + stream->index,
         0,
         sizeof(stream->buffer) - stream->index);
}

ncpReadStatus processNcpInput(Stream *stream,
                              ncpMsgHandler *handler)
{
  ncpReadStatus result = NCP_READ_PENDING;
  uint16_t length = emberFetchHighLowInt16u(stream->buffer + 2);
  uint8_t messageType = stream->buffer[1];

  if (stream->buffer[0] != '['
      || MAX_UART_LINK_TYPE < messageType
      || length == 0) {
    // We get here if there is a framing error
    return NCP_READ_FORMAT_ERROR;
  }

  if (UART_LINK_HEADER_SIZE + length <= stream->index) {
    handler(messageType, stream->buffer + UART_LINK_HEADER_SIZE, length);
    emRemoveStreamBytes(stream, length + UART_LINK_HEADER_SIZE);
    result = NCP_READ_PROGRESS;
  }

  return (stream->index == 0
          ? NCP_READ_DONE
          : result);
}
