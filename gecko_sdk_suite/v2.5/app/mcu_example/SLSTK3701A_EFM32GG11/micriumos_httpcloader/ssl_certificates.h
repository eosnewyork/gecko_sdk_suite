/***************************************************************************//**
 * @file
 * @brief Sample certificates and private keys for SSL/TLS ports
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

#ifndef  SSL_CERTIFICATES_H
#define  SSL_CERTIFICATES_H

#include  <cpu/include/cpu.h>

#define  SSL_CLIENT_CERT      SSL_ClientCert_Custom
#define  SSL_CLIENT_KEY       SSL_ClientKey_Custom

extern  const  CPU_CHAR       SSL_ROOT_CA[];
extern  const  CPU_CHAR       SSL_ClientKey_Custom[];

extern  const  CPU_INT32U     SSL_ClientKeyLen;
extern  const  CPU_INT32U     SSL_RootCALen;

#endif  /* SSL_CERTIFICATES_H */
