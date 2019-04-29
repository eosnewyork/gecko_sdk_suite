/***************************************************************************//**
 * @file
 * @brief LCD display functions for Spectrum Analyzer
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

#include <string.h>
#include "display.h"
#include "dmd.h"
#include "glib.h"
#include "bspconfig.h"

// Define dimensions of FFT and spectrogram graph
#define FFTHEIGHT 50
#define FFTWIDTH 120
#define SPECHEIGHT 64
#define SPECWIDTH 128

// Color mode of display
#define COLORBITS 3

// Rough background noise value, used for magnitude normalization
#define NOISE_FLOOR 100000

/// Globally declared glib context configuration
static GLIB_Context_t glibContext;

/// Bitmap buffers for Spectrogram
static uint8_t spectrogramA[SPECHEIGHT * 128 * COLORBITS / 8] = { 0 };
static uint8_t spectrogramB[SPECHEIGHT * 128 * COLORBITS / 8] = { 0 };

/// Flag for toggling between bitmap buffers
static bool bitMapFlag;

/// Enumerated 3 bit color options
/// Colors are ordered for 3 bit 0bBGR
typedef enum _Colors{
  BLACK,
  RED,
  GREEN,
  YELLOW,
  BLUE,
  MAGENTA,
  CYAN,
  WHITE
} Color;

/// Global status variable for debugging purposes
static EMSTATUS status;

/// Defined display clipping regions
static const GLIB_Rectangle_t fftClippingRegion = { .xMin = 0,
                                                    .xMax = FFTWIDTH,
                                                    .yMin = 0,
                                                    .yMax = FFTHEIGHT };
static const GLIB_Rectangle_t spectraClippingRegion = { .xMin = 0,
                                                        .xMax = SPECWIDTH,
                                                        .yMin = 128,
                                                        .yMax = (128 - SPECHEIGHT) };

// Helper function prototypes
static void updateSpectrogram(float *, uint8_t *, uint8_t *);

/**************************************************************************//**
 * @brief Initialize LCD display
 *****************************************************************************/
void LCD_Init()
{
  // Initialize the DMD module for the DISPLAY device driver.
  status = DMD_init(0);
  if (DMD_OK != status) {
    while (1) ;
  }

  // Initialize GLIB
  status = GLIB_contextInit(&glibContext);
  if (GLIB_OK != status) {
    while (1) ;
  }

  // Set default colors
  glibContext.backgroundColor = Black;
  glibContext.foregroundColor = White;

  // Display start screen, set font to narrow after title
  GLIB_drawString(&glibContext,
                  (char *)&"Giant Gecko\n Spectrum\n Analyzer",
                  sizeof("Giant Gecko\n Spectrum\n Analyzer"),
                  20,
                  30,
                  true);
  glibContext.font = GLIB_FontNarrow6x8;
  GLIB_drawString(&glibContext,
                  (char *)&"BTN0 for Mic input\nBTN1 for ADC input",
                  sizeof("BTN0 for Mic input\nBTN1 for ADC input"),
                  12,
                  108,
                  true);

  DMD_updateDisplay();
}

/**************************************************************************//**
 * @brief Draw spectrogram graph
 *****************************************************************************/
void LCD_BuildGraph()
{
  GLIB_clear(&glibContext);

  // Draw X axis scale
  GLIB_drawString(&glibContext,
                  (char *)&"0(Hz)              8k",
                  sizeof("0(Hz)              8k"),
                  0,
                  FFTHEIGHT + 4,
                  true);

  // Draw legend showing magnitude scale to the right of the FFT graph
  GLIB_Rectangle_t colorBlock = { .xMin = FFTWIDTH + 3,
                                  .xMax = FFTWIDTH + 6,
                                  .yMin = FFTHEIGHT - 8,
                                  .yMax = FFTHEIGHT - 1 };

  // Temporary array of GLIB 8 bit colors ordered by increasing magnitude
  uint32_t colorArray[7] = { Red, Magenta, Blue, Cyan, Green, Yellow, White };

  // Draw 8 Rectangles with different colors stacked on top of each other
  for (int i = 0; i < 7; i++) {
    glibContext.foregroundColor = colorArray[i];
    GLIB_drawRectFilled(&glibContext, &colorBlock);
    colorBlock.yMin -= 7;
    colorBlock.yMax -= 7;
  }

  // Reset foreground color
  glibContext.foregroundColor = White;
}

/**************************************************************************//**
 * @brief Update display with new frequency response data
 *****************************************************************************/
void LCD_UpdateFrequencyResponse(float *buffer, float normValue, bool noise)
{
  // Check if normValue is below defined 'noise floor' value if boolean is set
  if (noise) {
    // Artificially increase normValue as to not normalize to noise floor
    normValue = normValue + NOISE_FLOOR;
  }

  // Normalize buffer to fit on screen,
  for (int i = 0; i < 128; i++) {
    buffer[i] = (buffer[i] * FFTHEIGHT) / normValue;
  }
  // Clear regions to be redrawn
  GLIB_setClippingRegion(&glibContext, &fftClippingRegion);
  GLIB_clearRegion(&glibContext);
  GLIB_setClippingRegion(&glibContext, &spectraClippingRegion);
  GLIB_resetClippingRegion(&glibContext);

  // Update and draw Spectrogram
  if (bitMapFlag) {
    updateSpectrogram(buffer, spectrogramA, spectrogramB);
    GLIB_drawBitmap(&glibContext,
                    0,
                    (128 - SPECHEIGHT),
                    128,
                    (SPECHEIGHT - 1),
                    spectrogramA
                    );
  } else {
    updateSpectrogram(buffer, spectrogramB, spectrogramA);
    GLIB_drawBitmap(&glibContext,
                    0,
                    (128 - SPECHEIGHT),
                    128,
                    (SPECHEIGHT - 1),
                    spectrogramB
                    );
  }

  // Draw the first FFTWIDTH frequency bins (lowest positive frequency portion)
  for (int i = 0; i < FFTWIDTH; i++) {
    status = GLIB_drawLineV(&glibContext, i, FFTHEIGHT, FFTHEIGHT - (int32_t)buffer[i]);
  }

  // Refresh screen
  status = DMD_updateDisplay();

  // toggle bitmap selection flag
  bitMapFlag = !bitMapFlag;
}

/**************************************************************************//**
 * @brief Helper function to update spectrogram bitmap
 *****************************************************************************/
static void updateSpectrogram(float *buffer,
                              uint8_t *newBitmap,
                              uint8_t *oldBitmap)
{
  // Buffer word to store 3 pixels of 3 bit color data
  uint32_t newPix;

  // Build and draw spectrogram bitmap (3 bit Display)
  for (int i = 0; i < 16; i++) {
    // Clear 24 bit block and buffer word
    // Nearest 8 bit multiple of 3
    newPix = 0;
    newBitmap[3 * i]   = 0x00;
    newBitmap[3 * i + 1] = 0x00;
    newBitmap[3 * i + 2] = 0x00;

    // Determine color for 8 pixel block from normalized magnitude
    // Enumerated values are bit shifted into proper 3 bit alignment
    for (int j = 0; j < 8; j++) {
      if (buffer[(i * 8) + j] < 1) {
        newPix |= BLACK << (3 * j);
      } else if (buffer[(i * 8) + j] < 9) {
        newPix |= RED << (3 * j);
      } else if (buffer[(i * 8) + j] < 16) {
        newPix |= MAGENTA << (3 * j);
      } else if (buffer[(i * 8) + j] < 23) {
        newPix |= BLUE << (3 * j);
      } else if (buffer[(i * 8) + j] < 30) {
        newPix |= CYAN << (3 * j);
      } else if (buffer[(i * 8) + j] < 37) {
        newPix |= GREEN << (3 * j);
      } else if (buffer[(i * 8) + j] < 44) {
        newPix |= YELLOW << (3 * j);
      } else if (buffer[(i * 8) + j] >= 44) {
        newPix |= WHITE << (3 * j);
      }
    }

    // Pack buffer word into bitmap (24 bit blocks)
    newBitmap[3 * i + 0] = (uint8_t)(newPix);
    newBitmap[3 * i + 1] = (uint8_t)(newPix >> 8);
    newBitmap[3 * i + 2] = (uint8_t)(newPix >> 16);
  }

  // Flip Bitmaps with updated row added to newBitmap
  // Oldest row is overwritten
  memcpy((newBitmap + 128 * COLORBITS / 8),
         oldBitmap,
         ((SPECHEIGHT - 1) * 128 * COLORBITS / 8)
         );
}
