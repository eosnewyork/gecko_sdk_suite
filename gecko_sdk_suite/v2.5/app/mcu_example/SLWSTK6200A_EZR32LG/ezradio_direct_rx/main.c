/***************************************************************************//**
 * @file
 * @brief EZRadio direct receive example
 * This example shows how to easily implement a direct receiver application for
 * your controller using EZRadio or EZRadioPRO devices.
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
#include <stdlib.h>

#include "spidrv.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "gpiointerrupt.h"
#include "rtcdriver.h"

#include "ezradio_cmd.h"
#include "ezradio_api_lib.h"
#include "ezradio_plugin_manager.h"

#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"
#include "bspconfig.h"
#include "image.h"

/** Display device */
static DISPLAY_Device_t displayDevice;

/** Image widht and height definitions */
#define IMAGE_HIGHT           62u
#define BYTES_PER_LINE        (LS013B7DH03_WIDTH / 8)
#define BYTES_PER_FRAME       (IMAGE_HIGHT * BYTES_PER_LINE)

/** Push button callback functionns. */
static void GPIO_PB0_IRQHandler(uint8_t pin);

#if !defined(__CROSSWORKS_ARM) && defined(__GNUC__)
/* sniprintf does not process floats, but occupy less flash memory ! */
#define snprintf    sniprintf
#endif

/** RTC frequency */
#define APP_RTC_FREQ_HZ 500u
/** RTC timeout */
#define APP_RTC_TIMEOUT_MS (1000u / APP_RTC_FREQ_HZ)

/** RTC set time is expired */
static volatile bool rtcTick = false;

/** Signal change request */
static volatile bool changeSignal = false;

/** Signal enable bit */
static volatile bool signalEnabled = false;

/** Timer used to issue time elapsed interrupt. */
static RTCDRV_TimerID_t rtcTickTimer;
static RTCDRV_TimerID_t rtcRepeateTimer;

/***************************************************************************//**
 * @brief Setup GPIO interrupt for pushbuttons.
 ******************************************************************************/
static void GpioSetup(void)
{
  /* Enable GPIO clock */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Initialize GPIO interrupt */
  GPIOINT_Init();

  /* Configure PB0 as input and enable interrupt */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);
  GPIOINT_CallbackRegister(BSP_GPIO_PB0_PIN, GPIO_PB0_IRQHandler);
}

/***************************************************************************//**
 * @brief GPIO Interrupt handler (PB1)
 *        Switches between analog and digital clock modes.
 ******************************************************************************/
static void GPIO_PB0_IRQHandler(uint8_t pin)
{
  (void)pin;

  changeSignal = true;
}

/***************************************************************************//**
 * @brief Draws Silicon Labs logo.
 ******************************************************************************/
void drawPicture(void)
{
  char *pFrame;

  /* Retrieve the properties of the display. */
  if ( DISPLAY_DeviceGet(0, &displayDevice) != DISPLAY_EMSTATUS_OK) {
    while (1) ;
  }

  /* Load pointer to picture buffor */
  pFrame = (char *) image_bits;

  /* Write to LCD */
  displayDevice.pPixelMatrixDraw(&displayDevice, pFrame,
                                 /* start coloumn, width */
                                 0, displayDevice.geometry.width,
                                 /* start row, height */
                                 0, IMAGE_HIGHT);
}

/***************************************************************************//**
 * @brief   Register a callback function to be called repeatedly at the
 *          specified frequency.
 *
 * @param[in] pFunction  Pointer to function that should be called at the
 *                       given frequency.
 * @param[in] pParameter Pointer argument to be passed to the callback function.
 * @param[in] frequency  Frequency at which to call function at.
 *
 * @return  0 for successful or
 *         -1 if the requested frequency is not supported.
 ******************************************************************************/
int RepeatCallbackRegister(void(*pFunction)(void*),
                           void* pParameter,
                           unsigned int frequency)
{
  if (ECODE_EMDRV_RTCDRV_OK
      == RTCDRV_AllocateTimer(&rtcRepeateTimer)) {
    if (ECODE_EMDRV_RTCDRV_OK
        == RTCDRV_StartTimer(rtcRepeateTimer, rtcdrvTimerTypePeriodic, frequency,
                             (RTCDRV_Callback_t)pFunction, pParameter)) {
      return 0;
    }
  }

  return -1;
}

/***************************************************************************//**
 * @brief GPIO Interrupt handler (PB0)
 *        Increments the time by one minute.
 ******************************************************************************/
void RTC_App_IRQHandler()
{
  rtcTick = true;
}

/***************************************************************************//**
 * @brief  Main function of the example.
 ******************************************************************************/
int main(void)
{
  /* EZRadio driver init data and handler */
  EZRADIODRV_HandleData_t appRadioInitData = EZRADIODRV_INIT_DEFAULT;
  EZRADIODRV_Handle_t appRadioHandle = &appRadioInitData;

  /* EZRadio response structure union */
  ezradio_cmd_reply_t ezradioReply;

  /* Chip errata */
  CHIP_Init();

  /* HFXO 48MHz, divided by 1 */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  /* Setup GPIO for pushbuttons. */
  GpioSetup();

  /* Initialize RTC for timing. */
  RTCDRV_Init();

  /* Initialize the display module. */
  if (DISPLAY_EMSTATUS_OK != DISPLAY_Init()) {
    /* Display initialization failed. */
    while (1) ;
  }

  /* Retarget stdio to the display. */
  if (TEXTDISPLAY_EMSTATUS_OK != RETARGET_TextDisplayInit()) {
    /* Text display initialization failed. */
    while (1) ;
  }

  /* Set RTC to generate interrupt 250ms. */
  if (ECODE_EMDRV_RTCDRV_OK
      != RTCDRV_AllocateTimer(&rtcTickTimer) ) {
    while (1) ;
  }
  if (ECODE_EMDRV_RTCDRV_OK
      != RTCDRV_StartTimer(rtcTickTimer, rtcdrvTimerTypePeriodic, APP_RTC_TIMEOUT_MS,
                           (RTCDRV_Callback_t)RTC_App_IRQHandler, NULL) ) {
    while (1) ;
  }

  /* Print header */
  printf("\n\n\n\n\n\n\n\n  EZRadio Direct Rx\n");

  /* Initialize EZRadio device. */
  ezradioInit(appRadioHandle);

  /* Print EZRadio device number. */
  ezradio_part_info(&ezradioReply);
  printf("   Device: Si%04x\n\n", ezradioReply.PART_INFO.PART);

  /* Print instructions. */
  printf(" Press PB0 to send\n  enable/disable\n  reception.\n");

  /* Draw logo */
  drawPicture();

  /* Enter infinite loop that will take care of ezradio plugin manager and packet transmission. */
  while (1) {
    /* Run radio plug-in manager */
    ezradioPluginManager(appRadioHandle);

    if (rtcTick) {
      rtcTick = false;

      /* Send a packet if requested */
      if (changeSignal) {
        changeSignal = false;
        /* Try to send the packet */
        if ( signalEnabled ) {
          signalEnabled = false;
          ezradioStopDirectReceive();
          printf("Direct receive is OFF\n");
        } else {
          signalEnabled = true;
          ezradioStartDirectReceive(appRadioHandle);
          printf("Direct receive is ON\n");
        }
      }
    }
  }
}
