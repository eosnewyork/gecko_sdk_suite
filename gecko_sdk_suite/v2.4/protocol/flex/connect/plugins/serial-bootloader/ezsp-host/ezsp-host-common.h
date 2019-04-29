/***************************************************************************//**
 * @file ezsp-host-common.h
 * @brief Common header for EZSP Host functions
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/
#ifndef __EZSP_HOST_COMMON_H__
#define __EZSP_HOST_COMMON_H__

/** @brief Configuration parameters: values must be defined before calling ashResetNcp()
 * or ashStart(). Note that all times are in milliseconds.
 */
typedef struct {
  char serialPort[40];  /*!< The serial port name. */
  uint32_t baudRate;      /*!< Baud rate (bits/second). */
  uint8_t  stopBits;      /*!< Stop bits. */
  uint8_t  rtsCts;        /*!< True enables RTS/CTS flow control, false XON/XOFF. */
  uint16_t outBlockLen;   /*!< Maximum bytes to buffer before writing to serial port. */
  uint16_t inBlockLen;    /*!< Maximum bytes to read ahead from the serial port. */
  uint8_t  traceFlags;    /*!< Trace output control bit flags. */
  uint8_t  txK;           /*!< Maximum frames sent without being ACKed (1-7). */
  uint8_t  randomize;     /*!< Enables randomizing DATA frame payloads. */
  uint16_t ackTimeInit;   /*!< Adaptive rec'd ACK timeout initial value. */
  uint16_t ackTimeMin;    /*!< Adaptive rec'd ACK timeout minimum value. */
  uint16_t ackTimeMax;    /*!< Adaptive rec'd ACK timeout maximum value. */
  uint16_t timeRst;       /*!< Time allowed to receive RSTACK after NCP is reset. */
  uint8_t  nrLowLimit;    /*!< If free buffers < limit, host receiver isn't ready. */
  uint8_t  nrHighLimit;   /*!< If free buffers > limit, host receiver is ready. */
  uint16_t nrTime;        /*!< Time until a set nFlag must be resent (max 2032). */
  uint8_t  resetMethod;   /*!< A method used to reset NCP. */
  uint8_t  ncpType;       /*!< Type of NCP processor. */
} EzspHostConfig;

extern EzspStatus hostError;
extern EzspStatus ncpError;

#if defined(EZSP_UART)
  #define EZSP_ASH
#endif

#if defined(EZSP_ASH) && defined (EZSP_USB)
  #error Conflicting EZSP modes
#endif

#endif //__EZSP_HOST_COMMON_H__
