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
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef APITYPES_H_
#define APITYPES_H_

#include <stdint.h>

typedef unsigned char  uint8;
typedef signed char   int8;
typedef unsigned short uint16;
typedef signed short   int16;
typedef unsigned long  uint32;
typedef signed long    int32;

typedef struct bd_addr_t
{
    unsigned char addr[6];

}bd_addr;

typedef bd_addr hw_addr;
typedef struct
{
    unsigned char len;
    unsigned char data[];
}uint8array;
typedef struct
{
    unsigned short len;
    unsigned char data[];
}uint16array;

typedef struct
{
    unsigned char len;
    signed char data[];
}string;

typedef union
{
        uint32_t u;
        uint8_t  a[4];
}ipv4;
#endif /* APITYPES_H_ */
