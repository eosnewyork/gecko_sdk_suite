/***************************************************************************//**
 * @file
 * @brief USB HID keyboard device example.
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
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "em_usb.h"
#include "descriptors.h"

/***************************************************************************//**
 *
 * This example shows how a HID keyboard can be implemented.
 *
 ******************************************************************************/

// Typedef's and defines.

#define SCAN_TIMER              1       // Timer used to scan keyboard.
#define SCAN_RATE               50

#define BUTTON_PORT             gpioPortD
#define BUTTON_PIN              5

// Function prototypes.

static void OutputReportReceived(uint8_t report);
static void StateChange(USBD_State_TypeDef oldState,
                        USBD_State_TypeDef newState);

// Variables

static int      keySeqNo;           // Current position in report table.
static bool     keyPushed;          // Current pushbutton status.

static const USBD_Callbacks_TypeDef callbacks = {
  .usbReset        = NULL,
  .usbStateChange  = StateChange,
  .setupCmd        = HIDKBD_SetupCmd,
  .isSelfPowered   = NULL,
  .sofInt          = NULL
};

static const USBD_Init_TypeDef usbInitStruct = {
  .deviceDescriptor    = &USBDESC_deviceDesc,
  .configDescriptor    = USBDESC_configDesc,
  .stringDescriptors   = USBDESC_strings,
  .numberOfStrings     = sizeof(USBDESC_strings) / sizeof(void*),
  .callbacks           = &callbacks,
  .bufferingMultiplier = USBDESC_bufferingMultiplier,
  .reserved            = 0
};

static uint32_t ledColor = 0;

/***************************************************************************//**
 * @brief main - the entrypoint after reset.
 ******************************************************************************/
int main(void)
{
  HIDKBD_Init_t hidInitStruct;
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;

  // Chip errata
  CHIP_Init();

  // Increase VMCU to 3.3 V if powered by MCU USB port.
  EMU->R5VOUTLEVEL = 10U << _EMU_R5VOUTLEVEL_OUTLEVEL_SHIFT;

  EMU_DCDCInit(&dcdcInit);

  // If first word of user data page is non-zero, enable Energy Profiler trace
  BSP_TraceProfilerSetup();

  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet(BUTTON_PORT, BUTTON_PIN, gpioModeInputPull, 1);
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  BSP_LedsInit();
  BSP_ExtLedSet(0, ledColor);

  // Initialize HID keyboard driver.
  hidInitStruct.hidDescriptor = (void*)USBDESC_HidDescriptor;
  hidInitStruct.setReportFunc = OutputReportReceived;
  HIDKBD_Init(&hidInitStruct);

  // Initialize and start USB device stack.
  USBD_Init(&usbInitStruct);

  //
  // When using a debugger it is practical to uncomment the following three
  // lines to force host to re-enumerate the device.
  //

  // USBD_Disconnect();
  // USBTIMER_DelayMs(1000);
  // USBD_Connect();

  for (;; ) {
  }
}

/***************************************************************************//**
 * @brief
 *   Timeout function for keyboard scan timer.
 *   Scan keyboard to check for key press/release events.
 *   This function is called at a fixed rate.
 ******************************************************************************/
static void ScanTimeout(void)
{
  bool pushed;
  HIDKBD_KeyReport_t *report;

  // Check pushbutton
  pushed = GPIO_PinInGet(BUTTON_PORT, BUTTON_PIN) == 0;

  if (!keyPushed) {
    ledColor ^= 1;
    BSP_ExtLedSet(0, ledColor);
  }

  if ( pushed != keyPushed ) { // Any change in keyboard status ?
    if ( pushed ) {
      report = (void*)&USBDESC_reportTable[keySeqNo];
    } else {
      report = (void*)&USBDESC_noKeyReport;
    }

    // Pass keyboard report on to the HID keyboard driver.
    HIDKBD_KeyboardEvent(report);
  }

  // Keep track of the new keypush event (if any)
  if ( pushed && !keyPushed ) {
    // Advance to next position in report table
    keySeqNo++;
    if ( keySeqNo == (sizeof(USBDESC_reportTable) / sizeof(HIDKBD_KeyReport_t))) {
      keySeqNo = 0;
    }
    ledColor |= 1;
    BSP_ExtLedSet(0, ledColor);
  }
  keyPushed = pushed;

  // Restart keyboard scan timer
  USBTIMER_Start(SCAN_TIMER, SCAN_RATE, ScanTimeout);
}

/***************************************************************************//**
 * @brief
 *   Callback function called each time the USB device state is changed.
 *   Starts keyboard scanning when device has been configured by USB host.
 *
 * @param[in] oldState The device state the device has just left.
 * @param[in] newState The new device state.
 ******************************************************************************/
static void StateChange(USBD_State_TypeDef oldState,
                        USBD_State_TypeDef newState)
{
  // Call HIDKBD drivers state change event handler.
  HIDKBD_StateChangeEvent(oldState, newState);

  if ( newState == USBD_STATE_CONFIGURED ) {
    // We have been configured, start scanning the keyboard !
    if ( oldState != USBD_STATE_SUSPENDED ) { // Resume ?
      keySeqNo        = 0;
      keyPushed       = false;
    }
    ledColor = 3;
    BSP_ExtLedSet(0, 3);
    USBTIMER_Start(SCAN_TIMER, SCAN_RATE, ScanTimeout);
  } else if ( (oldState == USBD_STATE_CONFIGURED)
              && (newState != USBD_STATE_SUSPENDED)    ) {
    // We have been de-configured, stop keyboard scanning.
    USBTIMER_Stop(SCAN_TIMER);
    ledColor &= ~1;
    BSP_ExtLedSet(0, ledColor);
  } else if ( newState == USBD_STATE_SUSPENDED ) {
    // We have been suspended, stop keyboard scanning.
    // Reduce current consumption to below 2.5 mA.
    ledColor = 0;
    BSP_ExtLedSet(0, 0);
    BSP_ExtLedSet(1, 0);
    USBTIMER_Stop(SCAN_TIMER);
  }
}

/***************************************************************************//**
 * @brief
 *   Callback function called when the data stage of a USB_HID_SET_REPORT
 *   setup command has completed.
 *
 * @param[in] report Output report byte.
 *                   @n Bit 0 : State of keyboard NumLock LED.
 *                   @n Bit 1 : State of keyboard CapsLock LED.
 *                   @n Bit 2 : State of keyboard ScrollLock LED.
 ******************************************************************************/
static void OutputReportReceived(uint8_t report)
{
  //
  // We have received new data for NumLock, CapsLock and ScrollLock indicators.
  // The combination of NumLock, CapsLock and ScrollLock will create an RGB
  // color on LED1.
  //

  BSP_ExtLedSet(1, report);
}
