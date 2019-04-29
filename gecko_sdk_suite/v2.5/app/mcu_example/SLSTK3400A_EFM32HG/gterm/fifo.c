/***************************************************************************//**
 * @file
 * @brief FIFO API implementation.
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

#include "fifo.h"
#include "em_core.h"

void FIFO_Init(Fifo_t *fifo, uint8_t *data, size_t size)
{
  fifo->size = size;
  fifo->data = data;
  fifo->content = 0;
  fifo->idxInp = 0;
  fifo->idxOut = 0;
  fifo->overflow = 0;
}

size_t FIFO_Content(Fifo_t *fifo)
{
  return fifo->content;
}

size_t FIFO_GetMulti(Fifo_t *fifo, uint8_t *data, size_t size)
{
  size_t act = 0;

  for (size_t i = 0; i < size; i++) {
    if (FIFO_Content(fifo) == 0) {
      break;
    }
    data[act] = FIFO_GetSingle(fifo);
    act++;
  }

  return act;
}

uint8_t FIFO_GetSingle(Fifo_t *fifo)
{
  uint8_t data = 0;

  CORE_ATOMIC_SECTION(
    if (fifo->content > 0) {
    data = fifo->data[fifo->idxOut];
    fifo->idxOut = (fifo->idxOut + 1) % fifo->size;
    fifo->content--;
  }
    )

  return data;
}

void FIFO_PutMultiple(Fifo_t *fifo, uint8_t *data, size_t count, bool expandCr)
{
  for (uint32_t i = 0; i < count; i++) {
    FIFO_PutSingle(fifo, data[i]);
    if ((expandCr) && (data[i] == '\r')) {
      FIFO_PutSingle(fifo, '\n');
    }
  }
}

void FIFO_PutSingle(Fifo_t *fifo, uint8_t data)
{
  CORE_ATOMIC_SECTION(
    if (fifo->content < fifo->size) {
    fifo->data[fifo->idxInp] = data;
    fifo->idxInp = (fifo->idxInp + 1) % fifo->size;
    fifo->content++;
  } else {
    fifo->overflow = 0x01;
  }
    )
}
