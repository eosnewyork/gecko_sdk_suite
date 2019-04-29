/***************************************************************************//**
 * # License
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is Third Party Software licensed by Silicon Labs from a third party
 * and is governed by the sections of the MSLA applicable to Third Party
 * Software and the additional terms set forth below.
 *
 ******************************************************************************/
/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2012  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.14 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to Energy Micro AS whose registered office
is situated at  Sandakerveien 118, N-0484 Oslo, NORWAY solely
for  the  purposes  of  creating  libraries  for Energy Micros ARM Cortex-M3, M4F
processor-based  devices,  sublicensed  and distributed  under the terms and
conditions  of  the   End  User  License Agreement supplied by Energy Micro AS. 
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : reversi.h
Purpose     : Simple 'reversi' game.
----------------------------------------------------------------------
*/

#include <Global.h>

#ifdef __cplusplus
extern "C" {
#endif


/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define NUM_CELL_X        8
#define NUM_CELL_Y        8

/*********************************************************************
*
*       Types
*
**********************************************************************
*/

typedef struct {
  U8  aCells[NUM_CELL_X][NUM_CELL_Y];
  U8  aMoves[NUM_CELL_X][NUM_CELL_Y];
  int ActPlayer; // 1 or 2
} BOARD;

typedef char REVERSI_AI_Func(const BOARD* pBoard, int* px, int* py);


/*********************************************************************
*
*       Function declarations
*
**********************************************************************
*/

void _StartNewGame(void);
int _CalcValidMoves(BOARD* pBoard);
void _MakeMove(BOARD* pBoard, int x, int y);
int _CalcScore(const BOARD* pBoard);

/*************************** End of file ****************************/

#ifdef __cplusplus
}
#endif

