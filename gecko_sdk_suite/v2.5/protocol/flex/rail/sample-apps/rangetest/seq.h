/***************************************************************************//**
 * @file
 * @brief Program sequences for the Range Test Software Example.
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

#ifndef SEQ_H_
#define SEQ_H_

// ----------------------------------------------------------------------------
// Type definitions

// Function pointer types
typedef bool (*seqInputHandler_t)(pbState_t *);
typedef void (*seqDisplayHandler_t)(GLIB_Context_t *);
typedef bool (*seqRunner_t)();

// Kinds of Range Test program states
enum {
  SEQ_INIT,
  SEQ_MENU,
  SEQ_TRX,
};

// Sequences each Range Test Mode has in terms of what kinds of functions
// have to be called in each Mode
typedef struct {
  seqInputHandler_t input;
  seqDisplayHandler_t display;
  seqRunner_t runner;
} seqHandler_t;

// Sequence handlers arranged in a state machine-like variable.
extern seqHandler_t seqHandler;

/**************************************************************************//**
 * Initializes the sequence of the Range Test program execution
 *****************************************************************************/
void seqInit(void);

/**************************************************************************//**
 * Sequence execution of the Range Test based on the sequence handler state
 * machine.
 *****************************************************************************/
void seqSet(uint8_t newIdx);

/**************************************************************************//**
 * Changes the mode of operation into to another one.
 *****************************************************************************/
void seqRun();

#endif /* SEQ_H_ */
