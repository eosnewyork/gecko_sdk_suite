/***************************************************************************//**
 * @file
 * @brief Sleep command for the shell.
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

#include "bsp.h"
#include "cmd_declarations.h"
#include "em_cmu.h"
#include "em_cryotimer.h"
#include "em_emu.h"
#include "em_rtcc.h"
#include "retargetserial.h"
#include "shell_util.h"

// -----------------------------------------------------------------------------
// Local defines and enums

typedef enum {
  EM0, // Energy Mode 0
  EM1, // Energy Mode 1
  EM2, // ...
  EM3, // ...
  EM4H, // Energy Mode 4 Hibernate
  EM4S, // Energy Mode 4 ShutOff
} emType_TypeDef;

// -----------------------------------------------------------------------------
// Local function prototype

static CPU_INT16S rtccWakeUp(SHELL_OUT_FNCT outFunc, emType_TypeDef eMode, int seconds);
static CPU_INT16S cryotimerWakeUp(SHELL_OUT_FNCT outFunc, int wakeUpPeriod);

static void sleepHook(bool beforeSleep);

// -----------------------------------------------------------------------------
// Global functions

/***************************************************************************//**
 * @brief
 *   Entering sleep mode:
 *     Different kind of Energy modes can be entered and different
 *     wake-up timer can be set.
 *
 * @param argc
 *   Number of arguments
 *
 * @param argv
 *   Array of pointers to strings which are the arguments.
 *
 * @param outFunc
 *   Not used.
 *
 * @param pcmd_param
 *   Not used.
 *
 * @return
 *   SHELL_EXEC_ERR on error, SHELL_EXEC_ERR_NONE otherwise.
 ******************************************************************************/
CPU_INT16S sleepCmd(CPU_INT16U argc,
                    CPU_CHAR *argv[],
                    SHELL_OUT_FNCT outFunc,
                    SHELL_CMD_PARAM *cmdParam)
{
  emType_TypeDef eMode = EM2;
  CPU_INT16S ret_val = SHELL_EXEC_ERR_NONE;
  int seconds = 1; // Default to 1 second
  RTOS_ERR err;

  (void)cmdParam;

  for (int i = 1; i < argc; i++) {
    if (!Str_Cmp(argv[i], "-em")) {
      if (!Str_Cmp(argv[i + 1], "1")) {
        shellPrintf(outFunc, "EM1 selected\n");
        eMode = EM1;
        i++;
      } else if (!Str_Cmp(argv[i + 1], "2")) {
        shellPrintf(outFunc, "EM2 selected\n");
        eMode = EM2;
        i++;
      } else if (!Str_Cmp(argv[i + 1], "3")) {
        shellPrintf(outFunc, "EM3 selected\n");
        eMode = EM3;
        i++;
      } else if (!Str_Cmp(argv[i + 1], "4H")) {
        shellPrintf(outFunc, "EM4 Hibernate selected\n");
        eMode = EM4H;
        i++;
      } else if (!Str_Cmp(argv[i + 1], "4S")) {
        shellPrintf(outFunc, "EM4 Shutoff selected\n");
        eMode = EM4S;
        i++;
      } else {
        shellPrintf(outFunc, "sleepCmd: Invalid energy mode '%s'.\n", argv[i + 1]);
        return SHELL_EXEC_ERR;
      }
    } else if (!Str_Cmp(argv[i], "-s")) {
      seconds = shellStrtol(argv[++i], &err);

      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        shellPrintf(outFunc, "sleep: Invalid argument -s '%s'.\n", argv[i]);
        return SHELL_EXEC_ERR;
      }

    } else if (!Str_Cmp(argv[i], "-help")) {
      shellPrintf(outFunc, "sleep - Enter energy modes\n"
             "Valid arguments\n"
             " -em MODE  : Enter the energy mode specified by MODE.\n"
             "             Supported MODEs are 1, 2, 3, 4H or 4S\n"
             " -s N      : Wake-up after N seconds. Default is 1 second.\n"
             " -help     : This help page.\n");
    } else {
      shellPrintf(outFunc, "sleep: Invalid argument '%s'.\n", argv[i]);
      return SHELL_EXEC_ERR;
    }
  }

  if (eMode == EM4S) {
    ret_val = cryotimerWakeUp(outFunc, seconds);   // Set-up cryotimer interrupt
  } else {
    ret_val = rtccWakeUp(outFunc, eMode, seconds); // Set-up RTCC interrupt
  }

  sleepHook(true);
  switch (eMode) {
    case EM1:
      EMU_EnterEM1(); // Entering energy mode 1
      break;
    case EM2:
      EMU_EnterEM2(true);
      break;
    case EM3:
      EMU_EnterEM3(true);
      break;
    case EM4H:
      EMU_EnterEM4H(); // Entering energy mode 4 Hibernate
      break;
    case EM4S:
      EMU->EM4CTRL |= EMU_EM4CTRL_RETAINULFRCO; // Retain ULFRCO Oscillator
      EMU_EnterEM4S(); // Entering energy mode 4 ShutOff
      break;
    default:
      break;
  }
  sleepHook(false);

  return ret_val;
}

// -----------------------------------------------------------------------------
// Local functions

/***************************************************************************//**
 * @brief
 *   To do before going into energy mode
 *
 * @param beforesleep
 *   true if before entering energy mode, false if after wake up
 ******************************************************************************/
static void sleepHook(bool beforeSleep)
{
  if (beforeSleep) {
    RETARGET_SerialFlush(); // Wait for UART TX buffer to be empty
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  } else {
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
  }
}

/***************************************************************************//**
 * @brief
 *   Setting Wake-up interrupt using RTCC before entering the
 *   specified energy mode.
 *
 * @param eMode
 *   Energy mode to be entered.
 *
 * @param wakeUpTime
 *   Wake-up time
 *
 * @return
 *   SHELL_EXEC_ERR on error, SHELL_EXEC_ERR_NONE otherwise.
 ******************************************************************************/
static CPU_INT16S rtccWakeUp(SHELL_OUT_FNCT outFunc, emType_TypeDef eMode, int seconds)
{
  if (eMode == EM3 || eMode == EM4H) {
    CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_ULFRCO); // Select ULFRCO oscillator
  } else {
    CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFRCO); // Select LFRCO oscillator
  }

  CMU_ClockEnable(cmuClock_RTCC, true); // Enable RTCC clock
  CMU_ClockEnable(cmuClock_HFLE, true); // Enable HFLE clock

  RTCC_Reset(); // Reset RTCC related registers

  RTCC_Init_TypeDef init = RTCC_INIT_DEFAULT;
  init.cntWrapOnCCV1 = 1;
  init.presc = rtccCntPresc_1;
  init.enable = false;
  RTCC_Init(&init); // Initialize RTCC

  NVIC_EnableIRQ(RTCC_IRQn); // Enable RTCC IRQ from interrupt vector

  RTCC_CCChConf_TypeDef chInit = RTCC_CH_INIT_COMPARE_DEFAULT;
  RTCC_ChannelInit(1, &chInit); // Initialize RTCC channel 1

  uint32_t clockFreq = CMU_ClockFreqGet(cmuClock_RTCC); // Get clock frequency
  uint32_t wakeUpTimer = clockFreq * seconds; // Calculate time from seconds

  RTCC_ChannelCCVSet(1, wakeUpTimer); // Specify time for wake-up
  RTCC_CounterSet(0); // Rest RTCC counter
  RTCC_EM4WakeupEnable(eMode == EM4H);
  RTCC_IntClear(RTCC_IFC_CC1);
  RTCC_IntEnable(RTCC_IFC_CC1); // Enable RTCC interrupt
  RTCC_Enable(true);

  shellPrintf(outFunc, "Wake-up scheduled in %d seconds\n", seconds);

  return SHELL_EXEC_ERR_NONE;
}

/***************************************************************************//**
 * @brief
 *   Setting Wake-up interrupt using Cryotimer before entering the
 *   specified energy mode.
 *
 * @param seconds
 *   Wake-up time
 *
 * @return
 *   SHELL_EXEC_ERR on error, SHELL_EXEC_ERR_NONE otherwise.
 ******************************************************************************/
static CPU_INT16S cryotimerWakeUp(SHELL_OUT_FNCT outFunc, int seconds)
{
  CMU_ClockEnable(cmuClock_CRYOTIMER, true); // Enable cryotimer clock

  CRYOTIMER_Init_TypeDef init = CRYOTIMER_INIT_DEFAULT;
  init.enable = false;
  CRYOTIMER_Init(&init); // Reset Cryotimer
  init.osc = cryotimerOscULFRCO;
  init.presc = cryotimerPresc_1;
  init.em4Wakeup = true;
  init.enable = true;
  CRYOTIMER_Init(&init); // Initialize Cryotimer

  uint32_t clockFreq =  SystemULFRCOClockGet();
  uint32_t cycles = clockFreq * seconds;

  // Finding the highest bit position of the variable "cycles"
  uint32_t wakeUpPeriod = 31 - __CLZ(cycles);

  // Convert actual time to sleep back to seconds
  double periodSeconds = ((double)(0x1 << wakeUpPeriod) / clockFreq);

  CRYOTIMER_PeriodSet(wakeUpPeriod);
  CRYOTIMER_IntClear(CRYOTIMER_IFC_PERIOD);
  CRYOTIMER_IntEnable(CRYOTIMER_IEN_PERIOD); // Enable cryotimer interrupt

  shellPrintf(outFunc, "Converting %d seconds to CRYOTIMER periods\n", seconds);
  shellPrintf(outFunc, "Wake-up scheduled in %.3f seconds\n", periodSeconds);

  return SHELL_EXEC_ERR_NONE;
}
/***************************************************************************//**
 * @brief
 *   Callback function for rtcc timer
 ******************************************************************************/
void RTCC_IRQHandler(void)
{
  RTCC_IntClear(RTCC_IFC_CC1);
  RTCC_IntDisable(RTCC_IFC_CC1);
}
