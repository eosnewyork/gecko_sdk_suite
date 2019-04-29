/***************************************************************************//**
 * @file
 * @brief QSPI indirect example
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
#include "qspi.h"
#include "em_assert.h"
#include "em_qspi.h"
#include "em_core.h"
#include <stdlib.h>
#include <string.h>

#define QSPI_FIFO            (QSPI0_MEM_BASE | 0x0F000000UL)
#define QSPI_FIFO_SIZE        1024
#define QSPI_FIFO_READ_PART    512
#define QSPI_FIFO_WRITE_PART   512
#define QSPI_QUEUE_SIZE          8
#define QSPI_STATE_READY         1
#define QSPI_STATE_SCHEDULED     2
#define QSPI_STATE_DONE          3

typedef struct {
  uint32_t addr;
  size_t size;
  const void *buffer;
  uint8_t state;
} QSPI_WriteOperation_t;

typedef struct {
  uint32_t addr;
  size_t size;
  void *buffer;
  uint8_t state;
} QSPI_ReadOperation_t;

static QSPI_WriteOperation_t writeQueue[QSPI_QUEUE_SIZE];
static QSPI_ReadOperation_t readQueue[QSPI_QUEUE_SIZE];
static int writeHead = 0;
static int writeTail = 0;
static int readHead = 0;
static int readTail = 0;
volatile uint32_t *qspiFifo = (volatile uint32_t *)QSPI_FIFO;

static void QSPI_ReadFifo(int id);
static void QSPI_WriteFifo(int id);
static uint32_t QSPI_FifoAvailable(void);
static uint32_t QSPI_FifoSpace(void);
static void QSPI_TriggerReadIndirect(int id);
static void QSPI_TriggerWriteIndirect(int id);

/***************************************************************************//**
 * @brief QSPI interrupt handler
 *
 * @detail The interrupt handler will handle different events from the QSPI
 *   hardware module. When a read or write operation is done then the next
 *   operation in the queue will be started. When the read partition of the
 *   fifo crosses the watermark level then data is copied from the fifo and
 *   into the destination buffer, and if the write partition of the fifo
 *   crosses the watermark level then data will be copied from the source
 *   buffer into the hardware fifo.
 ******************************************************************************/
void QSPI0_IRQHandler(void)
{
  uint32_t flags = QSPI0->IRQSTATUS;
  QSPI0->IRQSTATUS |= flags;

  if (flags & QSPI_IRQSTATUS_INDIRECTOPDONE) {
    // Handle completed write operation
    if ((QSPI0->INDIRECTWRITEXFERCTRL & QSPI_INDIRECTWRITEXFERCTRL_INDOPSDONESTATUS) != 0U) {
      QSPI0->INDIRECTWRITEXFERCTRL |= _QSPI_INDIRECTWRITEXFERCTRL_INDOPSDONESTATUS_MASK;
      writeQueue[writeTail].state = QSPI_STATE_DONE;
      writeTail++;
      if (writeTail >= QSPI_QUEUE_SIZE) {
        writeTail = 0;
      }

      // Trigger next write operation if it's ready
      if (writeQueue[writeTail].state == QSPI_STATE_READY) {
        QSPI_TriggerWriteIndirect(writeTail);
      }
    }

    // Handle completed read operation
    if ((QSPI0->INDIRECTREADXFERCTRL & QSPI_INDIRECTREADXFERCTRL_INDOPSDONESTATUS) != 0U) {
      QSPI0->INDIRECTREADXFERCTRL |= _QSPI_INDIRECTREADXFERCTRL_INDOPSDONESTATUS_MASK;
      readQueue[readTail].state = QSPI_STATE_DONE;
      readTail++;
      if (readTail >= QSPI_QUEUE_SIZE) {
        readTail = 0;
      }

      if (readQueue[readTail].state == QSPI_STATE_READY) {
        QSPI_TriggerReadIndirect(readTail);
      }
    }
  }

  if (flags & QSPI_IRQSTATUS_INDIRECTXFERLEVELBREACH) {
    // Watermark interrupt triggered and write is in progress fill fifo
    if ((QSPI0->INDIRECTWRITEXFERCTRL & QSPI_INDIRECTWRITEXFERCTRL_WRSTATUS) != 0U) {
      QSPI_WriteFifo(writeTail);
    }
    // Watermark interrupt triggered and read is in progress empty fifo
    if ((QSPI0->INDIRECTREADXFERCTRL & QSPI_INDIRECTREADXFERCTRL_RDSTATUS) != 0U) {
      QSPI_ReadFifo(readTail);
    }
  }

  if (flags & QSPI_IRQSTATUS_INDIRECTREADREJECT) {
    EFM_ASSERT(false);
  }
}

/***************************************************************************//**
 * @brief Get the number of free space available in the write partition of
 *   the fifo.
 *
 * @return
 *   number of bytes available in the write partition of the fifo.
 ******************************************************************************/
static uint32_t QSPI_FifoSpace(void)
{
  uint32_t n = (QSPI0->SRAMFILL & _QSPI_SRAMFILL_SRAMFILLINDACWRITE_MASK)
               >> _QSPI_SRAMFILL_SRAMFILLINDACWRITE_SHIFT;
  return QSPI_FIFO_WRITE_PART - (n * 4);
}

/***************************************************************************//**
 * @brief Get the number of bytes available in the read partition of the fifo
 *
 * @return
 *   Number of bytes available for reading from the fifo.
 ******************************************************************************/
static uint32_t QSPI_FifoAvailable(void)
{
  uint32_t n = (QSPI0->SRAMFILL & _QSPI_SRAMFILL_SRAMFILLINDACREAD_MASK)
               >> _QSPI_SRAMFILL_SRAMFILLINDACREAD_SHIFT;
  return n * 4;
}

/***************************************************************************//**
 * @brief Fill the fifo with data from an application buffer.
 *
 * @param[in] id
 *   The id of a write operation in the queue.
 ******************************************************************************/
static void QSPI_WriteFifo(int id)
{
  EFM_ASSERT(writeQueue[id].state == QSPI_STATE_SCHEDULED);

  const uint32_t *src = writeQueue[id].buffer;

  // Copy 32 bit words to the fifo
  while ((QSPI_FifoSpace() > 0) && (writeQueue[id].size >= 4)) {
    *qspiFifo = *src++;
    writeQueue[id].size -= 4;
  }

  // When there is 1-3 bytes left to write then handle it here
  if (writeQueue[id].size > 0 && writeQueue[id].size < 4) {
    *qspiFifo = *src++;
    writeQueue[id].size = 0;
  }
  writeQueue[id].buffer = src;
}

/***************************************************************************//**
 * @brief Read data from the fifo into an application buffer.
 *
 * @param[in] id
 *   id of a read operation in the queue.
 ******************************************************************************/
static void QSPI_ReadFifo(int id)
{
  EFM_ASSERT(readQueue[id].state == QSPI_STATE_SCHEDULED);

  uint32_t *dst = readQueue[id].buffer;

  // Read 32 bit words from the fifo
  while ((QSPI_FifoAvailable() > 0) && (readQueue[id].size >= 4)) {
    *dst++ = *qspiFifo;
    readQueue[id].size -= 4;
  }

  // When there is 1-3 bytes left to read then handle it here
  if ((readQueue[id].size > 0) && (readQueue[id].size < 4)) {
    uint32_t value = *qspiFifo;
    memcpy((uint8_t *)dst, (uint8_t *)&value, readQueue[id].size);
    readQueue[id].size = 0;
  }
  readQueue[id].buffer = dst;
}

/***************************************************************************//**
 * @brief Initialize QSPI for indirect access.
 *
 * @detail
 *   This function must be called one time before any calls to
 *   QSPI_WriteIndirect() or QSPI_ReadIndirect(). This function will modify
 *   configuration registers of the QSPI in order to make it ready for indirect
 *   operations.
 ******************************************************************************/
void QSPI_InitIndirect(void)
{
  // Enable indirect access by disabling direct access
  QSPI0->CONFIG = QSPI0->CONFIG & ~_QSPI_CONFIG_ENBDIRACCCTLR_MASK;

  // Configure QSPI FIFO address
  QSPI0->INDAHBADDRTRIGGER = (uint32_t)qspiFifo;

  // Setup the indirect transfers AHB trigger address range
  QSPI0->INDIRECTTRIGGERADDRRANGE = 0xF;

  // Set the watermark registers
  QSPI0->INDIRECTWRITEXFERWATERMARK = 0x100;
  QSPI0->INDIRECTREADXFERWATERMARK  = 0x10;

  // Partition the FIFO into 512 bytes read and 512 bytes write
  QSPI0->SRAMPARTITIONCFG = QSPI_FIFO_READ_PART / 4;

  // Enable completion and watermark interrupt
  QSPI0->IRQMASK = QSPI_IRQMASK_INDIRECTOPDONEMASK
                   | QSPI_IRQMASK_INDIRECTXFERLEVELBREACHMASK
                   | QSPI_IRQMASK_INDIRECTREADREJECTMASK;

  for (int i = 0; i < QSPI_QUEUE_SIZE; i++) {
    writeQueue[i].state = QSPI_STATE_DONE;
    readQueue[i].state = QSPI_STATE_DONE;
  }
}

/***************************************************************************//**
 * @brief Write data to the external flash.
 *
 * @param[in] buffer
 *   A buffer containing the data to be transfered to the external flash. Note
 *   that the application has responsibility to not modify the buffer while
 *   an operation is in progress.
 *
 * @param[in] addr
 *   The flash address to write data to.
 *
 * @param[in] n
 *   Number of bytes to transfer.
 *
 * @return id
 *   When a write operation is enqueued succesfully the return value will be an
 *   integer >= 0. The return value is the id of the write operation, this id
 *   can be used later to wait for completion of the transfer.
 *   The return value will be -1 if there is no more space in the write queue.
 ******************************************************************************/
int QSPI_WriteIndirect(const void * buffer, uint32_t addr, size_t n)
{
  int id = writeHead;
  CORE_irqState_t irqState;

  // check if there is room in the queue
  if (writeQueue[id].state != QSPI_STATE_DONE) {
    return -1;
  }

  irqState = CORE_EnterCritical();
  // Save information about the operation in an array
  writeQueue[id].addr = addr;
  writeQueue[id].size = n;
  writeQueue[id].buffer = buffer;
  writeQueue[id].state = QSPI_STATE_READY;

  // Check that we do not have an indirect write in progress already
  if ((QSPI0->INDIRECTWRITEXFERCTRL & _QSPI_INDIRECTWRITEXFERCTRL_WRSTATUS_MASK) == 0) {
    // Can safely trigger operation to be started
    QSPI_TriggerWriteIndirect(id);
  }
  CORE_ExitCritical(irqState);

  // Move to next element in the queue
  writeHead++;
  if (writeHead >= QSPI_QUEUE_SIZE) {
    writeHead = 0;
  }

  return id;
}

/***************************************************************************//**
 * @brief Trigger indirect write operation.
 *
 * @param[in] id
 *   Id of the write operation to start.
 ******************************************************************************/
static void QSPI_TriggerWriteIndirect(int id)
{
  EFM_ASSERT(writeQueue[id].state == QSPI_STATE_READY);

  writeQueue[id].state = QSPI_STATE_SCHEDULED;

  // Setup indirect transfer FLASH start address
  QSPI0->INDIRECTWRITEXFERSTART = writeQueue[id].addr;

  // Setup the number of bytes to be transferred
  QSPI0->INDIRECTWRITEXFERNUMBYTES = writeQueue[id].size;

  // Start the indirect write operation
  QSPI0->INDIRECTWRITEXFERCTRL = QSPI_INDIRECTWRITEXFERCTRL_START;

  // Fill the QSPI fifo
  QSPI_WriteFifo(id);
}

/***************************************************************************//**
 * @brief Read data from the external flash.
 *
 * @param[out] buffer
 *   Pointer to a buffer where the data from the external flash will be placed.
 *
 * @param[in] addr
 *   The flash address to read from.
 *
 * @param[in] n
 *   Number of bytes to read.
 *
 * @return id
 *   When a read operation is enqueued succesfully the return value will be an
 *   integer >= 0. The return value is the id of the read operation.
 *   The return value will be -1 if there is no more space in the read queue.
 ******************************************************************************/
int QSPI_ReadIndirect(void * buffer, uint32_t addr, size_t n)
{
  int id = readHead;
  CORE_irqState_t irqState;

  // check if there is room in the queue
  if (readQueue[id].state != QSPI_STATE_DONE) {
    return -1;
  }

  irqState = CORE_EnterCritical();
  // Save information about the operation in an array
  readQueue[id].addr = addr;
  readQueue[id].size = n;
  readQueue[id].buffer = buffer;
  readQueue[id].state = QSPI_STATE_READY;

  // Check that we do not have an indirect read in progress already
  if ((QSPI0->INDIRECTREADXFERCTRL & _QSPI_INDIRECTREADXFERCTRL_RDSTATUS_MASK) == 0) {
    QSPI_TriggerReadIndirect(id);
  }
  CORE_ExitCritical(irqState);

  // Move to next element in the queue
  readHead++;
  if (readHead >= QSPI_QUEUE_SIZE) {
    readHead = 0;
  }

  return id;
}

/***************************************************************************//**
 * @brief Trigger indirect read operation
 *
 * @param[in] id
 *   The id of a read operation from the queue.
 ******************************************************************************/
static void QSPI_TriggerReadIndirect(int id)
{
  EFM_ASSERT(readQueue[id].state == QSPI_STATE_READY);

  readQueue[id].state = QSPI_STATE_SCHEDULED;

  // Setup indirect transfer FLASH start address
  QSPI0->INDIRECTREADXFERSTART = readQueue[id].addr;

  // Setup the number of bytes to be transferred
  QSPI0->INDIRECTREADXFERNUMBYTES = readQueue[id].size;

  // Trigger indirect read access by setting Indirect Read transfer control register bit
  QSPI0->INDIRECTREADXFERCTRL = QSPI_INDIRECTREADXFERCTRL_START;
}

/***************************************************************************//**
 * @brief Wait for one specific read transaction to complete.
 *
 * @param[in] id
 *   The id of the read transaction to wait for.
 ******************************************************************************/
void QSPI_WaitRead(int id)
{
  EFM_ASSERT((id >= 0) && (id < QSPI_QUEUE_SIZE));

  bool done = false;

  while (!done) {
    done = readQueue[id].state == QSPI_STATE_DONE;
  }
}

/***************************************************************************//**
 * @brief Wait for one specific write transaction to complete.
 *
 * @param[in] id
 *   The id of the write transaction to wait for.
 ******************************************************************************/
void QSPI_WaitWrite(int id)
{
  EFM_ASSERT((id >= 0) && (id < QSPI_QUEUE_SIZE));

  bool done = false;

  while (!done) {
    done = writeQueue[id].state == QSPI_STATE_DONE;
  }
}

/***************************************************************************//**
 * @brief Wait for all read and write operations to complete
 ******************************************************************************/
void QSPI_Wait(void)
{
  bool done = false;

  while (!done) {
    done = ((QSPI0->INDIRECTWRITEXFERCTRL & QSPI_INDIRECTWRITEXFERCTRL_WRSTATUS) == 0U)
           && ((QSPI0->INDIRECTREADXFERCTRL & QSPI_INDIRECTREADXFERCTRL_RDSTATUS) == 0U);
  }
}
