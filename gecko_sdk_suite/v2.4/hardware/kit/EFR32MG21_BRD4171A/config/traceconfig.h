/***************************************************************************//**
 * @file
 * @brief Provide SWO/ETM TRACE configuration parameters.
 * @version 5.6.0
 *******************************************************************************
 * # License
 * <b>Copyright 2017 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef TRACECONFIG_H
#define TRACECONFIG_H

//#define BSP_TRACE_SWO_LOCATION GPIO_ROUTELOC0_SWVLOC_LOC0

// Enable output on GPIO SWV pin
#define TRACE_ENABLE_PINS() \
  GPIO_PinModeSet((GPIO_Port_TypeDef)GPIO_SWV_PORT, GPIO_SWV_PIN, gpioModePushPull, 0);

//  No ETM trace support on this WSTK.

#endif
