/***************************************************************************//**
 * @file
 * @brief helper functions for drawing capsense button states on the LCD
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

#ifndef LCD_H
#define LCD_H

typedef struct _demoobject {
  int  posX;
  int  posY;
  int  status;
} DemoObject;

/***************************************************************************//**
 * @brief initialize and clear the 128x128 lcd display (Happy Gecko)
 *                   or the 160 segment display (Leopard/Wonder Gecko)
 ******************************************************************************/
void initLCD(void);

/***************************************************************************//**
 * @brief update lcd display frame
 ******************************************************************************/
void updateLCD(void);

#endif /* LCD_H */
