/***************************************************************************//**
 * @brief Buffer allocation and management routines for Host.
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

#ifndef __HOST_BUFFER_MANAGEMENT_H__
#define __HOST_BUFFER_MANAGEMENT_H__

typedef void (*Marker)(Buffer *buffer);
typedef void (*BufferMarker)(void);

void emInitializeBuffers(void);
Buffer emAllocateBuffer(uint16_t length);
uint8_t *emGetBufferPointer(Buffer buffer);
uint16_t emGetBufferLength(Buffer buffer);
void emMarkBuffer(Buffer *root);
void emReclaimUnusedBuffers(const BufferMarker *markers);
Buffer emGetBufferLink(Buffer buffer, uint8_t i);
void emSetBufferLink(Buffer buffer, uint8_t i, Buffer newLink);

#define QUEUE_LINK   0
#define PAYLOAD_LINK 1

#define emGetQueueLink(buffer)   (emGetBufferLink((buffer), QUEUE_LINK))
#define emGetPayloadLink(buffer) (emGetBufferLink((buffer), PAYLOAD_LINK))

#define emSetQueueLink(buffer, new) \
  (emSetBufferLink((buffer), QUEUE_LINK, (new)))
#define emSetPayloadLink(buffer, new) \
  (emSetBufferLink((buffer), PAYLOAD_LINK, (new)))

#define emFillBuffer(contents, length) \
  emReallyFillBuffer(contents, length, false)

Buffer emReallyFillBuffer(const uint8_t *contents, uint16_t length, bool async);

// 'contents' may be NULL, in which case NULL_BUFFER is returned.
Buffer emFillStringBuffer(const uint8_t *contents);

void emBufferUsage(const char *tag);
void emEndBufferUsage(void);

#endif
