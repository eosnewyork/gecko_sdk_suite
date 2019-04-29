/***************************************************************************//**
 * @file
 * @brief Trace buffer for debug
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
#ifndef TRACE_H_
#define TRACE_H_

/* Set to 0 to remove the trace debugging capability */
#define TRACE_INCLUDE    1

#if TRACE_INCLUDE == 1

#define TRACE_BUFF_SIZE    1008

void TraceInit(void);
void TraceByte(uint8_t Byte);
void TraceWord(uint16_t Word);
void TraceDWord(uint32_t DWord);

#else

#define TRACE_BUFF_SIZE    0

#define TraceInit()
#define TraceByte(Byte)
#define TraceWord(Word)
#define TraceDWord(DWord)
#endif /* TRACE_INCLUDE */
#endif /* TRACE_H_ */
