/***************************************************************************//**
 * @file
 * @brief Giant Gecko energy mode setups (See Data Sheet Table 3.4), header file
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

#ifndef EMODES_H
#define EMODES_H

#include "em_cmu.h"

/* Enter energy mode functions */
void em_EM0_Hfxo(void);

void em_EM0_Hfrco(CMU_HFRCOBand_TypeDef band);

void em_EM1_Hfxo(void);

void em_EM1_Hfrco(CMU_HFRCOBand_TypeDef band);

void em_EM2_LfrcoRTC(void);

void em_EM3(void);

void em_EM4(void);

#endif // EMODES_H
