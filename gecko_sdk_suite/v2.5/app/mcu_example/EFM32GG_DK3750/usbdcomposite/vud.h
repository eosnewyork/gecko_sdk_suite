/***************************************************************************//**
 * @file
 * @brief USB Vendor Uniqueue Device.
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

#ifndef __VUD_H
#define __VUD_H

#ifdef __cplusplus
extern "C" {
#endif

void VUD_Init(void);
int  VUD_SetupCmd(const USB_Setup_TypeDef *setup);

#ifdef __cplusplus
}
#endif

#endif /* __VUD_H */
