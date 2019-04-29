/***************************************************************************//**
 * @file
 * @brief Range Test application configurations.
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

#ifndef APP_CONFIGURATION_H_
#define APP_CONFIGURATION_H_

#if (defined APP_USE_GENERATED_CONFIGURATION)
/* Include the generated radio configuration */
#include "app-config.h"
#else
/* Include the default radio configuration for the board */
#if   ( (defined SL_WSTK6200A) || (defined SL_WSTK6220A) )
#include "app-config_R60-868MHz-13dBm.h"
#elif ( (defined SL_WSTK6202A) || (defined SL_WSTK6222A) )
#include "app-config_R63-915MHz-20dBm.h"
#elif (defined SL_WSTK6223A)
#include "app-config_R63-490MHz-20dBm.h"
#elif (defined SL_WSTK6224A)
#include "app-config_R63-169MHz-20dBm.h"
#else
#error No application configuration is defined! Create your own application configuration or define your kit properly!
#endif
#endif

#endif /* APP_CONFIGURATION_H_ */
