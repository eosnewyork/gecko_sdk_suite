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

#ifndef __API_RENAME_UNDEF_H__
#define __API_RENAME_UNDEF_H__

// Core
#undef emberCalibrateCurrentChannel
#undef emberCurrentStackTasks
#undef emberGetCounter
#undef emberGetMaximumPayloadLength
#undef emberGetRadioChannel
#undef emberGetRadioPower
#undef emberInit
#undef emberMacGetParentAddress
#undef emberNetworkState
#undef emberSetMacParams
#undef emberSetRadioChannel
#undef emberSetRadioPower
#undef emberSetRadioPowerMode
#undef emberSetSecurityKey
#undef emberStackIdleTimeMs
#undef emberStackIsUp
#undef emberTick

// Messaging
#undef emberGetChildFlags
#undef emberMacMessageSend
#undef emberMacSetAllocateAddressFlag
#undef emberMacSetPanCoordinator
#undef emberMessageSend
#undef emberPollForData
#undef emberPurgeIndirectMessages
#undef emberRemoveChild
#undef emberSetPollDestinationAddress

// Network Management
#undef emberFormNetwork
#undef emberFrequencyHoppingStartClient
#undef emberFrequencyHoppingStartServer
#undef emberFrequencyHoppingStop
#undef emberGetAuxiliaryAddressFilteringEntry
#undef emberJoinCommissioned
#undef emberJoinNetworkExtended
#undef emberNetworkInit
#undef emberPermitJoining
#undef emberResetNetworkState
#undef emberSetApplicationBeaconPayload
#undef emberSetAuxiliaryAddressFilteringEntry
#undef emberStartActiveScan
#undef emberStartEnergyScan

#endif // __API_RENAME_UNDEF_H__
