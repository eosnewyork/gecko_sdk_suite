/***************************************************************************//**
 * @file
 * @brief FIFO API definition.
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

#ifndef FIFO_H
#define FIFO_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  size_t size;
  uint8_t *data;
  size_t content;
  size_t idxInp;
  size_t idxOut;
  uint8_t overflow;
} Fifo_t;

void FIFO_Init(Fifo_t *fifo, uint8_t *data, size_t size);
size_t FIFO_Content(Fifo_t *fifo);
size_t FIFO_GetMulti(Fifo_t *fifo, uint8_t *data, size_t size);
uint8_t FIFO_GetSingle(Fifo_t *fifo);
void FIFO_PutMultiple(Fifo_t *fifo, uint8_t *data, size_t count, bool expandCr);
void FIFO_PutSingle(Fifo_t *fifo, uint8_t data);

#ifdef __cplusplus
}
#endif

#endif // FIFO_H
