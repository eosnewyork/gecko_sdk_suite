/***************************************************************************//**
 * @file
 * @brief USB CDC Serial Port adapter example project.
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
#include "em_cmu.h"
#include "em_dma.h"
#include "em_dma.h"
#include "em_gpio.h"
#include "dmactrl.h"
#include "em_usart.h"
#include "bsp.h"
#include "bsp_trace.h"

#include "em_usb.h"
#include "cdc.h"
#include "descriptors.h"

/***************************************************************************//**
 *
 * This example shows how a CDC based USB to Serial port adapter can be
 * implemented.
 *
 * Use the file EFM32-Cdc.inf to install a USB serial port device driver
 * on the host PC.
 *
 * This implementation uses DMA to transfer data between UART1 and memory
 * buffers.
 *
 ******************************************************************************/

static const USBD_Callbacks_TypeDef callbacks =
{
  .usbReset        = NULL,
  .usbStateChange  = CDC_StateChangeEvent,
  .setupCmd        = CDC_SetupCmd,
  .isSelfPowered   = NULL,
  .sofInt          = NULL
};

static const USBD_Init_TypeDef usbInitStruct =
{
  .deviceDescriptor    = &USBDESC_deviceDesc,
  .configDescriptor    = USBDESC_configDesc,
  .stringDescriptors   = USBDESC_strings,
  .numberOfStrings     = sizeof(USBDESC_strings) / sizeof(void*),
  .callbacks           = &callbacks,
  .bufferingMultiplier = USBDESC_bufferingMultiplier,
  .reserved            = 0
};

/***************************************************************************//**
 * @brief main - the entrypoint after reset.
 ******************************************************************************/
int main(void)
{
  BSP_Init(BSP_INIT_DEFAULT);   /* Initialize DK board register access */

  /* If first word of user data page is non-zero, enable Energy Profiler trace */
  BSP_TraceProfilerSetup();

  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  /* Initialize the communication class device. */
  CDC_Init();

  /* Initialize and start USB device stack. */
  USBD_Init(&usbInitStruct);

  /*
   * When using a debugger it is practical to uncomment the following three
   * lines to force host to re-enumerate the device.
   */
  /*USBD_Disconnect();         */
  /*USBTIMER_DelayMs(1000);    */
  /*USBD_Connect();            */

  for (;; ) {
  }
}
