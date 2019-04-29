/***************************************************************************//**
 * @file
 * @brief Prototypes of all functions for shell demo.
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
#ifndef CMD_DECLARATIONS_H
#define CMD_DECLARATIONS_H

#include <string.h>

#include <common/include/rtos_err.h>
#include <common/include/rtos_utils.h>
#include <common/include/shell.h>
#include <cpu/include/cpu.h>

// -----------------------------------------------------------------------------
// Function prototypes

CPU_INT16S ShellDefaultShellout (CPU_CHAR *pbuf,
                                 CPU_INT16U buf_len,
                                 void *popt);

CPU_INT16S helpCmd              (CPU_INT16U argc,
                                 CPU_CHAR *argv[],
                                 SHELL_OUT_FNCT outFunc,
                                 SHELL_CMD_PARAM *cmdParam);

CPU_INT16S initCmd              (CPU_INT16U argc,
                                 CPU_CHAR *argv[],
                                 SHELL_OUT_FNCT outFunc,
                                 SHELL_CMD_PARAM *cmdParam);

CPU_INT16S selftestCmd          (CPU_INT16U argc,
                                 CPU_CHAR *argv[],
                                 SHELL_OUT_FNCT outFunc,
                                 SHELL_CMD_PARAM *cmdParam);

CPU_INT16S sleepCmd             (CPU_INT16U argc,
                                 CPU_CHAR *argv[],
                                 SHELL_OUT_FNCT outFunc,
                                 SHELL_CMD_PARAM *cmdParam);

// -----------------------------------------------------------------------------
// Global variables

extern SHELL_CMD commandTable[]; // Table of shell commands

#endif // CMD_DECLARATIONS_H
