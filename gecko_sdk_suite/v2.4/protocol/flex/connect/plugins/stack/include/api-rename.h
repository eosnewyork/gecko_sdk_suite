/**
 * @file api-rename.h
 * @brief Ember Connect API renaming (to be used for bare metal SoC
 * applications).
 *
 * <!--Copyright 2018 by Silicon Labs. All rights reserved.              *80*-->
 */

#ifndef __API_RENAME_H__
#define __API_RENAME_H__

// Core
#define emberCalibrateCurrentChannel                       emApiCalibrateCurrentChannel
#define emberCurrentStackTasks                             emApiCurrentStackTasks
#define emberGetCounter                                    emApiGetCounter
#define emberGetMaximumPayloadLength                       emApiGetMaximumPayloadLength
#define emberGetRadioChannel                               emApiGetRadioChannel
#define emberGetRadioPower                                 emApiGetRadioPower
#define emberInit                                          emApiInit
#define emberNetworkState                                  emApiNetworkState
#define emberSetMacParams                                  emApiSetMacParams
#define emberSetRadioChannel                               emApiSetRadioChannel
#define emberSetRadioPower                                 emApiSetRadioPower
#define emberSetRadioPowerMode                             emApiSetRadioPowerMode
#define emberSetSecurityKey                                emApiSetSecurityKey
#define emberStackIdleTimeMs                               emApiStackIdleTimeMs
#define emberStackIsUp                                     emApiStackIsUp
#define emberTick                                          emApiTick

// Messaging
#define emberGetChildFlags                                 emApiGetChildFlags
#define emberMacMessageSend                                emApiMacMessageSend
#define emberMacSetAllocateAddressFlag                     emApiMacSetAllocateAddressFlag
#define emberMacSetPanCoordinator                          emApiMacSetPanCoordinator
#define emberMessageSend                                   emApiMessageSend
#define emberPollForData                                   emApiPollForData
#define emberPurgeIndirectMessages                         emApiPurgeIndirectMessages
#define emberRemoveChild                                   emApiRemoveChild
#define emberSetPollDestinationAddress                     emApiSetPollDestinationAddress

// Network Management
#define emberFormNetwork                                   emApiFormNetwork
#define emberFrequencyHoppingStartClient                   emApiFrequencyHoppingStartClient
#define emberFrequencyHoppingStartServer                   emApiFrequencyHoppingStartServer
#define emberFrequencyHoppingStop                          emApiFrequencyHoppingStop
#define emberGetAuxiliaryAddressFilteringEntry             emApiGetAuxiliaryAddressFilteringEntry
#define emberJoinCommissioned                              emApiJoinCommissioned
#define emberJoinNetworkExtended                           emApiJoinNetworkExtended
#define emberNetworkInit                                   emApiNetworkInit
#define emberPermitJoining                                 emApiPermitJoining
#define emberResetNetworkState                             emApiResetNetworkState
#define emberSetApplicationBeaconPayload                   emApiSetApplicationBeaconPayload
#define emberSetAuxiliaryAddressFilteringEntry             emApiSetAuxiliaryAddressFilteringEntry
#define emberStartActiveScan                               emApiStartActiveScan
#define emberStartEnergyScan                               emApiStartEnergyScan

#endif // __API_RENAME_H__
