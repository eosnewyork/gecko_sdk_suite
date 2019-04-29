/***************************************************************************//**
 * @brief Ember Connect APIs and handlers for sending and receiving messages.
 * See @ref message for documentation.
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

/**
 * @addtogroup message
 * @brief Ember Connect APIs sending and receiving messages.
 *
 * See message.h for source code.
 * @{
 */

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

/** @brief \deprecated  The maximum length in bytes of the application payload
 *  for an unsecured message.  This define has been deprecated, you should use
 *  the ::emberGetMaximumPayloadLength API instead.
 */
#define EMBER_MAX_UNSECURED_APPLICATION_PAYLOAD_LENGTH     111

/** @brief \deprecated  The maximum length in bytes of the application payload
 * for a secured message. This define has been deprecated, you should use the
 * ::emberGetMaximumPayloadLength API instead.
 */
#define EMBER_MAX_SECURED_APPLICATION_PAYLOAD_LENGTH       102

/** @brief The maximum allowed endpoint value.
 */
#define EMBER_MAX_ENDPOINT                                 0xF

/** @name Handlers*/
//@{

/** @brief A callback invoked when the stack has completed sending a message.
 *
 * @param status   An ::EmberStatus value of:
 * - ::EMBER_SUCCESS if an ACK was received from the destination or no ACK was
 * requested.
 * - ::EMBER_MAC_NO_ACK_RECEIVED if an ACK was requested and no ACK was
 * received.
 * - ::EMBER_MAC_INDIRECT_TIMEOUT if the destination is a sleepy node and the
 * packet timed-out before the sleepy node sent a data request.
 * - ::EMBER_MAC_INDIRECT_MESSAGE_PURGED if the destination is a sleepy node
 *  and it was removed from the child table while the packet was stored in the
 *  indirect queue.
 * - ::EMBER_PHY_TX_CCA_FAIL if the node failed all the clear channel assessment
 * attempts.
 * - ::EMBER_PHY_TX_INCOMPLETE if the transmission was not completed correctly.
 *
 * @param message  An ::EmberOutgoingMessage describing the outgoing packet.
 */
void emberMessageSentHandler(EmberStatus status, EmberOutgoingMessage *message);

/** @brief A callback invoked when a node of ::EMBER_MAC_MODE_DEVICE type or
 * ::EMBER_MAC_MODE_SLEEPY_DEVICE type has completed sending a MAC frame.
 *
 * @param status   An ::EmberStatus value of:
 * - ::EMBER_SUCCESS if an ACK was received from the destination or no ACK was
 * requested.
 * - ::EMBER_MAC_NO_ACK_RECEIVED if an ACK was requested and no ACK was
 * received.
 * - ::EMBER_MAC_INDIRECT_TIMEOUT if the MAC frame was sent out via the indirect
 * queue and the it timed-out before a data request was received.
 * - ::EMBER_MAC_INDIRECT_MESSAGE_PURGED if the MAC frame was sent out via the
 * indirect queue and it was removed prior to a data request being received.
 * - ::EMBER_PHY_TX_CCA_FAIL if the node failed all the clear channel assessment
 * attempts.
 * - ::EMBER_PHY_TX_INCOMPLETE if the transmission was not completed correctly.
 *
 * @param message  An ::EmberOutgoingMacMessage describing the outgoing MAC
 * frame.
 */
void emberMacMessageSentHandler(EmberStatus status,
                                EmberOutgoingMacMessage *message);

/** @brief A callback invoked when a packet has been received.
 *
 * @param message  An ::EmberIncomingMessage describing the incoming packet.
 */
void emberIncomingMessageHandler(EmberIncomingMessage *message);

/** @brief A callback invoked when a node of ::EMBER_MAC_MODE_DEVICE type or
 * ::EMBER_MAC_MODE_SLEEPY_DEVICE has received a MAC frame.
 *
 * @param message  An ::EmberMacIncomingMessage describing the incoming packet.
 */
void emberIncomingMacMessageHandler(EmberIncomingMacMessage *message);

//@}//END Handlers

/** @name APIs*/
//@{

/** @brief Sends a message to the passed destination short ID.
 *
 * @param destination The destination node short ID.
 *
 * @param endpoint        The destination endpoint of the outgoing message. This
 *                        value can't exceed ::EMBER_MAX_ENDPOINT.
 *
 * @param messageTag      A value chosen by the application. This value will be
 *                        passed in the corresponding
 *                        ::emberMessageSentHandler() call.
 *
 * @param messageLength The size in bytes of the message payload. Use the
 * ::emberGetMaximumPayloadLength() API to determine the maximum message length
 * allowed.
 *
 * @param message A pointer to an array of bytes containing the message payload.
 *
 * @param options  Specifies the ::EmberMessageOptions for the outgoing message.
 *
 * @return an ::EmberStatus value of:
 * - ::EMBER_SUCCESS if the message was accepted by the stack. If a success
 * status is returned, the ::emberMessageSentHandler() callback is invoked
 * by the stack to indicate whether the message was successfully delivered or
 * the reason for failure.
 * - ::EMBER_INVALID_CALL if the node is not joined to a network, the packet
 * length is 0, the passed TX options indicates some feature that is not
 * supported, the passed endpoint exceeds ::EMBER_MAX_ENDPOINT or the node is
 * of ::EMBER_MAC_MODE_DEVICE device type or ::EMBER_MAC_MODE_SLEEPY_DEVICE (use
 * ::emberMacMessageSend instead).
 * - ::EMBER_MESSAGE_TOO_LONG if the message does not fit in a single frame.
 * - ::EMBER_PHY_TX_BUSY if the message cannot be sent since the node does not
 * support MAC queuing and the radio is currently busy.
 * - ::EMBER_MAC_TRANSMIT_QUEUE_FULL if the outgoing MAC queue is currently
 * full.
 * - ::EMBER_NO_BUFFERS if the stack could not allocate enough RAM to store the
 * submitted message.
 * - ::EMBER_MAC_UNKNOWN_DESTINATION if the node is part of a star network and
 * the destination node does not appear in the node's routing table.
 * - ::EMBER_MAC_SECURITY_NOT_SUPPORTED if the message was requested to be sent
 * out secured and either the local node does not support security or the
 * destination node is known to not support security.
 * - ::EMBER_MAC_BUSY if the message was not accepted because the MAC is
 * currently performing some critical operation.
 */
EmberStatus emberMessageSend(EmberNodeId destination,
                             uint8_t endpoint,
                             uint8_t messageTag,
                             EmberMessageLength messageLength,
                             uint8_t *message,
                             EmberMessageOptions options);

/** @brief Creates a MAC level frame and sends it to the passed destination.
 * This API can only be used for nodes of ::EMBER_MAC_MODE_DEVICE node type or
 * ::EMBER_MAC_MODE_SLEEPY_DEVICE node type.
 *
 * @param macFrame    A pointer to an ::EmberMacFrame struct that specifies the
 * source and destination addresses and the source and destination PAN IDs for
 * the message to be sent. Note that if the source/destination PAN ID is not
 * specified, it defaults to the node's PAN ID. Also, the destination
 * address mode must be either ::EMBER_MAC_ADDRESS_MODE_SHORT or
 * ::EMBER_MAC_ADDRESS_MODE_LONG. Also note that if the
 * ::EMBER_OPTIONS_SECURITY_ENABLED in the passed options, the source address
 * must be ::EMBER_MAC_ADDRESS_MODE_LONG.
 *
 * @param messageTag      A value chosen by the application. This value will be
 *                        passed in the corresponding
 *                        ::emberMacMessageSentHandler() call.
 *
 * @param messageLength The size in bytes of the message payload.
 *                      TODO: mention max length and getMaxPayload() API
 *
 * @param message A pointer to an array of bytes containing the message payload.
 *
 * @param options  Specifies the ::EmberMessageOptions for the outgoing message.
 *
 * @return an ::EmberStatus value of:
 * - ::EMBER_SUCCESS if the message was accepted by the stack. If a success
 * status is returned, the ::emberMacMessageSentHandler() callback will be
 * invoked by the stack to indicate whether the message was successfully
 * delivered or the reason for failure.
 * - ::EMBER_INVALID_CALL if the node is of a node type other than
 * ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE, the packet length
 * is 0, the passed TX options indicates some feature that is not supported,
 * the destination address mode is set to ::EMBER_MAC_ADDRESS_MODE_NONE or the
 * ::EMBER_OPTIONS_SECURITY_ENABLED option is set and the source address is set
 * to a mode other than ::EMBER_MAC_ADDRESS_MODE_LONG.
 * - ::EMBER_MESSAGE_TOO_LONG if the message does not fit in a single frame.
 * - ::EMBER_PHY_TX_BUSY if the message cannot be sent since the node does not
 * support MAC queuing and the radio is currently busy.
 * - ::EMBER_MAC_TRANSMIT_QUEUE_FULL if the outgoing MAC queue is currently
 * full.
 * - ::EMBER_NO_BUFFERS if the stack could not allocate enough RAM to store the
 * submitted message.
 * - ::EMBER_MAC_SECURITY_NOT_SUPPORTED if the message was requested to be sent
 * out with a security level that is not supported.
 * - ::EMBER_MAC_BUSY if the message was not accepted because the MAC is
 * currently performing a critical operation.
 */
EmberStatus emberMacMessageSend(EmberMacFrame *macFrame,
                                uint8_t messageTag,
                                EmberMessageLength messageLength,
                                uint8_t *message,
                                EmberMessageOptions options);

/** @brief Sends a data request command to the parent node. Note that if the
 * node short ID is a value of ::EMBER_USE_LONG_ADDRESS, the node shall use its
 * long ID as source address.
 *
 * @return and ::EmberStatus value of:
 * - ::EMBER_SUCCESS if the data poll was accepted by the MAC layer.
 * - ::EMBER_INVALID_CALL if the node is not joined to a network, the node is
 * not an end device, a ::EMBER_MAC_MODE_DEVICE or a
 * ::EMBER_MAC_MODE_SLEEPY_DEVICE, or the node is of ::EMBER_MAC_MODE_DEVICE
 * or ::EMBER_MAC_MODE_SLEEPY_DEVICE node type, is not joined to a coordinator
 * and the poll destination was not correctly set via the
 * ::emberSetPollDestinationAddress API.
 * - ::EMBER_MAC_BUSY if the MAC is currently performing a critical
 * operation.
 */
EmberStatus emberPollForData(void);

/** @brief Sets data polls destination address for nodes of
 * ::EMBER_MAC_MODE_DEVICE node type or EMBER_MAC_MODE_SLEEPY_DEVICE node type.
 *
 * @return and ::EmberStatus value of ::EMBER_SUCCESS if the data poll
 * destination was correctly set, a value of ::EMBER_INVALID_CALL otherwise.
 */
EmberStatus emberSetPollDestinationAddress(EmberMacAddress *destination);

/**
 * @brief Purges all indirect transmissions from the indirect message queue.
 *
 * @return an ::EmberStatus value of ::EMBER_SUCCESS if messages all indirect
 * messages were purged, or another ::EmberStatus value indicating the reason
 * of failure.
 **/
EmberStatus emberPurgeIndirectMessages(void);

/**
 * @brief Returns the maximum payload according to the passed source and
 * destination addressing modes, the passed secured flag, and the current
 * configuration of the node.
 *
 * @param srcAddressMode   An ::EmberMacAddressMode value indicating the mode
 * of the source address. Note, this parameter is only meaningful if the node
 * was started as ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE.
 *
 * @param dstAddressMode   An ::EmberMacAddressMode value indicating the mode
 * of the destination address. Note, this parameter is only meaningful if the
 * node was started as ::EMBER_MAC_MODE_DEVICE or
 * ::EMBER_MAC_MODE_SLEEPY_DEVICE.
 *
 * @param intrapan   Indicates whether the frame is an interpan frame or not.
 * Note, this parameter is only meaningful if the node was started as
 * ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE.
 *
 * @param secured   Indicates whether the frame should be secured or not.
 *
 * @return an invalid value of 0xFF if the node is currently active on a
 * network or any of the passed parameters are invalid. Otherise, it returns the
 * maximum payload length achievable according to the passed parameters.
 **/
uint8_t emberGetMaximumPayloadLength(EmberMacAddressMode srcAddressMode,
                                     EmberMacAddressMode dstAddressMode,
                                     bool interpan,
                                     bool secured);

//@}//END APIs

#endif //__MESSAGE_H__

/** @} // END addtogroup
 */
