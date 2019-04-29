/***************************************************************************//**
 * @file
 * @brief USB Isolated Example
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

#define ECHOBUF_SIZE    80

static char usbEchoBuffer[ECHOBUF_SIZE];

int main(void)
{
  int chara;
  int index;

  // Chip errata
  CHIP_Init();

  // Initialize LEUART/USART and map LF to CRLF
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

  printf("\nEFM32 Wonder Gecko Isolated USB example\n");

#if defined(RETARGET_VCOM)
  printf("Virtual COM port enabled.\n");
#endif

  for (index = 0; index < ECHOBUF_SIZE; index++) {
    usbEchoBuffer[index] = (char) 'a' + index;
  }

  // Retrieve characters, print local echo and full line back
  index = 0;
  while (1) {
    // Retrieve new character
    chara = getchar();
    if (chara > 0) {
      // Output character - most terminals use CRLF
      if (chara == '\r') {
        usbEchoBuffer[index] = '\0';
        // Output entire line
        printf("\n%s\n", usbEchoBuffer);
        index = 0;
      } else {
        // Filter non-printable characters
        if ((chara < ' ') || (chara > '~')) {
          continue;
        }

        if (index < ECHOBUF_SIZE) {
          /* Enter into buffer */
          usbEchoBuffer[index] = chara;
          index++;
        } else {
          // Ignore character, buffer is full
        }

        // Local echo
        putchar(chara);
      }
    }
  }
}
