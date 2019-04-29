/***************************************************************************//**
 * @file
 * @brief USB microphone audio device example.
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

#include <stdlib.h>

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_pdm.h"
#include "em_timer.h"
#include "bsp_trace.h"
#include "retargetserial.h"
#include "em_usb.h"
#include "descriptors.h"
#include "dmadrv.h"

/*
 *  Audio handling:
 *  ---------------
 *
 *                The audio path explained.
 *                =========================
 *
 *                          | PDM coded audio in          8 buffers
 *                          V
 *                       +-----+        +------+         +--------+
 *                       | PDM |------->| LDMA |-------->| Buffer |--+
 *                       +-----+        +------+         +--------+  |--+
 *                                         |                +--------+  |
 *                                         V                 | +--------+
 *                                      +------+             |
 *                                      | LDMA |             |
 *                                      | IRQ  |             |
 *                                      +------+             |
 *                                                           |
 *                                      +-----+              |
 *                    To USB host <-----| USB |<-------------
 *                                      +-----+
 *
 *
 *  1. The PDM peripheral is setup to aquire and filter PDM coded data from two
 *     external PDM microphones. PDM will output PCM encoded audio at a rate of
 *     44,100 16 bit audio samples per second in both left and right channels.
 *     (This is the original audio CD sampling rate).
 *
 *  2. The LDMA is setup to do ping-pong transfers using two LDMA descriptors
 *     which link to each other. On each LDMA transfer complete interrupt, LDMA
 *     descriptors are patched to direct the data stream into the correct audio
 *     buffer.
 *
 *  3. Audio data is passed on to host computer via USB. Data is sent over USB
 *     once every millisecond using an isochronous IN endpoint. USB data packets
 *     are usually 176 bytes long but every 10th packet is 180 bytes long.
 *     This adds up to the correct number of samples per second.
 *
 */

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

//#define DEBUG_USB_AUDIO           // Uncomment to add debugging features.

#if defined(DEBUG_USB_AUDIO)
#include <stdio.h>
#define DEBUG_PUTCHAR(c) putchar(c)
#define DEBUG_GPIO_PIN_TOGGLE(p) GPIO_PinOutToggle(gpioPortA, p)
#else
#define DEBUG_PUTCHAR(c)
#define DEBUG_GPIO_PIN_TOGGLE(p)
#endif

#define ISO_IN_EP           0x81    // USB endpoint address.

// Bytes per USB frame (per ms) when audio sampling rate is 44,100 Hz
// This number does not add up, so in every 10th frame we need to transfer an
// additional 4 bytes.
#define BYTES_PER_FRAME     176U

#define AUDIO_BUFFER_SIZE   MAX_BUFFER_SIZE
#define AUDIO_BUFFER_COUNT  8U

// Macros defining where to route PDM microphone signals.
#define MIC_EN_PORT         gpioPortA
#define MIC_EN_PIN          8
#define MIC_CLK_PORT        gpioPortB
#define MIC_CLK_PIN         12
#define MIC_DATA_PORT       gpioPortB
#define MIC_DATA_PIN        11
#define MIC_CLK_PDM_LOC     3U
#define MIC_DATA_PDM_LOC    3U

/*******************************************************************************
 ******************************  PROTOTYPES ************************************
 ******************************************************************************/

static int      audioDataSent(USB_Status_TypeDef status,
                              uint32_t xferred,
                              uint32_t remaining);
static bool     dmaCallback(unsigned int channel,
                            unsigned int sequenceNo,
                            void *userParam);
static int      muteSettingReceived(USB_Status_TypeDef status,
                                    uint32_t xferred,
                                    uint32_t remaining);
static void     pdmSetup(void);
static int      setupCmd(const USB_Setup_TypeDef *setup);
static void     stateChange(USBD_State_TypeDef oldState,
                            USBD_State_TypeDef newState);

/*******************************************************************************
 ***************************   LOCAL VARIABLES   *******************************
 ******************************************************************************/

static const USBD_Callbacks_TypeDef callbacks = {
  .usbReset        = NULL,
  .usbStateChange  = stateChange,
  .setupCmd        = setupCmd,
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

// 8 audio buffers
STATIC_UBUF(audioBuffer0, AUDIO_BUFFER_SIZE);
STATIC_UBUF(audioBuffer1, AUDIO_BUFFER_SIZE);
STATIC_UBUF(audioBuffer2, AUDIO_BUFFER_SIZE);
STATIC_UBUF(audioBuffer3, AUDIO_BUFFER_SIZE);
STATIC_UBUF(audioBuffer4, AUDIO_BUFFER_SIZE);
STATIC_UBUF(audioBuffer5, AUDIO_BUFFER_SIZE);
STATIC_UBUF(audioBuffer6, AUDIO_BUFFER_SIZE);
STATIC_UBUF(audioBuffer7, AUDIO_BUFFER_SIZE);

static struct {
  unsigned int  len;
  uint8_t       *buffer;
} audioArray[AUDIO_BUFFER_COUNT] = {
  { 0, audioBuffer0 }, { 0, audioBuffer1 }, { 0, audioBuffer2 }, { 0, audioBuffer3 },
  { 0, audioBuffer4 }, { 0, audioBuffer5 }, { 0, audioBuffer6 }, { 0, audioBuffer7 }
};

// LDMA transfer configuration.
static const LDMA_TransferCfg_t xferCfg =
  LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_PDM_RXDATAV);
// Two LDMA descriptors linking to each other (ping-pong).
static LDMA_Descriptor_t dmaDesc[] = {
  LDMA_DESCRIPTOR_LINKREL_P2M_WORD(&PDM->RXDATA, audioBuffer0,
                                   BYTES_PER_FRAME / sizeof(uint32_t), 1),
  LDMA_DESCRIPTOR_LINKREL_P2M_WORD(&PDM->RXDATA, audioBuffer1,
                                   BYTES_PER_FRAME / sizeof(uint32_t), -1)
};

static volatile uint16_t        altSetting;
static volatile unsigned int    audioBufferNo;
static volatile unsigned int    audioFrameCnt;
static unsigned int             dmaChId;  // The channel Id assigned by DMADRV.
static volatile bool            mute;
STATIC_UBUF(silenceBuffer, AUDIO_BUFFER_SIZE);
static uint32_t                 smallBuffer;
static volatile unsigned int    usbBufferNo;
static volatile unsigned int    usbFrameCnt;
static volatile unsigned int    usbTxCnt;

/***************************************************************************//**
 * @brief main - the entrypoint after reset.
 ******************************************************************************/
int main(void)
{
  CHIP_Init();                  // Handle chip errata.

  // Increase VMCU to 3.3 V if powered by MCU USB port.
  EMU->R5VOUTLEVEL = 10U << _EMU_R5VOUTLEVEL_OUTLEVEL_SHIFT;

  // If first word of user data page is non-zero, enable Energy Profiler trace.
  BSP_TraceProfilerSetup();

  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

#if defined(DEBUG_USB_AUDIO)
  // Initialize VCOM USART port for debugging purposes.
  RETARGET_SerialInit();        // Initialize USART.
  RETARGET_SerialCrLf(1);       // Map LF to CRLF.
  printf("\nEFM32GG12 USB Microphone Audio example\n");

  // Setup GPIO to trigger scope for debugging.
  GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortA, 1, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortA, 2, gpioModePushPull, 1);
#endif

  memset(silenceBuffer, 0, sizeof(silenceBuffer));

  // Initialize and start USB device stack.
  USBD_Init(&usbInitStruct);

  // Setup the PDM peripheral.
  pdmSetup();

  // Start LDMA from PDM->RXDATA into audio buffers.
  DMADRV_Init();
  DMADRV_AllocateChannel(&dmaChId, NULL);
  DMADRV_LdmaStartTransfer((int)dmaChId,
                           (void*)&xferCfg,
                           (void*)&dmaDesc[0],
                           dmaCallback,
                           NULL);

  for (;; ) {
  }
}

/***************************************************************************//**
 * @brief
 *   Setup the PDM peripheral.
 ******************************************************************************/
static void pdmSetup(void)
{
  PDM_Init_TypeDef     pdmInit = PDM_INIT_DEFAULT;
  CMU_DPLLInit_TypeDef pllInit = CMU_DPLL_LFXO_TO_40MHZ;

  // Setup GPIO's.
  GPIO_PinModeSet(MIC_EN_PORT, MIC_EN_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(MIC_CLK_PORT, MIC_CLK_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(MIC_DATA_PORT, MIC_DATA_PIN, gpioModeInput, 0);

  // Set fast slew rate on PDM mic CLK and DATA pins.
  GPIO_SlewrateSet(MIC_CLK_PORT, 7U, 7U);

  // Lock PLL to 1,411,209 Hz to achive 44,100 kHz PCM sampling rate
  // when using 32x PDM oversampling.
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
  pllInit.frequency = 1411209;
  pllInit.m = 14;
  pllInit.n = 645;
  CMU_DPLLLock(&pllInit);

  // Enable PDM peripheral clock.
  CMU_ClockEnable(cmuClock_PDM, true);
  // Select PDM reference clock source and enable it.
  CMU_ClockSelectSet(cmuClock_PDMREF, cmuSelect_HFRCO);
  CMU_ClockEnable(cmuClock_PDMREF, true);

  // Route PDM signals.
  PDM->ROUTELOC0 = (PDM->ROUTELOC0 & ~_PDM_ROUTELOC0_DAT0LOC_MASK)
                   | (MIC_DATA_PDM_LOC << _PDM_ROUTELOC0_DAT0LOC_SHIFT);
  PDM->ROUTELOC1 = MIC_CLK_PDM_LOC << _PDM_ROUTELOC1_CLKLOC_SHIFT;
  PDM->ROUTEPEN |= PDM_ROUTEPEN_CLKPEN | PDM_ROUTEPEN_DAT0PEN;

  // Configure PDM peripheral.
  pdmInit.start = false;
  PDM_Init(PDM, &pdmInit);

  // Turn on microphones.
  GPIO_PinOutSet(MIC_EN_PORT, MIC_EN_PIN);

  // Wait for microphone power on.
  USBTIMER_DelayMs(100);

  // Start PDM.
  PDM_Clear(PDM);
  PDM_FifoFlush(PDM);
  PDM_Start(PDM);
}

/***************************************************************************//**
 * @brief Callback called when an LDMA data transfer has completed.
 *        Patch correct audio buffer pointer into LDMA descriptor, also check
 *        if there is a need to modify LDMA transfer length.
 *
 * @param[in] channel
 *  The DMA channel number.
 *
 * @param[in] sequenceNo
 *  The number of times the callback has been called.
 *  The value is 1 on the first callback.
 *
 * @param[in] userParam
 *  Optional user parameter supplied on LDMA invocation.
 *
 * @return
 *   When doing ping-pong transfers, return true to continue or false to
 *   stop transfers.
 ******************************************************************************/
static bool dmaCallback(unsigned int channel,
                        unsigned int sequenceNo,
                        void *userParam)
{
  long diff;
  unsigned int descIndex, audioIndex, audioSampleBytes, udiff;

  (void)channel;
  (void)userParam;

  DEBUG_GPIO_PIN_TOGGLE(1);

  // Find which DMA descriptor that is next in sequence and can
  // still be modified (not yet consumed by LDMA hardware) and
  // the audiobuffer to be used by that transfer.
  descIndex  = (sequenceNo - 1U) & 1U;
  // audioBufferNo contains the buffer number of the transfer just completed.
  // audioBufferNo+1 is in progess.
  // audioBufferNo+2 has not yet started.
  audioIndex = (audioBufferNo + 2U) % AUDIO_BUFFER_COUNT;

  audioSampleBytes = BYTES_PER_FRAME;

  if (altSetting == 1U) {       // Is normal bandwidth interface active ?
    audioFrameCnt++;
    // If audio framerate is a bit faster than host expect, insert an
    // extra sample in next frame to reduce the rate.
    udiff = audioFrameCnt;
    if (udiff > usbFrameCnt + 1U) {
      audioSampleBytes = BYTES_PER_FRAME + 4U;
      DEBUG_GPIO_PIN_TOGGLE(2);
    }

    // Check if LDMA/USB frame rate synchronization is far off.
    diff = (int)audioFrameCnt;
    diff -= (int)usbFrameCnt;
    if (labs(diff) > 10 ) {
      // Try forcing a re-sync.
      usbFrameCnt   = 0U;
      audioFrameCnt = 0U;
      // We want audioBufferNo and usbBufferNo to be 4 apart so that DMA is
      // not filling samples into a buffer beeing transmitted on USB.
      audioBufferNo = 7U;
      usbBufferNo   = 4U;
    }
  }

  // Update audiobuffer length member.
  audioArray[audioIndex].len = audioSampleBytes;
  // Patch dma descriptor count.
  dmaDesc[descIndex].xfer.xferCnt = (audioSampleBytes / sizeof(uint32_t)) - 1U;
  // Patch dma descriptor destination address.
  dmaDesc[descIndex].xfer.dstAddr = (uint32_t)audioArray[audioIndex].buffer;

  // Post processing of audio (otional). Samples are of type int16_t and
  // alternate between left and right channel.
  //postProcess((uint16_t*)audioArray[audioBufferNo].buffer,
  //            audioArray[audioBufferNo].len) / sizeof(uint16_t);

  audioBufferNo = (audioBufferNo + 1U) % AUDIO_BUFFER_COUNT;

  return true;
}

/***************************************************************************//**
 * @brief Callback called when a USB data transfer has completed.
 *        Fire off a new USB transfer.
 *        Keeps track of number of samples sent vs. expected, and do sample
 *        insertion removal as needed.
 *
 * @param[in] status
 *   The transfer status.
 *
 * @param[in] xferred
 *   Number of bytes actually transferred.
 *
 * @param[in] remaining
 *   Number of bytes not transferred.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
static int audioDataSent(USB_Status_TypeDef status,
                         uint32_t xferred,
                         uint32_t remaining)
{
  int  retVal;
  int32_t sample;
  int16_t *pSample;
  uint8_t *pBuffer;
  unsigned int cnt, expected;

  (void)xferred;
  (void)remaining;

  retVal = (int)USB_STATUS_OK;

  if (status == USB_STATUS_OK) {
    DEBUG_GPIO_PIN_TOGGLE(0);
    if (altSetting == 1U) {       // Is normal bandwidth interface active ?
      usbFrameCnt++;
    }

    usbTxCnt++;
    if (usbTxCnt == 10U) {
      // Send extra sample on every 10th USB frame to ensure samples add
      // up to 44,100 per second.
      usbTxCnt = 0U;
      expected = BYTES_PER_FRAME + 4U;
    } else {
      expected = BYTES_PER_FRAME;
    }

    if (mute) {
      pBuffer = silenceBuffer;
    } else {
      pBuffer = audioArray[usbBufferNo].buffer;
      cnt     = audioArray[usbBufferNo].len;
      usbBufferNo = (usbBufferNo + 1U) % AUDIO_BUFFER_COUNT;

      if (cnt != expected) {
        if (expected == BYTES_PER_FRAME + 4U) {
          // Insert a sample in each channel.
          // Average last sample and next to last sample and insert the new
          // sample between last and next to last sample.
          pSample    = (int16_t*)&pBuffer[cnt - 8U];
          pSample[4] = pSample[2];
          pSample[5] = pSample[3];
          sample     = (pSample[0] + pSample[4]) / 2;
          pSample[2] = sample;
          sample     = (pSample[1] + pSample[5]) / 2;
          pSample[3] = sample;
        } else {
          // Remove a sample from each channel.
          // Average last 4 samples (N,N-1,N-2,N-3),
          // replace sample N-2 with the average,
          // replace sample N-1 with N.
          pSample    = (int16_t*)&pBuffer[cnt - 16U];
          sample     = (pSample[0] + pSample[2] + pSample[4] + pSample[6]) / 4;
          pSample[2] = sample;
          sample     = (pSample[1] + pSample[3] + pSample[5] + pSample[7]) / 4;
          pSample[3] = sample;
          pSample[4] = pSample[6];
          pSample[5] = pSample[7];
        }
      }
    }

    retVal = USBD_Write(ISO_IN_EP, pBuffer, (int)expected, audioDataSent);
  }

  return retVal;
}

/***************************************************************************//**
 * @brief
 *   Handle USB setup commands.
 *
 * @param[in] setup Pointer to the setup packet received.
 *
 * @return USB_STATUS_OK if command accepted.
 *         USB_STATUS_REQ_UNHANDLED when command is unknown, the USB device
 *         stack will handle the request.
 ******************************************************************************/
static int setupCmd(const USB_Setup_TypeDef *setup)
{
  int       retVal;
  uint8_t   *pBuffer = (uint8_t*) &smallBuffer;

  retVal = (int)USB_STATUS_REQ_UNHANDLED;

  if (setup->Type == USB_SETUP_TYPE_CLASS) {
    switch (setup->bRequest) {
      case USB_AUDIO_GET_CUR:
        /********************/
        // Report current MUTE status to host.
        if ((setup->Direction == USB_SETUP_DIR_IN)
            && (setup->Recipient == USB_SETUP_RECIPIENT_INTERFACE)
            && (setup->wLength == 1U)) {
          if ((setup->wIndex == 0x0200U)
              && (setup->wValue == 0x0100U)) {
            // wIndex LSB is interface no, must be 0.
            // wIndex MSB is entityID, must be 2 ("Feature Unit ID2").
            // wValue LSB is channel number, must be 0 (master).
            // wValue MSB is control selector, must be 1 (first control is mute).
            *pBuffer = (uint8_t)mute;
            retVal = USBD_Write(0, pBuffer, setup->wLength, NULL);
          }
        }
        break;

      case USB_AUDIO_SET_CUR:
        /********************/
        // Get new MUTE setting from host.
        if ((setup->Direction == USB_SETUP_DIR_OUT)
            && (setup->Recipient == USB_SETUP_RECIPIENT_INTERFACE)
            && (setup->wLength == 1U)) {
          if ((setup->wIndex == 0x0200U)
              && (setup->wValue == 0x0100U)) {
            // wIndex LSB is interface no, must be 0.
            // wIndex MSB is entityID, must be 2 ("Feature Unit ID2").
            // wValue LSB is channel number, must be 0 (master).
            // wValue MSB is control selector, must be 1 (first control is mute).
            retVal = USBD_Read(0, pBuffer, setup->wLength, muteSettingReceived);
          }
        }
        break;
    }
  }
  // Set/Get interface alternate setting.
  // (Re-implement standard SET/GET_INTERFACE commands.)
  else if (setup->Type == USB_SETUP_TYPE_STANDARD) {
    if ((setup->bRequest == SET_INTERFACE)
        && (setup->wIndex == 1U)      // Interface number.
        && (setup->wLength == 0U)
        && (setup->Recipient == USB_SETUP_RECIPIENT_INTERFACE)) {
      // setup->wValue contains a new Alternate Setting value.
      if (setup->wValue == 0U) {       // The zero bandwidth interface.
        altSetting = setup->wValue;
        retVal = (int)USB_STATUS_OK;
        DEBUG_PUTCHAR('Z');
      } else if (setup->wValue == 1U) { // The normal bandwidth interface.
        altSetting    = setup->wValue;
        usbFrameCnt   = 0U;
        audioFrameCnt = 0U;
        audioBufferNo = 0U;
        usbBufferNo   = 4U;
        usbTxCnt      = 1U;
        retVal = (int)USB_STATUS_OK;
        DEBUG_PUTCHAR('N');
      }
    } else if ((setup->bRequest == GET_INTERFACE)
               && (setup->wValue == 0U)
               && (setup->wIndex == 1U) // Interface number.
               && (setup->wLength == 1U)
               && (setup->Recipient == USB_SETUP_RECIPIENT_INTERFACE)) {
      *pBuffer = (uint8_t)altSetting;
      retVal = USBD_Write(0, pBuffer, 1, NULL);
    }
  }

  return retVal;
}

/***************************************************************************//**
 * @brief Callback called when a new mute on/off setting is received.
 *
 * @param[in] status
 *   The transfer status.
 *
 * @param[in] xferred
 *   Number of bytes actually transferred.
 *
 * @param[in] remaining
 *   Number of bytes not transferred.
 *
 * @return
 *   @ref USB_STATUS_OK on success, else an appropriate error code.
 ******************************************************************************/
static int muteSettingReceived(USB_Status_TypeDef status,
                               uint32_t xferred,
                               uint32_t remaining)
{
  (void)status;
  (void)xferred;
  (void)remaining;

  mute = (smallBuffer & 0xFFU) != 0U;

  if (mute) {
    DEBUG_PUTCHAR('M');
  } else {
    DEBUG_PUTCHAR('m');
  }

  return (int)USB_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Called whenever the USB device has changed its device state.
 *
 * @param[in] oldState
 *   The device USB state just leaved. See USBD_State_TypeDef.
 *
 * @param[in] newState
 *   New (the current) USB device state. See USBD_State_TypeDef.
 ******************************************************************************/
static void stateChange(USBD_State_TypeDef oldState,
                        USBD_State_TypeDef newState)
{
  if (newState == USBD_STATE_CONFIGURED) {
    // We have been configured, start sending audio data.
    mute = false;
    altSetting    = 0U;
    usbFrameCnt   = 0U;
    audioFrameCnt = 0U;
    audioBufferNo = 0U;
    usbBufferNo   = 4U;
    usbTxCnt      = 1U;
    USBD_Write(ISO_IN_EP, silenceBuffer, BYTES_PER_FRAME, audioDataSent);
  } else if (oldState == USBD_STATE_CONFIGURED) {
    // We have been de-configured.
    altSetting = 0U;
    USBD_AbortTransfer(ISO_IN_EP);
  }

  if (newState == USBD_STATE_SUSPENDED) {
    // We have been suspended.
    // Reduce current consumption to below 2.5 mA.
  }
}
