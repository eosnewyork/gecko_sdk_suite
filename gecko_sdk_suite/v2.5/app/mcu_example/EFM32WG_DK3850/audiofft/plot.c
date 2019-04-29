/***************************************************************************//**
 * @file
 * @brief  Simple wrapper for some emWin functions.
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

#include "plot.h"
#include "em_device.h"
#include "em_common.h"
#include "arm_math.h"
#include "logo.h"
#include "GUIResource.h"

/* The size of the plotting area. */
#define GRAPH_WIDTH  265
#define GRAPH_HEIGHT 122
#define MAX_POINTS   256

static int      xPosGraph, yPosGraph, numPoints;
static int16_t  pData[MAX_POINTS];

/***************************************************************************//**
 * @brief Draw Y axis labels just left of the plotting area.
 * @param[in] xPos X position of plotting area.
 * @param[in] yPos Y position of plotting area.
 ******************************************************************************/
static void DrawLabel(int xPos, int yPos)
{
  int FontSizeY, i, x0, y0;

  GUI_SetFont(&GUI_FontD9_AA4);
  GUI_SetTextMode(GUI_TM_TRANS);
  FontSizeY = GUI_GetFontSizeY();
  for (i = 0; i < 5; i++) {
    x0 = xPos - 7;
    y0 = yPos + 100 - i * 20 - 7;
    GUI_GotoXY(x0 + 7, y0 + 7 - FontSizeY / 2);
    GUI_SetTextAlign(GUI_TA_HCENTER);
    GUI_SetColor(GUI_WHITE);
    GUI_DispDecMin((i + 1) * 2);
  }
}

/***************************************************************************//**
 * @brief Draw a graph inside the plotting area.
 * @param[in] xPos X position of plotting area.
 * @param[in] yPos Y position of plotting area.
 * @param[in] py Graph data array.
 ******************************************************************************/
static void DrawGraphAt(int xPos, int yPos, float* py)
{
  int i;
  float f;
  int16_t val;

  /* Draw dark grey background. */
  GUI_SetColor(GUI_DARKGRAY);
  GUI_FillRoundedRect(xPos, yPos, xPos + GRAPH_WIDTH, yPos + GRAPH_HEIGHT, 4);

  /* Draw gray grid lines. */
  GUI_SetColor(GUI_GRAY);
  for (i = 0; i < 12; i++) {
    GUI_DrawHLine(yPos + 10 + i * 10, xPos + 2, xPos + GRAPH_WIDTH - 2);
  }

  for (i = 0; i < numPoints; i++) {
    /* Scale plot data. */
    f = py[i] / 175;

    val = (int16_t)f;

    /* Clip large values. */
    pData[i] = GRAPH_HEIGHT - SL_MIN(val, GRAPH_HEIGHT - 5);
  }

  /* Draw the actual graph. */
  GUI_SetColor(GUI_WHITE);
  GUI_DrawGraph(pData, numPoints, xPos + 3, yPos - 2);
}

/***************************************************************************//**
 * @brief Draw a graph.
 * @param[in] plotData Graph data array.
 ******************************************************************************/
void PLOT_Plot(float *plotData)
{
  DrawGraphAt(xPosGraph, yPosGraph, plotData);
}

/***************************************************************************//**
 * @brief Initialize PLOT module.
 * @param[in] points Number of graph data points along X axis.
 ******************************************************************************/
void PLOT_Init(int points)
{
  int i;

  numPoints = SL_MIN(points, MAX_POINTS);

  /* Calculate positions. */
  xPosGraph = (LCD_GetXSize() - GRAPH_WIDTH)  / 2;
  yPosGraph = (LCD_GetYSize() - GRAPH_HEIGHT) / 2;

  for (i = 0; i < GUI_MULTIBUF_GetNumBuffers(); i++) {
    /* Make sure that all buffers are initialized. */
    GUI_MULTIBUF_Begin();

    GUI_SetBkColor(GUI_BLACK);
    GUI_Clear();
    GUI_DrawBitmap(&bmlogo, (LCD_GetXSize() - bmlogo.XSize) / 2, 188);

    /* Y axis label. */
    DrawLabel(xPosGraph - 10, yPosGraph);

    GUI_MULTIBUF_End();         /* Drawing finished. Buffers will be flipped. */
  }
}

/***************************************************************************//**
 * @brief Write a string centered at specified position.
 * @param[in] str String.
 * @param[in] xpos X position.
 * @param[in] ypos Y position.
 ******************************************************************************/
void PLOT_Puts(const char *str, int xpos, int ypos)
{
  GUI_SetTextMode(GUI_TM_NORMAL);
  GUI_SetColor(GUI_WHITE);
  GUI_SetFont(&GUI_Font8x13_1);
  GUI_DispStringHCenterAt(str, xpos, ypos);
}
