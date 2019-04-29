/***************************************************************************//**
 * @file
 * @brief
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

#ifndef LCD_CONF_H
#define LCD_CONF_H

#if defined(__cplusplus)
extern "C" {
#endif

void LCD_X_Config(void);
void LCD_X_InitDriver(void);
int  LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData);

#if defined(__cplusplus)
}
#endif

#endif
