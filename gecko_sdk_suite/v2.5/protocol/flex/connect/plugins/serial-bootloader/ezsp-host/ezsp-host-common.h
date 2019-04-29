/***************************************************************************//**
 * @brief Common header for EZSP Host functions
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
