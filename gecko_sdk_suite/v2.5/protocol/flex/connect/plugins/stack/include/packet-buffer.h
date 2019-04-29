/***************************************************************************//**
 * @brief Macros and APIs defined for packet-buffer.
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

// None of these should endup in doxygen
#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern EmberMessageBuffer *emPacketBufferLinks;

#define LOG_PACKET_BUFFER_SIZE 5
#define PACKET_BUFFER_SIZE (1 << LOG_PACKET_BUFFER_SIZE)

#define emberAllocateStackBuffer() (emberAllocateLinkedBuffers(1))
#define emberStackBufferLink(buffer) \
  (emPacketBufferLinks[(buffer)])
#define emberLinkedBufferContents(buffer) emberMessageBufferContents(buffer)

EmberMessageBuffer emberAllocateLinkedBuffers(uint8_t count);
EmberStatus emberAppendToLinkedBuffers(EmberMessageBuffer buffer,
                                       uint8_t *contents,
                                       uint8_t length);
uint16_t emberMessageBufferLength(EmberMessageBuffer buffer);
void emberHoldMessageBuffer(EmberMessageBuffer buffer);
void emberReleaseMessageBuffer(EmberMessageBuffer buffer);
uint8_t *emberMessageBufferContents(EmberMessageBuffer buffer);
EmberStatus emberAppendPgmStringToLinkedBuffers(EmberMessageBuffer buffer, PGM_P suffix);
EmberMessageBuffer emberFillLinkedBuffers(uint8_t *contents, uint8_t length);

#endif // !DOXYGEN_SHOULD_SKIP_THIS
