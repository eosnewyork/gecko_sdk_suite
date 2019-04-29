/***************************************************************************//**
 * @file
 * @brief Display application specific configuration file.
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

#ifndef __DISPLAYCONFIGAPP_H__
#define __DISPLAYCONFIGAPP_H__

/* Include TEXTDISPLAY support because this example prints text to the
 * display. */
#define INCLUDE_TEXTDISPLAY_SUPPORT

/* Use number font of size 16x20 pixels */
#define TEXTDISPLAY_NUMBER_FONT_16x20

/* Disable scroll mode on the text display. */
#define RETARGETTEXTDISPLAY_SCROLL_MODE  (false)

/* Add Carriage Return (CR) to Line Feed (LF) characters
 * on the text display. */
#define RETARGETTEXTDISPLAY_LINE_FEED_MODE  (true)

#endif /* __DISPLAYCONFIGAPP_H__ */
