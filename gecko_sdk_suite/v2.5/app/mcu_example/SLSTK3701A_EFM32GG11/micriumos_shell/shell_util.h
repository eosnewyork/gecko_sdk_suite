/***************************************************************************//**
 * @file
 * @brief Interface for utility functions.
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
#ifndef SHELL_UTIL_H
#define SHELL_UTIL_H

#include <common/include/rtos_err.h>
#include <common/include/shell.h>

// -----------------------------------------------------------------------------
// Function prototypes

int shellStrtol(const char *str, RTOS_ERR *err);
void shellPrint(SHELL_OUT_FNCT outFunc, const char * s);
void shellPrintf(SHELL_OUT_FNCT outFunc, const char * format, ...);

#endif /* SHELL_UTIL_H */
