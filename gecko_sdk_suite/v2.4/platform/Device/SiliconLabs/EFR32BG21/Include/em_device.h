/**************************************************************************//**
 * @file em_device.h
 * @brief part header file master include file.
 * @version 5.6.0
 ******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories, Inc. http://www.silabs.com</b>
 ******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.@n
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.@n
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Laboratories, Inc.
 * has no obligation to support this Software. Silicon Laboratories, Inc. is
 * providing the Software 'AS IS', with no express or implied warranties of any
 * kind, including, but not limited to, any implied warranties of
 * merchantability or fitness for any particular purpose or warranties against
 * infringement of any proprietary rights of a third party.
 *
 * Silicon Laboratories, Inc. will not be liable for any consequential,
 * incidental, or special damages, or any other relief, or for any claim by
 * any third party, arising from your use of this Software.
 *
 *****************************************************************************/

#ifndef EM_DEVICE_H
#define EM_DEVICE_H
#if defined(EFR32BG21A010F1024IM32)
#include "efr32bg21a010f1024im32.h"

#elif defined(EFR32BG21A010F512IM32)
#include "efr32bg21a010f512im32.h"

#elif defined(EFR32BG21A020F1024IM32)
#include "efr32bg21a020f1024im32.h"

#elif defined(EFR32BG21A020F512IM32)
#include "efr32bg21a020f512im32.h"

#elif defined(EFR32BG21B010F1024IM32)
#include "efr32bg21b010f1024im32.h"

#elif defined(EFR32BG21B010F512IM32)
#include "efr32bg21b010f512im32.h"

#elif defined(EFR32BG21B020F1024IM32)
#include "efr32bg21b020f1024im32.h"

#elif defined(EFR32BG21B020F512IM32)
#include "efr32bg21b020f512im32.h"

#else
#error "em_device.h: PART NUMBER undefined"
#endif
#endif /* EM_DEVICE_H */
