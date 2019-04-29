/***************************************************************************//**
 * @file
 * @brief I2C simple poll-based master mode driver for the DK/STK.
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

#include <stddef.h>
#include "em_cmu.h"
#include "em_gpio.h"
#include "i2cspmconfig.h"
#include "i2cspm.h"
#include "em_assert.h"
#include "em_emu.h"
#include "em_core.h"

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Initalize I2C peripheral
 *
 * @details
 *   This driver supports master mode only, single bus-master. In addition
 *   to configuring the I2C peripheral module, it also configures DK/STK
 *   specific setup in order to use the I2C bus.
 *
 * @param[in] init
 *   Pointer to I2C initialization structure
 ******************************************************************************/
void I2CSPM_Init(I2CSPM_Init_TypeDef *init)
{
  int i;
  CMU_Clock_TypeDef i2cClock;
  I2C_Init_TypeDef i2cInit;

  EFM_ASSERT(init != NULL);

  CMU_ClockEnable(cmuClock_HFPER, true);

  /* Select I2C peripheral clock */
  if (false) {
#if defined(I2C0)
  } else if (init->port == I2C0) {
    i2cClock = cmuClock_I2C0;
#endif
#if defined(I2C1)
  } else if (init->port == I2C1) {
    i2cClock = cmuClock_I2C1;
#endif
  } else {
    /* I2C clock is not defined */
    EFM_ASSERT(false);
    return;
  }
  CMU_ClockEnable(i2cClock, true);

  /* Output value must be set to 1 to not drive lines low. Set
     SCL first, to ensure it is high before changing SDA. */
  GPIO_PinModeSet(init->sclPort, init->sclPin, gpioModeWiredAndPullUp, 1);
  GPIO_PinModeSet(init->sdaPort, init->sdaPin, gpioModeWiredAndPullUp, 1);

  /* In some situations, after a reset during an I2C transfer, the slave
     device may be left in an unknown state. Send 9 clock pulses to
     set slave in a defined state. */
  for (i = 0; i < 9; i++) {
    GPIO_PinOutSet(init->sclPort, init->sclPin);
    GPIO_PinOutClear(init->sclPort, init->sclPin);
  }

  /* Enable pins and set location */
  init->port->ROUTE = I2C_ROUTE_SDAPEN
                      | I2C_ROUTE_SCLPEN
                      | (init->portLocation << _I2C_ROUTE_LOCATION_SHIFT);

  /* Set emlib init parameters */
  i2cInit.enable = true;
  i2cInit.master = true; /* master mode only */
  i2cInit.freq = init->i2cMaxFreq;
  i2cInit.refFreq = init->i2cRefFreq;
  i2cInit.clhr = init->i2cClhr;

  I2C_Init(init->port, &i2cInit);
}

static volatile I2C_TransferReturn_TypeDef transferStatus;

void I2C0_IRQHandler(void)
{
  transferStatus = I2C_Transfer(I2C0);
}

/***************************************************************************//**
 * @brief
 *   Perform I2C transfer
 *
 * @details
 *   This driver only supports master mode, single bus-master. It does not
 *   return until the transfer is complete, polling for completion.
 *
 * @param[in] i2c
 *   Pointer to the peripheral port (we only allow I2C0 for now)
 *
 * @param[in] seq
 *   Pointer to sequence structure defining the I2C transfer to take place. The
 *   referenced structure must exist until the transfer has fully completed.
 ******************************************************************************/
I2C_TransferReturn_TypeDef I2CSPM_Transfer(I2C_TypeDef *i2c, I2C_TransferSeq_TypeDef *seq)
{
  (void) i2c;
  uint32_t timeout = I2CSPM_TRANSFER_TIMEOUT;
  bool done = false;
  /* Do an interrupt driven transfer */
  NVIC_EnableIRQ(I2C0_IRQn);
  transferStatus = I2C_TransferInit(I2C0, seq);

  while ( !done ) {
    CORE_ATOMIC_SECTION(
      if ( transferStatus == i2cTransferInProgress && timeout-- ) {
      EMU_EnterEM1();
    } else {
      done = true;
    }
      )
  }
  NVIC_DisableIRQ(I2C0_IRQn);
  return transferStatus;
}
