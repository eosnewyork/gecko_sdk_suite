/**
 * @file api-rename-undef.h
 * @brief Ember Connect API renaming (to be used for bare metal SoC
 * applications).
 *
 * <!--Copyright 2018 by Silicon Labs. All rights reserved.              *80*-->
 */

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
