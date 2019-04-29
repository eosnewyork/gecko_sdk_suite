/***************************************************************************//**
 * @file
 * @brief terminal example for SLSTK3400A_EFM32HG
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

#include <stdio.h>
#include <stdlib.h>
#include "em_chip.h"
#include "em_gpio.h"
#include "bsp.h"
#include "bspconfig.h"
#include "uartdrv.h"
#include "dmadrv.h"
#include "em_dma.h"
#include "em_core.h"
#include "fifo.h"
#include "gpiointerrupt.h"
#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplayconfig.h"
#include "retargettextdisplay.h"

//****************************************************************************

#define STR_INTRO       "GTERM is up.\r\n"
#define STR_BUTTON_0    "button-0 "
#define STR_BUTTON_1    "button-1\r\n"

#define LED_RX          0
#define LED_TX          1

#define EXPAND_CR_TO_CRLF       (true)

// Define receive/transmit operation queues
DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_RX_BUFS, rxBufferQueueI0);
DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS, txBufferQueueI0);

// Configuration for UART interface
#define COM_UART_INIT_DATA                                                                 \
  {                                                                                        \
    BSP_BCC_USART,                                      /* USART port                   */ \
    115200,                                             /* Baud rate                    */ \
    BSP_BCC_LOCATION >> _USART_ROUTE_LOCATION_SHIFT,    /* USART pins location number   */ \
    (USART_Stopbits_TypeDef)USART_FRAME_STOPBITS_ONE,   /* Stop bits                    */ \
    (USART_Parity_TypeDef)USART_FRAME_PARITY_NONE,      /* Parity                       */ \
    (USART_OVS_TypeDef)USART_CTRL_OVS_X16,              /* Oversampling mode            */ \
    false,                                              /* Majority vote disable        */ \
    uartdrvFlowControlNone,                             /* Flow control                 */ \
    gpioPortC,                                          /* CTS port number              */ \
    0,                                                  /* CTS pin number               */ \
    gpioPortC,                                          /* RTS port number              */ \
    1,                                                  /* RTS pin number               */ \
    (UARTDRV_Buffer_FifoQueue_t *)&rxBufferQueueI0,     /* RX operation queue           */ \
    (UARTDRV_Buffer_FifoQueue_t *)&txBufferQueueI0      /* TX operation queue           */ \
  }

#define COM_RX_DMA_BUFFER_COUNT     EMDRV_UARTDRV_MAX_CONCURRENT_RX_BUFS
#define COM_RX_DMA_BUFFER_SIZE      2
#define COM_TX_DMA_BUFFER_SIZE      8

#define COM_RX_FIFO_BUFFER_SIZE     2000

//****************************************************************************

static UARTDRV_HandleData_t     comUartHandleData;
static UARTDRV_Handle_t         comUartHandle = &comUartHandleData;

static uint8_t comRxDmaBuffers[COM_RX_DMA_BUFFER_COUNT][COM_RX_DMA_BUFFER_SIZE];
static uint8_t comTxDmaBuffer[COM_TX_DMA_BUFFER_SIZE];

static uint8_t comRxFifoBuffer[COM_RX_FIFO_BUFFER_SIZE];
static Fifo_t  comRxFifoHandle;

static bool     buttonPressed0 = false;
static bool     buttonPressed1 = false;

//****************************************************************************

static void comRxCallback(struct UARTDRV_HandleData *handle,
                          Ecode_t transferStatus,
                          uint8_t *data,
                          UARTDRV_Count_t transferCount)
{
  (void)handle;

  BSP_LedSet(LED_RX);

  EFM_ASSERT(transferStatus == ECODE_OK);
  EFM_ASSERT(transferCount == COM_RX_DMA_BUFFER_SIZE);

  FIFO_PutMultiple(&comRxFifoHandle, data, transferCount, EXPAND_CR_TO_CRLF);
  UARTDRV_Receive(comUartHandle, data, COM_RX_DMA_BUFFER_SIZE, comRxCallback);
}

static void comInit(void)
{
  UARTDRV_Init_t initDataUart = COM_UART_INIT_DATA;
  Ecode_t eSta;

  // Enable vcom
  GPIO_PinModeSet(BSP_BCC_ENABLE_PORT, BSP_BCC_ENABLE_PIN, gpioModePushPull, 1);

  // Init uart driver
  eSta = UARTDRV_Init(comUartHandle, &initDataUart);
  if (ECODE_OK != eSta) {
    while (1) ;
  }

  // Feed the driver with input buffers
  for (size_t i = 0; i < COM_RX_DMA_BUFFER_COUNT; i++) {
    eSta = UARTDRV_Receive(comUartHandle, &comRxDmaBuffers[i][0], COM_RX_DMA_BUFFER_SIZE, comRxCallback);
    if (ECODE_OK != eSta) {
      while (1) ;
    }
  }
}

static size_t comGets(char *strPtr, size_t strSiz)
{
  size_t act;

  act = FIFO_GetMulti(&comRxFifoHandle, (uint8_t *)strPtr, strSiz - 1);
  strPtr[act] = '\0';

  if (act > 0) {
    BSP_LedClear(LED_RX);
  }

  return act;
}

static void comPuts(const char *pStr)
{
  BSP_LedSet(LED_TX);
  UARTDRV_TransmitB(comUartHandle, (uint8_t *)pStr, strlen(pStr));
  BSP_LedClear(LED_TX);
}

/***************************************************************************//**
 * @brief  Gpio callback
 * @param  pin - pin which triggered interrupt
 ******************************************************************************/
static void gpioCallback(uint8_t pin)
{
  if (pin == BSP_GPIO_PB0_PIN) {
    buttonPressed0 = true;
  } else if (pin == BSP_GPIO_PB1_PIN) {
    buttonPressed1 = true;
  }
}

/***************************************************************************//**
 * @brief  Gpio init. Initialize button pins to trigger falling edge interrupts.
 *  Register callbacks for that interrupts.
 ******************************************************************************/
static void gpioInit(void)
{
  // Enable GPIO in CMU
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Initialize GPIO interrupt dispatcher
  GPIOINT_Init();

  // Configure PB0 and PB1 as input
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 0);
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInput, 0);

  // Register callbacks before setting up and enabling pin interrupt.
  GPIOINT_CallbackRegister(BSP_GPIO_PB0_PIN, gpioCallback);
  GPIOINT_CallbackRegister(BSP_GPIO_PB1_PIN, gpioCallback);

  // Set falling edge interrupt for both ports
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);
  GPIO_IntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, false, true, true);
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  DISPLAY_Init();
  /* Retarget stdio to a text display. */
  if (RETARGET_TextDisplayInit() != TEXTDISPLAY_EMSTATUS_OK) {
    while (1) ;
  }

  BSP_LedsInit();
  FIFO_Init(&comRxFifoHandle, comRxFifoBuffer, COM_RX_FIFO_BUFFER_SIZE);
  comInit();
  gpioInit();

  printf(STR_INTRO);
  comPuts(STR_INTRO);

  while (1) {
    if (buttonPressed0) {
      buttonPressed0 = false;
      comPuts(STR_BUTTON_0);
    }
    if (buttonPressed1) {
      buttonPressed1 = false;
      comPuts(STR_BUTTON_1);
    }

    if (comGets((char *)comTxDmaBuffer, sizeof(comTxDmaBuffer))) {
      printf((char *)comTxDmaBuffer);
    }
  }
}
