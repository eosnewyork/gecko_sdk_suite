/***************************************************************************//**
 * @file
 * @brief VCOM example for SLSTK3401A starter kit
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
#include "em_chip.h"
#include "retargetserial.h"

/*
 * Virtual COM port through USB is used by default in this example.
 * See retargetserialconfig.h for information about how to redirect to
 * EXP header pins.
 */

/** Input buffer size */
#define ECHOBUFSIZE    80
/** Input buffer */
static char echoBuffer[ECHOBUFSIZE];

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  int c;
  int index;

  /* Chip errata */
  CHIP_Init();

  /* Initialize LEUART/USART and map LF to CRLF */
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

#if defined(RETARGET_LEUART0)
  printf("\nEFM32 Pearl Gecko LEUART example\n");
#else
  printf("\nEFM32 Pearl Gecko USART example\n");
#endif

#if defined(RETARGET_VCOM)
  printf("Virtual COM port enabled.\n");
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

        if (index < ECHOBUFSIZE - 1) {
          /* Enter into buffer */
          echoBuffer[index] = c;
          index++;
        } else {
          /* Ignore character, buffer is full */
        }

        /* Local echo */
        putchar(c);
      }
    }
  }
}
