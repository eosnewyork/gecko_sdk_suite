/***************************************************************************//**
 * @file
 * @brief
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

#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <stddef.h>

#include "xbee/platform.h"
#include "xbee/device.h"
#include "xbee/ipv4.h"
#include "xbee/atcmd.h"
#include "xbee/byteorder.h"
#include "xbee/tx_status.h"

typedef enum {
  GET,
  POST,
  PUT,
  DELETE,
  HEAD
} HttpRequest_t;

/* All content uses charset=ISO-8859-1 */
typedef enum {
  HTTP_CONTENT_UNKNOWN, /* ??? */
  HTTP_CONTENT_JSON, /* application/json */
  HTTP_CONTENT_HTML, /* text/html */
  HTTP_CONTENT_XML   /* text/xml */
} HttpContent_t;

typedef struct {
  HttpContent_t type;   /* the type of data in body (see enum) */
  int bufLength;        /* Amount of space allocated for receiving the body */
  int contentLength;    /* Bytes in the body */
  char *buf;            /* Contents of the body */
  int status;
} HttpDataIn_t;

typedef struct {
  HttpContent_t type;   /* the type of data in body (see enum) */
  int contentLength;    /* Bytes in the body */
  char *buf;            /* Contents of the body */
} HttpDataOut_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
   @brief Sends an AI command in order to refresh connection status
   @param[in]   xbee     A pointer to the XBee device object
 */
void checkConnection(xbee_dev_t *xbee);

/**
   @brief sends a data point to a specified data stream

   @param[in]   data     A string that represents the data in ascii
   @param[in]   streamID A string that contains the streamID
   @param[in]   type     A string that contains the type of the data
   @param[in]   pos      String contains GNSS data of form "[lon,lat,height]"
   @param[in]   timeout  The timeout in seconds

   @retval 0          Success
   @retval -EINVAL    Invalid parameter passed to function
   @retval -ETIMEDOUT A send or receive call has timed out
   @retval -EBUSY     The XBee is busy and returned a tx status of 0x32
   @retval -EIO       While receiving, we got an invalid response
 */
int sendDataToCloud(char *data,
                    const char *streamID,
                    const char *type,
                    char * pos,
                    int timeout);

/**
   @brief receives a data point from a specified data stream

   @param[out]  data     A string that represents the data in ascii
   @param[in]   streamID A string that contains the streamID
   @param[in]   timeout  The timeout in seconds

   @retval 0          Success
   @retval -EINVAL    Invalid parameter passed to function
   @retval -ETIMEDOUT A send or receive call has timed out
   @retval -EBUSY     The XBee is busy and returned a tx status of 0x32
   @retval -EIO       While receiving, we got an invalid response
 */
int getDataFromCloud(char *data, const char *streamID, int timeout);

/**
   @brief create data stream for a specified stream ID

   @param[in]   streamID A string that contains the streamID
   @param[in]   type     A string that contains the type of the data
   @param[in]   units    A string containing the name of the units used
   @param[in]   timeout  The timeout in seconds

   @retval 0          Success
   @retval -EINVAL    Invalid parameter passed to function
   @retval -ETIMEDOUT A send or receive call has timed out
   @retval -EBUSY     The XBee is busy and returned a tx status of 0x32
   @retval -EIO       While receiving, we got an invalid response
 */
int initCloudDataStream(const char *streamID,
                        const char *type,
                        const char* units,
                        int timeout);

/**
   @brief Get the host IP address
   @return the host name string
 */
uint32_t httpGetIpadr(void);

/**
   @brief Get the host name
   @return the host name string
 */
char * httpGetHostName(void);

/**
   @brief Get the number of failed send attempts
   @return The number of failed send attempts
 */
int getConsecutiveFailures(void);

/**
   @brief Get the last transmit status received
   @return the last transmit status received
 */
int getLastTXStatus(void);

/**
   @brief Get the remote managers FQDN from the XBee and resolve it.
         This fills in the ipv4 envelope that is in static memory

   @param[in]   xbee   A pointer to the xbee device to set in the envelope
   @param[in]   secure A boolean indicating if SSL should be used

   @retval 0          Success
   @retval -EINVAL    Invalid parameter passed to function

   @note This must be called before http_client_connect
 */
int httpClientInitConnection(xbee_dev_t *xbee, bool_t secure);

/**
   @brief Send an http request and receive a response

   @param[in]  env       A pointer to the ipv4 enevelope to fill
   @param[in]  type     The type of http request (GET, PUT ...)
   @param[in]  hostName The URL that you are requesting from
   @param[in]  path     The path to the resource requesting
   @param[in]  dataOut  A struct containing the information to send
   @param[out] dataIn   An allocated struct to receive the response in
   @param[in]  timeout  Time in seconds before this function times out (pass a negative int for no timeout)

   @retval 0          Success
   @retval -EINVAL    Invalid parameter passed or passed buffer too small
   @retval -ETIMEDOUT A send or receive call has timed out
   @retval -EBUSY     The XBee is busy and returned a tx status of 0x32
   @retval -EIO       While receiving, we got an invalid response
   @note use http_client_initConnection(...) to initialize env and get hostname
 */
int httpClientConnect(xbee_ipv4_envelope_t *env,
                      HttpRequest_t type,
                      const char *hostName,
                      const char *path,
                      HttpDataOut_t *dataOut,
                      HttpDataIn_t *dataIn,
                      int timeout);

/**
   @brief Encodes input to base 64 and then sets output length

   @param[in]      input        A pointer to the buffer to be encoded
   @param[in]      inputLength  The length of the buffer to be encoded
   @param[out]     output       An allocated buffer, output of encoding
   @param[in,out]  outputLen    The length of output buffer, after completion,
                                 this is changed to be the length of
                                 encoded character array

   @retval 0          Success
   @retval -EINVAL    Invalid parameter passed or passed buffer too small
   @note Used for encoding authorization
 */
int base64Enc(const char *input,
              int inputLength,
              char *output,
              int *outputLen);
#ifdef __cplusplus
}
#endif

#endif /* HTTP_CLIENT_H */
