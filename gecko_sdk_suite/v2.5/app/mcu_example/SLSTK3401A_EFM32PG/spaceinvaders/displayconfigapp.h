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

/* Exclude pixel matrix allocation support which is not used by the
   spaceinvaders game. The game allocates it's own static framebuffer.
 */
#undef PIXEL_MATRIX_ALLOC_SUPPORT

#endif /* __DISPLAYCONFIGAPP_H__ */
