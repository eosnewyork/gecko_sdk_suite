/***************************************************************************//**
 * @file
 * @brief Utility functions used in this example.
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
#include "shell_util.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

static char line[256];

/***************************************************************************//**
 * @brief
 *   strtol() from stdlib.h library with error handling.
 *
 * @param str
 *   string to be converted to int
 *
 * @param p_err
 *   Set to RTOS_ERR_FAIL on error.
 *
 * @return
 *   Converted int or 0 if conversion fails.
 ******************************************************************************/
int shellStrtol(const char *str, RTOS_ERR  *p_err)
{
  char *endptr;
  errno = 0;
  int ret_val = strtol(str, &endptr, 0); // strtol

  if ((errno == ERANGE && (ret_val == INT_MAX || ret_val == INT_MIN))
      || (errno != 0 && ret_val == 0))
  {
    printf("shellStrtol: Over/underflow occurred\n");
    goto error;
  } else if (endptr == str) {
    printf("shellStrtol: No digits were found\n");
    goto error;
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return ret_val;
  }

  error:
  RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
  return 0;
}

/***************************************************************************//**
 * @brief
 *   Print a single string using the output function
 *
 * @param outFunc
 *   The output function which is passed to commands.
 *
 * @param s
 *   String
 ******************************************************************************/
void shellPrint(SHELL_OUT_FNCT outFunc, const char * s)
{
  outFunc((CPU_CHAR *)s, strlen(s), NULL);
}

/***************************************************************************//**
 * @brief
 *   Print a formatted string using the output function
 *
 * @param outFunc
 *   The output function which is passed to commands.
 *
 * @param format
 *   A printf formatted string
 ******************************************************************************/
void shellPrintf(SHELL_OUT_FNCT outFunc, const char * format, ...)
{
  va_list argptr;
  va_start(argptr, format);
  vsnprintf(line, sizeof(line), format, argptr);
  va_end(argptr);
  shellPrint(outFunc, line);
}
