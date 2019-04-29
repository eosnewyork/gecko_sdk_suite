/***************************************************************************//**
 * @file
 * @brief USART1 prototypes and definitions
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
#ifndef __TFT_CONSOLE_H
#define __TFT_CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int x, y;
} TFT_Pos;

void TFT_Init(void);
void TFT_CrLf(int on);
int  TFT_WriteChar(char c);
void TFT_Puts(char *pchar);
void TFT_Print(const char *format, ...);

void TFT_PosGet(TFT_Pos * pos);
void TFT_PosSet(const TFT_Pos * pos);

#ifdef __cplusplus
}
#endif

#endif /* __TFT_CONSOLE_H */
