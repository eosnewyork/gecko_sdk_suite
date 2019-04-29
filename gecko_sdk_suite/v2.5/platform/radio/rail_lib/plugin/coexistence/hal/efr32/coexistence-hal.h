/***************************************************************************//**
 * @file
 * @brief This file contains the EFR32 radio coexistence interface.
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

#ifndef __COEXISTENCE_HAL_H__
#define __COEXISTENCE_HAL_H__

#ifdef HAL_CONFIG
  #include "hal-config.h"
#endif // HAL_CONFIG
#include "coexistence/common/coexistence.h"
#include "em_gpio.h"
#include "gpiointerrupt.h"

typedef struct COEX_HAL_GpioConfig {
  /** GPIO port */
  uint8_t port;

  /** GPIO pin */
  uint8_t pin;

  /** GPIO interrupt number */
  uint8_t intNo;

  /** GPIO assert polarity */
  bool polarity;

  /** GPIO PWM enabled */
  bool pwmEnabled;

  /** GPIO mode */
  uint8_t mode;

  /** GPIO ISR */
  GPIOINT_IrqCallbackPtr_t isr;

  /** GPIO config */
  COEX_GpioConfig_t config;
} COEX_HAL_GpioConfig_t;

void COEX_HAL_Init(void);
bool COEX_HAL_ConfigRequest(COEX_HAL_GpioConfig_t *gpioConfig);
bool COEX_HAL_ConfigPwmRequest(COEX_HAL_GpioConfig_t *gpioConfig);
bool COEX_HAL_ConfigRadioHoldOff(COEX_HAL_GpioConfig_t *gpioConfig);
bool COEX_HAL_ConfigPriority(COEX_HAL_GpioConfig_t *gpioConfig);
bool COEX_HAL_ConfigGrant(COEX_HAL_GpioConfig_t *gpioConfig);
bool COEX_HAL_ConfigDp(uint8_t pulseWidthUs);
uint8_t COEX_HAL_GetDpPulseWidth(void);
bool COEX_HAL_SetDpPulseWidth(uint8_t pulseWidthUs);

#if defined(BSP_COEX_GNT_PIN) && !defined(BSP_COEX_GNT_INTNO)
#define BSP_COEX_GNT_INTNO BSP_COEX_GNT_PIN
#endif //defined(BSP_COEX_GNT_PIN) && !defined(BSP_COEX_GNT_INTNO)

#if defined(BSP_COEX_PRI_PIN) && !defined(BSP_COEX_PRI_INTNO)
#define BSP_COEX_PRI_INTNO BSP_COEX_PRI_PIN
#endif //defined(BSP_COEX_PRI_PIN) && !defined(BSP_COEX_PRI_INTNO)

#if defined(BSP_COEX_PWM_REQ_PIN) && !defined(BSP_COEX_PWM_REQ_INTNO)
#define BSP_COEX_PWM_REQ_INTNO BSP_COEX_PWM_REQ_PIN
#endif //defined(BSP_COEX_PWM_REQ_PIN) && !defined(BSP_COEX_PWM_REQ_INTNO)

#if defined(BSP_COEX_REQ_PIN) && !defined(BSP_COEX_REQ_INTNO)
#define BSP_COEX_REQ_INTNO BSP_COEX_REQ_PIN
#endif //defined(BSP_COEX_REQ_PIN) && !defined(BSP_COEX_REQ_INTNO)

#if defined(BSP_COEX_RHO_PIN) && !defined(BSP_COEX_RHO_INTNO)
#define BSP_COEX_RHO_INTNO BSP_COEX_RHO_PIN
#endif //defined(BSP_COEX_RHO_PIN) && !defined(BSP_COEX_RHO_INTNO)

#if defined(PER_REG_BLOCK_SET_OFFSET)
#define COEX_HAL_GPIO_ADDR(port, polarity) ((uint32_t)&GPIO->P[port].DOUT \
                                            + (polarity                   \
                                               ? PER_REG_BLOCK_SET_OFFSET \
                                               : PER_REG_BLOCK_CLR_OFFSET))

#elif defined(PER_BITSET_MEM_BASE)
#define COEX_HAL_GPIO_ADDR(port, polarity) ((uint32_t)&GPIO->P[port].DOUT           \
                                            - PER_MEM_BASE + (polarity              \
                                                              ? PER_BITSET_MEM_BASE \
                                                              : PER_BITCLR_MEM_BASE))

#endif //defined(PER_REG_BLOCK_SET_OFFSET) || defined(PER_BITSET_MEM_BASE)

#if defined(BSP_COEX_REQ_PORT) && defined(BSP_COEX_PRI_PORT) && defined(COEX_HAL_GPIO_ADDR)
#define COEX_HAL_FAST_REQUEST 1
#define COEX_HAL_ReadGpio(port,                  \
                          pin,                   \
                          polarity)              \
  (!!GPIO_PinInGet((GPIO_Port_TypeDef)port, pin) \
   == !!polarity)

#define COEX_HAL_SetGpio(port,                                              \
                         pin,                                               \
                         polarity)                                          \
  do {                                                                      \
    *((volatile uint32_t *)COEX_HAL_GPIO_ADDR(port, polarity)) = 1U << pin; \
  } while (0)

#define COEX_HAL_ReadRequest()         \
  COEX_HAL_ReadGpio(BSP_COEX_REQ_PORT, \
                    BSP_COEX_REQ_PIN,  \
                    BSP_COEX_REQ_ASSERT_LEVEL)
#ifdef BSP_COEX_PWM_REQ_PORT
#define COEX_HAL_SetPwmRequest()          \
  COEX_HAL_SetGpio(BSP_COEX_PWM_REQ_PORT, \
                   BSP_COEX_PWM_REQ_PIN,  \
                   BSP_COEX_PWM_REQ_ASSERT_LEVEL)
#define COEX_HAL_ClearPwmRequest()        \
  COEX_HAL_SetGpio(BSP_COEX_PWM_REQ_PORT, \
                   BSP_COEX_PWM_REQ_PIN,  \
                   !BSP_COEX_PWM_REQ_ASSERT_LEVEL)
#else //!BSP_COEX_PWM_REQ_PORT
#define COEX_HAL_SetPwmRequest() //no-op
#define COEX_HAL_ClearPwmRequest() //no-op
#endif //BSP_COEX_PWM_REQ_PORT

#define COEX_HAL_SetRequest()         \
  COEX_HAL_SetGpio(BSP_COEX_REQ_PORT, \
                   BSP_COEX_REQ_PIN,  \
                   BSP_COEX_REQ_ASSERT_LEVEL)
#define COEX_HAL_ClearRequest()       \
  COEX_HAL_SetGpio(BSP_COEX_REQ_PORT, \
                   BSP_COEX_REQ_PIN,  \
                   !BSP_COEX_REQ_ASSERT_LEVEL)
#define COEX_HAL_SetPriority()        \
  COEX_HAL_SetGpio(BSP_COEX_PRI_PORT, \
                   BSP_COEX_PRI_PIN,  \
                   BSP_COEX_PRI_ASSERT_LEVEL)
#define COEX_HAL_ClearPriority()      \
  COEX_HAL_SetGpio(BSP_COEX_PRI_PORT, \
                   BSP_COEX_PRI_PIN,  \
                   !BSP_COEX_PRI_ASSERT_LEVEL)

#endif //defined(BSP_COEX_REQ_PORT) && defined(BSP_COEX_PRI_PORT) && defined(COEX_HAL_GPIO_ADDR)
#endif  // __COEXISTENCE_HAL_H__
