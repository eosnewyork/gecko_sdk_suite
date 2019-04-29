/***************************************************************************//**
 * @file
 * @brief lcd output functions for Spectrum Analyzer
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
#ifndef SRC_LCD_H_
#define SRC_LCD_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************//**
 * @brief Initialize LCD display
 *
 * @param void
 * @returns void
 *
 * @detail Initializes the FFT, and draws the start screen
 *****************************************************************************/
void LCD_Init(void);

/**************************************************************************//**
 * @brief Draw spectrogram graph
 *
 * @param void
 * @returns void
 *
 * @detail Draws the static elements of the spectrogram display. This includes
 * the axis, units, and color magnitude legend.
 *****************************************************************************/
void LCD_BuildGraph(void);

/**************************************************************************//**
 * @brief Update display with new frequency response data
 *
 * @param buffer is a pointer to the array of frequency bin data
 * @param normValue is the value that should correspond to 100% magnitude on
 * the display
 * @param noise is a boolean that should be set if the data has a significant
 * degree of noise, and prevents noise from being normalized to 100% magnitude
 *
 * @returns void
 *
 * @detail Draws the dynamic elements of the spectrogram display. This includes
 * the updated frequency response, and correspondingly adding that data to the
 * waterfall spectrogram.
 *****************************************************************************/
void LCD_UpdateFrequencyResponse(float *buffer, float normValue, bool noise);

#ifdef __cplusplus
// *INDENT-OFF*
}
// *INDENT-ON*
#endif

#endif /* SRC_LCD_H_ */
