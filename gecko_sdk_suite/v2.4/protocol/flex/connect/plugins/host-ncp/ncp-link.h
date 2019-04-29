// File: ncp-link.h
//
// Description: Connect the NCP to a communications layer for
// sending and receiving management and packet data.
//
// Copyright 2014 by Silicon Laboratories. All rights reserved.            *80*

#ifndef __NCP_LINK_H__
#define __NCP_LINK_H__

#include "stack/include/ember-types.h"

///////////////////////////////////////////////////////////////////////////////
// Custom types used by the NCP link layer
///////////////////////////////////////////////////////////////////////////////

// The type of management command being sent to/from the host
typedef enum {
  MANAGEMENT_IDLE,              // Internal state, not sent in messages
  MANAGEMENT_COMMAND,           // A command
  MANAGEMENT_NOTIFY,            // An error and warning messages sent by the NCP
  // The NCP sends a RESPONSE_DONE or RESPONSE_ERROR when it finishes
  // processing a command.
  MANAGEMENT_RESPONSE_DONE,     // End of responses to most recent command
  MANAGEMENT_RESPONSE_ERROR,    // If a command was not parsed successfully,
                                // An error message is sent as a notification,
                                // followed by an empty RESPONSE_ERROR message.
} ManagementType;

///////////////////////////////////////////////////////////////////////////////
// Functions implemented by the lower level layer
///////////////////////////////////////////////////////////////////////////////

/**
 * Initializes the communication link used by the NCP. This function should
 * be implemented for the appliacation-specific communication mechanism. If
 * it is ommitted, a default initialization that does nothing will be used.
 */
void ncpLinkInit(void);

/**
 * Resets the NCP communication link. This function should clear any state held
 * in the link and ensure that it's ready to work again.
 */
void ncpLinkReset(void);

/**
 * Blocks until an incoming message is available for the
 * NCP link or the timeout expires.
 * @param timeoutMs The timeout in miliseconds.
 * @return Returns true if a message is available and false if the timeout
 * expired.
 */
bool ncpLinkWaitForIncoming(uint32_t timeoutMs);

/**
 * Attempts to exit the wait state entered by the ncpLinkWaitForIncoming()
 * command. This function can be used to stop the NCP main loop from blocking
 * after an asynchronous event, such as an interrupt.
 * NOTE: Code in this function must be safe to call from within interrupt
 * context.
 */
void ncpLinkAbortWaitForIncoming(void);

/**
 * Checks for incoming messages for the NCP, reads them
 * out, and calls the appropriate processing functions. This function must be implemented
 * for the specific link and should not block waiting for new messages.
 */
void ncpLinkProcessIncoming(void);

/**
 * Sends an IP packet received by the node to the host
 * application or thread for further processing.
 * @param secured If true then this packet is to be secured on output.
 * @param len The length in bytes of the data packet.
 */
//void ncpLinkIpPacketReceived(SerialLinkMessageType type, Buffer b);

/**
 * Takes a management command and sends it out over the
 * current link. The type is given and should be included so that it
 * can be decoded on the other side of the link.
 * @param managementType The type of management command being sent.
 * @param data A pointer to the management command data.
 * @param dataLength The length of the management command being sent.
 * @return Returns True if the management command can be sent or false
 * if unable to send.
 */
bool ncpLinkSendManagementCommand(ManagementType managementType,
                                  const uint8_t *data,
                                  uint8_t len);

/**
 * This function is only used in environments where both the host and stack
 * code run on the same chip (e.g., RTOS). In this situation, the function
 * is called to send a management command from the host to the virtual
 * NCP thread. On platforms that don't support this, return false.
 * @param managementType The type of management command being sent.
 * @param data A pointer to the management command data.
 * @param dataLength The length of the management command being sent.
 * @return Returns True if the management command can be sent or false
 * if unable to send.
 */
bool ncpLinkSendManagementCommandHost(ManagementType managementType,
                                      const uint8_t *data,
                                      uint8_t len);

/**
 * If this link is using any Ember buffers, it should mark them here to prevent
 * garbage collection. If not using Ember buffers, this function
 * should be stubbed out.
 */
void ncpLinkMarkBuffers(void);

/*
 * This function is called when the chip is powering down to clean up any
 * transactions over the link. This function should return true if there are
 * more messages that you're waiting to transmit over the link and false
 * otherwise.
 * @return true if you have more messages to send out over the communications
 * link.
 */
bool ncpLinkPreparingForPowerDown(void);

/**
 * This function is called when an error occurs while sending management commands
 * over the NCP link. In RTOS builds it can be called by either the host
 * or VNCP threads as indicated by the hostToNcp parameter.
 * @param data A pointer to the data that was not sent.
 * @param len The length of the data that was not sent in bytes.
 * @param hostToNcp True if this message was from the host to VNCP in RTOS
 * builds and false otherwise.
 */
void ncpLinkManagementErrorHandler(const uint8_t *data,
                                   uint8_t len,
                                   bool hostToNcp);

///////////////////////////////////////////////////////////////////////////////
// Functions that are implemented in this layer
///////////////////////////////////////////////////////////////////////////////

/*
 * Call this function to send a raw IPv6 packet to the networking stack.
 * @param secured Whether this packet should be secured at the MAC layer.
 * @param b A buffer containing the raw IPv6 packet.
 * @return true if we were able to process the packet and false if something
 * went wrong and the packet should be retried again later.
 */
//bool ncpLinkIpPacketHandler(SerialLinkMessageType type, Buffer b);

/*
 * Call this function to handle a management packet that was sent over the
 * NCP link.
 * @param managementType The type of management command received.
 * @param data A pointer to the data for this management packet.
 * @param len The length of the management command to process.
 * @return true if we were able to process the packet and false if something
 * went wrong.
 */
bool ncpLinkManagementPacketHandler(ManagementType managementType,
                                    uint8_t *data,
                                    uint8_t len);

//void ncpLinkUartTransmit(PacketHeader header);

#endif //__NCP_LINK_H__
