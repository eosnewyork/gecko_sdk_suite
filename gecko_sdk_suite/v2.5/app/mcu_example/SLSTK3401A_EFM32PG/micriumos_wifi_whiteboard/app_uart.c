/***************************************************************************//**
 * @file
 * @brief Wireless Whiteboard's UART Application Module
 * WGM110 and GLIB demo for the SLSTK3401A running on MicOS.
 * This module initializes the UART to be used by the WiFi module.
 * It also manages the RX interrupts by using a ring buffer.
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

#define APP_UART_MODULE

#include "em_usart.h"
#include "em_cmu.h"
#include "em_gpio.h"

#include "bspconfig.h"

#include "wifi_bglib.h"

#include <kernel/include/os.h>

#include "app_uart.h"

// -----------------------------------------------------------------------------
// Constants

// Receive buffer
#define APP_UART_RX_BUF_MAX_SIZE 256

// -----------------------------------------------------------------------------
// Global Variables

/// Callback function pointer
APP_UART_RxCallback_t appUartRxCallbackPtr;

/// Receive ring buffer read index
static volatile uint16_t appUartRxReadIx;

/// Receive ring buffer write index
static volatile uint16_t appUartRxWriteIx;

/// Receive ring buffer
static volatile uint8_t appUartRxBuf[APP_UART_RX_BUF_MAX_SIZE];

// -----------------------------------------------------------------------------
// Function Prototypes

static void AppUartSetup(void);

/**************************************************************************//**
 * @brief Initialize the UART module.
 * @return Void.
 *****************************************************************************/
void APP_UART_Init(void)
{
  appUartRxReadIx  = 0;
  appUartRxWriteIx = 0;
  APP_UART_RxCount = 0;

  AppUartSetup();
}

/**************************************************************************//**
 * @brief Register a callback function at every received byte.
 * @param callback_ptr Pointer to a callback function that will
 *                     be called every time a Byte is received.
 * @return Void.
 *****************************************************************************/
void APP_UART_RegisterCallback(APP_UART_RxCallback_t callbackPtr)
{
  CPU_SR_ALLOC();

  CPU_CRITICAL_ENTER();

  if (callbackPtr != (APP_UART_RxCallback_t)0) {
    appUartRxCallbackPtr = callbackPtr;
  } else {
    appUartRxCallbackPtr = (APP_UART_RxCallback_t)0;
  }

  CPU_CRITICAL_EXIT();
}

/**************************************************************************//**
 * @brief  Function used to configure the UART.
 * This function will configure the UART to be used by the WiFi module.
 * @return Void.
 *****************************************************************************/
static void AppUartSetup(void)
{
  // Enable peripheral clocks
  CMU_ClockEnable(cmuClock_HFPER, true);

  // Configure GPIO pins
  CMU_ClockEnable(cmuClock_GPIO, true);

  // TX pin, to avoid a false start, configure output as high
  GPIO_PinModeSet(BSP_BCC_TXPORT,
                  BSP_BCC_TXPIN,
                  gpioModePushPull,
                  1);

  // RX pin
  GPIO_PinModeSet(BSP_BCC_RXPORT,
                  BSP_BCC_RXPIN,
                  gpioModeInput,
                  0);

  // Disable VCOM
  GPIO_PinModeSet(BSP_BCC_ENABLE_PORT,
                  BSP_BCC_ENABLE_PIN,
                  gpioModePushPull,
                  0);

  // CTS pin
  GPIO_PinModeSet(gpioPortA, 3, gpioModePushPull, 1);

  // RTS pin
  GPIO_PinModeSet(gpioPortA, 2, gpioModeInput, 0);

  // Base the configuration off the default
  USART_TypeDef           *usart = BSP_BCC_USART;
  USART_InitAsync_TypeDef init   = USART_INITASYNC_DEFAULT;

  CMU_ClockEnable(BSP_BCC_CLK, true);

  // Configure USART for basic async operation
  init.enable = usartDisable;
  USART_InitAsync(usart, &init);

  // Configure routes.
  usart->ROUTELOC0 = USART_ROUTELOC0_RXLOC_LOC0
                     | USART_ROUTELOC0_TXLOC_LOC0
                     | USART_ROUTELOC0_CLKLOC_LOC0
                     | USART_ROUTELOC0_CSLOC_LOC0;

  usart->ROUTELOC1 = USART_ROUTELOC1_CTSLOC_LOC30
                     | USART_ROUTELOC1_RTSLOC_LOC30;

  // Enable pins, and disable any unnecessary pins.
  usart->ROUTEPEN = (USART_ROUTEPEN_RXPEN
                    | USART_ROUTEPEN_TXPEN
                    | USART_ROUTEPEN_CTSPEN
                    | USART_ROUTEPEN_RTSPEN)
                    & (~USART_ROUTEPEN_CLKPEN)
                    & (~USART_ROUTEPEN_CSPEN);

  // Enable CTS and set CTS active low, RTS active low
  usart->CTRLX = USART_CTRLX_CTSEN
                 & (~USART_CTRLX_CTSINV)
                 & (~USART_CTRLX_RTSINV);

  // Set CS active low, TX active high and RX active high
  usart->CTRL &= (~USART_CTRL_CSINV)
                 & (~USART_CTRL_TXINV)
                 & (~USART_CTRL_RXINV);

  // Clear any previous RX interrupts
  USART_IntClear(BSP_BCC_USART, USART_IF_RXDATAV);
  NVIC_ClearPendingIRQ(USART0_RX_IRQn);

  // Configure the interrupt handler
  NVIC_EnableIRQ(USART0_RX_IRQn);
}

/**************************************************************************//**
 * @brief Start the UART.
 * This function is used to start receiving from the UART.
 * @return Void.
 *****************************************************************************/
void APP_UART_Start(void)
{
  // Clear any previous RX interrupts
  USART_IntClear(BSP_BCC_USART, USART_IF_RXDATAV);
  NVIC_ClearPendingIRQ(USART0_RX_IRQn);

  // Enable RX interrupts
  USART_IntEnable(BSP_BCC_USART, USART_IF_RXDATAV);
  NVIC_EnableIRQ(USART0_RX_IRQn);

  // Finally enable the UART module
  USART_Enable(BSP_BCC_USART, usartEnable);
}

/**************************************************************************//**
 * @brief Stop the UART.
 * This function is used to stop receiving from the UART.
 * @return Void.
 *****************************************************************************/
void APP_UART_Stop(void)
{
  USART_Enable(BSP_BCC_USART, usartDisable);
}

/**************************************************************************//**
 * @brief Clear UART buffer.
 * Function used to clear the UART's receive buffer and indexes.
 * @return Void.
 *****************************************************************************/
void APP_UART_ClearBuffer(void)
{
  CPU_SR_ALLOC();

  CPU_CRITICAL_ENTER();
  appUartRxReadIx  = 0;
  appUartRxWriteIx = 0;
  APP_UART_RxCount = 0;
  CPU_CRITICAL_EXIT();
}

/**************************************************************************//**
 * @brief Read byte.
 * This function returns any received byte from the ring buffer.
 * @return Returns one byte from the buffer.
 *****************************************************************************/
char APP_UART_Read(void)
{
  CPU_SR_ALLOC();
  char c;

  c = 0;

  if (APP_UART_RxCount > 0) {
    c = appUartRxBuf[appUartRxReadIx];
    appUartRxReadIx++;
    if (appUartRxReadIx == APP_UART_RX_BUF_MAX_SIZE) {
      appUartRxReadIx = 0;
    }
    CPU_CRITICAL_ENTER();
    APP_UART_RxCount--;
    CPU_CRITICAL_EXIT();
  }

  return c;
}

/**************************************************************************//**
 * @brief Write a byte.
 * This function is used by the BGLIB to write a byte to the UART.
 * @param[in] len1  Data length for buffer data1
 * @param[in] data1 First data buffer to be sent
 * @param[in] len2  Data length for buffer data 2
 * @param[in] data2 Second data buffer to be sent
 *****************************************************************************/
void APP_UART_Write(uint8 len1, uint8* data1, uint16 len2, uint8* data2)
{
  while (len1--) {
    USART_Tx(BSP_BCC_USART, *data1++);
  }
  while (len2--) {
    USART_Tx(BSP_BCC_USART, *data2++);
  }
}

/**************************************************************************//**
 * @brief UART Receive interrupt handler.
 * This function is used to handle the interrupt from
 * the UART at every received byte.
 *****************************************************************************/
void USART0_RX_IRQHandler(void)
{
  CPU_SR_ALLOC();

  CPU_CRITICAL_ENTER();
  // Make the kernel aware of the beginning of the interrupt
  OSIntEnter();
  CPU_CRITICAL_EXIT();

  if (BSP_BCC_USART->STATUS & USART_STATUS_RXDATAV) {
    // Buffer each received byte
    appUartRxBuf[appUartRxWriteIx] = USART_Rx(BSP_BCC_USART);
    appUartRxWriteIx++;
    CPU_CRITICAL_ENTER();
    APP_UART_RxCount++;
    CPU_CRITICAL_EXIT();
    // Wrap around
    if (appUartRxWriteIx == APP_UART_RX_BUF_MAX_SIZE) {
      appUartRxWriteIx = 0;
    }
    // Check for any overflow and flush the buffer if necessary
    if (APP_UART_RxCount > APP_UART_RX_BUF_MAX_SIZE) {
      appUartRxWriteIx = 0;
      appUartRxReadIx  = 0;
      CPU_CRITICAL_ENTER();
      APP_UART_RxCount = 0;
      CPU_CRITICAL_EXIT();
    }
    // Signal that a byte has been received
    if (appUartRxCallbackPtr != (APP_UART_RxCallback_t)0) {
        (*appUartRxCallbackPtr)();
    }
  }

  // Make the kernel aware that the interrupt has ended.
  OSIntExit();
}
