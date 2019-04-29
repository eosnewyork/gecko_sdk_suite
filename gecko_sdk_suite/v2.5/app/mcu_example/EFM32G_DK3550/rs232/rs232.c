/***************************************************************************//**
 * @file
 * @brief UART/LEUART/RS232 example for EFM32G_DK3550 development kit
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
#include "em_device.h"
#include "em_emu.h"
#include "bsp.h"
#include "retargetserial.h"

/* Note! You can set compile time define -DRETARGET_LEUART1 to build this
 * example to use LEUART instead of default UART1. See retargetserial.h for
 * details. */

/** RS232 input buffer size */
#define ECHOBUFSIZE    80
/** RS232 Input buffer */
char echoBuffer[ECHOBUFSIZE];

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  int c;
  int index;

  /* Initialize DK board register access */
  BSP_Init(BSP_INIT_DEFAULT);

  /* Initialize USART and map LF to CRLF */
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

#ifdef RETARGET_LEUART1
  printf("\nEFM32G_DK3550 LEUART1 example\n");
#else
  printf("\nEFM32G_DK3550 UART1 example\n");
#endif
  for (index = 0; index < ECHOBUFSIZE; index++) {
    echoBuffer[index] = (char) 'a' + index;
  }

  /* Retrieve characters, print local echo and full line back */
  index = 0;
  while (1) {
    /* Retrieve new character */
    c = getchar();
    if (c > 0) {
      /* Output character - most terminals use CRLF */
      if (c == '\r') {
        echoBuffer[index] = '\0';
        /* Output entire line */
        printf("\n%s\n", echoBuffer);
        index = 0;
      } else {
        /* Filter non-printable characters */
        if ((c < ' ') || (c > '~')) {
          continue;
        }

        /* Enter into buffer */
        echoBuffer[index] = c;
        index++;
        if (index == ECHOBUFSIZE) {
          /* Flush buffer */
          index = 0;
        }
        /* Local echo */
        putchar(c);
      }
    } else {
      /* Enter EM1 when idle */
      EMU_EnterEM1();
    }
  }
}
