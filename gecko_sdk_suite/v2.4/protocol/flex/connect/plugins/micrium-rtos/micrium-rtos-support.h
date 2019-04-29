// Copyright 2018 Silicon Laboratories, Inc.

#ifndef _MICRIUM_RTOS_SUPPORT_H_
#define _MICRIUM_RTOS_SUPPORT_H_

#include <kernel/include/os.h>
#include "micrium-rtos-support-gen.h"

#define FLAG_STACK_ACTION_PENDING                       ((OS_FLAGS)0x01)
#define FLAG_STACK_CALLBACK_PENDING                     ((OS_FLAGS)0x02)
#define FLAG_IPC_COMMAND_PENDING                        ((OS_FLAGS)0x04)
#define FLAG_IPC_RESPONSE_PENDING                       ((OS_FLAGS)0x08)

#define POLL_CLI_PERIOD_MS                              250

//------------------------------------------------------------------------------
// Public APIs

void emberAfPluginMicriumRtosCpuInit(void);

void emberAfPluginMicriumRtosInitAndRunTasks(void);

void emberAfPluginMicriumRtosAcquireBufferSystemMutex(void);

void emberAfPluginMicriumRtosReleaseBufferSystemMutex(void);

//------------------------------------------------------------------------------
// Internal APIs - generic OS

void emAfPluginMicriumRtosInitTasks(void);

void emAfPluginMicriumRtosStackTask(void *p_arg);

void emAfPluginMicriumRtosAppFrameworkTask(void *p_arg);

bool emAfPluginMicriumRtosIsCurrentTaskStackTask(void);

OS_TCB *emAfPluginMicriumRtosGetStackTcb(void);

void emAfPluginMicriumRtosWakeUpConnectStackTask(void);

//------------------------------------------------------------------------------
// Internal APIs - sleep

void emAfPluginMicriumRtosSleepHandler(void);

bool emAfPluginMicriumRtosStackTaskIsDeepSleepAllowed(void);

uint32_t emAfPluginMicriumRtosStackTaskGetNextWakeUpTimeMs(void);

uint32_t emAfPluginMicriumRtosAfTaskGetNextWakeUpTimeMs(void);

bool emAfPluginMicriumRtosAfTaskIsDeepSleepAllowed(void);

//------------------------------------------------------------------------------
// Internal APIs - IPC

void emAfPluginMicriumRtosIpcInit(void);

void emAfPluginMicriumRtosSendBlockingCommand(uint16_t identifier,
                                              const char *format,
                                              ...);

void emAfPluginMicriumRtosSendResponse(uint16_t identifier,
                                       const char *format,
                                       ...);

void emAfPluginMicriumRtosSendCallbackCommand(uint16_t identifier,
                                              const char *format,
                                              ...);

void emAfPluginMicriumRtosProcessIncomingApiCommand(void);

void emAfPluginMicriumRtosHandleIncomingApiCommand(uint16_t commandId);

bool emAfPluginMicriumRtosProcessIncomingCallbackCommand(void);

void emAfPluginMicriumRtosHandleIncomingCallbackCommand(uint16_t commandId,
                                                        uint8_t *callbackParams);

void emAfPluginMicriumRtosAcquireCommandMutex(void);

void emAfPluginMicriumRtosReleaseCommandMutex(void);

void emAfPluginMicriumRtosFetchApiParams(PGM_P format, ...);

void emAfPluginMicriumRtosFetchCallbackParams(uint8_t *callbackParams,
                                              PGM_P format,
                                              ...);

#endif // _MICRIUM_RTOS_SUPPORT_H_
