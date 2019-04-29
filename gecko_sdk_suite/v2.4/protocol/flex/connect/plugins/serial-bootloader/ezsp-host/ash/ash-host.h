/** @file ash-host.h
 * @brief Header for ASH Host functions
 *
 * See @ref ash_util for documentation.
 *
 * <!-- Copyright 2007-2010 by Ember Corporation. All rights reserved.       -->
 */

/** @addtogroup ash_util
 *
 * See ash-host.h.
 *
 *@{
 */

#ifndef __ASH_HOST_H__
#define __ASH_HOST_H__
#include "../ezsp-host-common.h"

#define ASH_MAX_TIMEOUTS          6   /*!< Timeouts before link is judged down. */
#define ASH_MAX_WAKE_TIME         150 /*!< The maximum time in milliseconds for NCP to wake */

#define ASH_PORT_LEN              40  /*!< A length of the serial port name string. */

// Bits in traceFlags to enable various host trace outputs
#define TRACE_FRAMES_BASIC        1   /*!< Frames sent and received. */
#define TRACE_FRAMES_VERBOSE      2   /*!< Basic frames + internal variables. */
#define TRACE_EVENTS              4   /*!< Events. */
#define TRACE_EZSP                8   /*!< EZSP commands, responses, and callbacks. */
#define TRACE_EZSP_VERBOSE        16  /*!< Additional EZSP information. */

// resetMethod values
#define ASH_RESET_METHOD_RST      0   /*!< Send RST frame. */
#define ASH_RESET_METHOD_DTR      1   /*!< Reset using DTR. */
#define ASH_RESET_METHOD_CUSTOM   2   /*!< A hook for a user-defined reset. */
#define ASH_RESET_METHOD_NONE     3   /*!< No reset - for testing. */

// ncpType values
#define ASH_NCP_TYPE_EM2XX_EM3XX  0   /*!< EM2XX or EM3XX */

// ashSelectHostConfig() values
#define ASH_HOST_CONFIG_EM2XX_EM3XX_115200_RTSCTS   0
#define ASH_HOST_CONFIG_EM2XX_EM3XX_57600_XONXOFF   1

/** @brief Configuration parameters: values must be defined before calling ashResetNcp()
 * or ashStart(). Note that all times are in milliseconds.
 */
typedef struct {
  char serialPort[ASH_PORT_LEN];  /*!< A serial port name. */
  uint32_t baudRate;      /*!< Baud rate (bits/second). */
  uint8_t  stopBits;      /*!< Stop bits. */
  uint8_t  rtsCts;        /*!< True enables RTS/CTS flow control, false XON/XOFF. */
  uint16_t outBlockLen;   /*!< Maximum bytes to buffer before writing to the serial port. */
  uint16_t inBlockLen;    /*!< Maximum bytes to read ahead from the serial port. */
  uint8_t  traceFlags;    /*!< Trace output control bit flags. */
  uint8_t  txK;           /*!< Maximum frames sent without being ACKed (1-7). */
  uint8_t  randomize;     /*!< Enables randomizing DATA frame payloads. */
  uint16_t ackTimeInit;   /*!< Adaptive rec'd ACK timeout initial value. */
  uint16_t ackTimeMin;    /*!< Adaptive rec'd ACK timeout minimum value. */
  uint16_t ackTimeMax;    /*!< Adaptive rec'd ACK timeout maximum value. */
  uint16_t timeRst;       /*!< Time allowed to receive RSTACK after the NCP is reset. */
  uint8_t  nrLowLimit;    /*!< If free buffers < limit, host receiver isn't ready. */
  uint8_t  nrHighLimit;   /*!< If free buffers > limit, host receiver is ready. */
  uint16_t nrTime;        /*!< A time until a set nFlag must be resent (max 2032). */
  uint8_t  resetMethod;   /*!< A method used to reset NCP. */
  uint8_t  ncpType;       /*!< A type of the NCP processor. */
} AshHostConfig;

#define ashReadConfig(member) \
  (ashHostConfig.member)

#define ashReadConfigOrDefault(member, defval) \
  (ashHostConfig.member)

#define ashWriteConfig(member, value) \
  do { ashHostConfig.member = value; } while (0)

#define BUMP_HOST_COUNTER(mbr) do { ashCount.mbr++; } while (0)
#define ADD_HOST_COUNTER(op, mbr) do { ashCount.mbr += op; }  while (0)

typedef struct {
  uint32_t txBytes;             /*!< Total bytes transmitted. */
  uint32_t txBlocks;            /*!< Blocks transmitted. */
  uint32_t txData;              /*!< DATA frame data fields bytes transmitted. */
  uint32_t txAllFrames;         /*!< Frames of all types transmitted. */
  uint32_t txDataFrames;        /*!< DATA frames transmitted. */
  uint32_t txAckFrames;         /*!< ACK frames transmitted. */
  uint32_t txNakFrames;         /*!< NAK frames transmitted. */
  uint32_t txReDataFrames;      /*!< DATA frames retransmitted. */
  uint32_t txN0Frames;          /*!< ACK and NAK frames with nFlag 0 transmitted. */
  uint32_t txN1Frames;          /*!< ACK and NAK frames with nFlag 1 transmitted. */
  uint32_t txCancelled;         /*!< Frames cancelled (with ASH_CAN byte). */

  uint32_t rxBytes;             /*!< Total bytes received. */
  uint32_t rxBlocks;            /*!< Blocks received.         */
  uint32_t rxData;              /*!< DATA frame data fields bytes received. */
  uint32_t rxAllFrames;         /*!< Frames of all types received.         */
  uint32_t rxDataFrames;        /*!< DATA frames received.                    */
  uint32_t rxAckFrames;         /*!< ACK frames received.                       */
  uint32_t rxNakFrames;         /*!< NAK frames received.                       */
  uint32_t rxReDataFrames;      /*!< Retransmitted DATA frames received.        */
  uint32_t rxN0Frames;          /*!< ACK and NAK frames with nFlag 0 received.  */
  uint32_t rxN1Frames;          /*!< ACK and NAK frames with nFlag 1 received.  */
  uint32_t rxCancelled;         /*!< Frames cancelled (with ASH_CAN byte). */

  uint32_t rxCrcErrors;         /*!< Frames with CRC errors. */
  uint32_t rxCommErrors;        /*!< Frames with comm errors (with ASH_SUB byte). */
  uint32_t rxTooShort;          /*!< Frames shorter than the minimum. */
  uint32_t rxTooLong;           /*!< Frames longer than the maximum. */
  uint32_t rxBadControl;        /*!< Frames with an illegal control byte. */
  uint32_t rxBadLength;         /*!< Frames with an illegal length for type of frame. */
  uint32_t rxBadAckNumber;      /*!< Frames with bad ACK numbers. */
  uint32_t rxNoBuffer;          /*!< DATA frames discarded due to lack of buffers. */
  uint32_t rxDuplicates;        /*!< Duplicate retransmitted DATA frames. */
  uint32_t rxOutOfSequence;     /*!< DATA frames received out of sequence. */
  uint32_t rxAckTimeouts;       /*!< Received ACK timeouts. */
} AshCount;

extern AshHostConfig ashHostConfig;
extern AshCount ashCount;
extern bool ncpSleepEnabled;

/** @brief Selects a set of host configuration parameters. To select
 * a configuration other than the default, must be called before ashStart().
 *
 * @param config  one of the following:
 *  -            ::ASH_HOST_CONFIG_EM2XX_EM3XX_115200_RTSCTS (default)
 *  -            ::ASH_HOST_CONFIG_EM2XX_EM3XX_57600_XONXOFF
 *
 *
 * @return
 * - ::EZSP_SUCCESS
 * _ ::EZSP_ASH_HOST_FATAL_ERROR
 */
EzspStatus ashSelectHostConfig(uint8_t config);

/** @brief Initializes the ASH protocol and waits until the NCP
 * finishes rebooting or a non-recoverable error occurs.
 *
 * @return
 * - ::EZSP_SUCCESS
 * - ::EZSP_ASH_HOST_FATAL_ERROR
 * - ::EZSP_ASH_NCP_FATAL_ERROR
 */
EzspStatus ashStart(void);

/** @brief Stops the ASH protocol, flushes and closes the serial port,
 *  clears all queues, stops timers, and so on. Does not affect any host configuration
 *  parameters.
 */
void ashStop(void);

/** @brief Adds a DATA frame to the transmit queue to send to the NCP.
 *  Frames that are too long or too short will not be sent. Frames
 *  will not be added to the queue if the host is not in the Connected
 *  state, if the NCP is not ready to receive a DATA frame, or if there
 *  is no room in the queue.
 *
 * @param len    length of data field.
 *
 * @param inptr  a pointer to array containing the data to be sent.
 *
 * @return
 * - ::EZSP_SUCCESS
 * - ::EZSP_ASH_NO_TX_SPACE
 * - ::EZSP_DATA_FRAME_TOO_SHORT
 * - ::EZSP_DATA_FRAME_TOO_LONG
 * - ::EZSP_ASH_NOT_CONNECTED
 */
EzspStatus ashSend(uint8_t len, const uint8_t *inptr);

/** @brief Initializes the ASH serial port and, if enabled,
 *  resets the NCP. The method used to do the reset is specified by the
 *  the host configuration parameter resetMethod.
 *
 *  When the reset method is sending a RST frame, the caller should
 *  retry NCP resets a few times if it fails.
 *
 * @return
 * - ::EZSP_SUCCESS
 * - ::EZSP_ASH_HOST_FATAL_ERROR
 * - ::EZSP_ASH_NCP_FATAL_ERROR
 */
EzspStatus ashResetNcp(void);

/** @brief Wakes up the NCP by sending two 0xFF bytes. When the NCP wakes,
 *  it sends back an 0xFF byte.
 *
 * @param init set true on the first call to this function, starts timer.
 *
 * @return
 * - ::EZSP_ASH_IN_PROGRESS       NCP is not yet awake, but has not timed out.
 * - ::EZSP_SUCCESS               NCP is swake.
 * - ::EZSP_ASH_HOST_FATAL_ERROR  NCP did not wake within ASH_MAX_WAKE_TIME.
 */
EzspStatus ashWakeUpNcp(bool init);

/** @brief Indicates whether the host is in the Connected state.
 *  If not, the host and NCP cannot exchange DATA frames. Note that
 *  this function does not actively confirm that communication with
 *  NCP is healthy, but returns its last known status.
 *
 * @return
 * - true      host and NCP can exchange DATA frames.
 * - false     host and NCP cannot now exchange DATA frames.
 */
bool ashIsConnected(void);

/** @brief Manages outgoing communication to the NCP, including
 *  DATA frames and the frames used for initialization and
 *  error detection and recovery.
 */
void ashSendExec(void);

/** @brief Processes all received frames.
 *  Received DATA frames are appended to the receive queue if there is room.
 *
 * @return
 * - ::EZSP_SUCCESS
 * - ::EZSP_ASH_IN_PROGRESS
 * - ::EZSP_ASH_NO_RX_DATA
 * - ::EZSP_ASH_NO_RX_SPACE
 * - ::EZSP_ASH_HOST_FATAL_ERROR
 * - ::EZSP_ASH_NCP_FATAL_ERROR
 */
EzspStatus ashReceiveExec(void);

/** @brief Returns the next DATA frame received, if there is one.
 *  To be more precise, the head of the receive queue is copied into the
 *  specified buffer and then freed.
 *
 * @param len     length of the DATA frame if one was returned.
 *
 * @param buffer  array into which the DATA frame should be copied.
 *
 * @return
 * - ::EZSP_SUCCESS
 * - ::EZSP_ASH_NO_RX_DATA
 * - ::EZSP_ASH_NOT_CONNECTED
 */
EzspStatus ashReceive(uint8_t *len, uint8_t *buffer);

/** @brief Returns true if the host can sleep without causing errors in the
 *  ASH protocol.
 *
 */
bool ashOkToSleep(void);

#endif //__ASH_HOST_H___

/** @} // END addtogroup
 */
