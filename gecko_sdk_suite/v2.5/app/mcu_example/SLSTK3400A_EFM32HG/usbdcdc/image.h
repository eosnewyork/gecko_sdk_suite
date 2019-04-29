/***************************************************************************//**
 * @file
 * @brief Pictures for memLCD
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

#ifndef IMAGE_H
#define IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

static const char usbConnectedImage[] =
{
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xf1, 0xff, 0xff, 0xff, 0xf3, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x03, 0xff, 0xff, 0xff, 0xf9, 0xff, 0xff, 0xff, 0xf1, 0xf1, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0xfe, 0xff, 0xff, 0xf8, 0xff, 0xff,
  0xff, 0xf1, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x70, 0xfe, 0xff,
  0xff, 0xf8, 0xff, 0xff, 0xff, 0xf1, 0x78, 0xf0, 0x11, 0x8f, 0x38, 0x1e,
  0x7f, 0xfc, 0x3f, 0xfc, 0xc1, 0x38, 0x0e, 0xff, 0xff, 0xf1, 0x38, 0xe0,
  0x01, 0x0e, 0x30, 0x8e, 0x3f, 0xfe, 0x0f, 0xf8, 0xc0, 0x18, 0x03, 0xfc,
  0xff, 0x01, 0xf8, 0xc7, 0x21, 0x0c, 0x61, 0x8e, 0x3f, 0xfe, 0x8f, 0x71,
  0xdc, 0x88, 0x63, 0xfc, 0xff, 0x00, 0x78, 0xc0, 0x71, 0x8c, 0x63, 0xc6,
  0x3f, 0x0e, 0xc6, 0x31, 0xfe, 0xc0, 0x71, 0xfc, 0xff, 0x00, 0x18, 0xc0,
  0x78, 0xc4, 0x63, 0xc6, 0x3f, 0x0e, 0x06, 0x30, 0x7e, 0xe0, 0x71, 0xfc,
  0xff, 0xf8, 0x1c, 0xe3, 0x78, 0xc4, 0x63, 0xe4, 0x3f, 0x3e, 0x07, 0x30,
  0x7e, 0xe0, 0x71, 0xfc, 0xff, 0x78, 0x8c, 0xe3, 0x38, 0xc6, 0x71, 0xe0,
  0x3f, 0x1e, 0xe7, 0x3f, 0x7e, 0xc4, 0x71, 0xfc, 0xff, 0x78, 0x0c, 0xe1,
  0x10, 0x86, 0xf0, 0xf0, 0x3f, 0x1c, 0xc7, 0x39, 0x6c, 0xc4, 0x31, 0xfe,
  0xff, 0x78, 0x1c, 0xe0, 0x00, 0x07, 0xf8, 0xf0, 0x7f, 0x00, 0x07, 0x38,
  0x60, 0x8c, 0x01, 0xff, 0x7f, 0x7c, 0x1c, 0xe2, 0x8c, 0x67, 0xfc, 0xf8,
  0xff, 0xc0, 0x0f, 0x7c, 0x70, 0x0e, 0x87, 0xff, 0xff, 0xff, 0xff, 0x7f,
  0xfc, 0xe3, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0x7f, 0xfc, 0xe3, 0x7f, 0xfc, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xfc, 0xe3, 0x1f, 0xfc,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f,
  0xfc, 0xe3, 0x1f, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x0e, 0xf8, 0x07, 0xff, 0xff, 0xff,
  0xff, 0xe3, 0xff, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x0e, 0xf0,
  0x01, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xf1, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xf1, 0x8e, 0xe3, 0x78, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf1,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0xcf, 0x63, 0xfc, 0xff, 0xc1, 0x87,
  0x47, 0x72, 0xe0, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0xcf, 0x63,
  0xfc, 0x7f, 0xc0, 0x01, 0x07, 0x72, 0xc0, 0xf9, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xf8, 0xc7, 0x67, 0xfc, 0x7f, 0xfe, 0x38, 0xc3, 0xf3, 0xc7, 0xf9,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xc7, 0x43, 0xfe, 0x7f, 0xf0, 0x00,
  0xe7, 0x71, 0xc0, 0xf8, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xf8, 0xc7, 0x63,
  0xfc, 0xff, 0xe0, 0x00, 0xe3, 0x31, 0xe0, 0xf8, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xf8, 0xc7, 0x63, 0xfc, 0xff, 0xc3, 0xfc, 0xe3, 0x11, 0xe7, 0xf8,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x78, 0xe7, 0x70, 0xbc, 0x7f, 0x67, 0xb8,
  0xf3, 0x19, 0xe3, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x07, 0xf8,
  0x80, 0x3f, 0xe0, 0x80, 0xf3, 0x39, 0xe0, 0xfc, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x83, 0x03, 0xfe, 0xc1, 0x7f, 0xf0, 0xc1, 0xf1, 0x38, 0x62, 0xfc,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xfc, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0xff, 0xff,
  0x7f, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x03, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x73, 0x3c, 0x9c, 0x63, 0x1e, 0x1e, 0xfe,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf1, 0x1c, 0x98,
  0x73, 0x0e, 0x0e, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xf1, 0xcc, 0x99, 0x31, 0xc6, 0xe7, 0xfc, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xf1, 0x0c, 0x18, 0x39, 0xe6, 0x07, 0xfc,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x79, 0x0c, 0x38,
  0x38, 0xe7, 0x07, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x39, 0xce, 0x3f, 0x3c, 0xe7, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x0e, 0x38, 0x3c, 0x07, 0x06, 0xfc,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x1f, 0x3c,
  0x1e, 0x0f, 0x0f, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x03, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xf8, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0x7f, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0x03, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0xf0, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x00,
  0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0xf0, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0xfe,
  0xff, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0x81, 0xff, 0xff, 0x00, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0xff, 0xff, 0x01, 0xfc, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xc0, 0xff,
  0xff, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x3f, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xfe, 0xff, 0xff, 0x0f, 0xf8, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0xf0, 0xff,
  0xff, 0x0f, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x00, 0xe0, 0xff, 0xff, 0x07, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0xc0, 0xff, 0xff, 0x03, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x80, 0xff,
  0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff,
  0x1f, 0x00, 0x00, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x7f, 0xfc, 0xff, 0x1f, 0x00, 0x00, 0xfe, 0x7f, 0xe0, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xf0, 0xff, 0x0f, 0x00, 0x00, 0xfe,
  0x3f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xc0, 0xff,
  0x0f, 0x00, 0x00, 0xfe, 0x0f, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x7f, 0x80, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x07, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8,
  0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xf0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x0f, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe,
  0x0f, 0x00, 0x00, 0xfe, 0xff, 0xff, 0x1f, 0xf0, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x7f, 0x00, 0xff, 0x0f, 0x00, 0x00, 0xfe, 0xff, 0xff, 0x7f, 0xe0,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xc0, 0xff, 0x1f, 0x00, 0x00, 0xfe,
  0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xf0, 0xff,
  0x1f, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x7f, 0xfc, 0xff, 0x3f, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0x03,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xfe, 0xff, 0x7f, 0x00, 0xc0, 0xff,
  0xff, 0xff, 0xff, 0x03, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0x07, 0xfc, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x0f,
  0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xfe, 0xff,
  0xff, 0xff, 0xff, 0x1f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xf0, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f,
  0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0x00, 0xe0, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0x00,
  0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0x03, 0xff, 0xff, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0x07, 0xfc, 0xff, 0x00, 0xe0, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00,
  0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0x1f, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00,
  0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x07, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xe0, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00,
  0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xe0, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

static const char usbDisconnectedImage[] =
{
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xf1, 0xff, 0xff, 0xff, 0xf3, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x03, 0xff, 0xff, 0xff, 0xf9, 0xff, 0xff, 0xff, 0xf1, 0xf1, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0xfe, 0xff, 0xff, 0xf8, 0xff, 0xff,
  0xff, 0xf1, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x70, 0xfe, 0xff,
  0xff, 0xf8, 0xff, 0xff, 0xff, 0xf1, 0x78, 0xf0, 0x11, 0x8f, 0x38, 0x1e,
  0x7f, 0xfc, 0x3f, 0xfc, 0xc1, 0x38, 0x0e, 0xff, 0xff, 0xf1, 0x38, 0xe0,
  0x01, 0x0e, 0x30, 0x8e, 0x3f, 0xfe, 0x0f, 0xf8, 0xc0, 0x18, 0x03, 0xfc,
  0xff, 0x01, 0xf8, 0xc7, 0x21, 0x0c, 0x61, 0x8e, 0x3f, 0xfe, 0x8f, 0x71,
  0xdc, 0x88, 0x63, 0xfc, 0xff, 0x00, 0x78, 0xc0, 0x71, 0x8c, 0x63, 0xc6,
  0x3f, 0x0e, 0xc6, 0x31, 0xfe, 0xc0, 0x71, 0xfc, 0xff, 0x00, 0x18, 0xc0,
  0x78, 0xc4, 0x63, 0xc6, 0x3f, 0x0e, 0x06, 0x30, 0x7e, 0xe0, 0x71, 0xfc,
  0xff, 0xf8, 0x1c, 0xe3, 0x78, 0xc4, 0x63, 0xe4, 0x3f, 0x3e, 0x07, 0x30,
  0x7e, 0xe0, 0x71, 0xfc, 0xff, 0x78, 0x8c, 0xe3, 0x38, 0xc6, 0x71, 0xe0,
  0x3f, 0x1e, 0xe7, 0x3f, 0x7e, 0xc4, 0x71, 0xfc, 0xff, 0x78, 0x0c, 0xe1,
  0x10, 0x86, 0xf0, 0xf0, 0x3f, 0x1c, 0xc7, 0x39, 0x6c, 0xc4, 0x31, 0xfe,
  0xff, 0x78, 0x1c, 0xe0, 0x00, 0x07, 0xf8, 0xf0, 0x7f, 0x00, 0x07, 0x38,
  0x60, 0x8c, 0x01, 0xff, 0x7f, 0x7c, 0x1c, 0xe2, 0x8c, 0x67, 0xfc, 0xf8,
  0xff, 0xc0, 0x0f, 0x7c, 0x70, 0x0e, 0x87, 0xff, 0xff, 0xff, 0xff, 0x7f,
  0xfc, 0xe3, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0x7f, 0xfc, 0xe3, 0x7f, 0xfc, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xfc, 0xe3, 0x1f, 0xfc,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f,
  0xfc, 0xe3, 0x1f, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x0e, 0xf8, 0x07, 0xff, 0xff, 0xff,
  0xff, 0xe3, 0xff, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x0e, 0xf0,
  0x01, 0xff, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xf1, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xe1, 0x8e, 0xe1, 0x70, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf1,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0xcf, 0x63, 0xfc, 0xff, 0xc1, 0x87,
  0x27, 0x72, 0xe0, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0xcf, 0x63,
  0xfc, 0x7f, 0xc0, 0x01, 0x07, 0x72, 0xc0, 0xf9, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xf8, 0xc7, 0x63, 0xfc, 0x7f, 0xfe, 0x38, 0xc7, 0xf3, 0xc7, 0xf9,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xc7, 0x63, 0xfe, 0x7f, 0xf8, 0x00,
  0xe3, 0x71, 0xc0, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xc7, 0x63,
  0xfe, 0xff, 0xe0, 0x00, 0xe3, 0x31, 0xc0, 0xf8, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xf8, 0xc7, 0x61, 0xfc, 0xff, 0xc3, 0xfc, 0xe3, 0x11, 0xe7, 0xf8,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x78, 0xe7, 0x70, 0xbc, 0xff, 0xc7, 0xb8,
  0xf3, 0x19, 0xe3, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x07, 0xf8,
  0x80, 0x3f, 0xe0, 0x80, 0xf3, 0x19, 0xe0, 0xfc, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x83, 0x03, 0xfe, 0xc1, 0x7f, 0xf0, 0xc1, 0xf3, 0x39, 0xe2, 0xfc,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xfc, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0xff, 0xff,
  0x7f, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x03, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x73, 0x3c, 0x9c, 0x63, 0x1e, 0x1e, 0xfe,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf1, 0x1c, 0x98,
  0x73, 0x0e, 0x0e, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xf1, 0xcc, 0x99, 0x31, 0xc6, 0xe7, 0xfc, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xf1, 0x0c, 0x18, 0x39, 0xe6, 0x07, 0xfc,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x79, 0x0c, 0x38,
  0x38, 0xe7, 0x07, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x39, 0xce, 0x3f, 0x3c, 0xe7, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x0e, 0x38, 0x3c, 0x07, 0x06, 0xfc,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x1f, 0x3c,
  0x1e, 0x0f, 0x0f, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x03, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xf8, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0x7f, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0x03, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0xf0, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x00,
  0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0xf0, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0xfe,
  0xff, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0x81, 0xff, 0xff, 0x00, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0xff, 0xff, 0x01, 0xfc, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xc0, 0xff,
  0xff, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x3f, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xfe, 0xff, 0xff, 0x0f, 0xf8, 0xff,
  0xff, 0xff, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0xf0, 0xff,
  0xff, 0x0f, 0xfc, 0xff, 0xff, 0xff, 0xcf, 0xfe, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x00, 0xe0, 0xff, 0xff, 0x07, 0xfe, 0xff, 0xff, 0xff, 0x77, 0xff,
  0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0xc0, 0xff, 0xff, 0x03, 0xff, 0xff,
  0xff, 0xff, 0xb7, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x80, 0xff,
  0xff, 0x81, 0xff, 0xff, 0xff, 0xff, 0xb3, 0xff, 0xff, 0x7f, 0xff, 0xff,
  0x1f, 0x00, 0x00, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xb3, 0xff,
  0xff, 0x7f, 0xfc, 0xff, 0x1f, 0x00, 0x00, 0xfe, 0x7f, 0xe0, 0xff, 0xff,
  0xff, 0xff, 0xbb, 0xff, 0xff, 0x7f, 0xf0, 0xff, 0x0f, 0x00, 0x00, 0xfe,
  0x3f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xdb, 0xff, 0xff, 0x7f, 0xc0, 0xff,
  0x0f, 0x00, 0x00, 0xfe, 0x0f, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xdb, 0xff,
  0xff, 0x7f, 0x80, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x07, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0xf8,
  0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00,
  0x00, 0x00, 0x00, 0xf0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x0f, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0xfe,
  0x0f, 0x00, 0x00, 0xfe, 0xff, 0xff, 0x1f, 0xf0, 0xff, 0xff, 0xd9, 0xff,
  0xff, 0x7f, 0x00, 0xff, 0x0f, 0x00, 0x00, 0xfe, 0xff, 0xff, 0x7f, 0xe0,
  0xff, 0xff, 0xd9, 0xff, 0xff, 0x7f, 0xc0, 0xff, 0x1f, 0x00, 0x00, 0xfe,
  0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xd9, 0xff, 0xff, 0x7f, 0xf0, 0xff,
  0x1f, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xdd, 0xff,
  0xff, 0x7f, 0xfc, 0xff, 0x3f, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0x03,
  0xff, 0xff, 0xed, 0xff, 0xff, 0x7f, 0xfe, 0xff, 0x7f, 0x00, 0xc0, 0xff,
  0xff, 0xff, 0xff, 0x03, 0xfe, 0xff, 0xec, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0x07, 0xfc, 0x7f, 0xee, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x0f,
  0xf8, 0x3f, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xfe, 0xff,
  0xff, 0xff, 0xff, 0x1f, 0xf0, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xf0, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f,
  0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0x00, 0xe0, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0x00,
  0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0x03, 0xff, 0xff, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0x07, 0xfc, 0xff, 0x00, 0xe0, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00,
  0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0x1f, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00,
  0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0x07, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xe0, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00,
  0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xe0, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

#ifdef __cplusplus
}
#endif

#endif /* IMAGE_H */