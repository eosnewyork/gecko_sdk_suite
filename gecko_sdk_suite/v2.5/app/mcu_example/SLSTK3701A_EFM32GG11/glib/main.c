/***************************************************************************//**
 * @file
 * @brief GLIB example for SLSTK3701A
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
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "display.h"
#include "dmd.h"
#include "glib.h"
#include "bspconfig.h"

#include "siliconlabs.h"
#include "gecko.h"
#include "testscreen.h"
#include "house.h"

#define GLIB_FONT_WIDTH           (glibContext.font.fontWidth \
                                   + glibContext.font.charSpacing)
#define GLIB_FONT_HEIGHT          (glibContext.font.fontHeight)

/* Center of display */
#define CENTER_X                  (glibContext.pDisplayGeometry->xSize / 2)
#define CENTER_Y                  (glibContext.pDisplayGeometry->ySize / 2)

#define MAX_X                     (glibContext.pDisplayGeometry->xSize - 1)
#define MAX_Y                     (glibContext.pDisplayGeometry->ySize - 1)

#define MIN_X                     0
#define MIN_Y                     0

#define INIT_DEMO_NO              0
#define MAX_DEMO_NO               14

#define MAX_STR_LEN               48

// The GLIB context
static GLIB_Context_t glibContext;

static volatile uint32_t demoNo = INIT_DEMO_NO;

// Forward static function declararations
static void GlibDemo(void);

/***************************************************************************//**
 * @brief Setup GPIO interrupt for pushbuttons.
 ******************************************************************************/
static void GpioSetup(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PB0 as input and enable interrupt
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);

  // Configure PB1 as input and enable interrupt
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, false, true, true);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/***************************************************************************//**
 * @brief Unified GPIO Interrupt handler (pushbuttons)
 *        PB0 Next test
 *        PB1 Previous
 ******************************************************************************/
void GPIO_Unified_IRQ(void)
{
  // Get and clear all pending GPIO interrupts
  uint32_t interruptMask = GPIO_IntGet();
  GPIO_IntClear(interruptMask);

  // Act on interrupts
  if (interruptMask & (1 << BSP_GPIO_PB0_PIN)) {
    // PB0: Next demo
    demoNo = (demoNo + 1) % (MAX_DEMO_NO + 1);
  }

  if (interruptMask & (1 << BSP_GPIO_PB1_PIN)) {
    // PB1: Previous demo
    demoNo--;
    if (demoNo > MAX_DEMO_NO) {
      demoNo = MAX_DEMO_NO;
    }
  }
}

/***************************************************************************//**
 * @brief GPIO Interrupt handler (PB0)
 *        Previous test
 ******************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  GPIO_Unified_IRQ();
}

/***************************************************************************//**
 * @brief GPIO Interrupt handler (PB1)
 *        Next test
 ******************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  GPIO_Unified_IRQ();
}

/***************************************************************************//**
 * @brief   Calculate 4 poly points
 ******************************************************************************/
static int32_t * gen4PolyPoints(uint32_t radius, int32_t xOff, int32_t yOff)
{
  static int32_t polyCoords[8];

  polyCoords[0] = CENTER_X - radius + xOff;
  polyCoords[1] = CENTER_Y + yOff;
  polyCoords[2] = CENTER_X + xOff;
  polyCoords[3] = CENTER_Y - radius + yOff;
  polyCoords[4] = CENTER_X + radius + xOff;
  polyCoords[5] = CENTER_Y + yOff;
  polyCoords[6] = CENTER_X + xOff;
  polyCoords[7] = CENTER_Y + radius + yOff;

  return (int32_t *)polyCoords;
}

/***************************************************************************//**
 * @brief  GLIB draw text demo function
 ******************************************************************************/
static void GlibDemoDrawText(GLIB_Context_t *pContext)
{
  char str[MAX_STR_LEN];

  // Print welcome message using NORMAL 8x8 font.
  GLIB_setFont(pContext, (GLIB_Font_t *)&GLIB_FontNormal8x8);
  snprintf(str, MAX_STR_LEN, "  EFM32 GLIB \nNormal 8x8 font");
  GLIB_drawString(&glibContext,
                  str,
                  strlen(str),
                  CENTER_X - ((GLIB_FONT_WIDTH * strlen(str)) / 4),
                  5,
                  0);

  // Use the NARROW 6x8 font
  GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
  snprintf(str, MAX_STR_LEN, "GLIB strings & fonts\n   Narrow 6x8 font  ");
  GLIB_drawString(&glibContext,
                  str,
                  strlen(str),
                  CENTER_X - ((GLIB_FONT_WIDTH * strlen(str)) / 4),
                  5 + (3 * GLIB_FONT_HEIGHT),
                  0);

  // Add 3 extra char spacing pixels between each font. Use 6 pixels line
  // spacing.
  glibContext.font.charSpacing = 3;
  glibContext.font.lineSpacing = 6;
  snprintf(str, MAX_STR_LEN, "  GLIB demo \nNarrow (6+3)x8");
  GLIB_drawString(&glibContext,
                  str,
                  strlen(str),
                  CENTER_X - ((strlen(str) * GLIB_FONT_WIDTH) / 4),
                  5 + (6 * GLIB_FONT_HEIGHT),
                  0);

  GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNumber16x20);
  snprintf(str, MAX_STR_LEN, "12345\n16:20");
  GLIB_drawString(&glibContext,
                  str,
                  strlen(str),
                  CENTER_X - ((strlen(str) * GLIB_FONT_WIDTH) / 4),
                  CENTER_Y + 14,
                  0);

  GLIB_setFont(pContext, (GLIB_Font_t *)&GLIB_FontNormal8x8);
}

/***************************************************************************//**
 * @brief  GLIB demo function
 ******************************************************************************/
static void GlibDemo(void)
{
  uint32_t currentDemoNo = 1;

  // Some test elements that cannot be declared inside the switch
  GLIB_Rectangle_t rectTopLeft = { 0, 0, CENTER_X - 1, CENTER_Y - 1 };
  GLIB_Rectangle_t rectTopRight = { CENTER_X, 0, MAX_X, CENTER_Y - 1 };
  GLIB_Rectangle_t rectBottomLeft = { 0, CENTER_Y, CENTER_X - 1, MAX_Y };
  GLIB_Rectangle_t rectBottomRight = { CENTER_X, CENTER_Y, MAX_X, MAX_Y };
  GLIB_Rectangle_t rectCenter = { MAX_X / 4,
                                  MAX_Y / 4,
                                  MAX_X * 3 / 4,
                                  MAX_Y * 3 / 4 };

  while (true) {
    if (currentDemoNo != demoNo) {
      currentDemoNo = demoNo;

      switch (demoNo) {
        case 0:
          glibContext.backgroundColor = White;
          glibContext.foregroundColor = Black;
          GLIB_clear(&glibContext);
          GlibDemoDrawText(&glibContext);
          DMD_updateDisplay();
          break;

        case 1:
          glibContext.backgroundColor = Black;
          glibContext.foregroundColor = White;
          GLIB_clear(&glibContext);
          GlibDemoDrawText(&glibContext);
          DMD_updateDisplay();
          break;

        case 2:
          glibContext.backgroundColor = White;
          GLIB_clear(&glibContext);
          GLIB_drawBitmap(&glibContext,
                          (MAX_X + 1 - SILICONLABS_BITMAP_WIDTH) / 2,
                          (MAX_Y + 1 - SILICONLABS_BITMAP_HEIGHT) / 2,
                          SILICONLABS_BITMAP_WIDTH,
                          SILICONLABS_BITMAP_HEIGHT,
                          siliconlabsBitmap);
          DMD_updateDisplay();
          break;

        case 3:
          glibContext.backgroundColor = Black;
          GLIB_clear(&glibContext);
          GLIB_drawBitmap(&glibContext,
                          (MAX_X + 1 - GECKO_BITMAP_WIDTH) / 2,
                          0,
                          GECKO_BITMAP_WIDTH,
                          GECKO_BITMAP_HEIGHT,
                          geckoBitmap);
          DMD_updateDisplay();
          break;

        case 4:
          GLIB_drawBitmap(&glibContext,
                          0,
                          0,
                          TESTSCREEN_BITMAP_WIDTH,
                          TESTSCREEN_BITMAP_HEIGHT,
                          testscreenBitmap);
          DMD_updateDisplay();
          break;
        case 5:
          GLIB_drawBitmap(&glibContext,
                          0,
                          0,
                          HOUSE_BITMAP_WIDTH,
                          HOUSE_BITMAP_HEIGHT,
                          houseBitmap);
          DMD_updateDisplay();
          break;

        case 6:
          glibContext.backgroundColor = Black;
          glibContext.foregroundColor = White;
          GLIB_clear(&glibContext);
          GLIB_drawCircle(&glibContext,
                          CENTER_X - 1,
                          CENTER_Y - 1,
                          DISPLAY0_HEIGHT / 4);
          GLIB_drawRect(&glibContext, &rectCenter);
          GLIB_drawLine(&glibContext, MIN_X, MAX_Y, MAX_X, MIN_Y);
          GLIB_drawLine(&glibContext, MIN_X, MIN_Y, MAX_X, MAX_Y);
          DMD_updateDisplay();
          break;

        case 7:
          glibContext.backgroundColor = White;
          glibContext.foregroundColor = Black;
          GLIB_clear(&glibContext);
          GLIB_drawCircleFilled(&glibContext, CENTER_X, MAX_Y, DISPLAY0_HEIGHT / 4);
          DMD_updateDisplay();
          break;

        case 8:
          glibContext.backgroundColor = White;
          glibContext.foregroundColor = Black;
          GLIB_clear(&glibContext);
          GLIB_drawCircle(&glibContext,
                          CENTER_X - (DISPLAY0_WIDTH / 8),
                          CENTER_Y,
                          DISPLAY0_HEIGHT / 4);
          GLIB_drawCircle(&glibContext,
                          CENTER_X + (DISPLAY0_WIDTH / 8),
                          CENTER_Y,
                          DISPLAY0_HEIGHT / 4);
          DMD_updateDisplay();
          break;

        case 9:
          GLIB_clear(&glibContext);

          glibContext.foregroundColor = Black;
          GLIB_drawRectFilled(&glibContext, &rectTopLeft);

          glibContext.foregroundColor = Red;
          GLIB_drawRectFilled(&glibContext, &rectTopRight);

          glibContext.foregroundColor = Green;
          GLIB_drawRectFilled(&glibContext, &rectBottomLeft);

          glibContext.foregroundColor = Blue;
          GLIB_drawRectFilled(&glibContext, &rectBottomRight);

          glibContext.foregroundColor = White;
          GLIB_drawCircleFilled(&glibContext,
                                CENTER_X / 2 - 1,
                                CENTER_Y / 2 - 1,
                                DISPLAY0_HEIGHT / 4 - 5);

          glibContext.foregroundColor = Cyan;
          GLIB_drawCircleFilled(&glibContext,
                                CENTER_X * 3 / 2 - 1,
                                CENTER_Y / 2 - 1,
                                DISPLAY0_HEIGHT / 4 - 5);

          glibContext.foregroundColor = Magenta;
          GLIB_drawCircleFilled(&glibContext,
                                CENTER_X / 2 - 1,
                                CENTER_Y * 3 / 2 - 1,
                                DISPLAY0_HEIGHT / 4 - 5);

          glibContext.foregroundColor = Yellow;
          GLIB_drawCircleFilled(&glibContext,
                                CENTER_X * 3 / 2 - 1,
                                CENTER_Y * 3 / 2 - 1,
                                DISPLAY0_HEIGHT / 4 - 5);
          DMD_updateDisplay();
          break;

        case 10:
          glibContext.backgroundColor = White;
          glibContext.foregroundColor = Black;
          GLIB_clear(&glibContext);
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(20, 0, 0));
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(40, 0, 0));
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(60, 0, 0));
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(80, 0, 0));
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(100, 0, 0));
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(120, 0, 0));
          DMD_updateDisplay();
          break;

        case 11:
          glibContext.backgroundColor = Black;
          glibContext.foregroundColor = White;
          GLIB_clear(&glibContext);
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(20, 0, 0));
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(40, 0, 0));
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(60, 0, 0));
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(80, 0, 0));
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(100, 0, 0));
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(120, 0, 0));
          DMD_updateDisplay();
          break;

        case 12:
          glibContext.backgroundColor = Cyan;
          glibContext.foregroundColor = Red;
          GLIB_clear(&glibContext);
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(20, 0, 0));
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(40, 0, 0));
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(60, 0, 0));
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(80, 0, 0));
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(100, 0, 0));
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(120, 0, 0));
          DMD_updateDisplay();
          break;
        case 13:
          glibContext.backgroundColor = Yellow;
          glibContext.foregroundColor = Blue;
          GLIB_clear(&glibContext);
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(20, 0, 0));
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(40, 0, 0));
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(60, 0, 0));
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(80, 0, 0));
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(100, 0, 0));
          GLIB_drawPolygon(&glibContext, 4, gen4PolyPoints(120, 0, 0));
          DMD_updateDisplay();
          break;

        case 14:
          glibContext.backgroundColor = White;
          glibContext.foregroundColor = Black;
          GLIB_clear(&glibContext);
          GLIB_drawPartialCircle(&glibContext, CENTER_X, CENTER_Y, 50, 0x9F);
          GLIB_drawCircle(&glibContext, CENTER_X, MAX_Y + 10, 60);
          GLIB_drawLine(&glibContext, MIN_X, MAX_Y, MAX_X, MIN_Y);
          GLIB_drawLine(&glibContext, MIN_X, MIN_Y, MAX_X, MAX_Y);
          DMD_updateDisplay();
          break;

        default:
          demoNo = INIT_DEMO_NO;
          break;
      }
    }
  }
}

/***************************************************************************//**
 * @brief  Main function of GLIB example.
 ******************************************************************************/
int main(void)
{
  EMSTATUS status;

  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_STK_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  // Init DCDC regulator and HFXO with kit specific parameters
  EMU_DCDCInit(&dcdcInit);
  CMU_HFXOInit(&hfxoInit);

  // Setup GPIO for pushbuttons.
  GpioSetup();

  // Initialize the DMD module for the DISPLAY device driver.
  status = DMD_init(0);
  if (DMD_OK != status) {
    while (1) {
    }
  }

  status = GLIB_contextInit(&glibContext);
  if (GLIB_OK != status) {
    while (1) {
    }
  }

  while (1) {
    GlibDemo();
  }
}
