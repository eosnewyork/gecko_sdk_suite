/***************************************************************************//**
 * @brief Ember Connect API renaming (to be used for bare metal SoC
 * applications).
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
#define emberMacGetParentAddress                           emApiMacGetParentAddress
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
