/***************************************************************************//**
 * @brief Functions to test the Host and NCP setup.
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

/**
 * Test command. Echoes data to the NCP.
 */
void emberEcho(uint8_t *data, uint8_t length);
void emberEchoReturn(uint8_t *data, uint8_t length);

/**
 * Test command. Software flow control.
 */
void emberStartXOnXOffTest(void);
