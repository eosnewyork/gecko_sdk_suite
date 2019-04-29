/***************************************************************************//**
 * @file
 * @brief Help command for the shell.
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

#include <common/include/shell.h>
#include <cpu/include/cpu.h>

#include "cmd_declarations.h"
#include "shell_util.h"

// -----------------------------------------------------------------------------
// Global functions

/***************************************************************************//**
 * @brief
 *   Shell command function for the 'help' command.
 *
 * @param argc
 *   Not used.
 *
 * @param argv
 *   Not used.
 *
 * @param out_fnct
 *   Not used.
 *
 * @param pcmd_param
 *   Not used.
 *
 * @return
 *   SHELL_EXEC_ERR on error, SHELL_EXEC_ERR_NONE otherwise.
 ******************************************************************************/
CPU_INT16S helpCmd(CPU_INT16U argc,
                   CPU_CHAR *argv[],
                   SHELL_OUT_FNCT outFunc,
                   SHELL_CMD_PARAM *cmdParam)
{
  int i;

  (void)argc; // Deliberately unused arguments
  (void)argv;
  (void)cmdParam;

  shellPrint(outFunc, "Usage: command [-arg [param]]\n");
  shellPrint(outFunc, "The available commands are:\n");

  for (i = 0; commandTable[i].Name; i++) {
    shellPrintf(outFunc, "\t%s\n", commandTable[i].Name);
  }

  return SHELL_EXEC_ERR_NONE;
}
