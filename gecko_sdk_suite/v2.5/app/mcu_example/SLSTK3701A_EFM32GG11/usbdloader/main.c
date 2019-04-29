/***************************************************************************//**
 * @file
 * @brief Gecko Bootloader USB Mass Storage Device example.
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
#include <inttypes.h>
#include "em_device.h"
#include "em_assert.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_core.h"
#include "em_gpio.h"
#include "retargetserial.h"
#include "bsp_trace.h"

#include "em_usb.h"
#include "msdd.h"
#include "msddmedia.h"
#include "gblfat12disk.h"
#include "descriptors.h"

#include "btl_interface.h"

extern const ApplicationProperties_t applicationProperties;

static const USBD_Callbacks_TypeDef callbacks =
{
  .usbReset        = NULL,
  .usbStateChange  = MSDD_StateChangeEvent,
  .setupCmd        = MSDD_SetupCmd,
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
  bool msddState;
  BootloaderInformation_t info;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

  // Chip errata
  CHIP_Init();

  // If first word of user data page is non-zero, enable Energy Profiler trace
  BSP_TraceProfilerSetup();

  CMU_HFXOInit(&hfxoInit);
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  CMU_OscillatorEnable(cmuOsc_LFXO, true, false);
  CMU_ClockEnable(cmuClock_GPIO, true);

  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);
  printf("\n\n***USBD Loader Demo***\n");

  bootloader_getInfo(&info);
  if (info.type == NO_BOOTLOADER) {
    printf("\nNo bootloader is present (first stage or main stage invalid)\n");
    EFM_ASSERT(false);
  }

  printf("\nCurrent APP version: %" PRIu32 "\n", applicationProperties.app.version);

  // Initialize Boot Loader
  if (bootloader_init() & BOOTLOADER_ERROR_INIT_BASE) {
    EFM_ASSERT(false);
  }

  // Initialize the Mass Storage Media.
  if (!MSDDMEDIA_Init()) {
    EFM_ASSERT(false);
  }

  // Initialize the Mass Storage Device.
  MSDD_Init(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);

  // Initialize and start USB device stack.
  USBD_Init(&usbInitStruct);

  printf("\nUSBD MSD ready\n\n");
  while (1) {
    msddState = MSDD_Handler();
    GBLFAT12DISK_reProgramCheck(msddState);
  }
}
