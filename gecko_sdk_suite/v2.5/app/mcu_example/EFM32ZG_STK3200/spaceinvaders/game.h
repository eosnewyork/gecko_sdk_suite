/***************************************************************************//**
 * @file
 * @brief Spaceinvaders game.
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

#ifndef _GAME_H
#define _GAME_H

#define SCREEN_BUFF_SIZE 2048

#include <stdbool.h>
#include "render.h"

#define GAME_VICTORY    2
#define GAME_OVER       1
#define GAME_RUNNING    0

typedef struct _gameobject {
  const Sprite *sprite;
  int  posX;
  int  posY;
  bool dead;
} GameObject;

void GAME_Redraw(void);
void GAME_Init(int lvl);
void GAME_MoveTank(int speed);
int  GAME_Update(void);
void GAME_FirePlayerMissile(void);

#endif /* _GAME_H */
