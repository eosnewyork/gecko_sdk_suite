/***************************************************************************//**
 * @brief Micrium RTOS IPC code.
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

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER

#include "stack/include/ember.h"

#include <kernel/include/os.h>
#include "micrium-rtos-support.h"

OS_FLAG_GRP emAfPluginMicriumRtosFlags;

static OS_MUTEX commandMutex;
static OS_Q callbackQueue;
static uint8_t apiCommandData[MAX_STACK_API_COMMAND_SIZE];
static EmberBuffer callbackBuffer[EMBER_AF_PLUGIN_MICRIUM_RTOS_MAX_CALLBACK_QUEUE_SIZE];

//------------------------------------------------------------------------------
// Forward declarations

static uint16_t formatBinaryManagementCommand(uint8_t *buffer,
                                              uint16_t bufferSize,
                                              uint16_t identifier,
                                              const char *format,
                                              va_list argumentList);

static RTOS_ERR pendCommandPendingFlag(void);
static void postCommandPendingFlag(void);
static void pendResponsePendingFlag(void);
static void postResponsePendingFlag(void);
static void postCallbackPendingFlag(void);

static void fetchParams(uint8_t *readPointer, PGM_P format, va_list args);

//------------------------------------------------------------------------------
// Internal APIs

void emAfPluginMicriumRtosIpcInit(void)
{
  RTOS_ERR err;
  uint8_t i;

  for (i = 0; i < EMBER_AF_PLUGIN_MICRIUM_RTOS_MAX_CALLBACK_QUEUE_SIZE; i++) {
    callbackBuffer[i] = EMBER_NULL_BUFFER;
  }

  OSFlagCreate(&emAfPluginMicriumRtosFlags,
               "IPC flags",
               (OS_FLAGS)0,
               &err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);

  OSMutexCreate(&commandMutex,
                "IPC mutex",
                &err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);

  OSQCreate(&callbackQueue,
            "Callback queue",
            EMBER_AF_PLUGIN_MICRIUM_RTOS_MAX_CALLBACK_QUEUE_SIZE,
            &err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);
}

void emAfPluginMicriumRtosSendBlockingCommand(uint16_t identifier,
                                              const char *format,
                                              ...)
{
  va_list argumentList;

  // This API can not be called from the stack task (the stack task calls
  // stack APIs directly).
  assert(!emAfPluginMicriumRtosIsCurrentTaskStackTask());

  // Write the command in the command API buffer.
  va_start(argumentList, format);
  formatBinaryManagementCommand(apiCommandData,
                                sizeof(apiCommandData),
                                identifier,
                                format,
                                argumentList);
  va_end(argumentList);

  // Post the "command pending" flag, wake up the stack and pend for the
  // "response" pending flag.
  postCommandPendingFlag();
  emAfPluginMicriumRtosWakeUpConnectStackTask();
  pendResponsePendingFlag();
}

void emAfPluginMicriumRtosSendResponse(uint16_t identifier,
                                       const char *format,
                                       ...)
{
  va_list argumentList;

  // This API must be called from the stack task.
  assert(emAfPluginMicriumRtosIsCurrentTaskStackTask());

  // Write the command in the command API buffer.
  va_start(argumentList, format);
  formatBinaryManagementCommand(apiCommandData,
                                sizeof(apiCommandData),
                                identifier,
                                format,
                                argumentList);
  va_end(argumentList);

  postResponsePendingFlag();
}

void emAfPluginMicriumRtosProcessIncomingApiCommand(void)
{
  RTOS_ERR err = pendCommandPendingFlag();

  // This API must be called from the stack task.
  assert(emAfPluginMicriumRtosIsCurrentTaskStackTask());

  // There is a pending command (otherwise this would return
  // RTOS_ERR_WOULD_BLOCK).
  if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
    uint16_t commandId =
      emberFetchHighLowInt16u(apiCommandData);

    emAfPluginMicriumRtosHandleIncomingApiCommand(commandId);
  }
}

void emAfPluginMicriumRtosSendCallbackCommand(uint16_t identifier,
                                              const char *format,
                                              ...)
{
  RTOS_ERR err;
  va_list argumentList;
  const char *formatFinger;
  uint8_t commandLength = 2; // command ID
  uint8_t i;

  // This API must be called from the stack task.
  assert(emAfPluginMicriumRtosIsCurrentTaskStackTask());

  // Compute the amount of memory we need to allocate.
  va_start(argumentList, format);
  for (formatFinger = (char *) format; *formatFinger != 0; formatFinger++) {
    switch (*formatFinger) {
      case 'u':
        va_arg(argumentList, unsigned int);
        commandLength++;
        break;
      case 's':
        va_arg(argumentList, int);
        commandLength++;
        break;
      case 'v':
        va_arg(argumentList, unsigned int);
        commandLength += sizeof(uint16_t);
        break;
      case 'w':
        va_arg(argumentList, uint32_t);
        commandLength += sizeof(uint32_t);
        break;
      case 'b': {
        va_arg(argumentList, const uint8_t*);
        commandLength += va_arg(argumentList, int) + 1;
        break;
      }
      default:
        assert(false);
        break;
    }
  }
  va_end(argumentList);

  // Enqueing the callback in the app framework task queue.
  emberAfPluginMicriumRtosAcquireBufferSystemMutex();
  for (i = 0; i < EMBER_AF_PLUGIN_MICRIUM_RTOS_MAX_CALLBACK_QUEUE_SIZE; i++) {
    if (callbackBuffer[i] == EMBER_NULL_BUFFER) {
      callbackBuffer[i] = emberAllocateBuffer(commandLength);
      break;
    }
  }
  emberAfPluginMicriumRtosReleaseBufferSystemMutex();

  // Queue is full or no memory available: either way just return.
  if (i >= EMBER_AF_PLUGIN_MICRIUM_RTOS_MAX_CALLBACK_QUEUE_SIZE
      || callbackBuffer[i] == EMBER_NULL_BUFFER) {
    return;
  }

  // Write the callback command to the callback buffer.
  va_start(argumentList, format);
  formatBinaryManagementCommand(emberGetBufferPointer(callbackBuffer[i]),
                                commandLength,
                                identifier,
                                format,
                                argumentList);
  va_end(argumentList);

  OSQPost(&callbackQueue,
          &callbackBuffer[i],
          commandLength,
          OS_OPT_POST_FIFO,
          &err);

  // Wake up the AF task.
  postCallbackPendingFlag();

  // TODO: once we implement stack callback dispatching to custom application
  // tasks, we should add the copy of the callback to the queue of every custom
  // application task that chose to listen to this specific callback.
}

bool emAfPluginMicriumRtosProcessIncomingCallbackCommand(void)
{
  OS_MSG_SIZE messageLength;
  RTOS_ERR err;
  uint8_t *callbackBufferPtr;

  // This API can not be called from the stack task (the stack task calls
  // stack APIs directly).
  assert(!emAfPluginMicriumRtosIsCurrentTaskStackTask());

  callbackBufferPtr = OSQPend(&callbackQueue,
                              0,
                              OS_OPT_PEND_NON_BLOCKING,
                              &messageLength,
                              NULL,
                              &err);

  if (callbackBufferPtr) {
    uint8_t callbackData[MAX_STACK_CALLBACK_COMMAND_SIZE];
    uint16_t commandId;

    // Lock the buffer system to prevent defragmentation while we access the
    // buffer.
    emberAfPluginMicriumRtosAcquireBufferSystemMutex();
    MEMCOPY(callbackData,
            emberGetBufferPointer(*callbackBufferPtr),
            messageLength);
    // This assignment is safe because the vNCP acquires the buffer system mutex
    // before manipulating any entry in callbackBuffer[].
    *callbackBufferPtr = EMBER_NULL_BUFFER;
    emberAfPluginMicriumRtosReleaseBufferSystemMutex();

    commandId = emberFetchHighLowInt16u(callbackData);

    emAfPluginMicriumRtosHandleIncomingCallbackCommand(commandId,
                                                       callbackData + 2);

    return true;
  }

  return false;
}

bool emAfPluginMicriumRtosIsCurrentTaskStackTask(void)
{
  return (OSTCBCurPtr == emAfPluginMicriumRtosGetStackTcb());
}

void emAfPluginMicriumRtosAcquireCommandMutex(void)
{
  RTOS_ERR err;

  assert(!emAfPluginMicriumRtosIsCurrentTaskStackTask());

  OSMutexPend((OS_MUTEX *)&commandMutex,
              (OS_TICK)0,
              (OS_OPT)OS_OPT_PEND_BLOCKING,
              (CPU_TS*)NULL,
              &err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);
}

void emAfPluginMicriumRtosReleaseCommandMutex(void)
{
  RTOS_ERR err;

  assert(!emAfPluginMicriumRtosIsCurrentTaskStackTask());

  OSMutexPost((OS_MUTEX *)&commandMutex,
              (OS_OPT)OS_OPT_POST_NONE,
              &err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);
}

void emAfPluginMicriumRtosFetchApiParams(PGM_P format, ...)
{
  va_list argumentList;

  va_start(argumentList, format);
  // Start fetching right after the command ID
  fetchParams(apiCommandData + 2, format, argumentList);
  va_end(argumentList);
}

void emAfPluginMicriumRtosFetchCallbackParams(uint8_t *callbackParams,
                                              PGM_P format,
                                              ...)
{
  va_list argumentList;
  va_start(argumentList, format);
  fetchParams(callbackParams, format, argumentList);
  va_end(argumentList);
}

//------------------------------------------------------------------------------
// Internal callbacks

void emAfPluginMicriumRtosMarkBuffersCallback(void)
{
  uint8_t i;

  for (i = 0; i < EMBER_AF_PLUGIN_MICRIUM_RTOS_MAX_CALLBACK_QUEUE_SIZE; i++) {
    emberMarkBuffer(&callbackBuffer[i]);
  }
}

//------------------------------------------------------------------------------
// Static functions

// TODO: the two functions below can be consolidated

static void fetchParams(uint8_t *readPointer, PGM_P format, va_list args)
{
  char *c = (char *)format;
  PointerType ptr;

  while (*c) {
    ptr = va_arg(args, PointerType);
    switch (*c) {
      case 's': {
        uint8_t *realPointer = (uint8_t *)ptr;
        *realPointer = (uint8_t)*readPointer;
        readPointer++;
      }
      break;
      case 'u': {
        uint8_t *realPointer = (uint8_t *)ptr;
        *realPointer = (uint8_t)*readPointer;
        readPointer++;
      }
      break;
      case 'v': {
        uint16_t *realPointer = (uint16_t *)ptr;
        *realPointer = emberFetchHighLowInt16u(readPointer);
        readPointer += 2;
      }
      break;
      case 'w': {
        uint32_t *realPointer = (uint32_t *)ptr;
        *realPointer = emberFetchHighLowInt32u(readPointer);
        readPointer += 4;
      }
      break;
      case 'b': {
        uint8_t *realArray = (uint8_t *)ptr;
        uint8_t *lengthPointer = (uint8_t *)va_arg(args, PointerType);
        *lengthPointer = *readPointer;
        readPointer++;
        MEMMOVE(realArray, readPointer, *lengthPointer);
        readPointer += *lengthPointer;
      }
      break;
      case 'p': {
        uint8_t **realPointer = (uint8_t **)ptr;
        uint8_t *lengthPointer = (uint8_t *)va_arg(args, PointerType);
        *lengthPointer = *readPointer;
        readPointer++;
        *realPointer = readPointer;
        readPointer += *lengthPointer;
      }
      break;
      default:
        ; // meh
    }
    c++;
  }
}

static uint16_t formatBinaryManagementCommand(uint8_t *buffer,
                                              uint16_t bufferSize,
                                              uint16_t identifier,
                                              const char *format,
                                              va_list argumentList)
{
  uint8_t *finger = buffer;
  const char *formatFinger;

  MEMSET(buffer, 0, bufferSize);

  // store the identifier
  emberStoreHighLowInt16u(finger, identifier);
  finger += sizeof(uint16_t);

  // We use primitive types to retrieve va_args because gcc on some
  // platforms (eg., unix hosts) complains about automatic variable
  // promotion in variadic functions.
  // [eg: warning: 'uint8_t' is promoted to 'int' when passed through '...']
  //
  // An exception is made for the uint32_t case:  Here we need to check the size
  // of unsigned int, which can be smaller on platforms such as the c8051.

  for (formatFinger = (char *) format; *formatFinger != 0; formatFinger++) {
    switch (*formatFinger) {
      case 'u':
        *finger++ = va_arg(argumentList, unsigned int);
        break;
      case 's':
        *finger++ = va_arg(argumentList, int);
        break;
      case 'v':
        emberStoreHighLowInt16u(finger, va_arg(argumentList, unsigned int));
        finger += sizeof(uint16_t);
        break;
      case 'w':
        if (sizeof(unsigned int) < sizeof(uint32_t)) {
          emberStoreHighLowInt32u(finger, va_arg(argumentList, uint32_t));
        } else {
          emberStoreHighLowInt32u(finger, va_arg(argumentList, unsigned int));
        }
        finger += sizeof(uint32_t);
        break;
      case 'b': {
        const uint8_t *data = va_arg(argumentList, const uint8_t*);
        // store the size, must fit into a byte
        uint8_t dataSize = va_arg(argumentList, int);
        *finger++ = dataSize;

        if (dataSize > 0) {
          // Checking for NULL here save's every caller from checking.  We assume
          // the if dataSize is not zero then we should send all zeroes.
          if (data != NULL) {
            MEMCOPY(finger, data, dataSize);
          } else {
            MEMSET(finger, 0, dataSize);
          }
        }

        finger += dataSize;
        break;
      }
      default:
        // confused!
        assert(false);
        break;
    }
  }

  uint16_t length = finger - buffer;
  // sanity check
  assert(length <= bufferSize);

  return length;
}

static RTOS_ERR pendCommandPendingFlag(void)
{
  RTOS_ERR err;

  OSFlagPend(&emAfPluginMicriumRtosFlags,
             (OS_FLAGS)FLAG_IPC_COMMAND_PENDING,
             0,
             OS_OPT_PEND_NON_BLOCKING
             + OS_OPT_PEND_FLAG_SET_ANY
             + OS_OPT_PEND_FLAG_CONSUME,
             NULL,
             &err);

  return err;
}

static void postCommandPendingFlag(void)
{
  RTOS_ERR err;

  OSFlagPost(&emAfPluginMicriumRtosFlags,
             (OS_FLAGS)FLAG_IPC_COMMAND_PENDING,
             OS_OPT_POST_FLAG_SET,
             &err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);
}

static void pendResponsePendingFlag(void)
{
  RTOS_ERR err;

  OSFlagPend(&emAfPluginMicriumRtosFlags, (OS_FLAGS)FLAG_IPC_RESPONSE_PENDING,
             0,
             OS_OPT_PEND_BLOCKING
             + OS_OPT_PEND_FLAG_SET_ANY
             + OS_OPT_PEND_FLAG_CONSUME,
             NULL,
             &err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);
}

static void postResponsePendingFlag(void)
{
  RTOS_ERR err;

  OSFlagPost(&emAfPluginMicriumRtosFlags,
             (OS_FLAGS)FLAG_IPC_RESPONSE_PENDING,
             OS_OPT_POST_FLAG_SET,
             &err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);
}

static void postCallbackPendingFlag(void)
{
  RTOS_ERR err;

  OSFlagPost(&emAfPluginMicriumRtosFlags,
             (OS_FLAGS)FLAG_STACK_CALLBACK_PENDING,
             OS_OPT_POST_FLAG_SET,
             &err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);
}
