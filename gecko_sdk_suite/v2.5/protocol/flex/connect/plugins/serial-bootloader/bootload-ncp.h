/***************************************************************************//**
 * @brief SPI/UART Interface to bootloading the NCP from a connected host.
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

typedef enum {
  SHUTDOWN_NOT_REQUESTED, // this is the normal state when the app is running
  SHUTDOWN_REQUESTED,     // after Bootloader has been launched
  SHUTDOWN_DONE,          // after NCP connection has been closed
} ShutDownState;

bool emberBootloadImage(const char* filepath, uint32_t offset, uint32_t length);
bool emStartNcpBootloaderCommunications(void);
void emPostNcpBootload(bool success);
bool emRebootNcpAfterBootload(void);

// These primitives are called by the Xmodem code to pass data via
// the specific mechanism (UART or SPI).
bool emBootloadSendData(const uint8_t *data, uint16_t length);
bool emBootloadSendByte(uint8_t byte);
bool emBootloadWaitChar(uint8_t *data, bool expect, uint8_t expected);
