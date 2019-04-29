/***************************************************************************//**
 * @file
 * @brief CAN Bus demo
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
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_can.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "glib.h"
#include "display.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#define GLIB_FONT_WIDTH           (glibContext.font.fontWidth \
                                   + glibContext.font.charSpacing)
#define GLIB_FONT_HEIGHT          (glibContext.font.fontHeight)

#define MAX_STR_LEN               48

// CAN bus interfaces
#define CAN_TX_IF                 0
#define CAN_RX_IF                 1

/*******************************************************************************
 *****************************   Variables   ***********************************
 ******************************************************************************/

// GLIB_Context
static GLIB_Context_t glibContext;

// String to be shown on the display
static char strOnDisplay[MAX_STR_LEN];

// BTN0 interrupt flag
static volatile bool btn0Pressed = false;
// BTN1 interrupt flag
static volatile bool btn1Pressed = false;

// message arrival flag on CAN0
static volatile bool CAN0Received = false;
// message arrival flag on CAN1
static volatile bool CAN1Received = false;

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/

// CAN action counter
typedef struct {
  // Number of transmission from CAN0
  uint32_t txZero;
  // Number of reception by CAN0
  uint32_t rxZero;
  // Number of transmission from CAN1
  uint32_t txOne;
  // Number of reception by CAN1
  uint32_t rxOne;
} CANStat_t;

/***************************************************************************//**
 * @brief GRIO_EVEN_IRQHandler
 * Interrupt Service Routine for GPIO interrupt
 ******************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  GPIO_IntClear(0x1 << BSP_GPIO_PB0_PIN);
  btn0Pressed = true;
}

/***************************************************************************//**
 * @brief GRIO_ODD_IRQHandler
 * Interrupt Service Routine for GPIO interrupt
 ******************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  GPIO_IntClear(0x1 << BSP_GPIO_PB1_PIN);
  btn1Pressed = true;
}

/***************************************************************************//**
 * @brief CANn_IRQHandler
 * Interrupt Service Routine for CAN interrupt
 ******************************************************************************/
void CAN0_IRQHandler(void)
{
  if (CAN0->STATUS & CAN_STATUS_RXOK) {
    CAN0Received = true;
  }
  CAN0->STATUS = 0x0;
  CAN_MessageIntClear(CAN0, 0xFFFFFFFF);
}

/***************************************************************************//**
 * @brief CANn_IRQHandler
 * Interrupt Service Routine for CAN interrupt
 ******************************************************************************/
void CAN1_IRQHandler(void)
{
  if (CAN1->STATUS & CAN_STATUS_RXOK) {
    CAN1Received = true;
  }
  CAN1->STATUS = 0x0;
  CAN_MessageIntClear(CAN1, 0xFFFFFFFF);
}

/***************************************************************************//**
 * @brief initialization of GPIO interrupt for BTN0 or BTN1
 * @param buttonNr: BTN0 or BTN1
 ******************************************************************************/
static void setUpBTN(uint8_t buttonNr)
{
  if (buttonNr == 0 || buttonNr == 1) {
    CMU_ClockEnable(cmuClock_GPIO, true);

    GPIO_Unlock();
    GPIO_IntClear(0x1 << (BSP_GPIO_PB0_PIN + buttonNr));

    if (!buttonNr) {
      NVIC_EnableIRQ(GPIO_EVEN_IRQn);
    } else {
      NVIC_EnableIRQ(GPIO_ODD_IRQn);
    }

    GPIO_PinModeSet(gpioPortC, BSP_GPIO_PB0_PIN + buttonNr, gpioModeInput, 0);
    GPIO_ExtIntConfig(gpioPortC, BSP_GPIO_PB0_PIN + buttonNr, BSP_GPIO_PB0_PIN
                      + buttonNr, false, true, true);
  }
}

/***************************************************************************//**
 *  @brief  Draw text function using GLIB example
 *
 *  @param text
 *  Pointer to the string that is drawn
 *
 *  @param x0
 *  Start x-coordinate for the string (Upper left corner)
 *
 *  @param y0
 *  Start y-coordinate for the string (Upper left corner)
 ******************************************************************************/
static void writeTextOnTheDisplay(const char *text, int32_t x0, int32_t y0)
{
  // Print text on the display
  GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNormal8x8);
  GLIB_drawString(&glibContext,
                  text,
                  strlen(text),
                  x0,
                  y0,
                  0);
}

/***************************************************************************//**
 * @brief initialization of CAN
 *
 * @param can: CAN0 or CAN1
 *
 * @param mode: Operating mode
 ******************************************************************************/
static void setUpCAN(CAN_TypeDef *can, CAN_Mode_TypeDef mode)
{
  // Enable the clocks
  CMU_ClockEnable(cmuClock_HFPER, true);

  if (can == CAN0) {
    NVIC_EnableIRQ(CAN0_IRQn);

    // Enable the CANn clock
    CMU_ClockEnable(cmuClock_CAN0, true);

    // Reset the CAN device
    CAN_Reset(can);
    CAN_Enable(can, false);

    // Set the gpio for rx (X) and tx (Y)
    GPIO_PinModeSet(gpioPortC, 0, gpioModeInput, 0);
    GPIO_PinModeSet(gpioPortC, 1, gpioModePushPull, 0);

    // Set the ROUTE to match the pins
    CAN_SetRoute(can, true, 0, 0);
  } else if (can == CAN1) {
    NVIC_EnableIRQ(CAN1_IRQn);

    CMU_ClockEnable(cmuClock_CAN1, true);

    CAN_Reset(can);
    CAN_Enable(can, false);

    GPIO_PinModeSet(gpioPortA, 12, gpioModeInput, 0);
    GPIO_PinModeSet(gpioPortA, 13, gpioModePushPull, 0);
    CAN_SetRoute(can, true, 5, 5);
  }

  // Reset Message Objects
  CAN_ResetMessages(can, 0);
  CAN_ResetMessages(can, 1);

  // Set the bit timing to get a bitrate of 500 kbit/s
  // The bitrate MUST be chosen based on the configuration of the clock.
  // The desired bitrate might be unreachable depending on the clock frequency.
  CAN_SetBitTiming(can, 500000, 6, 7, 2, 1);

  // Set the CAN device mode
  CAN_SetMode(can, mode);

  // Enable CAN
  CAN_Enable(can, true);
}

/***************************************************************************//**
 *  @brief  Show CAN actions on the display
 *
 *  @param stat
 *  is CAN action struct consisting the number of transmissions and receptions
 *
 *  @param message is a message object used to send/receive data frame
 ******************************************************************************/
static void actionDisplay(CANStat_t stat,
                          CAN_MessageObject_TypeDef *message)
{
  // Clear screen
  GLIB_clear(&glibContext);

  // Show CAN mode on the display
  snprintf(strOnDisplay, MAX_STR_LEN, "CAN DEMO");
  writeTextOnTheDisplay(strOnDisplay, 5, 5);

  // Show CAN0 actions on the display
  snprintf(strOnDisplay, MAX_STR_LEN, "CAN0\nRX: %ld\nTX: %ld\n",
           stat.rxZero, stat.txZero);
  writeTextOnTheDisplay(strOnDisplay, 5, 5 + (2 * GLIB_FONT_HEIGHT));

  // Show CAN1 actions on the display
  snprintf(strOnDisplay, MAX_STR_LEN, "CAN1\nRX: %ld\nTX: %ld\n",
           stat.rxOne, stat.txOne);
  writeTextOnTheDisplay(strOnDisplay, 5, 5 + (6 * GLIB_FONT_HEIGHT));

  // Show the message content on the display
  if (message->data[0] != '\0') {
    snprintf(strOnDisplay, MAX_STR_LEN, "Message:\n%s\n", message->data);
    writeTextOnTheDisplay(strOnDisplay, 5, 5 + (10 * GLIB_FONT_HEIGHT));
  }
  // Update display
  DMD_updateDisplay();
}

/***************************************************************************//**
 *  @brief  CAN message config function
 *
 *  @param can is CAN peripheral
 *  @param message is a message object
 *  @param msgNum is the message object number
 *  @param id is the message id
 *  @param dlc is the message length
 *  @param mask is the message mask
 *  @param tx is true if the message object is used for transmission
 ******************************************************************************/
void configMessageObj(CAN_TypeDef *can,
                      CAN_MessageObject_TypeDef *message,
                      uint8_t msgNum,
                      uint8_t id,
                      uint8_t dlc,
                      uint32_t mask,
                      bool tx)
{
  message->msgNum = msgNum;
  message->id = id;
  message->dlc = dlc;
  message->mask = mask;

  CAN_ConfigureMessageObject(can, CAN_TX_IF, message->msgNum,
                             true, tx, false, true, true);

  if (tx) {
    // Configure valid, tx/rx, remoteTransfer for the given Message Object
    CAN_ConfigureMessageObject(can, CAN_TX_IF, message->msgNum,
                               true, tx, false, true, true);
  } else {
    // Configure valid, tx/rx, remoteTransfer for the given Message Object
    CAN_ConfigureMessageObject(can, CAN_RX_IF, message->msgNum,
                               true, tx, false, true, true);
    // Set filter to receive any message with any id
    CAN_SetIdAndFilter(can, CAN_RX_IF, true, message, true);
  }
}

/***************************************************************************//**
 *  @brief  CAN demo function:
 *          two CAN devices needs to be connected
 ******************************************************************************/
static void runCANDemo(void)
{
  uint32_t i;

  // Initialize CAN_ActionCounter
  CANStat_t stat;
  stat.rxZero = 0, stat.rxOne = 0;
  stat.txZero = 0, stat.txOne = 0;

  // Initialize CAN peripherals
  CAN_Mode_TypeDef mode = canModeNormal;
  setUpCAN(CAN0, mode);
  setUpCAN(CAN1, mode);

  // Initialize a message using 5th Message Object in the RAM to send
  CAN_MessageObject_TypeDef message;
  configMessageObj(CAN0, &message, 5, 1, 8, 0, true);
  configMessageObj(CAN1, &message, 5, 1, 8, 0, true);

  // Initialize a message using 6th Message Object in the RAM for reception
  CAN_MessageObject_TypeDef receiver;
  configMessageObj(CAN0, &receiver, 6, 0, 8, 0, false);
  configMessageObj(CAN1, &receiver, 6, 0, 8, 0, false);

  // Initial display
  GLIB_clear(&glibContext);
  snprintf(strOnDisplay, MAX_STR_LEN, "CAN DEMO");
  writeTextOnTheDisplay(strOnDisplay, 5, 5);
  snprintf(strOnDisplay, MAX_STR_LEN, "Press a button\nto get started");
  writeTextOnTheDisplay(strOnDisplay, 5, 5 + (2 * GLIB_FONT_HEIGHT));
  DMD_updateDisplay();

  // Infinite loop
  while (true) {
    // Send message using CAN0 if BTN0 is pressed
    if (btn0Pressed) {
      // Increment message id
      message.id++;
      snprintf((char*)message.data, message.dlc, "BTN0 %ld", message.id);

      // Send message
      CAN_SendMessage(CAN0, CAN_TX_IF, &message, true);

      // Set Led0
      BSP_LedSet(0);
      BSP_LedClear(1);

      // Update display of CAN actions
      stat.txZero++;

      // Erasing data that has been sent
      for (i = 0; i < message.dlc; ++i) {
        message.data[i] = 0;
      }

      actionDisplay(stat, &message);

      // Reset flag
      btn0Pressed = false;
    }

    // Send message using CAN0 if BTN1 is pressed
    if (btn1Pressed) {
      message.id++;
      snprintf((char*)message.data, message.dlc, "BTN1 %ld", message.id);

      CAN_SendMessage(CAN1, CAN_TX_IF, &message, true);

      // Set Led1
      BSP_LedSet(1);
      BSP_LedClear(0);

      stat.txOne++;

      // Erasing data that has been sent
      for (i = 0; i < message.dlc; ++i) {
        message.data[i] = 0;
      }

      actionDisplay(stat, &message);

      btn1Pressed = false;
    }

    // Message arrival check on CAN0
    if (CAN0Received) {
      // Erasing data to be sure we read what we sent.
      for (i = 0; i < receiver.dlc; ++i) {
        receiver.data[i] = 0;
      }

      CAN_ReadMessage(CAN0, CAN_RX_IF, &receiver);

      // Set Led0
      BSP_LedSet(0);
      BSP_LedClear(1);

      stat.rxZero++;
      actionDisplay(stat, &receiver);

      CAN0Received = false;
    }

    // Message arrival check on CAN1
    if (CAN1Received) {
      // Erasing data to be sure we read what we sent.
      for (i = 0; i < receiver.dlc; ++i) {
        receiver.data[i] = 0;
      }

      CAN_ReadMessage(CAN1, CAN_RX_IF, &receiver);

      // Set Led1
      BSP_LedSet(1);
      BSP_LedClear(0);

      stat.rxOne++;
      actionDisplay(stat, &receiver);

      CAN1Received = false;
    }
  }
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

  // Chip errata
  CHIP_Init();

  // Init DCDC regulator
  EMU_DCDCInit(&dcdcInit);

  SystemHFXOClockSet(48000000);
  CMU_HFXOInit(&hfxoInit);
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  // Initialize buttons to generate interrupt when clicked
  setUpBTN(0);
  setUpBTN(1);

  // Initialize LED driver
  BSP_LedsInit();

  // Glib Init
  DMD_init(0);
  GLIB_contextInit(&glibContext);
  glibContext.backgroundColor = White;
  glibContext.foregroundColor = Black;
  GLIB_clear(&glibContext);

  // CAN_demo function
  runCANDemo();
}
