/***************************************************************************//**
 * @file
 * @brief Wireless Whiteboard's UART Application Module
 * WGM110 and GLIB demo for the SLSTK3701A running on MicOS.
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

#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"

#include "wifi_bglib.h"

#include <kernel/include/os.h>

#include "app_uart.h"

// -----------------------------------------------------------------------------
// Constants

// Ports and Pins
#define APP_UART_INSTANCE      USART5
#define APP_UART_TX_IRQN       USART5_TX_IRQn
#define APP_UART_RX_IRQN       USART5_RX_IRQn
#define APP_UART_TX_PORT       gpioPortE
#define APP_UART_TX_PIN        8
#define APP_UART_RX_PORT       gpioPortE
#define APP_UART_RX_PIN        9
#define APP_UART_CTS_PORT      gpioPortA
#define APP_UART_CTS_PIN       12
#define APP_UART_RTS_PORT      gpioPortA
#define APP_UART_RTS_PIN       13
#define APP_UART_VCOM_EN_PORT  gpioPortE
#define APP_UART_VCOM_EN_PIN   1

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
  USART_Reset(APP_UART_INSTANCE);

  // Enable peripheral clocks
  CMU_ClockEnable(cmuClock_HFPER, true);

  // Configure GPIO pins
  CMU_ClockEnable(cmuClock_GPIO, true);

  // To avoid a false start, configure output as high
  // Tx
  GPIO_PinModeSet(APP_UART_TX_PORT,
                  APP_UART_TX_PIN,
                  gpioModePushPull,
                  1);
  // Rx
  GPIO_PinModeSet(APP_UART_RX_PORT,
                  APP_UART_RX_PIN,
                  gpioModeInput,
                  0);

  // CTS pin (software flow control)
  GPIO_PinModeSet(APP_UART_CTS_PORT,
                  APP_UART_CTS_PIN,
                  gpioModeInput,
                  0);

  // RTS pin (software flow control)
  GPIO_PinModeSet(APP_UART_RTS_PORT,
                  APP_UART_RTS_PIN,
                  gpioModePushPull,
                  1);

  // Disable VCOM
  GPIO_PinModeSet(APP_UART_VCOM_EN_PORT,
                  APP_UART_VCOM_EN_PIN,
                  gpioModePushPull,
                  0);

  // Base the configuration off the default
  USART_TypeDef *usart = APP_UART_INSTANCE;
  USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;

  CMU_ClockEnable(cmuClock_USART5, true);

  // Configure USART for basic async operation
  init.enable = usartDisable;
  USART_InitAsync(usart, &init);

  // Configure routes.
  usart->ROUTELOC0 = USART_ROUTELOC0_RXLOC_LOC0
                     | USART_ROUTELOC0_TXLOC_LOC0;

  // Enable pins.
  usart->ROUTEPEN = USART_ROUTEPEN_RXPEN
                    | USART_ROUTEPEN_TXPEN;

  // Clear any previous RX and TX interrupts
  USART_IntClear(APP_UART_INSTANCE, USART_IF_RXDATAV);
  USART_IntClear(APP_UART_INSTANCE, USART_IF_TXBL);
  NVIC_ClearPendingIRQ(APP_UART_TX_IRQN);
  NVIC_ClearPendingIRQ(APP_UART_RX_IRQN);

  // Configure the interrupt handlers
  NVIC_EnableIRQ(APP_UART_TX_IRQN);
  NVIC_EnableIRQ(APP_UART_RX_IRQN);
}

/**************************************************************************//**
 * @brief Start the UART.
 * This function is used to start receiving from the UART.
 * @return Void.
 *****************************************************************************/
void APP_UART_Start(void)
{
  // Clear any previous RX and TX interrupts
  USART_IntClear(APP_UART_INSTANCE, USART_IF_RXDATAV);
  USART_IntClear(APP_UART_INSTANCE, USART_IF_TXBL);;
  NVIC_ClearPendingIRQ(APP_UART_TX_IRQN);
  NVIC_ClearPendingIRQ(APP_UART_RX_IRQN);

  // Enable RX interrupt and IRQ for both TX and RX
  USART_IntEnable(APP_UART_INSTANCE, USART_IF_RXDATAV);
  NVIC_EnableIRQ(APP_UART_TX_IRQN);
  NVIC_EnableIRQ(APP_UART_RX_IRQN);

  // Finally enable the UART module
  USART_Enable(APP_UART_INSTANCE, usartEnable);
}

/**************************************************************************//**
 * @brief Stop the UART.
 * This function is used to stop receiving from the UART.
 * @return Void.
 *****************************************************************************/
void APP_UART_Stop(void)
{
  USART_Enable(APP_UART_INSTANCE, usartDisable);
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
  unsigned int ctsLine;

  while (len1--) {
    // Check if the CTS line is asserted before sending a byte (active low)
    ctsLine = GPIO_PinInGet(APP_UART_CTS_PORT, APP_UART_CTS_PIN);
    while (ctsLine) {
      // Wait until the CTS line gets asserted (active low)
      ctsLine = GPIO_PinInGet(APP_UART_CTS_PORT, APP_UART_CTS_PIN);
    }
    USART_Tx(APP_UART_INSTANCE, *data1++);
  }

  while (len2--) {
    // Check if the CTS line is asserted before sending a byte (active low)
    ctsLine = GPIO_PinInGet(APP_UART_CTS_PORT, APP_UART_CTS_PIN);
    while (ctsLine) {
      // Wait until the CTS line gets asserted (active low)
      ctsLine = GPIO_PinInGet(APP_UART_CTS_PORT, APP_UART_CTS_PIN);
    }
    USART_Tx(APP_UART_INSTANCE, *data2++);
  }

  // Enable TX interrupt to assert RTS when transmission is complete
  USART_IntEnable(APP_UART_INSTANCE, USART_IF_TXBL);
}

/**************************************************************************//**
 * @brief UART Receive interrupt handler.
 * This function is used to handle the interrupt from
 * the UART at every received byte.
 *****************************************************************************/
void USART5_RX_IRQHandler(void)
{
  CPU_SR_ALLOC();

  CPU_CRITICAL_ENTER();
  // Make the kernel aware of the beginning of the interrupt
  OSIntEnter();
  CPU_CRITICAL_EXIT();

  // Valid data received interrupt
  if (APP_UART_INSTANCE->IF & USART_IF_RXDATAV) {
    // Buffer each received byte
    appUartRxBuf[appUartRxWriteIx] = USART_Rx(APP_UART_INSTANCE);
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

/**************************************************************************//**
 * @brief UART Transmit interrupt handler.
 * This function is used to handle the interrupt from
 * the UART at transmission complete.
 *****************************************************************************/
void USART5_TX_IRQHandler(void)
{
  CPU_SR_ALLOC();

  CPU_CRITICAL_ENTER();
  // Make the kernel aware of the beginning of the interrupt
  OSIntEnter();
  CPU_CRITICAL_EXIT();

  // Transmit data complete interrupt
  if (APP_UART_INSTANCE->IF & USART_IF_TXBL) {
    USART_IntDisable(APP_UART_INSTANCE, USART_IF_TXBL);
    // Assert the RTS line (active low)
    GPIO_PinOutClear(APP_UART_RTS_PORT, APP_UART_RTS_PIN);
  }

  // Make the kernel aware that the interrupt has ended.
  OSIntExit();
}
