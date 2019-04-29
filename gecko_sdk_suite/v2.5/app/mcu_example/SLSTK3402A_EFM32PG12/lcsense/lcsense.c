/***************************************************************************//**
 * @file
 * @brief LESENSE demo for SLSTK3402A_EFM32PG12.
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

#include "em_acmp.h"
#include "em_assert.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_core.h"
#include "em_device.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_lesense.h"
#include "em_pcnt.h"
#include "em_prs.h"
#include "em_vdac.h"

#include "rtcdriver.h"

#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"

#include "bspconfig.h"
#include "bsp_trace.h"

/***************************************************************************//**
 * Macro definitions
 ******************************************************************************/

#define LCSENSE_NUMOF_EVENTS          5U

#define LCSENSE_SENSOR_PORT    gpioPortA
#define LCSENSE_SENSOR_PIN            4U
#define LCSENSE_SENSOR_CHANNEL       12U
#define LCSENSE_DAC_CHANNEL           4U

#define LCSENSE_BUTTON_FLAG       (1 << 6)

#define AWAKE_TIME_SEC                4U

// Type definition for global state.
typedef enum {
  MODE_SINGLE = 0,
  MODE_5X     = 1
} LCSENSE_GlobalMode_TypeDef;

// Type definition for global state.
typedef enum {
  WAKEUP_STATE = 0,
  AWAKE_STATE = 1,
  SLEEP_STATE = 2
} LCSENSE_GlobalState_TypeDef;

/***************************************************************************//**
 * Global variables
 ******************************************************************************/
static volatile LCSENSE_GlobalMode_TypeDef appModeGlobal = MODE_SINGLE;
static volatile LCSENSE_GlobalState_TypeDef appStateGlobal = WAKEUP_STATE;
static volatile uint8_t eventCounter = 0U;
static DISPLAY_Device_t displayDevice;     // Display device handle.

/* RTC callback parameters. */
static void (*rtcCallback)(void*) = NULL;
static void * rtcCallbackArg = 0;
volatile uint32_t rtcCallbacks = 0;

/** Timer used for time how long we should be awake. */
RTCDRV_TimerID_t awakeTimerId;

/***************************************************************************//**
 * Prototypes
 ******************************************************************************/
void LESENSE_IRQHandler(void);
void PCNT0_IRQHandler(void);
void GPIO_EVEN_IRQHandler(void);

static void setupCMU(void);
static void setupVDAC(void);
static void setupGPIO(void);
static void setupACMP(void);
static void setupLESENSE(void);
static void setupPRS(void);
static void setupPCNT(void);
static void setupDisplay(void);
static void updateDisplay(void);
static void turnOffDisplay(void);
static void turnOnDisplay(void);
static void awakeTimerCallback(RTCDRV_TimerID_t id, void *user);
static void memLcdCallback(RTCDRV_TimerID_t id, void *user);

/***************************************************************************//**
 * @brief  Setup the CMU
 ******************************************************************************/
void setupCMU(void)
{
  // Ensure core frequency has been updated
  SystemCoreClockUpdate();

  // Select clock source for HF clock.
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
  // Select clock source for LFA clock.
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
  // Select clock source for LFE clock.
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFRCO);
  // Enable clock to the interface with low energy modules.
  CMU_ClockEnable(cmuClock_HFLE, true);
  // Enable HF peripheral clock.
  CMU_ClockEnable(cmuClock_HFPER, true);
  // Enable clock for GPIO.
  CMU_ClockEnable(cmuClock_GPIO, true);
  // Enable clock for ACMP0.
  CMU_ClockEnable(cmuClock_ACMP0, true);
  // Enable clock for PRS.
  CMU_ClockEnable(cmuClock_PRS, true);
  // Enable clock for PCNT.
  CMU_ClockEnable(cmuClock_PCNT0, true);
  // Enable clock on RTCC.
  CMU_ClockEnable(cmuClock_RTCC, true);
  // Enable clock for LESENSE.
  CMU_ClockEnable(cmuClock_LESENSE, true);
  // Set clock divider for LESENSE.
  CMU_ClockDivSet(cmuClock_LESENSE, cmuClkDiv_1);
  // Enable clock for VDAC
  CMU_ClockEnable(cmuClock_VDAC0, true);
}

/***************************************************************************//**
 * @brief  Sets up the VDAC
 ******************************************************************************/
void setupVDAC(void)
{
  // Configuration structure for the VDAC
  VDAC_Init_TypeDef vdacInit = VDAC_INIT_DEFAULT;

  // Configuration structure for VDAC channel
  VDAC_InitChannel_TypeDef vdacChInit = VDAC_INITCHANNEL_DEFAULT;

  vdacInit.asyncClockMode  = true;         // Use asynchronous clock mode.
  vdacInit.reference       = vdacRefAvdd;  // VDD as reference reference.

  vdacChInit.trigMode      = vdacTrigModeLesense;// Conversion trigged by LESENSE.

  // Initialize channel
  VDAC_InitChannel(VDAC0, &vdacChInit, 1);

  // Initialize VDAC
  VDAC_Init(VDAC0, &vdacInit);

  // Configure DAC0OUT1 on ALT#0 (PD12)
  VDAC0->OPA[1].OUT = VDAC_OPA_OUT_ALTOUTEN | VDAC_OPA_OUT_ALTOUTPADEN_OUT0;

  // Set data for DAC channel 1 to 1/2 of the reference (VDD)
  VDAC_Channel1OutputSet(VDAC0, 0x800);

  // Enable channel 1
  VDAC_Enable(VDAC0, 1, true);
}

/***************************************************************************//**
 * @brief  Setup the GPIO
 ******************************************************************************/
void setupGPIO(void)
{
  // Configure measuring pin as push pull
  GPIO_PinModeSet(LCSENSE_SENSOR_PORT, LCSENSE_SENSOR_PIN, gpioModePushPull, 0);
  // Enable push button 0 pin as input.
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 0);
  // Enable interrupts for that pin.
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);
  // Enable GPIO_EVEN interrupt vector in NVIC.
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);
}

/***************************************************************************//**
 * @brief  Setup the ACMP
 ******************************************************************************/
void setupACMP(void)
{
  // ACMP configuration constant table.
  ACMP_Init_TypeDef initACMP = ACMP_INIT_DEFAULT;

  initACMP.fullBias          = true;                 // Enable full bias
  initACMP.biasProg          = 0x05;                 // Set bias programming level
  initACMP.hysteresisLevel_0 = acmpHysteresisLevel0; // No hysteresis
  initACMP.hysteresisLevel_1 = acmpHysteresisLevel0; // No hysteresis
  initACMP.enable            = false;                // Do not enable after configuration

  static const ACMP_VAConfig_TypeDef configVA =
  {
    .div0 = 0x22, // ACMP negative input just above voltage generated by VDAC
    .div1 = 0x22, // ACMP negative input just above voltage generated by VDAC
    .input = acmpVAInputVDD // Choose VDD as input to VADIV
  };

  ACMP_VASetup(ACMP0, &configVA);

  ACMP_Init(ACMP0, &initACMP);

  // Configure inputs VADIV and VSS (this input is overridden by LESENSE)
  ACMP_ChannelSet(ACMP0, acmpInputVADIV, acmpInputVSS);

  ACMP_Enable(ACMP0);

  while (!(ACMP0->IF & ACMP_IF_WARMUP)) ;

  // Configure external input control from LESENSE. Must be done after warmup as
  // LESENSE disables the ACMP until it is needed for a channel scan.
  ACMP_ExternalInputSelect(ACMP0, acmpExternalInputAPORT3X);
}

/***************************************************************************//**
 * @brief  Setup the LESENSE
 ******************************************************************************/
void setupLESENSE(void)
{
  // LESENSE central configuration constant table.
  LESENSE_Init_TypeDef initLESENSE = LESENSE_INIT_DEFAULT;

  // LESENSE channel configuration constant table.
  LESENSE_ChAll_TypeDef initChs = LESENSE_SCAN_CONF_DEFAULT;

  initLESENSE.coreCtrl.biasMode     = lesenseBiasModeDutyCycle;// Duty cycle bias
  initLESENSE.coreCtrl.storeScanRes = false;                   // Do not store scan results
  initLESENSE.timeCtrl.startDelay   = 1U;                      // 1 cycle startup delay
  initLESENSE.perCtrl.dacCh1En      = true;                    // LESENSE controls DAC channel 1
  initLESENSE.perCtrl.acmp0Mode     = lesenseACMPModeMux;      // LESENSE controls the input mux of ACMP0
  initLESENSE.perCtrl.acmp1Mode     = lesenseACMPModeDisable;  // LESENSE does not control ACMP1
  initLESENSE.perCtrl.warmupMode    = lesenseWarmupModeNormal; // Do not keep ACMP and DAC warm

  initChs.Ch[LCSENSE_SENSOR_CHANNEL].enaScanCh   = true;                     // Enable ch scan
  initChs.Ch[LCSENSE_SENSOR_CHANNEL].enaPin      = true;                     // Enable ch pin
  initChs.Ch[LCSENSE_SENSOR_CHANNEL].enaInt      = true;                     // Enable ch interrupt
  initChs.Ch[LCSENSE_SENSOR_CHANNEL].chPinExMode = lesenseChPinExLow;        // Pin low during excitation
  initChs.Ch[LCSENSE_SENSOR_CHANNEL].storeCntRes = true;                     // Enable to store counter in result register
  initChs.Ch[LCSENSE_SENSOR_CHANNEL].exClk       = lesenseClkHF;             // Use HF clock for excitation timing
  initChs.Ch[LCSENSE_SENSOR_CHANNEL].exTime      = 15U;                      // 15(+1) clock cycles excitation time
  initChs.Ch[LCSENSE_SENSOR_CHANNEL].sampleDelay = 1U;                       // 1(+1) clock cycle sample delay
  initChs.Ch[LCSENSE_SENSOR_CHANNEL].measDelay   = 0U;                       // 0 cycles measurement delay
  initChs.Ch[LCSENSE_SENSOR_CHANNEL].sampleMode  = lesenseSampleModeCounter; // User counter in comparison
  initChs.Ch[LCSENSE_SENSOR_CHANNEL].intMode     = lesenseSetIntPosEdge;     // Interrupt is generated on rising edge
  initChs.Ch[LCSENSE_SENSOR_CHANNEL].cntThres    = 0U;                       // Threshold set to 0, calibration done later

  // When LESENSE controls the VDAC, it also controls the alternative VDAC
  // outputs. As our DAC_LC_EXCITE pin is a VDAC alternate output we need to
  // configure the LESENSE channel (ch 4) that is connected to this pin to
  // to be driven by the VDAC. As we are not enabling the actual LESENSE channel
  // (only the pin output), we only need to configure the idle config to output
  // the VDAC as the channel always then be in idle mode.
  // Note that configuring this extra channel is not needed when using the main
  // VDAC outputs or APORT.
  initChs.Ch[LCSENSE_DAC_CHANNEL].enaPin        = true;                 // Enable ch pin
  initChs.Ch[LCSENSE_DAC_CHANNEL].chPinIdleMode = lesenseChPinIdleDACC; // Pin driven by DAC when IDLE

  // Initialize LESENSE interface with RESET.
  LESENSE_Init(&initLESENSE, true);

  // Configure scan channels.
  LESENSE_ChannelAllConfig(&initChs);

  // Set scan frequency (in Hz).
  (void)LESENSE_ScanFreqSet(0U, 10U);

  // Set clock divisor for LF clock.
  LESENSE_ClkDivSet(lesenseClkLF, lesenseClkDiv_2);

  // Set clock divisor for HF clock.
  LESENSE_ClkDivSet(lesenseClkHF, lesenseClkDiv_1);

  // Start scanning LESENSE channels.
  LESENSE_ScanStart();

  // We start scanning and assume there is no metal close at this point. We can
  // then use the measurement values as a basis for the detection threshold.
  // Because of analog settling time, the first couple of measurements will be
  // off, so we use the last buffer value (the 16th result) as our threshold
  // basis. The threshold is then set equal to the 16th result subtracted by 1
  // as the values could fluctuate with 1 LSB and we don't want false triggers.
  // E.g. if the last result reads 16, the threshold is 15 and the measured
  // value must be 14 or less to trigger a detection.

  // Waiting for buffer to be full
  while (!(LESENSE->STATUS & LESENSE_STATUS_BUFFULL)) ;

  // Use the last result -1 as threshold value
  LESENSE_ChannelThresSet(LCSENSE_SENSOR_CHANNEL,
                          0,
                          LESENSE_ScanResultDataBufferGet(15) - 1);
}

/***************************************************************************//**
 * @brief  Setup the PRS
 ******************************************************************************/
void setupPRS(void)
{
  // PRS channel 0 configuration to listen to LESENSE channel 12. The PCNT then
  // counts based on this PRS channel.
  PRS_SourceAsyncSignalSet(0U,
                           PRS_CH_CTRL_SOURCESEL_LESENSEH,
                           PRS_CH_CTRL_SIGSEL_LESENSESCANRES12);
}

/***************************************************************************//**
 * @brief  Setup the PCNT
 ******************************************************************************/
void setupPCNT(void)
{
  // PCNT configuration constant table.
  PCNT_Init_TypeDef initPCNT = PCNT_INIT_DEFAULT;

  initPCNT.top = LCSENSE_NUMOF_EVENTS;

  // Initialize PCNT.
  PCNT_Init(PCNT0, &initPCNT);
  // Enable PRS input S0 in PCNT.
  PCNT_PRSInputEnable(PCNT0, pcntPRSInputS0, true);

  // Set top value
  PCNT0->TOPB = LCSENSE_NUMOF_EVENTS - 1U;

  // Enable the PCNT peripheral.
  PCNT_Enable(PCNT0, pcntModeOvsSingle);
}

/***************************************************************************//**
 * @brief  Setup the display
 ******************************************************************************/
void setupDisplay(void)
{
  DISPLAY_Init();

  // Retrieve the properties of the display.
  if (DISPLAY_DeviceGet(0, &displayDevice) != DISPLAY_EMSTATUS_OK) {
    // Unable to get display handle.
    EFM_ASSERT(false);
  }

  // Retarget stdio to the display.
  if (TEXTDISPLAY_EMSTATUS_OK != RETARGET_TextDisplayInit()) {
    // Text display initialization failed.
    EFM_ASSERT(false);
  }
}

/***************************************************************************//**
 * @brief  Update the display with mode and detection number
 ******************************************************************************/
void updateDisplay()
{
  printf(TEXTDISPLAY_ESC_SEQ_CURSOR_HOME_VT100);

  printf("\n    LC SENSE"
         "\n"
         "\n  Push BTN0 to"
         "\n  switch mode "
         "\n\n");

  switch (appModeGlobal) {
    case MODE_SINGLE:
      printf("\n\n  Single Mode  ");
      break;
    case MODE_5X:
      printf("\n\n    5x Mode    ");
      break;
    default:
      break;
  }

  printf("\n\n\n\n Detections:%3d", eventCounter);
}

/***************************************************************************//**
 * @brief  Turn off the display
 ******************************************************************************/
void turnOffDisplay(void)
{
  // Clear LCD display.
  printf("\f");

  // Power down LCD display.
  displayDevice.pDisplayPowerOn(&displayDevice, false);
}

/***************************************************************************//**
 * @brief  Turn on the display
 ******************************************************************************/
void turnOnDisplay(void)
{
  // Power up LCD display.
  displayDevice.pDisplayPowerOn(&displayDevice, true);
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  CORE_DECLARE_IRQ_STATE;

  // Use default settings for DCDC, EM23
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  EMU_EM01Init_TypeDef em01Init = EMU_EM01INIT_DEFAULT;

  // Enable voltage downscaling in EM0 to 3.
  em01Init.vScaleEM01LowPowerVoltageEnable = true;
  em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;

  // Chip errata
  CHIP_Init();

  // Initialize DCDC.
  EMU_DCDCInit(&dcdcInit);

  // Initialize EM0123 with default parameters
  EMU_EM01Init(&em01Init);
  EMU_EM23Init(&em23Init);

  // If first word of user data page is non-zero, enable Energy Profiler trace
  BSP_TraceProfilerSetup();

  // Disable interrupts
  CORE_ENTER_ATOMIC();

  // Setup peripherals
  setupCMU();
  setupGPIO();
  setupACMP();
  setupPRS();
  setupPCNT();
  setupVDAC();
  setupLESENSE();

  // Setup RTC driver
  RTCDRV_Init();
  RTCDRV_AllocateTimer(&awakeTimerId);

  // Disable retention for unused peripherals
  EMU_PeripheralRetention((EMU_PeripheralRetention_TypeDef)
                          (emuPeripheralRetention_ALL
                           & ~(emuPeripheralRetention_ACMP0
                               | emuPeripheralRetention_VDAC0
                               | emuPeripheralRetention_PCNT0
                               | emuPeripheralRetention_LESENSE0)),
                          false);

  // Configure and update display
  setupDisplay();
  updateDisplay();

  // Enable interrupts in NVIC.
  NVIC_EnableIRQ(PCNT0_IRQn);
  NVIC_EnableIRQ(LESENSE_IRQn);

  // Initialization done, enable interrupts globally.
  CORE_EXIT_ATOMIC();

  // Go to infinite loop.
  while (1) {
    switch (appStateGlobal) {
      case WAKEUP_STATE:
        CMU_ClockEnable(cmuClock_RTCC, true);
        RTCDRV_StartTimer(awakeTimerId,
                          rtcdrvTimerTypeOneshot,
                          AWAKE_TIME_SEC * 1000,
                          awakeTimerCallback,
                          NULL);
        turnOnDisplay();
        updateDisplay();
        appStateGlobal = AWAKE_STATE;

      case AWAKE_STATE:
        break;

      case SLEEP_STATE:
        CMU_ClockEnable(cmuClock_RTCC, false);
        turnOffDisplay();
        break;

      default:
        break;
    }
    EMU_EnterEM2(false);
  }
}

/***************************************************************************//**
 * @brief   The actual callback for Memory LCD toggling
 * @param[in] id
 *   The id of the RTC timer (not used)
 ******************************************************************************/
static void memLcdCallback(RTCDRV_TimerID_t id, void *user)
{
  (void) id;
  (void) user;
  rtcCallback(rtcCallbackArg);

  rtcCallbacks++;
}

/***************************************************************************//**
 * @brief   Register a callback function at the given frequency.
 *
 * @param[in] pFunction  Pointer to function that should be called at the
 *                       given frequency.
 * @param[in] argument   Argument to be given to the function.
 * @param[in] frequency  Frequency at which to call function at.
 *
 * @return  0 for successful or
 *         -1 if the requested frequency does not match the RTC frequency.
 ******************************************************************************/
int rtcIntCallbackRegister(void (*pFunction)(void*),
                           void* argument,
                           unsigned int frequency)
{
  RTCDRV_TimerID_t timerId;
  rtcCallback    = pFunction;
  rtcCallbackArg = argument;

  RTCDRV_AllocateTimer(&timerId);

  RTCDRV_StartTimer(timerId,
                    rtcdrvTimerTypePeriodic,
                    1000 / frequency,
                    memLcdCallback,
                    NULL);

  return 0;
}

/***************************************************************************//**
 * @brief Callback used when awake period has passed
 ******************************************************************************/
static void awakeTimerCallback(RTCDRV_TimerID_t id, void *user)
{
  (void) id;
  (void) user;
  appStateGlobal = SLEEP_STATE;
}

/***************************************************************************//**
 * @brief  LESENSE interrupt handler
 ******************************************************************************/
void LESENSE_IRQHandler(void)
{
  // Negative edge interrupt on LESENSE CH12.
  if (LESENSE_IF_CH12 & LESENSE_IntGetEnabled()) {
    LESENSE_IntClear(LESENSE_IF_CH12);
  }

  // Increase counter only if in single mode, 5x mode is handled in PCNT ISR
  if (appModeGlobal == MODE_SINGLE) {
    eventCounter++;

    appStateGlobal = WAKEUP_STATE;
  }
}

/***************************************************************************//**
 * @brief  PCNT interrupt handler
 ******************************************************************************/
void PCNT0_IRQHandler(void)
{
  // Overflow interrupt on PCNT0.
  PCNT_IntClear(PCNT0, PCNT_IF_OF);

  // Increase the counter with the number of events that triggered the PCNT
  // overflow.
  eventCounter = eventCounter + LCSENSE_NUMOF_EVENTS;

  appStateGlobal = WAKEUP_STATE;
}

/***************************************************************************//**
 * @brief  GPIO even interrupt handler (for handling button events)
 ******************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  GPIO_IntClear(LCSENSE_BUTTON_FLAG);

  if (appModeGlobal == MODE_SINGLE) {
    appModeGlobal = MODE_5X;

    // Use PCNT interrupt instead of LESENSE to wake up in 5x mode
    LESENSE_IntDisable(LESENSE_IEN_CH12);

    // Reset PCNT counter value to 0
    PCNT_CounterTopSet(PCNT0, 0, LCSENSE_NUMOF_EVENTS - 1);

    PCNT_IntClear(PCNT0, PCNT_IF_OF);
    PCNT_IntEnable(PCNT0, PCNT_IEN_OF);
  } else {
    appModeGlobal = MODE_SINGLE;

    // Use LESENSE interrupt instead of PCNT to wake up in single mode
    LESENSE_IntClear(LESENSE_IFC_CH12);
    LESENSE_IntEnable(LESENSE_IEN_CH12);

    PCNT_IntDisable(PCNT0, PCNT_IEN_OF);
  }
  appStateGlobal = WAKEUP_STATE;
}
