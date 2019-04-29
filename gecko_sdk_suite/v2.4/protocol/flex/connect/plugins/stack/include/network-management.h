/**
 * @file network-management.h
 * See @ref network_management for documentation.
 *
 * <!--Copyright 2014 by Silicon Labs. All rights reserved.              *80*-->
 */

/**
 * @addtogroup network_management
 * @brief Ember Connect API for finding, forming, joining, and leaving Connect
 * networks.
 *
 * See network-management.h for source code.
 * @{
 */

#ifndef __NETWORK_FORMATION_H__
#define __NETWORK_FORMATION_H__

/** @brief The maximum length in bytes of the application beacon payload.
 */
#define EMBER_MAC_MAX_APP_BEACON_PAYLOAD_LENGTH     16

/** @brief The maximum length in bytes of the beacon fields (superframe, GTS,
 *   pending address) as per 802.15.4 specs.
 */
#define EMBER_MAC_MAX_BEACON_FIELDS_LENGTH          84

/** @name Handlers*/
//@{

/** @brief This stack handler is invoked if a beacon is received during the
 *  scanning procedure if this was initiated by the application with the
 *  ::emberStartActiveScan stack APIs.
 *
 * @deprecated This stack handler has been deprecated and will be removed in a
 *  future release. The application should implement
 *  ::emberIncomingBeaconExtendedHandler instead.
 *
 * @param panId   The source pan ID of the received beacon.
 *
 * @param nodeId  The source node ID of the received beacon.
 *
 * @param rssi  The RSSI the beacon was received with.
 *
 * @param permitJoining   The permit joining flag in the received beacon.
 *
 * @param payloadLength  The length in bytes of the application beacon payload
 *   of the received beacon.
 *
 * @param payload   A pointer to the application beacon payload of the received
 *   beacon.
 */
void emberIncomingBeaconHandler(EmberPanId panId,
                                EmberNodeId nodeId,
                                int8_t rssi,
                                bool permitJoining,
                                uint8_t payloadLength,
                                uint8_t *payload);

/** @brief This stack handler is invoked if a beacon is received during the
 *  scanning procedure if this was initiated by the application with the
 *  ::emberStartActiveScan stack APIs.
 *
 * @param panId   The source pan ID of the received beacon.
 *
 * @param source  The source node address of the received beacon.
 *
 * @param rssi  The RSSI the beacon was received with.
 *
 * @param permitJoining   The permit joining flag in the received beacon.
 *
 * @param beaconFieldsLength  The length in bytes of the beacon fields
 * defined as per 802.15.4 specs (superframe, GTS fields and pending address
 * fields) of the received beacon.
 *
 * @param beaconFields   A pointer to the the beacon fields defined as per
 * 802.15.4 specs (superframe, GTS fields and pending address fields) of the
 * received beacon.
 *
 * @param beaconPayloadLength  The length in bytes of the application beacon
 *  payload of the received beacon.
 *
 * @param beaconPayload   A pointer to the application beacon payload of the
 *  received beacon.
 */
void emberIncomingBeaconExtendedHandler(EmberPanId panId,
                                        EmberMacAddress *source,
                                        int8_t rssi,
                                        bool permitJoining,
                                        uint8_t beaconFieldsLength,
                                        uint8_t *beaconFields,
                                        uint8_t beaconPayloadLength,
                                        uint8_t *beaconPayload);

/** @brief This stack handler is invoked after the application calls the
 *  ::emberStartActiveScan stack API to inform the application that the scanning
 *  procedure is complete.
 */
void emberActiveScanCompleteHandler(void);

/** @brief This stack handler is invoked after the application calls the
 *  ::emberStartEnergyScan stack API to inform the application that the energy
 *  scan procedure is complete and to provide statistics.
 *
 *  @param mean        The average energy detected in dBm.
 *  @param min         The minimum energy detected in dBm.
 *  @param max         The maximum energy detected in dBm.
 *  @param variance    The variance of the energy detected in dBm.
 */
void emberEnergyScanCompleteHandler(int8_t mean,
                                    int8_t min,
                                    int8_t max,
                                    uint16_t variance);

/** @brief This stack handler is invoked after the application calls the
 *  ::emberFrequencyHoppingStartClient stack API to inform the application that
 * the synchronization process with the server is complete.
 * After the client is synched to a server, it may seamlessly perform the
 * resynchronization process if needed. Sleepy devices in particular
 * periodically perform the resynchronization process. If the client fails a
 * resynchronization process, it will inform the application by invoking the
 * ::emberStackStatusHandler handler with EMBER_MAC_SYNC_TIMEOUT status.
 * When this occurs, the client will be no longer synched to the server.
 *
 *  @param status     An ::EmberStatus value indicating whether the
 *   synchronization process with the server was completed successfully or the
 *   reason for failure.
 */
void emberFrequencyHoppingStartClientCompleteHandler(EmberStatus status);

//@}//END Handlers

/** @name APIs*/
//@{

/** @brief Initializes the radio and the Ember stack.
 *
 * Device configuration functions must be called before ::emberInit()
 * is called.
 *
 * @note The application must check the return value of this function. If the
 * initialization fails, normal messaging functions are not available.
 * Some failure modes are not fatal, but the application must follow certain
 * procedures to permit recovery.
 * Ignoring the return code results in unpredictable radio and API behavior.
 * (In particular, problems with association will occur.)
 *
 * @return An ::EmberStatus value indicating successful initialization or the
 *   reason for failure.
 */
EmberStatus emberInit(void);

/** @brief A periodic tick routine that should be called:
 * - in the application's main event loop,
 * - as soon as possible after any radio interrupts, and
 * - after ::emberInit().
 */
void emberTick(void);

/** @brief Resumes the network operation after a reboot.
 *
 * This API must be called on boot prior to ANY network operations.
 * It initializes the networking system and attempts to resume the
 * previous network identity and configuration.  If the node was not previously
 * joined, this routine should still be called.
 *
 * If the node was previously joined to a network it will retain its original
 * type (e.g., coordinator, router, end device, and so on.)
 *
 * ::EMBER_NOT_JOINED is returned if the node is not part of a network.
 *
 * @return An ::EmberStatus value that indicates one of the following:
 *   - successful initialization,
 *   - ::EMBER_NOT_JOINED if the node is not part of a network, or
 *   - the reason for failure.
 */
EmberStatus emberNetworkInit(void);

/** @brief Starts an active scan. ::EMBER_SUCCESS signals that
 * the scan successfully started. Upon receiving a beacon,
 * the ::emberIncomingBeaconHandler stack handler is called. At
 * the end of the scanning procedure, the ::emberActiveScanCompleteHandler
 * stack handler is called. Note that, while a scan can be
 * initiated when the node is currently joined to a network, the node
 * will generally be unable to communicate with its PAN during
 * the scan period. In particular, time-sensitive network operations
 * might be affected because a scan operation will
 * prevent any network operation for the duration of the scan.
 *
 * Possible error responses and their meanings:
 * - ::EMBER_INVALID_CALL, the node is currently frequency hopping.
 * - ::EMBER_MAC_SCANNING, indicates an ongoing scan.
 * - ::EMBER_PHY_INVALID_CHANNEL, the specified channel is not a valid channel
 *   on the current platform.
 *
 * @param channel  The channel to scan.
 */
EmberStatus emberStartActiveScan(uint8_t channel);

/** @brief Starts an energy scan. ::EMBER_SUCCESS signals
 * that the scan successfully started. At the end of the scanning
 * procedure the ::emberEnergyScanCompleteHandler stack handler
 * is called. Note that, while a scan can be initiated
 * when the node is currently joined to a network, the node is generally
 * unable to communicate with its PAN during the scan period. In particular,
 * time-sensitive network operations might be affected because a scan operation
 * will prevent any network operation for the duration of the scan.
 *
 * Possible error responses and their meanings:
 * - ::EMBER_INVALID_CALL, the samples parameter is invalid or the node is
 *   currently frequency hopping.
 * - ::EMBER_MAC_SCANNING, indicates an ongoing scan.
 * - ::EMBER_PHY_INVALID_CHANNEL, the specified channel is not a valid channel
 *   on the current platform.
 * - ::EMBER_NO_BUFFERS, the stack doesn't have enough memory at the moment to
 *   perform the requested scan.
 *
 * @param channel  The channel to scan.
 *
 * @param samples   The number of energy samples to be produced. Each sample is
 *   performed averaging the detected energy over 8 symbols time (the actual
 *   length depends on the selected PHY configuration).
 */
EmberStatus emberStartEnergyScan(uint8_t channel, uint8_t samples);

/** @brief Allows the application to set the application portion of the
 * beacon payload. It's by default set to the empty string.
 *
 * @param payloadLength  The length in bytes of the application beacon payload
 *   to be set. This value can not exceed
 *   ::EMBER_MAC_MAX_APP_BEACON_PAYLOAD_LENGTH.
 *
 * @param payload   A pointer to the application beacon payload to be set.
 *
 * @return an ::EmberStatus value of ::EMBER_SUCCESS if the application beacon
 *   payload was successfully set, otherwise an ::EmberStatus value indicating
 *   the reason of failure.
 */
EmberStatus emberSetApplicationBeaconPayload(uint8_t payloadLength,
                                             uint8_t *payload);

/** @brief Forms a new network by becoming the coordinator.
 *
 * @param parameters  An ::EmberNetworkParameters value that specifies the
 *   network parameters of the network to be formed.
 *
 * @return An ::EmberStatus value that indicates either the successful formation
 *   of the new network or the reason that the network formation failed.
 */
EmberStatus emberFormNetwork(EmberNetworkParameters *parameters);

/** @brief Causes the stack to associate with the network using the
 * specified network parameters. It can take several seconds for the stack to
 * associate with the local network. Do not send messages until a call to the
 * ::emberStackStatusHandler() callback informs you that the stack is up.
 *
 * @param nodeType    Specification of the role that this node will have in
 *   the network. This role can be ::EMBER_STAR_RANGE_EXTENDER,
 *   ::EMBER_STAR_END_DEVICE, ::EMBER_STAR_SLEEPY_END_DEVICE,
 *   ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE. If the node is
 *   frequency hopping, the role can not be ::EMBER_STAR_RANGE_EXTENDER.
 *
 * @param nodeId    An ::EmberNodeId value indicating the short ID the node
 *   intends to use for addressing purposes. If this value is
 *   ::EMBER_NULL_NODE_ID, the network coordinator will allocate a new short
 *   address. Addresses should be allocated by the coordinator unless there is a
 *   specific need to join a network with a specific ID. If a specific ID is
 *   used, uniqueness should be guaranteed across the entire network by the
 *   application, via some out of band means. Notice that nodes of
 *   ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE require this
 *   parameter to be set to ::EMBER_NULL_NODE_ID.
 *
 * @param parameters  An ::EmberNetworkParameters value that specifies the
 *   network parameters of the network with which the node should associate.
 *
 * @return An ::EmberStatus value that indicates either:
 *   - that the association process began successfully or
 *   - the reason for failure.
 */
EmberStatus emberJoinNetworkExtended(EmberNodeType nodeType,
                                     EmberNodeId nodeId,
                                     EmberNetworkParameters *parameters);

#if defined(DOXYGEN_SHOULD_SKIP_THIS) || defined(UNIX_HOST)
/** @brief Causes the stack to associate with the network using the
 * specified network parameters. The network ID is assigned by the network
 * coordinator.
 * It can take several seconds for the stack to associate with the local
 * network. Do not send messages until a call to the ::emberStackStatusHandler()
 * callback informs you that the stack is up.
 *
 * @param nodeType    Specification of the role that this node will have in
 *   the network. This role can be ::EMBER_STAR_RANGE_EXTENDER,
 *   ::EMBER_STAR_END_DEVICE, ::EMBER_STAR_SLEEPY_END_DEVICE,
 *   ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE. If the node is
 *   frequency hopping, the role can not be ::EMBER_STAR_RANGE_EXTENDER.
 *
 * @param parameters  An ::EmberNetworkParameters value that specifies the
 *   network parameters of the network with which the node should associate.
 *
 * @return An ::EmberStatus value that indicates either:
 *   - that the association process began successfully or
 *   - the reason for failure.
 */
EmberStatus emberJoinNetwork(EmberNodeType nodeType,
                             EmberNetworkParameters *parameters);
#else
#define emberJoinNetwork(nodeType, parameters) \
  (emberJoinNetworkExtended((nodeType), EMBER_NULL_NODE_ID, (parameters)))
#endif // DOXYGEN_SHOULD_SKIP_THIS

/** @brief Tells the stack to allow other nodes to join the network
 * with this node as their parent.  Joining is initially disabled by default.
 * This function may only be called after the node is part of a network
 * and the stack is up.
 *
 * @param duration  A value of 0x00 disables joining. A value of 0xFF enables
 *   joining indefinitely.  Any other value enables joining for that number of
 *   seconds.
 *
 * @return an ::EmberStatus value of ::EMBER_SUCCESS if the permit joining
 *   was successfully set, otherwise an ::EmberStatus value indicating the
 *   reason of failure.
 */
EmberStatus emberPermitJoining(uint8_t duration);

/** @brief Causes the stack to go up with the passed network parameters without
 * performing any over-the-air message exchange.
 *
 * @param nodeType    Specifies the role that this node will have in
 *   the network. The only device types allowed in the commissioning API are
 *   ::EMBER_DIRECT_DEVICE, ::EMBER_MAC_MODE_DEVICE and
 *   ::EMBER_MAC_MODE_SLEEPY_DEVICE.
 *
 * @param nodeId  An ::EmberNodeId value that specifies the short ID the node
 * will have. The passed node ID must be a valid short address
 * (any value other than ::EMBER_NULL_NODE_ID or ::EMBER_BROADCAST_ADDRESS).
 * A value of ::EMBER_USE_LONG_ADDRESS is allowed only when commissioning the
 * node as ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE and will
 * cause the node to send MAC level control messages such as data polls or
 * beacons using long source addressing.
 *
 * @param parameters  An ::EmberNetworkParameters value that specifies the
 *   network parameters of the network the node should participate in.
 *
 * @return An ::EmberStatus value that indicates either:
 *   - that the node successfully commissioned the passed network parameters
 *   - the reason for failure.
 */
EmberStatus emberJoinCommissioned(EmberNodeType nodeType,
                                  EmberNodeId nodeId,
                                  EmberNetworkParameters *parameters);

/** @brief The maximum number of entries the auxiliary address filtering table
 * can hold.
 */
#define EMBER_MAX_AUXILIARY_ADDRESS_FILTERING_TABLE_LENGTH   2

/** @brief Sets an entry in the auxiliary address filtering table at a given
 * address. Nodes of ::EMBER_DIRECT_DEVICE device type can receive incoming
 * messages destined to any of the node IDs in the auxiliary address filtering
 * table (while also receiving  messages destined to actual node ID). If the
 * passed node ID is ::EMBER_NULL_NODE_ID, the entry is cleared.
 *
 * @param nodeId    An ::EmberNodeId value to be added to the auxiliary address
 * filtering table at the passed entry index.
 *
 * @param entryIndex  The index of the auxiliary address filtering table entry
 * to be set.
 *
 * @return An ::EmberStatus value of ::EMBER_SUCCESS if auxiliary address
 * filtering table entry was successfully set. An ::EmberStatus value of
 * ::EMBER_INVALID_CALL if the passed entry index is invalid.
 */
EmberStatus emberSetAuxiliaryAddressFilteringEntry(EmberNodeId nodeId,
                                                   uint8_t entryIndex);

/** @brief Retrieves the content of the auxiliary address filtering
 *  table at a given address.
 *
 * @param entryIndex  The index in the auxiliary address filtering table entry
 * to be retrieved.
 *
 * @return An ::EmberNodeId value of ::EMBER_NULL_NODE_ID if the passed entry
 *   index is invalid or if the passed entry index refers to an unused entry.
 *   Otherwise, it returns the content of the auxiliary address filtering table
 *   entry corresponding to the passed entry index.
 */
EmberNodeId emberGetAuxiliaryAddressFilteringEntry(uint8_t entryIndex);

/** @brief Forgets the current network and reverts to a network status of
 * ::EMBER_NO_NETWORK.
 */
void emberResetNetworkState(void);

/** @brief \deprecated Configures the "allocate address" flag in the association
 * request command for nodes joining as ::EMBER_MAC_MODE_DEVICE or
 * ::EMBER_MAC_MODE_SLEEPY_DEVICE. This flag should be set if the device wishes
 * to obtain a random 16-bit address from the coordinator. If cleared, the
 * coordinator will not assign a 16-bit address to the device.
 *
 * This API has been deprecated: nodes joining as ::EMBER_MAC_MODE_DEVICE or
 * ::EMBER_MAC_MODE_SLEEPY_DEVICE should pass ::EMBER_USE_LONG_ADDRESS to the
 * ::emberJoinNetworkExtended() API to prevent the coordinator from allocating a
 * short address and use long addressing instead.
 *
 * @param allocateAddress  Configured whether the short address should be
 * allocated by the coordinator when associating. Note, this only applies to
 * nodes joining as ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE.
 *
 * @return An ::EmberStatus value of ::EMBER_SUCCESS if the flag was
 *   successfully set, or another ::EmberStatus value indicating the reason of
 *   failure.
 */
EmberStatus emberMacSetAllocateAddressFlag(bool allocateAddress);

/** @brief Configures a ::EMBER_MAC_MODE_DEVICE node to be a PAN coordinator.
 * Note, this only applies to nodes that have been commissioned as
 * ::EMBER_MAC_MODE_DEVICE.
 *
 * @param isCoordinator  If set to true, the node will identify itself as the
 * PAN coordinator.
 *
 * @return An ::EmberStatus value of ::EMBER_SUCCESS if the coordinator flag was
 *   successfully set, or another ::EmberStatus value indicating the reason of
 *   failure.
 */
EmberStatus emberMacSetPanCoordinator(bool isCoordinator);

/** @brief Returns an ::EmberChildFLags bitmask indicating the child flags
 * of the child corresponding to the passed MAC address.
 *
 * @param address  A pointer to an ::EmberMacAddress that specifies the MAC
 * address of the child.
 *
 * @param flags  A pointer to an ::EmberChildFLags containing the child flags
 * of the child corresponding to the passed MAC address.
 *
 * @return An ::EmberStatus value of ::EMBER_SUCCESS if the child was found in
 *   the child table, or another ::EmberStatus value indicating the reason of
 *   failure.
 */
EmberStatus emberGetChildFlags(EmberMacAddress *address,
                               EmberChildFLags *flags);

/** @brief Removes the node corresponding to the passed MAC address from the
 * child table.
 *
 * @param address  A pointer to an ::EmberMacAddress that specifies the MAC
 * address of the child to be removed.
 *
 * @return An ::EmberStatus value of ::EMBER_SUCCESS if the node was
 *   successfully removed from the child table, or another ::EmberStatus value
 *   indicating the reason of failure.
 */
EmberStatus emberRemoveChild(EmberMacAddress *address);

/** @brief Starts operating as a frequency hopping server. This API can
 * only be invoked when the node is joined to a network.
 *
 * @return An ::EmberStatus value of ::EMBER_SUCCESS if the node successfully
 *   initiated frequency hopping server operations. An ::EmberStatus value of
 *   ::EMBER_INVALID_CALL if the node is not currently joined to a network or if
 *   the node is already performing frequency hopping.
 */
EmberStatus emberFrequencyHoppingStartServer(void);

/** @brief Starts operating as a frequency hopping client and synchronize
 * with the specified server. This API can be invoked on nodes that are already
 * joined to a network (with the exception of nodes started as
 * ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE) and nodes that are
 * not joined to a network yet. If the node is already performing frequency
 * hopping, this API returns ::EMBER_INVALID_CALL.
 * If this API returns ::EMBER_SUCCESS, the
 * ::emberFrequencyHoppingStartClientCompleteHandler is invoked
 * asynchronously to inform the application whether the node successfully
 * synchronized with the specified server or to inform the application of the
 * reason of failure.
 * Once the client is synched to a server, it may seamlessly perform the
 * resynchronization process if needed. Sleepy devices in particular
 * periodically perform the resynchronization process. If the client fails a
 * resynchronization process, it informs the application by invoking the
 * ::emberStackStatusHandler handler with EMBER_MAC_SYNC_TIMEOUT status.
 * When this occurs, the client will no longer be synched to the server.
 * The application may elect to attempt a new synchronization process by
 * invoking this API again.
 *
 * @param serverNodeId   An ::EmberNodeId value indicating the node ID of the
 *   server to synchronize with.
 *
 * @param serverPanId    An ::EmberPanId value indicating the PAN ID of the
 *   server to synchronize with. Note that this parameter is meaningful only if
 *   the node is not currently joined to any network.
 *
 * @return An ::EmberStatus value of ::EMBER_SUCCESS indicating that the node
 *   successfully initiated the synchronization process with the server,
 *   otherwise an ::EmberStatus value indicating the reason of failure.
 */
EmberStatus emberFrequencyHoppingStartClient(EmberNodeId serverNodeId,
                                             EmberPanId serverPanId);

/** @brief Stops frequency hopping. This API can only be invoked when
 * the node is frequency hopping.
 *
 * @return An ::EmberStatus value of ::EMBER_SUCCESS indicating that the node
 *   successfully stopped frequency hopping. An ::EmberStatus value of
 *   ::EMBER_INVALID_CALL if the node is not currently frequency hopping.
 */
EmberStatus emberFrequencyHoppingStop(void);
//@}//END APIs

#endif //__NETWORK_FORMATION_H__

/** @} // END addtogroup
 */
