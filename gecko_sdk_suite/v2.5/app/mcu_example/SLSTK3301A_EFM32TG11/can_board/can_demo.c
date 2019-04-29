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

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "bsp.h"
#include "em_can.h"
#include "segmentlcd.h"
#include "em_emu.h"
#include "em_gpio.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

// CAN bus interfaces
#define CAN_TX_IF                 0
#define CAN_RX_IF                 1

/*******************************************************************************
 *****************************   Variables   ***********************************
 ******************************************************************************/

// BTN0 interrupt flag
static volatile bool btn0Pressed = false;

// BTN1 interrupt flag
static volatile bool btn1Pressed = false;

// message arrival flag on CAN
static volatile bool CAN0Received = false;

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/

// CAN action counter
typedef struct {
  // Number of transmission from CAN0
  uint32_t txZero;
  // Number of reception by CAN0
  uint32_t rxZero;
} CANState_t;

/***************************************************************************//**
 * @brief GRIO_ODD_IRQHandler
 * Interrupt Service Routine for GPIO interrupt
 ******************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  if ((GPIO_IntGet() & _GPIO_IF_EXT_MASK) & (0x1 << BSP_GPIO_PB0_PIN)) {
    GPIO_IntClear(0x1 << BSP_GPIO_PB0_PIN);
    btn0Pressed = true;
  } else {
    GPIO_IntClear(0x1 << BSP_GPIO_PB1_PIN);
    btn1Pressed = true;
  }
}

/***************************************************************************//**
 * @brief CAN_IRQHandlers
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
 * @brief initialization of GPIO interrupt for BTN0 or BTN1
 * @param buttonNr: BTN0 or BTN1
 ******************************************************************************/
static void setUpBTN(uint8_t buttonNr)
{
  if (buttonNr == 0 || buttonNr == 1) {
    CMU_ClockEnable(cmuClock_GPIO, true);

    GPIO_Unlock();

    if (!buttonNr) {
      GPIO_IntClear(0x1 << BSP_GPIO_PB0_PIN);
      GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 1);
      GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);
    } else {
      GPIO_IntClear(0x1 << BSP_GPIO_PB1_PIN);
      GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInput, 1);
      GPIO_IntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, false, true, true);
    }

    NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
    NVIC_EnableIRQ(GPIO_ODD_IRQn);
  }
}

/***************************************************************************//**
 * @brief initialization of CAN
 *
 * @param can_Device: CAN0
 *
 * @param mode: Operating mode
 ******************************************************************************/
static void setUpCAN(CAN_TypeDef *can_Device, CAN_Mode_TypeDef mode)
{
  // Enable the clocks
  CMU_ClockEnable(cmuClock_HFPER, true);

  if (can_Device == CAN0) {
    NVIC_EnableIRQ(CAN0_IRQn);

    // Enable the CANn clock
    CMU_ClockEnable(cmuClock_CAN0, true);

    // Reset the CAN device
    CAN_Reset(can_Device);
    CAN_Enable(can_Device, false);

    // Set the gpio for rx (X) and tx (Y)
    GPIO_PinModeSet(gpioPortC, 0, gpioModeInput, 0);
    GPIO_PinModeSet(gpioPortC, 1, gpioModePushPull, 0);

    // Set the ROUTE to match the pins
    CAN_SetRoute(can_Device, true, 0, 0);
  }

  // Reset Message Objects
  CAN_ResetMessages(can_Device, 0);
  CAN_ResetMessages(can_Device, 1);

  // Set the bit timing to get a bitrate of 500 kbit/s
  // The bitrate MUST be chosen based on the configuration of the clock.
  // The desired bitrate might be unreachable depending on the clock frequency.
  CAN_SetBitTiming(can_Device, 500000, 6, 7, 2, 1);

  // Set the CAN device mode
  CAN_SetMode(can_Device, mode);

  // Enable CAN
  CAN_Enable(can_Device, true);
}

/***************************************************************************//**
 *  @brief  Show CAN actions on the display
 *
 *  @param state
 *  is CAN action struct consisting the number of transmissions and receptions
 *
 *  @param message is a message object used to send/receive data frame
 ******************************************************************************/
static void actionDisplay(CANState_t state,
                          CAN_MessageObject_TypeDef *message)
{
  // Buffer to store string to be shown on the display
  char str[9];
  (void) message;

  // Disable all segments
  SegmentLCD_AllOff();

  // Show CAN actions on the display
  sprintf(str, "R%ld T%ld", state.rxZero, state.txZero);
  SegmentLCD_Write(str);
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
  CANState_t state;
  state.rxZero = 0, state.txZero = 0;

  // Initialize CAN peripherals
  CAN_Mode_TypeDef mode = canModeNormal;
  setUpCAN(CAN0, mode);

  // Initialize a message using 5th Message Object in the RAM to send
  CAN_MessageObject_TypeDef message;
  configMessageObj(CAN0, &message, 5, 1, 8, 0, true);

  // Initialize a message using 6th Message Object in the RAM for reception
  CAN_MessageObject_TypeDef receiver;
  configMessageObj(CAN0, &receiver, 6, 0, 8, 0, false);

  // Infinite loop
  while (true) {
    // Send message using CAN if BTN0 is pressed
    if (btn0Pressed) {
      // Send message
      message.id++;

      snprintf((char*)message.data, message.dlc, "BTN0 %ld", message.id);
      CAN_SendMessage(CAN0, CAN_TX_IF, &message, true);

      // Set Led0
      BSP_LedSet(0);
      BSP_LedClear(1);

      // Update display of CAN actions
      state.txZero++;

      // Erasing data that has been sent.
      for (i = 0; i < message.dlc; ++i) {
        message.data[i] = 0;
      }

      actionDisplay(state, &message);

      // Reset flag
      btn0Pressed = false;
    }

    // Send message using CAN if BTN1 is pressed
    if (btn1Pressed) {
      // Send message
      message.id++;
      snprintf((char*)message.data, message.dlc, "BTN1 %ld", message.id);
      CAN_SendMessage(CAN0, CAN_TX_IF, &message, true);

      // Set Led1
      BSP_LedSet(1);
      BSP_LedClear(0);

      // Update display of CAN actions
      state.txZero++;

      // Erasing data that has been sent.
      for (i = 0; i < message.dlc; ++i) {
        message.data[i] = 0;
      }

      actionDisplay(state, &message);

      // Reset flag
      btn1Pressed = false;
    }

    // Message arrival check on CAN
    if (CAN0Received) {
      // Erasing data to be sure we read what we sent.
      for (i = 0; i < receiver.dlc; ++i) {
        receiver.data[i] = 0;
      }

      CAN_ReadMessage(CAN0, CAN_RX_IF, &receiver);

      BSP_LedToggle(0);
      BSP_LedToggle(1);

      state.rxZero++;
      actionDisplay(state, &receiver);

      CAN0Received = false;
    }
  }
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
  dcdcInit.anaPeripheralPower = emuDcdcAnaPeripheralPower_AVDD;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

  // Chip errata
  CHIP_Init();

  // Init DCDC regulator
  EMU_DCDCInit(&dcdcInit);

  CMU_HFXOInit(&hfxoInit);
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  // Initialize buttons to generate interrupt when clicked
  setUpBTN(0);
  setUpBTN(1);

  // Enable LCD without voltage boost
  SegmentLCD_Init(false);
  SegmentLCD_Write("CAN DEMO");

  // Initialize LED driver
  BSP_LedsInit();
  BSP_LedToggle(0);

  // CAN_demo function
  runCANDemo();
}
