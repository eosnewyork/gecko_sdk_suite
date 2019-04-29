/*************************************************************************//**
 * @file bsp_tdk.h
 * @brief Board support package for Touch Display Kit
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

#ifndef __BSP_TDK_H__
#define __BSP_TDK_H__

void  BSP_initEbiFlash          (void);
void  BSP_initEbiSram           (void);
void  BSP_initEbiTftDirectDrive (void);

void *BSP_getTftSRAMDirectDriveBaseAddress (void);
void  BSP_setTftSRAMDirectDriveBaseAddress (void *baseAddressIn);

#endif /* __BSP_TDK_H__ */
