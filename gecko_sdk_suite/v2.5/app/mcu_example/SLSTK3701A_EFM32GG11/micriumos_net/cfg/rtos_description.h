/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/*
*********************************************************************************************************
*
*                                           RTOS DESCRIPTION
*
*                                      CONFIGURATION TEMPLATE FILE
*
* File : rtos_description.h
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _RTOS_DESCRIPTION_H_
#define  _RTOS_DESCRIPTION_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <common/include/rtos_opt_def.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       ENVIRONMENT DESCRIPTION
*********************************************************************************************************
*********************************************************************************************************
*/

#define  RTOS_CPU_SEL                                       RTOS_CPU_SEL_ARM_V7_M

#define  RTOS_TOOLCHAIN_SEL                                 RTOS_TOOLCHAIN_AUTO

#define  RTOS_INT_CONTROLLER_SEL                            RTOS_INT_CONTROLLER_ARMV7_M


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       RTOS MODULES DESCRIPTION
*********************************************************************************************************
*********************************************************************************************************
*/

#define  RTOS_MODULE_NET_AVAIL

#define  RTOS_MODULE_NET_IF_ETHER_AVAIL

#define  RTOS_MODULE_KERNEL_AVAIL


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of rtos_description.h module include.            */
