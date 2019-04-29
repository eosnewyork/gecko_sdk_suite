/*************************************************************************//**
 * @file i2ccaptouch.h
 * @brief I2C touch support on Touch Display
 ******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef I2CCAPTOUCH_H
#define I2CCAPTOUCH_H

#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                                                                            */
/* Error code definitions                                                     */
/*                                                                            */
/******************************************************************************/

#define CAPT_OK                          (0x00000000)
#define CAPT_ERROR_BUFFER_TOO_SMALL      (0x00000001)
#define CAPT_ERROR_I2C_ERROR             (0x00000002)
#define CAPT_ERROR_PARSE_ERROR           (0x00000003)

/******************************************************************************/
/*                                                                            */
/* Other definitions                                                          */
/*                                                                            */
/******************************************************************************/

#define CAPT_STATUS_NEW                  (0x0)
#define CAPT_STATUS_ACTIVE               (0x2)
#define CAPT_STATUS_INACTIVE             (0x1)

/******************************************************************************/
/*                                                                            */
/* Type definitions                                                           */
/*                                                                            */
/******************************************************************************/

typedef struct _CAPT_Touch {
  uint8_t id;
  uint8_t status;
  float   x;
  float   y;
} CAPT_Touch;

/******************************************************************************/
/*                                                                            */
/* Global function declarations                                               */
/*                                                                            */
/******************************************************************************/

uint32_t CAPT_init     (void);
uint32_t CAPT_enable   (bool enable, bool wait);
uint32_t CAPT_getTouches (CAPT_Touch *touchBuffer, uint8_t len, uint8_t *nTouches);

#endif /* I2CCAPTOUCH_H */
