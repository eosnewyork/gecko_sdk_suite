/***************************************************************************//**
 * @file
 * @brief uC-Probe Terminal Example
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

#include  <kernel/include/os.h>
#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>
#include  <common/include/rtos_utils.h>

#include  "ex_probe_term.h"

#include  <string.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------- COMMAND LINE INTERFACE ---------------
#ifndef  PROBE_TERM_CFG_CMD_EN
#define  PROBE_TERM_CFG_CMD_EN                        1         // Interface enable.
#endif

#ifndef  PROBE_TERM_OS_CFG_CMD_RX_TASK_STK_SIZE
#define  PROBE_TERM_OS_CFG_CMD_RX_TASK_STK_SIZE     128         // OS task stack size.
#endif

#ifndef  PROBE_TERM_OS_CFG_CMD_RX_TASK_PRIO
#define  PROBE_TERM_OS_CFG_CMD_RX_TASK_PRIO          10         // OS task priority.
#endif

#ifndef  PROBE_TERM_OS_CFG_CMD_TX_TASK_STK_SIZE
#define  PROBE_TERM_OS_CFG_CMD_TX_TASK_STK_SIZE     128         // OS task stack size.
#endif

#ifndef  PROBE_TERM_OS_CFG_CMD_TX_TASK_PRIO
#define  PROBE_TERM_OS_CFG_CMD_TX_TASK_PRIO          11         // OS task priority.
#endif

//                                                                 ----------------- TRACE INTERFACE ------------------
#ifndef  PROBE_TERM_CFG_TRC_EN
#define  PROBE_TERM_CFG_TRC_EN                        1         // Interface enable.
#endif

#ifndef  PROBE_TERM_OS_CFG_TRC_TASK_STK_SIZE
#define  PROBE_TERM_OS_CFG_TRC_TASK_STK_SIZE        128         // OS task stack size.
#endif

#ifndef  PROBE_TERM_OS_CFG_TRC_TASK_PRIO
#define  PROBE_TERM_OS_CFG_TRC_TASK_PRIO             12         // OS task priority.
#endif

#ifndef  PROBE_TERM_CFG_BUF_SIZE
#define  PROBE_TERM_CFG_BUF_SIZE                     64         // Max size of the Rx and Tx message arrays.
#endif

#ifndef  PROBE_TERM_CFG_Q_SIZE
#define  PROBE_TERM_CFG_Q_SIZE                       16         // Max number of message arrays in the queues.
#endif

#ifndef  PROBE_TERM_OS_CFG_TASK_DLY_MSEC
#define  PROBE_TERM_OS_CFG_TASK_DLY_MSEC            100         // OS task delay in milliseconds to yield the CPU.
#endif

/********************************************************************************************************
 *                                            LOCAL DEFINES
 *******************************************************************************************************/

#define  PROBE_TERM_RX_STATE_IDLE            0                  // Rx State Machine: Idle           State.
#define  PROBE_TERM_RX_STATE_INIT            1                  // Rx State Machine: Initialization State.
#define  PROBE_TERM_RX_STATE_WAIT4_DS        2                  // Rx State Machine: Wait for Data-Sent.

#define  PROBE_TERM_TX_STATE_IDLE            0                  // Tx State Machine: Idle           State.
#define  PROBE_TERM_TX_STATE_INIT            1                  // Tx State Machine: Initialization State.
#define  PROBE_TERM_TX_STATE_WAIT4_CTS       2                  // Tx State Machine: Wait for Clear-To-Send.

#define  PROBE_TERM_TIMEOUT_MSEC          2000                  // Time to wait for probe to raise its flags.

#define  PROBE_TERM_Q_MSG_TYPE_CMD           1                  // Message for command line purposes.
#define  PROBE_TERM_Q_MSG_TYPE_TRC           2                  // Message for tracing      purposes.

#define  PROBE_TERM_MAX_BUF_SIZE            64u                 // This is the buffer size that uC/Probe has allocated.
//                                                                 This is not a configuration setting unless uC/Probe
//                                                                 is changed accordingly.

/********************************************************************************************************
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#if (PROBE_TERM_CFG_CMD_EN > 0 || PROBE_TERM_CFG_TRC_EN > 0)

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (PROBE_TERM_CFG_CMD_EN > 0)
typedef  struct  probe_term_cmd_data {
  CPU_INT08U RTS;                                               // Request-To-Send flag raised by the target/probe.
  CPU_INT08U DS;                                                // Data-Sent       flag raised by the target/probe.
  CPU_INT08U CTS;                                               // Clear-To-Send   flag raised by the target/probe.
  CPU_CHAR   Buf[PROBE_TERM_CFG_BUF_SIZE];                      // Used to rx/tx the command/response from probe/target.
  CPU_INT08U BufLen;                                            // Number of chars in the buffer.
} PROBE_TERM_CMD_DATA;

typedef  struct  probe_term_cmd {
  PROBE_TERM_CMD_DATA Target;                                   // Embedded target variables.
  PROBE_TERM_CMD_DATA Probe;                                    // uC/Probe variables.
} PROBE_TERM_CMD;
#endif

#if (PROBE_TERM_CFG_TRC_EN > 0)
typedef  struct  probe_term_trc_target {
  CPU_INT08U RTS;                                               // Request-To-Send flag raised by the target.
  CPU_INT08U DS;                                                // Data-Sent       flag raised by the target.
  CPU_CHAR   Buf[PROBE_TERM_CFG_BUF_SIZE];                      // Used to tx the tracing message to probe.
  CPU_INT08U BufLen;                                            // Number of chars in the buffer.
} PROBE_TERM_TRC_TARGET;

typedef  struct  probe_term_trc_probe {
  CPU_INT08U CTS;                                               // Clear-To-Send   flag raised by probe.
} PROBE_TERM_TRC_PROBE;

typedef  struct  probe_term_trc {                               // -------- UC/PROBE TERMINAL WINDOW VARIABLES --------
  PROBE_TERM_TRC_TARGET Target;                                 // Embedded target variables.
  PROBE_TERM_TRC_PROBE  Probe;                                  // uC/Probe variables.
} PROBE_TERM_TRC;
#endif

typedef  struct  probe_term {                                   // ------------- UC/PROBE TERMINAL WINDOW -------------
#if (PROBE_TERM_CFG_CMD_EN > 0)
  PROBE_TERM_CMD Cmd;                                           // Command line interface.
#endif
#if (PROBE_TERM_CFG_TRC_EN > 0)
  PROBE_TERM_TRC Trc;                                           // Tracing      interface.
#endif
} PROBE_TERM;

typedef struct  probe_term_states {                             // --------- UC/PROBE TERMINAL WINDOW STATES ----------
  CPU_BOOLEAN RxState;                                          // Current state of the Reception    state machine.
  CPU_BOOLEAN TxState;                                          // Current state of the Transmission state machine.
  CPU_INT08U  RxCtr;                                            // Counter for Reception    timeout purposes.
  CPU_INT08U  TxCtr;                                            // Counter for Transmission timeout purposes.
} PROBE_TERM_STATES;

typedef struct  probe_term_sm {                                 // ----- UC/PROBE TERMINAL WINDOW STATE MACHINES ------
#if (PROBE_TERM_CFG_CMD_EN > 0)
  PROBE_TERM_STATES Cmd;                                        // Command line interface.
#endif
#if (PROBE_TERM_CFG_TRC_EN > 0)
  PROBE_TERM_STATES Trc;                                        // Tracing      interface.
#endif
} PROBE_TERM_SM;

typedef struct  probe_term_q_msg PROBE_TERM_Q_MSG;

struct  probe_term_q_msg {                                      // ------ UC/PROBE TERMINAL WINDOW MESSAGE QUEUE ------
  CPU_CHAR         Msg[PROBE_TERM_CFG_BUF_SIZE];
  CPU_INT08U       MsgLen;
  PROBE_TERM_Q_MSG *NextPtr;
};

typedef struct  probe_term_q {
  PROBE_TERM_Q_MSG Q[PROBE_TERM_CFG_Q_SIZE];                    // This queue holds messages for trace and command line.
  PROBE_TERM_Q_MSG *FreePtr;                                    // Points to the next available item from the pool.
  PROBE_TERM_Q_MSG *HeadPtr;                                    // Points to the head of the message queue.
  PROBE_TERM_Q_MSG *TailPtr;                                    // Points to the tail of the message queue.
  CPU_INT32U       MsgCtr;                                      // Number of messages in the queue.
} PROBE_TERM_Q;

typedef struct  probe_term_q_data {
  PROBE_TERM_Q Cmd;                                             // Command line interface.
  PROBE_TERM_Q Trc;                                             // Tracing      interface.
} PROBE_TERM_Q_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

CPU_INT08U const ProbeTermBufSize = PROBE_TERM_CFG_BUF_SIZE;    // Max size of the buffers (255-chars max).

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (PROBE_TERM_CFG_CMD_EN > 0)
static OS_TCB  ProbeTermOS_CmdRxTaskTCB;
static CPU_STK ProbeTermOS_CmdRxTaskStk[PROBE_TERM_OS_CFG_CMD_RX_TASK_STK_SIZE];
static OS_TCB  ProbeTermOS_CmdTxTaskTCB;
static CPU_STK ProbeTermOS_CmdTxTaskStk[PROBE_TERM_OS_CFG_CMD_TX_TASK_STK_SIZE];
#endif

#if (PROBE_TERM_CFG_TRC_EN > 0)
static OS_TCB  ProbeTermOS_TrcTaskTCB;
static CPU_STK ProbeTermOS_TrcTaskStk[PROBE_TERM_OS_CFG_TRC_TASK_STK_SIZE];
#endif

static PROBE_TERM ProbeTerm;

static PROBE_TERM_SM ProbeTermSM;                               // Rx and Tx state machine variables.

static PROBE_TERM_Q_DATA ProbeTermQ;                            // Q data for the trace and cmd line interfaces.

static CPU_INT08U ProbeTermTimeoutCnt;                          // Nbr times flag should be rd until raised by uC/Probe.

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (PROBE_TERM_CFG_CMD_EN > 0)
static void ProbeTermCmdSM_Init(void);

static void ProbeTermCmdRxTaskHandler(void *p_arg);

static void ProbeTermCmdTxTaskHandler(void *p_arg);

static void ProbeTermCmdRxHandler(CPU_CHAR *p_str);

static void ProbeTermCmdPrint(CPU_CHAR *p_str);
#endif

#if (PROBE_TERM_CFG_TRC_EN > 0)
static void ProbeTermTrcSM_Init(void);

static void ProbeTermTrcTaskHandler(void *p_arg);
#endif

static void ProbeTermQ_Init(PROBE_TERM_Q *p_q,
                            CPU_INT08U   max);

static PROBE_TERM_Q *ProbeTermQ_GetRef(CPU_INT08U msg_type);

static PROBE_TERM_Q_MSG *ProbeTermQ_GetSlot(CPU_INT08U msg_type);

static void ProbeTermQ_PutMsg(CPU_INT08U       msg_type,
                              PROBE_TERM_Q_MSG *p_msg);

static CPU_BOOLEAN ProbeTermQ_GetMsg(CPU_INT08U msg_type);

static void ProbeTermQ_FreeMsg(CPU_INT08U       msg_type,
                               PROBE_TERM_Q_MSG *p_msg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                         CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#if PROBE_TERM_CFG_CMD_EN > 0
#if (PROBE_TERM_CFG_CMD_MAX_BUF_SIZE > PROBE_TERM_MAX_BUF_SIZE)
#error  "PROBE_TERM_CFG_CMD_MAX_BUF_SIZE cannot be greater than PROBE_TERM_MAX_BUF_SIZE"
#endif
#endif

#if PROBE_TERM_CFG_TRC_EN > 0
#if (PROBE_TERM_CFG_TRC_MAX_BUF_SIZE > 64u)
#error  "PROBE_TERM_CFG_TRC_MAX_BUF_SIZE cannot be greater than PROBE_TERM_MAX_BUF_SIZE"
#endif
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                             ProbeTermInit()
 *
 * @brief  Initializes the uC/Probe terminal window module for command line and tracing purposes.
 *******************************************************************************************************/
void ProbeTermInit(void)
{
  RTOS_ERR err;

  ProbeTermTimeoutCnt = PROBE_TERM_TIMEOUT_MSEC
                        / PROBE_TERM_OS_CFG_TASK_DLY_MSEC;      // Calculate the timeout count.

#if (PROBE_TERM_CFG_CMD_EN > 0)                                 // Initialize the terminal for command line purposes.
  ProbeTermCmdSM_Init();                                        // Reset the state machine for command line purposes.
  ProbeTermQ_Init(&(ProbeTermQ.Cmd), PROBE_TERM_CFG_Q_SIZE);    // Initialize the queue    for command line purposes.

  OSTaskCreate(&ProbeTermOS_CmdRxTaskTCB,
               "uC/Probe-Term Cmd-Line Rx Task",
               ProbeTermCmdRxTaskHandler,
               DEF_NULL,
               PROBE_TERM_OS_CFG_CMD_RX_TASK_PRIO,
               &ProbeTermOS_CmdRxTaskStk[0],
               PROBE_TERM_OS_CFG_CMD_RX_TASK_STK_SIZE / 10u,
               PROBE_TERM_OS_CFG_CMD_RX_TASK_STK_SIZE,
               0u,
               0u,
               DEF_NULL,
               OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
               &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  OSTaskCreate(&ProbeTermOS_CmdTxTaskTCB,
               "uC/Probe-Term Cmd-Line Tx Task",
               ProbeTermCmdTxTaskHandler,
               DEF_NULL,
               PROBE_TERM_OS_CFG_CMD_TX_TASK_PRIO,
               &ProbeTermOS_CmdTxTaskStk[0],
               PROBE_TERM_OS_CFG_CMD_TX_TASK_STK_SIZE / 10u,
               PROBE_TERM_OS_CFG_CMD_TX_TASK_STK_SIZE,
               0u,
               0u,
               DEF_NULL,
               OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
               &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
#endif

#if (PROBE_TERM_CFG_TRC_EN > 0)                                 // Initialize the terminal for tracing purposes.
  ProbeTermTrcSM_Init();                                        // Reset the state machine for tracing purposes.
  ProbeTermQ_Init(&(ProbeTermQ.Trc), PROBE_TERM_CFG_Q_SIZE);    // Initialize the queue    for tracing purposes.

  OSTaskCreate(&ProbeTermOS_TrcTaskTCB,
               "uC/Probe-Term Trace Task",
               ProbeTermTrcTaskHandler,
               DEF_NULL,
               PROBE_TERM_OS_CFG_TRC_TASK_PRIO,
               &ProbeTermOS_TrcTaskStk[0],
               PROBE_TERM_OS_CFG_TRC_TASK_STK_SIZE / 10u,
               PROBE_TERM_OS_CFG_TRC_TASK_STK_SIZE,
               0u,
               0u,
               DEF_NULL,
               OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
               &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
#endif
}

/****************************************************************************************************//**
 *                                           ProbeTermTrcPrint()
 *
 * @brief  Function to output trace events to the uC/Probe terminal window.
 *
 * @param  p_str  Pointer to string containing the trace event information.
 *******************************************************************************************************/
void ProbeTermTrcPrint(CPU_CHAR *p_str)
{
#if (PROBE_TERM_CFG_TRC_EN > 0)
  PROBE_TERM_Q_MSG *p_msg;
  CPU_INT16U       offset;
  CPU_INT08U       chunk_cnt;
  CPU_INT08U       chunk_ix;
  char             chunk[PROBE_TERM_CFG_BUF_SIZE];
  RTOS_ERR         err;

  if (p_str != DEF_NULL) {
    //                                                             Calculate number of chunks.
    chunk_cnt = (strlen(p_str) / PROBE_TERM_CFG_BUF_SIZE) + 1;

    for (chunk_ix = 0; chunk_ix < chunk_cnt; chunk_ix++) {
      offset = chunk_ix * PROBE_TERM_CFG_BUF_SIZE;
      strncpy(&chunk[0],
              &p_str[offset],
              PROBE_TERM_CFG_BUF_SIZE - 1);                     // Extract the chunk.

      //                                                           Get a slot from the queue.
      p_msg = ProbeTermQ_GetSlot(PROBE_TERM_Q_MSG_TYPE_TRC);

      if (p_msg != DEF_NULL) {                                  // Put the message in the queue.
        p_msg->MsgLen = strlen(chunk);
        strncpy(&(p_msg->Msg[0]), &chunk[0], PROBE_TERM_CFG_BUF_SIZE);
        ProbeTermQ_PutMsg(PROBE_TERM_Q_MSG_TYPE_TRC, p_msg);

        (void)OSTaskSemPost(&ProbeTermOS_TrcTaskTCB,            // Signal the trace task to process the message.
                            OS_OPT_POST_NONE,
                            &err);
        APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
      } else {
        break;                                                  // The queue is full, unable to print the message.
      }
    }
  }
#else
  APP_RTOS_ASSERT_CRITICAL_FAIL(; );
#endif
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          ProbeTermCmdSM_Init()
 *
 * @brief  Initializes the uC/Probe terminal window module for command line interface purposes.
 *******************************************************************************************************/

#if (PROBE_TERM_CFG_CMD_EN > 0)
static void ProbeTermCmdSM_Init(void)
{
  //                                                               Initialize the handshaking flags for command line.
  ProbeTerm.Cmd.Target.BufLen = 0;
  ProbeTerm.Cmd.Target.RTS = 0;
  ProbeTerm.Cmd.Target.DS = 0;
  ProbeTerm.Cmd.Target.CTS = 0;

  ProbeTerm.Cmd.Probe.BufLen = 0;
  ProbeTerm.Cmd.Probe.RTS = 0;
  ProbeTerm.Cmd.Probe.DS = 0;
  ProbeTerm.Cmd.Probe.CTS = 0;

  //                                                               Initialize the state machine for command line.
  ProbeTermSM.Cmd.RxCtr = 0;
  ProbeTermSM.Cmd.RxState = PROBE_TERM_RX_STATE_INIT;
  ProbeTermSM.Cmd.TxCtr = 0;
  ProbeTermSM.Cmd.TxState = PROBE_TERM_TX_STATE_IDLE;
}
#endif

/****************************************************************************************************//**
 *                                       ProbeTermCmdRxTaskHandler()
 *
 * @brief  This function implements the reception task's state machine.
 *******************************************************************************************************/

#if (PROBE_TERM_CFG_CMD_EN > 0)
static void ProbeTermCmdRxTaskHandler(void *p_arg)
{
  RTOS_ERR err;

  PP_UNUSED_PARAM(p_arg);

  while (DEF_ON) {
    OSTimeDlyHMSM(0, 0, 0, PROBE_TERM_OS_CFG_TASK_DLY_MSEC,
                  OS_OPT_TIME_HMSM_STRICT,
                  &err);

    switch (ProbeTermSM.Cmd.RxState) {
      case PROBE_TERM_RX_STATE_IDLE:
        break;

      case PROBE_TERM_RX_STATE_INIT:                                // Wait for any requests from uC/Probe.
        if (ProbeTerm.Cmd.Probe.RTS == 1) {
          ProbeTerm.Cmd.Target.CTS = 1;
          ProbeTermSM.Cmd.RxState = PROBE_TERM_RX_STATE_WAIT4_DS;
        }
        break;

      case PROBE_TERM_RX_STATE_WAIT4_DS:                            // Wait for uC/Probe to finish sending the data.
        if (ProbeTerm.Cmd.Probe.DS == 1) {
          ProbeTerm.Cmd.Target.CTS = 0;
          //                                                       Truncate the buffer to the maximum.
          if (ProbeTerm.Cmd.Probe.BufLen <= PROBE_TERM_CFG_BUF_SIZE) {
            ProbeTerm.Cmd.Probe.Buf[ProbeTerm.Cmd.Probe.BufLen] = '\0';
          } else {
            ProbeTerm.Cmd.Probe.Buf[PROBE_TERM_CFG_BUF_SIZE - 1] = '\0';
          }

          ProbeTermCmdRxHandler(&ProbeTerm.Cmd.Probe.Buf[0]);

          ProbeTermSM.Cmd.RxState = PROBE_TERM_RX_STATE_INIT;
          ProbeTermSM.Cmd.RxCtr = 0;
        } else if (ProbeTermSM.Cmd.RxCtr >= ProbeTermTimeoutCnt) {
          ProbeTermCmdSM_Init();                                    // Restart if uC/Probe takes too long to respond.
        } else {
          ProbeTermSM.Cmd.RxCtr++;                                  // Increase the timeout counter.
        }
        break;

      default:
        ProbeTermSM.Cmd.RxState = PROBE_TERM_RX_STATE_INIT;
        break;
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                       ProbeTermCmdTxTaskHandler()
 *
 * @brief  This function implements the transmission task's state machine.
 *******************************************************************************************************/

#if (PROBE_TERM_CFG_CMD_EN > 0)
static void ProbeTermCmdTxTaskHandler(void *p_arg)
{
  CPU_BOOLEAN msg_ok;
  RTOS_ERR    err_os;

  PP_UNUSED_PARAM(p_arg);

  while (DEF_ON) {
    OSTimeDlyHMSM(0, 0, 0, PROBE_TERM_OS_CFG_TASK_DLY_MSEC,
                  OS_OPT_TIME_HMSM_STRICT,
                  &err_os);

    switch (ProbeTermSM.Cmd.TxState) {
      case PROBE_TERM_TX_STATE_IDLE:
        (void)OSTaskSemPend(0,
                            OS_OPT_PEND_BLOCKING,
                            DEF_NULL,
                            &err_os);
        APP_RTOS_ASSERT_CRITICAL(err_os.Code == RTOS_ERR_NONE,; );

        msg_ok = ProbeTermQ_GetMsg(PROBE_TERM_Q_MSG_TYPE_CMD);
        if (msg_ok == DEF_OK) {
          ProbeTermSM.Cmd.TxState = PROBE_TERM_TX_STATE_INIT;
        }
        break;

      case PROBE_TERM_TX_STATE_INIT:                                // Initialize the transmission of data to uC/Probe.
        ProbeTerm.Cmd.Target.DS = 0;
        ProbeTerm.Cmd.Target.RTS = 1;
        ProbeTermSM.Cmd.TxState = PROBE_TERM_TX_STATE_WAIT4_CTS;
        break;

      case PROBE_TERM_TX_STATE_WAIT4_CTS:                           // Wait for uC/Probe to be ready to receive data.
        if (ProbeTerm.Cmd.Probe.CTS == 1) {
          ProbeTerm.Cmd.Target.RTS = 0;
          ProbeTerm.Cmd.Target.DS = 1;
          ProbeTermSM.Cmd.TxCtr = 0;
          ProbeTermSM.Cmd.TxState = PROBE_TERM_TX_STATE_IDLE;
        } else if (ProbeTermSM.Cmd.TxCtr >= ProbeTermTimeoutCnt) {
          ProbeTermCmdSM_Init();                                    // Restart if uC/Probe takes too long to respond.
        } else {
          ProbeTermSM.Cmd.TxCtr++;                                  // Increase the timeout counter.
        }
        break;

      default:
        ProbeTermSM.Cmd.TxState = PROBE_TERM_TX_STATE_IDLE;
        break;
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                         ProbeTermCmdRxHandler()
 *
 * @brief  Hook called to interpret received string from terminal.
 *
 * @param  p_str  Pointer to string containing input.
 *******************************************************************************************************/

#if (PROBE_TERM_CFG_CMD_EN > 0)
static void ProbeTermCmdRxHandler(CPU_CHAR *p_str)
{
  char buf[PROBE_TERM_CFG_BUF_SIZE];

  if (strncmp(p_str, "dir", 3) == 0) {
    ProbeTermCmdPrint("file.txt\nphoto.jpg\nsong.mp3\n");
  } else if (strncmp(p_str, "ipconfig", 8) == 0) {
    ProbeTermCmdPrint("IPv4 Address......: 10.10.1.149\n");
    ProbeTermCmdPrint("Subnet Mask.......: 255.255.255.0\n");
    ProbeTermCmdPrint("Default Gateway...: 10.10.1.1\n");
  } else if (strncmp(p_str, "echo", 4) == 0) {
    if (strlen(p_str) > 5) {
      strncpy(&buf[0], &p_str[5], PROBE_TERM_CFG_BUF_SIZE - 2);
      strncat(&buf[strlen(buf)], "\n\0", 2);
    } else {
      buf[0] = '\0';
    }
    ProbeTermCmdPrint(&buf[0]);
  } else if (strncmp(p_str, "trace", 5) == 0) {
    if (strlen(p_str) > 6) {
      strncpy(&buf[0], &p_str[6], PROBE_TERM_CFG_BUF_SIZE - 2);
      strncat(&buf[strlen(buf)], "\n\0", 2);
    } else {
      buf[0] = '\0';
    }
    ProbeTermCmdPrint("Ok.\n");
    ProbeTermTrcPrint(&buf[0]);
  } else {
    ProbeTermCmdPrint("Error: unrecognized or incomplete command line.\n");
  }
}
#endif

/****************************************************************************************************//**
 *                                           ProbeTermCmdPrint()
 *
 * @brief  Function to output the command line responses back to the uC/Probe terminal window.
 *
 * @param  p_str  Pointer to string containing the response.
 *******************************************************************************************************/

#if (PROBE_TERM_CFG_CMD_EN > 0)
static void ProbeTermCmdPrint(CPU_CHAR *p_str)
{
  PROBE_TERM_Q_MSG *p_msg;
  CPU_INT16U       offset;
  CPU_INT08U       chunk_cnt;
  CPU_INT08U       chunk_ix;
  char             chunk[PROBE_TERM_CFG_BUF_SIZE];
  RTOS_ERR         err;

  if (p_str != DEF_NULL) {
    chunk_cnt = (strlen(p_str) / PROBE_TERM_CFG_BUF_SIZE) + 1;

    for (chunk_ix = 0; chunk_ix < chunk_cnt; chunk_ix++) {
      offset = chunk_ix * PROBE_TERM_CFG_BUF_SIZE;
      strncpy(&chunk[0],
              &p_str[offset],
              PROBE_TERM_CFG_BUF_SIZE);                         // Extract a chunk.

      //                                                           Get a slot from the pool.
      p_msg = ProbeTermQ_GetSlot(PROBE_TERM_Q_MSG_TYPE_CMD);

      if (p_msg != DEF_NULL) {                                  // Put the message in the queue.
        if (chunk_ix + 1 < chunk_cnt) {
          p_msg->MsgLen = PROBE_TERM_CFG_BUF_SIZE;
        } else {
          p_msg->MsgLen = strlen(chunk);
        }
        strncpy(&(p_msg->Msg[0]), &chunk[0], PROBE_TERM_CFG_BUF_SIZE);
        ProbeTermQ_PutMsg(PROBE_TERM_Q_MSG_TYPE_CMD, p_msg);

        (void)OSTaskSemPost(&ProbeTermOS_CmdTxTaskTCB,          // Signal the command task to process the message.
                            OS_OPT_POST_NONE,
                            &err);
        APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
      } else {
        break;                                                  // The queue is full, unable to print the message.
      }
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                          ProbeTermTrcSM_Init()
 *
 * @brief  Initializes the uC/Probe terminal window state machine for tracing purposes.
 *******************************************************************************************************/

#if (PROBE_TERM_CFG_TRC_EN > 0)
static void ProbeTermTrcSM_Init(void)
{
  //                                                               Initialize the handshaking flags for tracing.
  ProbeTerm.Trc.Target.BufLen = 0;
  ProbeTerm.Trc.Target.RTS = 0;
  ProbeTerm.Trc.Target.DS = 0;

  ProbeTerm.Trc.Probe.CTS = 0;

  //                                                               Initialize the state machine for tracing purposes.
  ProbeTermSM.Trc.TxCtr = 0;
  ProbeTermSM.Trc.TxState = PROBE_TERM_TX_STATE_IDLE;
}
#endif

/****************************************************************************************************//**
 *                                        ProbeTermTrcTaskHandler()
 *
 * @brief  This function implements the transmission task's state machine.
 *******************************************************************************************************/

#if (PROBE_TERM_CFG_TRC_EN > 0)
static void ProbeTermTrcTaskHandler(void *p_arg)
{
  CPU_BOOLEAN msg_ok;
  RTOS_ERR    err_os;

  PP_UNUSED_PARAM(p_arg);

  while (DEF_ON) {
    OSTimeDlyHMSM(0, 0, 0, PROBE_TERM_OS_CFG_TASK_DLY_MSEC,
                  OS_OPT_TIME_HMSM_STRICT,
                  &err_os);

    switch (ProbeTermSM.Trc.TxState) {
      case PROBE_TERM_TX_STATE_IDLE:                                // Wait until a trace message is put in the queue.
        (void)OSTaskSemPend(0,
                            OS_OPT_PEND_BLOCKING,
                            DEF_NULL,
                            &err_os);
        APP_RTOS_ASSERT_CRITICAL(err_os.Code == RTOS_ERR_NONE,; );
        msg_ok = ProbeTermQ_GetMsg(PROBE_TERM_Q_MSG_TYPE_TRC);
        if (msg_ok == DEF_OK) {
          ProbeTermSM.Trc.TxState = PROBE_TERM_TX_STATE_INIT;
        }
        break;

      case PROBE_TERM_TX_STATE_INIT:                                // Initialize the transmission of data to uC/Probe.
        ProbeTerm.Trc.Target.DS = 0;
        ProbeTerm.Trc.Target.RTS = 1;
        ProbeTermSM.Trc.TxState = PROBE_TERM_TX_STATE_WAIT4_CTS;
        break;

      case PROBE_TERM_TX_STATE_WAIT4_CTS:                           // Wait for uC/Probe to be ready to receive data.
        if (ProbeTerm.Trc.Probe.CTS == 1) {
          ProbeTerm.Trc.Target.RTS = 0;
          ProbeTerm.Trc.Target.DS = 1;
          ProbeTermSM.Trc.TxCtr = 0;
          ProbeTermSM.Trc.TxState = PROBE_TERM_TX_STATE_IDLE;
        } else if (ProbeTermSM.Trc.TxCtr >= ProbeTermTimeoutCnt) {
          ProbeTermTrcSM_Init();                                    // Restart if uC/Probe takes too long to respond.
        } else {
          ProbeTermSM.Trc.TxCtr++;                                  // Increase the timeout counter.
        }
        break;

      default:
        ProbeTermSM.Trc.TxState = PROBE_TERM_TX_STATE_IDLE;
        break;
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                            ProbeTermQ_Init()
 *
 * @brief  Initializes the uC/Probe queues.
 *
 * @param  p_q  Pointer to the queue that needs to be initialized.
 *
 * @param  max  Maximum size of the queue.
 *******************************************************************************************************/
static void ProbeTermQ_Init(PROBE_TERM_Q *p_q,
                            CPU_INT08U   max)
{
  PROBE_TERM_Q_MSG *p_msg;
  CPU_INT08U       ix;

  for (ix = 0u; ix < (max - 1u); ix++) {
    p_msg = &(p_q->Q[ix]);
    p_msg->NextPtr = &(p_q->Q[ix + 1u]);
  }

  p_msg = &(p_q->Q[ix]);
  p_msg->NextPtr = DEF_NULL;
  p_q->MsgCtr = 0u;
  p_q->FreePtr = &(p_q->Q[0]);
}

/****************************************************************************************************//**
 *                                           ProbeTermQ_GetRef()
 *
 * @brief  Get a reference to the queue.
 *
 * @param  msg_type  Type of message queue, for command line or trace purposes.
 *
 * @return  Pointer to message object, if NO error(s).
 *          Pointer to NULL,           otherwise.
 *******************************************************************************************************/
static PROBE_TERM_Q *ProbeTermQ_GetRef(CPU_INT08U msg_type)
{
  PROBE_TERM_Q *p_q = DEF_NULL;

  switch (msg_type) {
#if (PROBE_TERM_CFG_TRC_EN > 0)
    case PROBE_TERM_Q_MSG_TYPE_TRC:
      p_q = &(ProbeTermQ.Trc);
      break;
#endif

#if (PROBE_TERM_CFG_CMD_EN > 0)
    case PROBE_TERM_Q_MSG_TYPE_CMD:
      p_q = &(ProbeTermQ.Cmd);
      break;
#endif
    default:
      break;
  }

  return (p_q);
}

/****************************************************************************************************//**
 *                                          ProbeTermQ_GetSlot()
 *
 * @brief  Get a slot from the pool of available message slots.
 *
 * @param  msg_type  Type of message slot, for command line or trace purposes.
 *
 * @return  Pointer to message object, if NO error(s).
 *          Pointer to NULL,           otherwise.
 *******************************************************************************************************/
static PROBE_TERM_Q_MSG *ProbeTermQ_GetSlot(CPU_INT08U msg_type)
{
  PROBE_TERM_Q     *p_q;
  PROBE_TERM_Q_MSG *p_msg;
  CPU_SR_ALLOC();

  CPU_CRITICAL_ENTER();
  p_q = ProbeTermQ_GetRef(msg_type);

  if (p_q == DEF_NULL) {
    CPU_CRITICAL_EXIT();
    return (DEF_NULL);
  }

  if (p_q->FreePtr == DEF_NULL) {
    CPU_CRITICAL_EXIT();
    return (DEF_NULL);
  }

  p_msg = p_q->FreePtr;
  p_q->FreePtr = p_q->FreePtr->NextPtr;
  p_q->MsgCtr++;
  CPU_CRITICAL_EXIT();

  p_msg->NextPtr = DEF_NULL;

  return (p_msg);
}

/****************************************************************************************************//**
 *                                           ProbeTermQ_PutMsg()
 *
 * @brief  Enqueue a message object.
 *
 * @param  msg_type  Type of message to enqueue, for command line or trace purposes.
 *
 * @param  p_msg     Pointer to the message that needs to be enqueued.
 *******************************************************************************************************/
static void ProbeTermQ_PutMsg(CPU_INT08U       msg_type,
                              PROBE_TERM_Q_MSG *p_msg)
{
  PROBE_TERM_Q *p_q;
  CPU_SR_ALLOC();

  CPU_CRITICAL_ENTER();
  p_q = ProbeTermQ_GetRef(msg_type);

  if (p_q == DEF_NULL) {
    CPU_CRITICAL_EXIT();
    return;
  }

  if (p_q->HeadPtr == DEF_NULL) {
    p_q->HeadPtr = p_msg;
  } else {
    p_q->TailPtr->NextPtr = p_msg;
  }
  p_q->TailPtr = p_msg;
  CPU_CRITICAL_EXIT();
}

/****************************************************************************************************//**
 *                                           ProbeTermQ_GetMsg()
 *
 * @brief  Returns a message slot to the pool.
 *
 * @param  msg_type  Type of message to return, for command line or trace purposes.
 *
 * @return  DEF_OK,   if NO error(s).
 *          DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN ProbeTermQ_GetMsg(CPU_INT08U msg_type)
{
  PROBE_TERM_Q     *p_q;
  PROBE_TERM_Q_MSG *p_msg;
  CPU_BOOLEAN      ret_val = DEF_FAIL;
  CPU_SR_ALLOC();

  CPU_CRITICAL_ENTER();
  p_q = ProbeTermQ_GetRef(msg_type);

  if (p_q == DEF_NULL) {
    CPU_CRITICAL_EXIT();
    return (ret_val);
  }

  p_msg = p_q->HeadPtr;                                         // Extract the message from the queue (FIFO).

  if (p_q->HeadPtr != DEF_NULL) {
    p_q->HeadPtr = p_msg->NextPtr;
  }
  CPU_CRITICAL_EXIT();

  if (p_msg != DEF_NULL) {
    switch (msg_type) {
#if (PROBE_TERM_CFG_TRC_EN > 0)
      case PROBE_TERM_Q_MSG_TYPE_TRC:
        strncpy(&ProbeTerm.Trc.Target.Buf[0], &(p_msg->Msg[0]), p_msg->MsgLen);
        ProbeTerm.Trc.Target.BufLen = p_msg->MsgLen;
        ret_val = DEF_OK;
        ProbeTermQ_FreeMsg(msg_type, p_msg);                    // Return slot to the pool.
        break;
#endif

#if (PROBE_TERM_CFG_CMD_EN > 0)
      case PROBE_TERM_Q_MSG_TYPE_CMD:
        strncpy(&ProbeTerm.Cmd.Target.Buf[0], &(p_msg->Msg[0]), p_msg->MsgLen);
        ProbeTerm.Cmd.Target.BufLen = p_msg->MsgLen;
        ret_val = DEF_OK;
        ProbeTermQ_FreeMsg(msg_type, p_msg);                    // Return slot to the pool.
        break;
#endif

      default:
        APP_RTOS_ASSERT_CRITICAL_FAIL(DEF_FAIL);
        break;
    }
  } else {
    ret_val = DEF_FAIL;                                         // No message to return, the queue is empty.
  }

  return (ret_val);
}

/****************************************************************************************************//**
 *                                          ProbeTermQ_FreeMsg()
 *
 * @brief  Returns a message slot to the pool.
 *
 * @param  msg_type  Type of message in the queue, for command line or trace purposes.
 *
 * @param  p_msg     Pointer to the message object that needs to be freed.
 *******************************************************************************************************/
static void ProbeTermQ_FreeMsg(CPU_INT08U       msg_type,
                               PROBE_TERM_Q_MSG *p_msg)
{
  PROBE_TERM_Q *p_q;
  CPU_SR_ALLOC();

  CPU_CRITICAL_ENTER();
  p_q = ProbeTermQ_GetRef(msg_type);

  if (p_q == DEF_NULL) {
    CPU_CRITICAL_EXIT();
    return;
  }

  if (p_q->FreePtr == DEF_NULL) {
    p_msg->NextPtr = DEF_NULL;
  } else {
    p_msg->NextPtr = p_q->FreePtr;
  }
  p_q->FreePtr = p_msg;
  p_q->MsgCtr--;
  CPU_CRITICAL_EXIT();
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (PROBE_TERM_CFG_CMD_EN > 0 || PROBE_TERM_CFG_TRC_EN > 0)
