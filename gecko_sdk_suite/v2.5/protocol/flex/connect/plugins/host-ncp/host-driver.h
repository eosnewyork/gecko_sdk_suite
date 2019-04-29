/***************************************************************************//**
 * @brief host-side driver for the IP NCP.
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

// Handle messages received from the NCP.

void ncpIncomingMessageHandler(SerialLinkMessageType type,
                               const uint8_t *message,
                               uint16_t length);

// Send messages to the NCP or management client.  This callback is
// implemented in host-driver-app.c.

extern bool logEnabled;
extern int ncpFileDescriptor;

uint16_t ncpWrite(int fd,
                  SerialLinkMessageType type,
                  const uint8_t *buffer,
                  uint16_t length);

extern Stream ncpStream;

void managementHandler(SerialLinkMessageType type,
                       const uint8_t *message,
                       uint16_t length);

void ncpMessageHandler(SerialLinkMessageType type,
                       const uint8_t *message,
                       uint16_t length);

ncpReadStatus readNcpAshInput
  (int fd, ncpMsgHandler messageHandler);

void txBufferFullHandler(const uint8_t *packet,
                         uint16_t packetLength,
                         uint16_t written);

void txFailedHandler(uint8_t fd,
                     const uint8_t *packet,
                     uint16_t packetLength,
                     uint16_t written);

void halHostSerialTick(void);
#ifdef EMBER_TEST
void halHostSerialInit(void);
#else
void halHostSerialInit(int argc, char **argv);
#endif
