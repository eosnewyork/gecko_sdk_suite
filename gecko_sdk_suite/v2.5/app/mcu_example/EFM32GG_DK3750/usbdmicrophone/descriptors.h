/***************************************************************************//**
 * @file
 * @brief USB descriptors for microphone audio device example.
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
#ifndef __SILICON_LABS_DESCRIPTORS_H__
#define __SILICON_LABS_DESCRIPTORS_H__

#include "em_usb.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_SAMPLE_RATE 44100

// Number of bytes transferred per USB frame (per ms) when accounting for up to
// 96 kHz sampling rate (stereo, 16 bit per sample).
#define MAX_BUFFER_SIZE   ((96000 * 2 * 2) / 1000)

extern const USB_DeviceDescriptor_TypeDef   USBDESC_deviceDesc;
extern const uint8_t                        USBDESC_configDesc[];
extern const void * const                   USBDESC_strings[3];
extern const uint8_t                        USBDESC_bufferingMultiplier[];

#ifdef __cplusplus
}
#endif

#endif /* __SILICON_LABS_DESCRIPTORS_H__ */
