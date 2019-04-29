/***************************************************************************//**
 * @file
 * @brief Probe Scope Example
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <ex_description.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>

#include  "ex_probe_scope.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                             CONFIGURATION
 *******************************************************************************************************/

#ifndef  PROBE_SCOPE_MAX_CH
#define  PROBE_SCOPE_MAX_CH                       8             // The max nbr of channels: [1,8].
#endif

#ifndef  PROBE_SCOPE_MAX_SAMPLES
#define  PROBE_SCOPE_MAX_SAMPLES               200              // The max nbr of samples per channel.
#endif

#ifndef  PROBE_SCOPE_16_BIT_EN
#define  PROBE_SCOPE_16_BIT_EN                    1             // The max size of each sample is 16-bits: [0,1].
#endif

#ifndef  PROBE_SCOPE_32_BIT_EN
#define  PROBE_SCOPE_32_BIT_EN                    1             // The max size of each sample is 32-bits: [0,1].
#endif

#ifndef  PROBE_SCOPE_SAMPLING_CLK_HZ_DFLT
#define  PROBE_SCOPE_SAMPLING_CLK_HZ_DFLT      1000             // Default freq (Hz) to cfg the timer at init.
#endif

#ifndef  PROBE_SCOPE_IPL
#define  PROBE_SCOPE_IPL                         13
#endif

/********************************************************************************************************
 *                                             CONSTANTS
 *******************************************************************************************************/

#define  PROBE_SCOPE_TRIG_NEG                  0                // Trigger on Negative going slope.
#define  PROBE_SCOPE_TRIG_POS                  1                // Trigger on Positive going slope.

#define  PROBE_SCOPE_STATE_START               0
#define  PROBE_SCOPE_STATE_SAMPLING_PRE_TRIG   1
#define  PROBE_SCOPE_STATE_SAMPLING_POST_TRIG  2
#define  PROBE_SCOPE_STATE_DATA_RDY            3

#define  PROBE_SCOPE_MODE_OFF                  0
#define  PROBE_SCOPE_MODE_SINGLE               1
#define  PROBE_SCOPE_MODE_CONTINUOUS           2
#define  PROBE_SCOPE_MODE_TRIG                 3

#define  PROBE_SCOPE_INT08U                    0
#define  PROBE_SCOPE_INT08S                    1
#define  PROBE_SCOPE_INT16U                    2
#define  PROBE_SCOPE_INT16S                    3
#define  PROBE_SCOPE_INT32U                    4
#define  PROBE_SCOPE_INT32S                    5
#define  PROBE_SCOPE_FP32                      6

#define  PROBE_SCOPE_CH1                       1
#define  PROBE_SCOPE_CH2                       2
#define  PROBE_SCOPE_CH3                       3
#define  PROBE_SCOPE_CH4                       4
#define  PROBE_SCOPE_CH5                       5
#define  PROBE_SCOPE_CH6                       6
#define  PROBE_SCOPE_CH7                       7
#define  PROBE_SCOPE_CH8                       8

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  struct  scope_ch PROBE_SCOPE_CH;
typedef  union   scope_ch_sample PROBE_SCOPE_CH_SAMPLE;

union  scope_ch_sample {                                        // A sample can be any of these data types.
  CPU_INT08U Val08U;
  CPU_INT08S Val08S;
#if PROBE_SCOPE_16_BIT_EN > 0
  CPU_INT16U Val16U;
  CPU_INT16S Val16S;
#endif
#if PROBE_SCOPE_32_BIT_EN > 0
  CPU_INT32U Val32U;
  CPU_INT32S Val32S;
  CPU_FP32   ValFP32;
#endif
};

struct  scope_ch {
  CPU_BOOLEAN En;                                               // Ch enabled (1) or disabled (0).
  CPU_INT08U  DataType;                                         // Source data type:
                                                                //  0 = CPU_INT08U
                                                                //  1 = CPU_INT08S
                                                                //  2 = CPU_INT16U (only if 'PROBE_SCOPE_16_BIT_EN').
                                                                //  3 = CPU_INT16S (only if 'PROBE_SCOPE_16_BIT_EN').
                                                                //  4 = CPU_INT32U (only if 'PROBE_SCOPE_32_BIT_EN').
                                                                //  5 = CPU_INT32S (only if 'PROBE_SCOPE_32_BIT_EN').
                                                                //  6 = CPU_FP32   (only if 'PROBE_SCOPE_32_BIT_EN').
  CPU_BOOLEAN           BitEn;                                  // Bit mode enabled (1) or disabled (0)
  CPU_INT08U            BitSel;                                 // Selected bit in bit mode (0..63)
  CPU_INT08U            *DataAddr;                              // Address of source data (symbol assigned to channel).
  PROBE_SCOPE_CH_SAMPLE TrigLevel;                              // Trigger level of the channel
  PROBE_SCOPE_CH_SAMPLE Samples[PROBE_SCOPE_MAX_SAMPLES];       // Storage for samples
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static PROBE_SCOPE_CH ProbeScope_Ch1;                           // Up to 8 scope channels (minimum 1 channel).

#if PROBE_SCOPE_MAX_CH >= 2
static PROBE_SCOPE_CH ProbeScope_Ch2;
#endif

#if PROBE_SCOPE_MAX_CH >= 3
static PROBE_SCOPE_CH ProbeScope_Ch3;
#endif

#if PROBE_SCOPE_MAX_CH >= 4
static PROBE_SCOPE_CH ProbeScope_Ch4;
#endif

#if PROBE_SCOPE_MAX_CH >= 5
static PROBE_SCOPE_CH ProbeScope_Ch5;
#endif

#if PROBE_SCOPE_MAX_CH >= 6
static PROBE_SCOPE_CH ProbeScope_Ch6;
#endif

#if PROBE_SCOPE_MAX_CH >= 7
static PROBE_SCOPE_CH ProbeScope_Ch7;
#endif

#if PROBE_SCOPE_MAX_CH >= 8
static PROBE_SCOPE_CH ProbeScope_Ch8;
#endif

volatile CPU_INT32U ProbeScope_DataRdyFlag;                     // Data is ready to be read by Probe.

volatile CPU_BOOLEAN ProbeScope_InitFlag;                       // Flag indicating MCU has reset.

static CPU_INT08U ProbeScope_Mode;                              // Scope Mode:
                                                                // 0 = Stop
                                                                // 1 = Single
                                                                // 2 = Continuous
                                                                // 3 = Triggered (always assumes pre-triggering)

static CPU_INT08U ProbeScope_TrigChSel;                         // Channel that will be monitored for the trigger.

volatile CPU_INT32U ProbeScope_TrigDispPos;                     // Position of trigger on the screen (in sample nbr):
                                                                // 0 = Far left,  identical to 100% post-triggering.
                                                                // PROBE_SCOPE_MAX_SAMPLES - 1 = Far right,
                                                                // identical to 100% pre-triggering.
volatile CPU_BOOLEAN ProbeScope_TrigFlag;                       // Flag indicating that we satisfied the trigger.
volatile CPU_INT32U  ProbeScope_TrigHoldOff;                    // How many samples hold off before retriggering.
static CPU_INT32U    ProbeScope_TrigHoldOffCtr;
volatile CPU_INT32U  ProbeScope_TrigIx;                         // Position in the sample buffer where trigger found.
static CPU_BOOLEAN   ProbeScope_TrigSlope;                      // Trigger Slope.

static CPU_INT32U ProbeScope_SampleIxPrev;
static CPU_INT32U ProbeScope_SampleIxCur;
static CPU_INT32U ProbeScope_SamplesPreTrigCtr;                 // Min nbr samples needed before allowed to trigger.
static CPU_INT32U ProbeScope_SamplesPostTrigCtr;                // How many samples taken after trigger detected:
                                                                // In 100% post-trigger,
                                                                // ProbeScope_SamplesPostTrigCtr ==
                                                                // PROBE_SCOPE_MAX_SAMPLES.
                                                                // In 100% pre-trigger,  ProbeScope_SamplesPostTrigCtr
                                                                // == 0.

static CPU_INT16U   ProbeScope_SamplingClkDiv;                  // Clk source divider (1 to 1000).
static CPU_INT16U   ProbeScope_SamplingClkDivCtr;
volatile CPU_INT32U ProbeScope_SamplingClkHz;                   // Sampling Clock Frequency in Hz.

static CPU_INT08U ProbeScope_State;                             // Scope State.

/********************************************************************************************************
 ********************************************************************************************************
 *                                             CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

static volatile CPU_INT16U  ProbeScopeDbg_MaxSamples = PROBE_SCOPE_MAX_SAMPLES;
static volatile CPU_INT08U  ProbeScopeDbg_MaxCh = PROBE_SCOPE_MAX_CH;
static volatile CPU_BOOLEAN ProbeScopeDbg_16BitEn = PROBE_SCOPE_16_BIT_EN;
static volatile CPU_BOOLEAN ProbeScopeDbg_32BitEn = PROBE_SCOPE_32_BIT_EN;

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void ProbeScope_ModeOff(void);

static void ProbeScope_ModeContinuous(void);

static void ProbeScope_ModeTrig(void);

static void ProbeScope_SampleChAll(void);

static void ProbeScope_SampleCh(PROBE_SCOPE_CH *p_ch);

static void ProbeScope_SamplePosNext(void);

static void ProbeScope_IsTrig(void);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           ProbeScope_Init()
 *
 * @brief  This function is used to initialize the internals of the Oscilloscope module
 *
 * @param  sampling_clk_hz    The frequency of the sampling clock (Hz)
 *******************************************************************************************************/
void ProbeScope_Init(CPU_INT32U sampling_clk_hz)
{
  static CPU_INT32U temp = 0;

  ProbeScope_InitFlag = 0;

  ProbeScope_Ch1.En = 0;
#if PROBE_SCOPE_MAX_CH >= 2
  ProbeScope_Ch2.En = 0;
#endif
#if PROBE_SCOPE_MAX_CH >= 3
  ProbeScope_Ch3.En = 0;
#endif
#if PROBE_SCOPE_MAX_CH >= 4
  ProbeScope_Ch4.En = 0;
#endif
#if PROBE_SCOPE_MAX_CH >= 5
  ProbeScope_Ch5.En = 0;
#endif
#if PROBE_SCOPE_MAX_CH >= 6
  ProbeScope_Ch6.En = 0;
#endif
#if PROBE_SCOPE_MAX_CH >= 7
  ProbeScope_Ch7.En = 0;
#endif
#if PROBE_SCOPE_MAX_CH >= 8
  ProbeScope_Ch8.En = 0;
#endif

  ProbeScope_Mode = PROBE_SCOPE_MODE_OFF;
  ProbeScope_State = PROBE_SCOPE_STATE_START;
  ProbeScope_TrigChSel = PROBE_SCOPE_CH1;
  ProbeScope_SampleIxPrev = 0;
  ProbeScope_SampleIxCur = 0;
  ProbeScope_TrigFlag = 0;
  ProbeScope_TrigDispPos = 0;
  ProbeScope_TrigHoldOff = 0;
  ProbeScope_TrigHoldOffCtr = 0;
  ProbeScope_TrigIx = 1;
  ProbeScope_DataRdyFlag = 0;
  ProbeScope_SamplesPreTrigCtr = ProbeScope_TrigDispPos;
  ProbeScope_SamplesPostTrigCtr = PROBE_SCOPE_MAX_SAMPLES;
  ProbeScope_TrigSlope = PROBE_SCOPE_TRIG_POS;
  temp = ProbeScopeDbg_MaxSamples;
  temp += ProbeScopeDbg_MaxCh;
  temp += ProbeScopeDbg_16BitEn;
  temp += ProbeScopeDbg_32BitEn;
  ProbeScope_SamplingClkDiv = 1;
  ProbeScope_SamplingClkDivCtr = 0;
  ProbeScope_SamplingClkHz = sampling_clk_hz;

  if (ProbeScope_InitFlag == 0) {
    ProbeScope_InitFlag = 1;
  }
}

/****************************************************************************************************//**
 *                                         ProbeScope_Sampling()
 *
 * @brief  This function must be called when your code needs to take a sample of all enabled
 *         channels.
 *******************************************************************************************************/
void ProbeScope_Sampling(void)
{
  switch (ProbeScope_Mode) {
    case PROBE_SCOPE_MODE_OFF:
      ProbeScope_ModeOff();
      break;

    case PROBE_SCOPE_MODE_SINGLE:
      ProbeScope_ModeTrig();
      break;

    case PROBE_SCOPE_MODE_CONTINUOUS:
      ProbeScope_ModeContinuous();
      break;

    case PROBE_SCOPE_MODE_TRIG:
      ProbeScope_ModeTrig();
      break;
  }
}

/****************************************************************************************************//**
 *                                         ProbeScope_ModeOff()
 *
 * @brief  This is the state machine called when we are not reading samples
 *******************************************************************************************************/
static void ProbeScope_ModeOff(void)
{
  ProbeScope_State = PROBE_SCOPE_STATE_START;                   // Force Triggered mode state to START.
  if (ProbeScope_SamplingClkDiv == 0) {
    ProbeScope_SamplingClkDiv = 1;
  }
}

/****************************************************************************************************//**
 *                                      ProbeScope_ModeContinuous()
 *
 * @brief  This is the state machine called when continuous mode is selected
 *******************************************************************************************************/
static void ProbeScope_ModeContinuous(void)
{
  if (ProbeScope_DataRdyFlag == 0) {                            // Read one buffer's worth of data.
    ProbeScope_TrigFlag = 0;
    if (ProbeScope_SamplingClkDivCtr > 1) {
      ProbeScope_SamplingClkDivCtr--;
    } else {
      if (ProbeScope_SamplingClkDiv == 0) {                     // Make sure clock divider is not 0.
        ProbeScope_SamplingClkDiv = 1;
      }
      ProbeScope_SamplingClkDivCtr = ProbeScope_SamplingClkDiv;
      ProbeScope_SampleChAll();                                 // Get first sample as 'previous' val for triggering.
      ProbeScope_SamplePosNext();                               // Position to next sample.
      if (ProbeScope_SampleIxCur == 0) {                        // Read one buffer's worth of data.
        ProbeScope_DataRdyFlag = 1;
        ProbeScope_TrigFlag = 1;
      }
    }
  }
  ProbeScope_State = PROBE_SCOPE_STATE_START;                   // Force Triggered mode state to START.
}

/****************************************************************************************************//**
 *                                         ProbeScope_ModeTrig()
 *
 * @brief  This is the state machine called when triggered mode is selected.
 *******************************************************************************************************/
static void ProbeScope_ModeTrig(void)
{
  if (ProbeScope_SamplingClkDivCtr > 1) {
    ProbeScope_SamplingClkDivCtr--;
  } else {
    if (ProbeScope_SamplingClkDiv == 0) {                       // Make sure clock divider is not 0.
      ProbeScope_SamplingClkDiv = 1;
    }
    ProbeScope_SamplingClkDivCtr = ProbeScope_SamplingClkDiv;
    switch (ProbeScope_State) {
      case PROBE_SCOPE_STATE_START:
        ProbeScope_DataRdyFlag = 0;
        ProbeScope_TrigFlag = 0;
        ProbeScope_SampleIxPrev = 0;
        ProbeScope_SampleIxCur = 0;
        ProbeScope_TrigHoldOffCtr = 0;
        ProbeScope_SamplesPostTrigCtr = 0;
        //                                                         Minimum number of samples pre-trigger.
        ProbeScope_SamplesPreTrigCtr = ProbeScope_TrigDispPos;
        ProbeScope_SampleChAll();                               // Get first sample as 'previous' value for triggering.
        ProbeScope_SamplePosNext();
        ProbeScope_State = PROBE_SCOPE_STATE_SAMPLING_PRE_TRIG;
        break;

      case PROBE_SCOPE_STATE_SAMPLING_PRE_TRIG:
        ProbeScope_SampleChAll();                               // Get the next sample.
        if (ProbeScope_SamplesPreTrigCtr > 0) {                 // Need a minimum number of samples before trigger.
          ProbeScope_SamplesPreTrigCtr--;
        } else {
          ProbeScope_IsTrig();                                  // See if trigger detected.
        }
        ProbeScope_SamplePosNext();
        if (ProbeScope_TrigFlag == 1) {                         // Rem nbr samples to collect before data available.
          ProbeScope_SamplesPostTrigCtr = PROBE_SCOPE_MAX_SAMPLES - ProbeScope_TrigDispPos - 2;
          ProbeScope_State = PROBE_SCOPE_STATE_SAMPLING_POST_TRIG;
        }
        break;

      case PROBE_SCOPE_STATE_SAMPLING_POST_TRIG:
        ProbeScope_SampleChAll();                               // Get the next sample.
        ProbeScope_SamplePosNext();
        if (ProbeScope_SamplesPostTrigCtr > 0) {
          ProbeScope_SamplesPostTrigCtr--;
        } else {
          if (ProbeScope_Mode == PROBE_SCOPE_MODE_SINGLE) {
            ProbeScope_Mode = PROBE_SCOPE_MODE_OFF;
            ProbeScope_State = PROBE_SCOPE_STATE_START;
          } else {
            //                                                     Load hold-off counter.
            ProbeScope_TrigHoldOffCtr = ProbeScope_TrigHoldOff;
            ProbeScope_State = PROBE_SCOPE_STATE_DATA_RDY;
          }
          ProbeScope_DataRdyFlag = 1;                           // Trace is available in the buf.
        }
        break;

      case PROBE_SCOPE_STATE_DATA_RDY:
        if (ProbeScope_TrigHoldOffCtr > 0) {                    // Hold-off before beeing able to re-trigger.
          ProbeScope_TrigHoldOffCtr--;
        } else {
          if (ProbeScope_DataRdyFlag == 0) {                    // Give Probe time to collect and display waveform(s).
            ProbeScope_TrigFlag = 0;
            //                                                     Minimum number of samples pre-trigger.
            ProbeScope_SamplesPreTrigCtr = ProbeScope_TrigDispPos;
            ProbeScope_State = PROBE_SCOPE_STATE_SAMPLING_PRE_TRIG;
            ProbeScope_SampleChAll();                           // Get first sample as 'previous' value for triggering.
            ProbeScope_SamplePosNext();
          }
        }
        break;

      default:
        ProbeScope_State = PROBE_SCOPE_STATE_START;
        break;
    }
  }
}

/****************************************************************************************************//**
 *                                       ProbeScope_SampleChAll()
 *
 * @brief  This function is called to get a sample from all the enabled channels.
 *******************************************************************************************************/
static void ProbeScope_SampleChAll(void)
{
  ProbeScope_SampleCh(&ProbeScope_Ch1);

#if PROBE_SCOPE_MAX_CH >= 2
  ProbeScope_SampleCh(&ProbeScope_Ch2);
#endif

#if PROBE_SCOPE_MAX_CH >= 3
  ProbeScope_SampleCh(&ProbeScope_Ch3);
#endif

#if PROBE_SCOPE_MAX_CH >= 4
  ProbeScope_SampleCh(&ProbeScope_Ch4);
#endif

#if PROBE_SCOPE_MAX_CH >= 5
  ProbeScope_SampleCh(&ProbeScope_Ch5);
#endif

#if PROBE_SCOPE_MAX_CH >= 6
  ProbeScope_SampleCh(&ProbeScope_Ch6);
#endif

#if PROBE_SCOPE_MAX_CH >= 7
  ProbeScope_SampleCh(&ProbeScope_Ch7);
#endif

#if PROBE_SCOPE_MAX_CH >= 8
  ProbeScope_SampleCh(&ProbeScope_Ch8);
#endif
}

/****************************************************************************************************//**
 *                                         ProbeScope_SampleCh()
 *
 * @brief  This function is called to read a sample from the desired channel and store it in its
 *               sample buffer.
 *
 * @param  p_ch      is a pointer to the desired channel to sample.
 *******************************************************************************************************/
static void ProbeScope_SampleCh(PROBE_SCOPE_CH *p_ch)
{
  PROBE_SCOPE_CH_SAMPLE msk;                                    // Mask used to select desired bit when in bit mode.
  PROBE_SCOPE_CH_SAMPLE *p_sample;                              // Pointer to where the sample will be stored.

  if (p_ch->En == 1) {
    p_sample = &p_ch->Samples[ProbeScope_SampleIxCur];          // Point to current position in sampling buffer.
    switch (p_ch->DataType) {
      case PROBE_SCOPE_INT08U:
        if (p_ch->BitEn == 0) {
          p_sample->Val08U = *(CPU_INT08U *)p_ch->DataAddr;
        } else {
          msk.Val08U = (CPU_INT08U)1 << p_ch->BitSel;
          if (*(CPU_INT08U *)p_ch->DataAddr & msk.Val08U) {
            p_sample->Val08U = 1;
          } else {
            p_sample->Val08U = 0;
          }
        }
        break;

      case PROBE_SCOPE_INT08S:
        if (p_ch->BitEn == 0) {
          p_sample->Val08S = *(CPU_INT08S *)p_ch->DataAddr;
        } else {
          msk.Val08S = (CPU_INT08S)1 << p_ch->BitSel;
          if (*(CPU_INT08S *)p_ch->DataAddr & msk.Val08S) {
            p_sample->Val08S = 1;
          } else {
            p_sample->Val08S = 0;
          }
        }
        break;

#if PROBE_SCOPE_16_BIT_EN > 0
      case PROBE_SCOPE_INT16U:
        if (p_ch->BitEn == 0) {
          p_sample->Val16U = *(CPU_INT16U *)p_ch->DataAddr;
        } else {
          msk.Val16U = (CPU_INT16U)1 << p_ch->BitSel;
          if (*(CPU_INT16U *)p_ch->DataAddr & msk.Val16U) {
            p_sample->Val16U = 1;
          } else {
            p_sample->Val16U = 0;
          }
        }
        break;

      case PROBE_SCOPE_INT16S:
        if (p_ch->BitEn == 0) {
          p_sample->Val16S = *(CPU_INT16S *)p_ch->DataAddr;
        } else {
          msk.Val16S = (CPU_INT16S)1 << p_ch->BitSel;
          if (*(CPU_INT16S *)p_ch->DataAddr & msk.Val16S) {
            p_sample->Val16S = 1;
          } else {
            p_sample->Val16S = 0;
          }
        }
        break;
#endif

#if PROBE_SCOPE_32_BIT_EN > 0
      case PROBE_SCOPE_INT32U:
        if (p_ch->BitEn == 0) {
          p_sample->Val32U = *(CPU_INT32U *)p_ch->DataAddr;
        } else {
          msk.Val32U = (CPU_INT32U)1 << p_ch->BitSel;
          if (*(CPU_INT32U *)p_ch->DataAddr & msk.Val32U) {
            p_sample->Val32U = 1;
          } else {
            p_sample->Val32U = 0;
          }
        }
        break;

      case PROBE_SCOPE_INT32S:
        if (p_ch->BitEn == 0) {
          p_sample->Val32S = *(CPU_INT32S *)p_ch->DataAddr;
        } else {
          msk.Val32S = (CPU_INT32S)1 << p_ch->BitSel;
          if (*(CPU_INT32S *)p_ch->DataAddr & msk.Val32S) {
            p_sample->Val32S = 1;
          } else {
            p_sample->Val32S = 0;
          }
        }
        break;

      case PROBE_SCOPE_FP32:
        p_sample->ValFP32 = *(CPU_FP32 *)p_ch->DataAddr;
        break;
#endif
    }
  }
}

/****************************************************************************************************//**
 *                                      ProbeScope_SamplePosNext()
 *
 * @brief  Position Sample Index for next sample.
 *******************************************************************************************************/
static void ProbeScope_SamplePosNext(void)
{
  ProbeScope_SampleIxPrev = ProbeScope_SampleIxCur;             // Update the position of the previous sample.
  ProbeScope_SampleIxCur++;                                     // Position to where next sample will be placed.
  if (ProbeScope_SampleIxCur >= PROBE_SCOPE_MAX_SAMPLES) {      // See if we need to wrap around.
    ProbeScope_SampleIxCur = 0;
  }
}

/****************************************************************************************************//**
 *                                          ProbeScope_IsTrig()
 *
 * @brief  This function is called to detect whether the trigger channel sees the trigger
 *         conditions. The channel needs to be enabled as well. To trigger the sampling, we must see
 *         the proper slope and the trigger level must be reached.
 *******************************************************************************************************/
static void ProbeScope_IsTrig(void)
{
  PROBE_SCOPE_CH        *p_ch;
  PROBE_SCOPE_CH_SAMPLE *p_sample_prev;
  PROBE_SCOPE_CH_SAMPLE *p_sample_cur;

  switch (ProbeScope_TrigChSel) {
    case PROBE_SCOPE_CH1:
      p_ch = &ProbeScope_Ch1;
      break;

#if PROBE_SCOPE_MAX_CH >= 2
    case PROBE_SCOPE_CH2:
      p_ch = &ProbeScope_Ch2;
      break;
#endif

#if PROBE_SCOPE_MAX_CH >= 3
    case PROBE_SCOPE_CH3:
      p_ch = &ProbeScope_Ch3;
      break;
#endif

#if PROBE_SCOPE_MAX_CH >= 4
    case PROBE_SCOPE_CH4:
      p_ch = &ProbeScope_Ch4;
      break;
#endif

#if PROBE_SCOPE_MAX_CH >= 5
    case PROBE_SCOPE_CH5:
      p_ch = &ProbeScope_Ch5;
      break;
#endif

#if PROBE_SCOPE_MAX_CH >= 6
    case PROBE_SCOPE_CH6:
      p_ch = &ProbeScope_Ch6;
      break;
#endif

#if PROBE_SCOPE_MAX_CH >= 7
    case PROBE_SCOPE_CH7:
      p_ch = &ProbeScope_Ch7;
      break;
#endif

#if PROBE_SCOPE_MAX_CH >= 8
    case PROBE_SCOPE_CH8:
      p_ch = &ProbeScope_Ch8;
      break;
#endif

    default:
      p_ch = &ProbeScope_Ch1;
      ProbeScope_TrigChSel = PROBE_SCOPE_CH1;
      break;
  }
  ProbeScope_TrigFlag = 0;
  if (p_ch->En == 1) {
    p_sample_prev = &p_ch->Samples[ProbeScope_SampleIxPrev];    // Point to previous position in sampling buffer.
    p_sample_cur = &p_ch->Samples[ProbeScope_SampleIxCur];      // Point to current  position in sampling buffer.
    if (ProbeScope_TrigSlope == PROBE_SCOPE_TRIG_POS) {
      switch (p_ch->DataType) {
        case PROBE_SCOPE_INT08U:
          if (p_ch->BitEn == 0) {
            if (p_sample_prev->Val08U < p_sample_cur->Val08U) {
              if (p_sample_prev->Val08U <= p_ch->TrigLevel.Val08U) {
                if (p_sample_cur->Val08U >= p_ch->TrigLevel.Val08U) {
                  ProbeScope_TrigFlag = 1;
                  ProbeScope_TrigIx = ProbeScope_SampleIxCur;
                }
              }
            }
          } else {
            if (p_sample_prev->Val08U == 0) {
              if (p_sample_cur->Val08U != 0) {
                ProbeScope_TrigFlag = 1;
                ProbeScope_TrigIx = ProbeScope_SampleIxCur;
              }
            }
          }
          break;

        case PROBE_SCOPE_INT08S:
          if (p_ch->BitEn == 0) {
            if (p_sample_prev->Val08S < p_sample_cur->Val08S) {
              if (p_sample_prev->Val08S <= p_ch->TrigLevel.Val08S) {
                if (p_sample_cur->Val08S >= p_ch->TrigLevel.Val08S) {
                  ProbeScope_TrigFlag = 1;
                  ProbeScope_TrigIx = ProbeScope_SampleIxCur;
                }
              }
            }
          } else {
            if (p_sample_prev->Val08S == 0) {
              if (p_sample_cur->Val08S != 0) {
                ProbeScope_TrigFlag = 1;
                ProbeScope_TrigIx = ProbeScope_SampleIxCur;
              }
            }
          }
          break;

#if PROBE_SCOPE_16_BIT_EN > 0
        case PROBE_SCOPE_INT16U:
          if (p_ch->BitEn == 0) {
            if (p_sample_prev->Val16U < p_sample_cur->Val16U) {
              if (p_sample_prev->Val16U <= p_ch->TrigLevel.Val16U) {
                if (p_sample_cur->Val16U >= p_ch->TrigLevel.Val16U) {
                  ProbeScope_TrigFlag = 1;
                  ProbeScope_TrigIx = ProbeScope_SampleIxCur;
                }
              }
            }
          } else {
            if (p_sample_prev->Val16U == 0) {
              if (p_sample_cur->Val16U != 0) {
                ProbeScope_TrigFlag = 1;
                ProbeScope_TrigIx = ProbeScope_SampleIxCur;
              }
            }
          }
          break;

        case PROBE_SCOPE_INT16S:
          if (p_ch->BitEn == 0) {
            if (p_sample_prev->Val16S < p_sample_cur->Val16S) {
              if (p_sample_prev->Val16S <= p_ch->TrigLevel.Val16S) {
                if (p_sample_cur->Val16S >= p_ch->TrigLevel.Val16S) {
                  ProbeScope_TrigFlag = 1;
                  ProbeScope_TrigIx = ProbeScope_SampleIxCur;
                }
              }
            }
          } else {
            if (p_sample_prev->Val16S == 0) {
              if (p_sample_cur->Val16S != 0) {
                ProbeScope_TrigFlag = 1;
                ProbeScope_TrigIx = ProbeScope_SampleIxCur;
              }
            }
          }
          break;
#endif

#if PROBE_SCOPE_32_BIT_EN > 0
        case PROBE_SCOPE_INT32U:
          if (p_ch->BitEn == 0) {
            if (p_sample_prev->Val32U < p_sample_cur->Val32U) {
              if (p_sample_prev->Val32U <= p_ch->TrigLevel.Val32U) {
                if (p_sample_cur->Val32U >= p_ch->TrigLevel.Val32U) {
                  ProbeScope_TrigFlag = 1;
                  ProbeScope_TrigIx = ProbeScope_SampleIxCur;
                }
              }
            }
          } else {
            if (p_sample_prev->Val32U == 0) {
              if (p_sample_cur->Val32U != 0) {
                ProbeScope_TrigFlag = 1;
                ProbeScope_TrigIx = ProbeScope_SampleIxCur;
              }
            }
          }
          break;

        case PROBE_SCOPE_INT32S:
          if (p_ch->BitEn == 0) {
            if (p_sample_prev->Val32S < p_sample_cur->Val32S) {
              if (p_sample_prev->Val32S <= p_ch->TrigLevel.Val32S) {
                if (p_sample_cur->Val32S >= p_ch->TrigLevel.Val32S) {
                  ProbeScope_TrigFlag = 1;
                  ProbeScope_TrigIx = ProbeScope_SampleIxCur;
                }
              }
            }
          } else {
            if (p_sample_prev->Val32S == 0) {
              if (p_sample_cur->Val32S != 0) {
                ProbeScope_TrigFlag = 1;
                ProbeScope_TrigIx = ProbeScope_SampleIxCur;
              }
            }
          }
          break;

        case PROBE_SCOPE_FP32:
          if (p_sample_prev->ValFP32 < p_sample_cur->ValFP32) {
            if (p_sample_prev->ValFP32 <= p_ch->TrigLevel.ValFP32) {
              if (p_sample_cur->ValFP32 >= p_ch->TrigLevel.ValFP32) {
                ProbeScope_TrigFlag = 1;
                ProbeScope_TrigIx = ProbeScope_SampleIxCur;
              }
            }
          }
          break;
#endif
      }
    } else {
      switch (p_ch->DataType) {
        case PROBE_SCOPE_INT08U:
          if (p_ch->BitEn == 0) {
            if (p_sample_prev->Val08U > p_sample_cur->Val08U) {
              if (p_sample_prev->Val08U >= p_ch->TrigLevel.Val08U) {
                if (p_sample_cur->Val08U <= p_ch->TrigLevel.Val08U) {
                  ProbeScope_TrigFlag = 1;
                  ProbeScope_TrigIx = ProbeScope_SampleIxCur;
                }
              }
            }
          } else {
            if (p_sample_prev->Val08U != 0) {
              if (p_sample_cur->Val08U == 0) {
                ProbeScope_TrigFlag = 1;
                ProbeScope_TrigIx = ProbeScope_SampleIxCur;
              }
            }
          }
          break;

        case PROBE_SCOPE_INT08S:
          if (p_ch->BitEn == 0) {
            if (p_sample_prev->Val08S > p_sample_cur->Val08S) {
              if (p_sample_prev->Val08S >= p_ch->TrigLevel.Val08S) {
                if (p_sample_cur->Val08S <= p_ch->TrigLevel.Val08S) {
                  ProbeScope_TrigFlag = 1;
                  ProbeScope_TrigIx = ProbeScope_SampleIxCur;
                }
              }
            }
          } else {
            if (p_sample_prev->Val08S != 0) {
              if (p_sample_cur->Val08S == 0) {
                ProbeScope_TrigFlag = 1;
                ProbeScope_TrigIx = ProbeScope_SampleIxCur;
              }
            }
          }
          break;

#if PROBE_SCOPE_16_BIT_EN > 0
        case PROBE_SCOPE_INT16U:
          if (p_ch->BitEn == 0) {
            if (p_sample_prev->Val16U > p_sample_cur->Val16U) {
              if (p_sample_prev->Val16U >= p_ch->TrigLevel.Val16U) {
                if (p_sample_cur->Val16U <= p_ch->TrigLevel.Val16U) {
                  ProbeScope_TrigFlag = 1;
                  ProbeScope_TrigIx = ProbeScope_SampleIxCur;
                }
              }
            }
          } else {
            if (p_sample_prev->Val16U != 0) {
              if (p_sample_cur->Val16U == 0) {
                ProbeScope_TrigFlag = 1;
                ProbeScope_TrigIx = ProbeScope_SampleIxCur;
              }
            }
          }
          break;

        case PROBE_SCOPE_INT16S:
          if (p_ch->BitEn == 0) {
            if (p_sample_prev->Val16S > p_sample_cur->Val16S) {
              if (p_sample_prev->Val16S >= p_ch->TrigLevel.Val16S) {
                if (p_sample_cur->Val16S <= p_ch->TrigLevel.Val16S) {
                  ProbeScope_TrigFlag = 1;
                  ProbeScope_TrigIx = ProbeScope_SampleIxCur;
                }
              }
            }
          } else {
            if (p_sample_prev->Val16S != 0) {
              if (p_sample_cur->Val16S == 0) {
                ProbeScope_TrigFlag = 1;
                ProbeScope_TrigIx = ProbeScope_SampleIxCur;
              }
            }
          }
          break;
#endif

#if PROBE_SCOPE_32_BIT_EN > 0
        case PROBE_SCOPE_INT32U:
          if (p_ch->BitEn == 0) {
            if (p_sample_prev->Val32U > p_sample_cur->Val32U) {
              if (p_sample_prev->Val32U >= p_ch->TrigLevel.Val32U) {
                if (p_sample_cur->Val32U <= p_ch->TrigLevel.Val32U) {
                  ProbeScope_TrigFlag = 1;
                  ProbeScope_TrigIx = ProbeScope_SampleIxCur;
                }
              }
            }
          } else {
            if (p_sample_prev->Val32U != 0) {
              if (p_sample_cur->Val32U == 0) {
                ProbeScope_TrigFlag = 1;
                ProbeScope_TrigIx = ProbeScope_SampleIxCur;
              }
            }
          }
          break;

        case PROBE_SCOPE_INT32S:
          if (p_ch->BitEn == 0) {
            if (p_sample_prev->Val32S > p_sample_cur->Val32S) {
              if (p_sample_prev->Val32S >= p_ch->TrigLevel.Val32S) {
                if (p_sample_cur->Val32S <= p_ch->TrigLevel.Val32S) {
                  ProbeScope_TrigFlag = 1;
                  ProbeScope_TrigIx = ProbeScope_SampleIxCur;
                }
              }
            }
          } else {
            if (p_sample_prev->Val32S != 0) {
              if (p_sample_cur->Val32S == 0) {
                ProbeScope_TrigFlag = 1;
                ProbeScope_TrigIx = ProbeScope_SampleIxCur;
              }
            }
          }
          break;

        case PROBE_SCOPE_FP32:
          if (p_sample_prev->ValFP32 > p_sample_cur->ValFP32) {
            if (p_sample_prev->ValFP32 >= p_ch->TrigLevel.ValFP32) {
              if (p_sample_cur->ValFP32 <= p_ch->TrigLevel.ValFP32) {
                ProbeScope_TrigFlag = 1;
                ProbeScope_TrigIx = ProbeScope_SampleIxCur;
              }
            }
          }
          break;
#endif
      }
    }
  }
}
