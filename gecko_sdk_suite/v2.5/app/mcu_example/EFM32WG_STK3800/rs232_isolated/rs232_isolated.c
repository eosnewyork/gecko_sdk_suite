/***************************************************************************//**
 * @file
 * @brief RS232 Isolated Example
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
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_leuart.h"
#include "retargetserial.h"
#include "segmentlcd.h"
#include "bspconfig.h"

static volatile uint8_t chara = 'A';

static volatile bool sendChara = true;

/**************************************************************************//**
 * @brief Setup GPIO
 *****************************************************************************/
static void gpioSetup(void)
{
  // Configure pins for push buttons as inputs
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 0);
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInput, 0);

  // Set falling edge interrupt for both ports
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);
  GPIO_IntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, false, true, true);

  // Enable interrupt in core for even and odd gpio interrupts
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);
  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/**************************************************************************//**
 * @brief Common GPIO IRQ handler for both odd and even numbered interrupts
 *****************************************************************************/
static void commonGpioIrqHandler(void)
{
  uint32_t intFlags = GPIO_IntGet();
  GPIO_IntClear(intFlags);

  // Change character according to button pushed
  if (intFlags & (1 << BSP_GPIO_PB0_PIN)) {
    chara--;
  }
  if (intFlags & (1 << BSP_GPIO_PB1_PIN)) {
    chara++;
  }

  // Check if character is out of range
  if (chara > 'Z') {
    chara = 'A';
    putchar('\n'); // New line when wrapping the alphabet
  }

  if (chara < 'A') {
    chara = 'Z';
  }

  sendChara = true;
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler for odd numbered pins
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  commonGpioIrqHandler();
}

/**************************************************************************//**
 * @brief GPIO Interrupt handler for even numbered pins
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  commonGpioIrqHandler();
}

/**************************************************************************//**
 * @brief Main
 *****************************************************************************/
int main(void)
{
  uint8_t rxChara;
  char lcdString[10];

  // Chip errata
  CHIP_Init();

  // Initialize UART and map LF to CRLF
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

  gpioSetup();

  SegmentLCD_Init(false);

  while (1) {
    // Retrieve new character
    rxChara = getchar();

    // Convert lower case to upper case
    if ((rxChara >= 'a') && (rxChara <= 'z')) {
      rxChara = rxChara - 32;
    }

    // Print upper case characters to LCD
    if ((rxChara >= 'A') && (rxChara <= 'Z')) {
      chara = rxChara;

      sprintf(lcdString, "IN %c", chara);
      SegmentLCD_Write(lcdString);
    }

    // Send character if button has been pushed
    if (sendChara) {
      sendChara = false;

      putchar(chara);

      // Print character on LCD
      sprintf(lcdString, "OUT %c", chara);
      SegmentLCD_Write(lcdString);
    }
  }
}
