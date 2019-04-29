/***************************************************************************//**
 * @file
 * @brief Common - Version Info
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _VERSION_H_
#define  _VERSION_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                                DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Version number of Micrium OS (Vx.yy.zz) times 10000.
#define  RTOS_VERSION                           50600u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

CPU_INT32U RTOS_Version(void);

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of version module include.
