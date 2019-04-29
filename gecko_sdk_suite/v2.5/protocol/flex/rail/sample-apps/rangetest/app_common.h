/***************************************************************************//**
 * @file
 * @brief This header file defines variables to be shared between the main
 * test application and customer specific sections.
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

#ifndef __APPS_COMMON_H__
#define __APPS_COMMON_H__

#include "em_gpio.h" // For ButtonArray definition
#ifdef CONFIGURATION_HEADER
#include CONFIGURATION_HEADER
#endif

/******************************************************************************
 * Constants
 *****************************************************************************/
#define COUNTOF(a) (sizeof(a) / sizeof(a[0]))

/******************************************************************************
 * Variable Export
 *****************************************************************************/
typedef struct ButtonArray{
  GPIO_Port_TypeDef   port;
  unsigned int        pin;
} ButtonArray_t;

#endif // __APPS_COMMON_H__
