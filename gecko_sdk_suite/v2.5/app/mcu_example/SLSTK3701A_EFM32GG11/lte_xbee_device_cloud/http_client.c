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

#include "http_client.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#ifndef USER
#error "You must define your username for the remote manager"
#endif
#ifndef PWD
#error "You must define your password for the remote manager"
#endif

#define BASE_64_MASK              0x3F /* Masks the bottom 6 bits for encoding base64 */
#define REQUEST_SIZE              512 /* Bytes allocated for http send buffer */
#define HTTP_BUFFER_LEN           512
#define HTTP_AUTHENTICATION_SIZE  128
#define MAX_URL_SIZE              64
#define CREDENTIALS               (USER ":" PWD)

static char sendBuff[HTTP_BUFFER_LEN];
static char recvBuff[HTTP_BUFFER_LEN];
static xbee_ipv4_envelope_t env;
static char hostName[MAX_URL_SIZE];
static char buff[REQUEST_SIZE];
static int failures = 0;
static int txStat = 0xFF;

static int credLen = HTTP_AUTHENTICATION_SIZE;
static char credentials[HTTP_AUTHENTICATION_SIZE] = { 0 };

/* Contains necessary data for state machines */
static struct HttpReceiveContext {
  HttpDataIn_t *thisRepsonse;
  enum {
    HTTP_RECEIVE_NONE,   /* No data received yet */
    HTTP_RECEIVE_HEADER, /* We are parsing headers */
    HTTP_RECEIVE_BODY,   /* We are reading in the body */
    HTTP_RECEIVE_DONE,   /* We have completed reading everything */
    HTTP_RECEIVE_ERROR
  } state;
  int bodyLenRead;
} recvContext;

/* Contains necessary data for state machines */
static struct HttpSendContext {
  enum {
    HTTP_SEND_NONE,   /* No data sent yet */
    HTTP_SEND_WAIT,   /* Data has been sent, waiting for confirmation */
    HTTP_SEND_RESEND, /* We did not receive confirmation, resend */
    HTTP_SEND_DONE,   /* Confirmation received, send complete */
    HTTP_SEND_ERROR
  } state;
  uint8_t frame_id;
} sendContext;

/* Local prototypes */
int statusHandler(xbee_dev_t *xbee,
                  const void FAR *raw,
                  uint16_t length,
                  void FAR *context);
int ipv4ReceiveHandler(xbee_dev_t *xbee,
                       const void FAR *raw,
                       uint16_t length,
                       void FAR *context);

/* Define our handlers in the dispatch table */
#define FRAME_HANDLE_TX_STATUS \
  { XBEE_FRAME_TX_STATUS, 0, statusHandler, NULL }

#define FRAME_HANDLE_IPV4 \
  { XBEE_FRAME_RECEIVE_IPV4, 0, ipv4ReceiveHandler, NULL }

/* Table used to dispatch received frames */
const xbee_dispatch_table_entry_t xbee_frame_handlers[] =
{
  XBEE_FRAME_HANDLE_LOCAL_AT,
  FRAME_HANDLE_IPV4,
  FRAME_HANDLE_TX_STATUS,
  XBEE_FRAME_TABLE_END
};

/***************************************************************************//**
 * @brief Null terminates headers at \r and then returns offset of next header
 ******************************************************************************/
int headerToString(uint8_t *temp, int payloadLen)
{
  int i;
  for (i = 0; i < payloadLen; i++) {
    /* Replace the first instance of a CR with null termination */
    if (temp[i] == '\r') {
      temp[i] = '\0';
      /* return after each \r */
      return i + 2;
    }
  }
  return 0;
}

/***************************************************************************//**
 * @brief Checks the header string passed for the fields we need
 ******************************************************************************/
void processHeader(uint8_t *startHeader)
{
  char type[64];
  char *temp;
  /* Get the Content Length */
  sscanf((char *) startHeader, "Content-Length: %i", &recvContext.thisRepsonse->contentLength);

  /* Set the content type and the charset used */
  if (sscanf((char *) startHeader, "Content-Type: %s", type) == 1) {
    temp = strchr(type, ';');
    if (temp != NULL) {
      *temp = '\0';
      temp += 1;
    }

    if (strstr(temp, "charset=ISO-8859-1") == NULL) {
      recvContext.thisRepsonse->type = HTTP_CONTENT_UNKNOWN;
      #ifdef HTTP_CLIENT_VERBOSE
      printf("\f ERROR unsupported charset %s", temp);
      #endif
      return;
    }

    if (strstr(type, "application/json") != NULL) {
      recvContext.thisRepsonse->type = HTTP_CONTENT_JSON;
    } else if (strstr(type, "text/html") != NULL) {
      recvContext.thisRepsonse->type = HTTP_CONTENT_HTML;
    } else if (strstr(type, "text/xml") != NULL) {
      recvContext.thisRepsonse->type = HTTP_CONTENT_XML;
    } else {
      recvContext.thisRepsonse->type = HTTP_CONTENT_UNKNOWN;
    }
  }
}

/***************************************************************************//**
 * @brief Used to confirm that the request was sent successfully
 ******************************************************************************/
int statusHandler(xbee_dev_t *xbee,
                  const void FAR *raw,
                  uint16_t length,
                  void FAR *context)
{
  (void) xbee;
  (void) length;
  (void) context;

  const xbee_frame_tx_status_t *status = raw;
  /* If we are waiting for a status */
  if (sendContext.state == HTTP_SEND_WAIT) {
    /* If this is the status we are looking for */
    if (sendContext.frame_id == status->frame_id) {
      txStat = status->delivery;
      if (status->delivery == XBEE_TX_DELIVERY_SUCCESS) {
        /* If the send was successful, we are done */
        sendContext.state = HTTP_SEND_DONE;
      } else {
        /* If the send was not successful, resend that packet */
        sendContext.state = HTTP_SEND_RESEND;
      }
    }
  }
  return 0;
}

/***************************************************************************//**
 * @brief Callback for response from server, uses state machine to parse data
 ******************************************************************************/
int ipv4ReceiveHandler(xbee_dev_t *xbee,
                       const void FAR *raw,
                       uint16_t length,
                       void FAR *context)
{
  (void) xbee;
  (void) context;

  const xbee_frame_receive_ipv4_t *ipv4 = raw;
  uint8_t *startHeader = (uint8_t *) ipv4->payload;
  uint8_t *temp = startHeader;
  int ret;
  int payloadLen = length - offsetof(xbee_frame_receive_ipv4_t, payload);

  if (payloadLen > 0) {
    switch (recvContext.state) {
      /* Receive the first line/status code */
      case HTTP_RECEIVE_NONE:
        ret = headerToString(temp, payloadLen);

        if (ret == 0) {
          recvContext.state = HTTP_RECEIVE_ERROR; // Could not find CRLF ERROR
          #ifdef HTTP_CLIENT_VERBOSE
          printf("\033[H" "\nNo CRLF in receiving first line\nDrop update\n");
          #endif
          break;
        } else {
          temp += ret;
        }

        payloadLen -= (temp - startHeader);
        /* Actually read the status code */
        if ( sscanf((char *) startHeader, "HTTP/1.1 %i", &(recvContext.thisRepsonse->status)) != 1 ) {
          /* We did not read the expected data */
          #ifdef HTTP_CLIENT_VERBOSE
          printf("\033[H" "\nError reading initial header\n");
          #endif
          recvContext.state = HTTP_RECEIVE_ERROR;
          break;
        } else {
          /* success in reading status */
          recvContext.state = HTTP_RECEIVE_HEADER;
          if (payloadLen > 0) {
            startHeader = temp;
          } else {
            break;
          }
        }
      // fallthrough

      /* Receiving all headers */
      case HTTP_RECEIVE_HEADER:

        while (payloadLen > 0 ) {
          ret = headerToString(temp, payloadLen);
          if (ret == 0) {
            #ifdef HTTP_CLIENT_VERBOSE
            printf("\033[H" "\nHeader split mid packet\n"); // Could not find \r\n drop this one and move on
            #endif
            break;
          } else {
            temp += ret;
          }

          payloadLen -= (temp - startHeader);

          /* If this header is just \r\n then we have moved onto the body */
          if (startHeader[0] == '\0') {
            recvContext.state = HTTP_RECEIVE_BODY;
            startHeader = temp;
            break;
          }

          /* Do header searching here */
          processHeader(startHeader);
          /* Move onto the next header */
          startHeader = temp;
        }
      // fallthrough

      /* Receive the body of the response */
      case HTTP_RECEIVE_BODY:

        /* If we have room in the buffer */
        if (recvContext.thisRepsonse->bufLength >= recvContext.bodyLenRead + payloadLen) {
          memcpy((recvContext.thisRepsonse->buf + recvContext.bodyLenRead), startHeader, payloadLen);
          recvContext.bodyLenRead += payloadLen;
        } else {
          recvContext.state = HTTP_RECEIVE_ERROR; // Buffer overflow
          #ifdef HTTP_CLIENT_VERBOSE
          printf("\033[H" "\nNot enough buffer space to copy body into\n");
          #endif
        }

        /* We have read all that we need to */
        if (recvContext.bodyLenRead >= recvContext.thisRepsonse->contentLength) {
          recvContext.state = HTTP_RECEIVE_DONE;
        }
        break;
      case HTTP_RECEIVE_DONE:
        break;
      case HTTP_RECEIVE_ERROR:
        break;
      default:
        #ifdef HTTP_CLIENT_VERBOSE
        printf("\fERROR IN RECEIVE STATE MACHINE");
        #endif
        recvContext.state = HTTP_RECEIVE_ERROR;
    }
  }
  return 0;
}

/***************************************************************************//**
 * @brief Callback for LA command for resolving a fully qualified domain name
 ******************************************************************************/
int resolveFQDNCallback(const xbee_cmd_response_t FAR *response)
{
  if (((response->flags & XBEE_CMD_RESP_MASK_STATUS) == XBEE_AT_RESP_SUCCESS)
      && response->value_length == 4) { /* Only copy if its a valid response */
    /* Copy the big-endian address */
    memcpy(response->context, response->value_bytes, 4);
  }
  if ((response->flags & XBEE_CMD_RESP_MASK_STATUS) == XBEE_AT_RESP_ERROR) {
    /* Not a valid FQDN */
    printf("\fError w\\ domain name:\n"
           "Be sure that xbee's \n"
           "EQ field is device\n"
           "clouds FQDN");
  }
  return 0;
}

/***************************************************************************//**
 * @brief Callback for EQ command to get remote manager's URL
 ******************************************************************************/
int getCloudFQDNCallback(const xbee_cmd_response_t FAR *response)
{
  /* Only copy if its a valid response */
  if (((response->flags & XBEE_CMD_RESP_MASK_STATUS) == XBEE_AT_RESP_SUCCESS)
      && response->value_length > 0 && response->value_length < 64) {
    /* The context is a pointer to the host name string to copy into */
    char *context = (char *) response->context;
    memcpy(context, response->value_bytes, response->value_length);
    context[response->value_length] = 0; //Null terminate fqdn
  }
  return 0;
}

/***************************************************************************//**
 * @brief If XBeee sends an AI response this updates the connection status
 * @param response The AT command response from AI issued by checkConnection()
 * @note This is only called the the host library
 ******************************************************************************/
int xbeeConnectCallback(const xbee_cmd_response_t FAR *response)
{
  xbee_dev_t *xbee = response->context;
  if (((response->flags & XBEE_CMD_RESP_MASK_STATUS) == XBEE_AT_RESP_SUCCESS)
      && response->value_length == 1) {
    if (xbee == NULL) {
      return -EINVAL;
    }
    if (response->value_bytes[0] == 0) {
      xbee->wpan_dev.flags |= WPAN_FLAG_JOINED;
    } else {
      xbee->wpan_dev.flags &= ~(WPAN_FLAG_JOINED);
    }
  } else if (response->flags & XBEE_CMD_RESP_FLAG_TIMEOUT) {
    /* The XBee is not responding, so we are essentially disconnected */
    xbee->wpan_dev.flags &= ~(WPAN_FLAG_JOINED);
  }
  return 0;
}

void sendAtRequest(xbee_dev_t *xbee,
                   const char *command,
                   const char *parameters,
                   xbee_cmd_callback_fn callback,
                   void FAR *context)
{
  int16_t request;
  RTOS_ERR  err;
  request = xbee_cmd_create(xbee, command);

  if (request > 0) {
    xbee_cmd_set_callback(request, callback, context);
    if (parameters != NULL) {
      xbee_cmd_set_param_bytes(request, parameters, strlen(parameters));
    }
    xbee_cmd_send(request);
    xbee_cmd_request_t *thisReq = _xbee_cmd_handle_to_address(request);
    /* Checks if the command has responded or timed-out */
    while (thisReq->device != NULL) {
      xbee_dev_tick(xbee);
      xbee_cmd_tick();
      OSTimeDly(50, OS_OPT_TIME_DLY, &err);
    }
  }
}

/***************************************************************************//**
 * @brief Blocks, Sends request to the XBee using LA command to resolve a FQDN
 ******************************************************************************/
uint32_t resolveFQDN(xbee_dev_t *xbee, char *fqdnStr)
{
  uint32_t ipBe = 0;

  while (!ipBe) {
    sendAtRequest(xbee, "LA", fqdnStr, resolveFQDNCallback, &ipBe);
    printf(".");
  }
  return ipBe;
}

/***************************************************************************//**
 * @brief Sends out an AI command to update connection status
 ******************************************************************************/
void checkConnection(xbee_dev_t *xbee)
{
  sendAtRequest(xbee, "AI", NULL, xbeeConnectCallback, NULL);
}

/***************************************************************************//**
 * @brief Blocks, Sends request to XBee using EQ to get configured cloud FQDN
 ******************************************************************************/
int getCloudFQDN(xbee_dev_t *xbee, char * buff, size_t buffLen)
{
  if (buff == NULL || buffLen > MAX_URL_SIZE) {
    return -EINVAL;
  }
  sendAtRequest(xbee, "EQ", NULL, getCloudFQDNCallback, buff);

  /* Check if we received back a remote manager address */
  if (buff[0] == 0) {
    return -EIO;
  }
  return 0;
}

/***************************************************************************//**
 * @brief Blocks, Sends ipv4 packet. This model is much slower, but does not
 *        require any buffering to resend a packet. Overall more reliable
 ******************************************************************************/
int httpClientSend(xbee_ipv4_envelope_t *env,
                   const char *buf,
                   size_t bufLen,
                   int options,
                   int timeout)
{
  int ret, temp;

  env->length = bufLen;
  env->payload = buf;
  env->options = options;
  sendContext.state = HTTP_SEND_WAIT;
  txStat = 0xFF;

  /* Store the next frame ID in context (without incrementing it) */
  temp = env->xbee->frame_id;
  sendContext.frame_id = xbee_next_frame_id(env->xbee);
  env->xbee->frame_id = temp;

  ret = xbee_ipv4_envelope_send(env);

  while (sendContext.state != HTTP_SEND_DONE) {
    while (sendContext.state == HTTP_SEND_WAIT) {
      if (ret != 0) {
        sendContext.state = HTTP_SEND_ERROR;
        ++failures;
        return ret;
      }
      xbee_dev_tick(env->xbee);

      if (xbee_seconds_timer() >= (uint32_t) timeout && timeout >= 0) {
        sendContext.state = HTTP_SEND_ERROR;
        return -ETIMEDOUT;
      }
    }
    if (sendContext.state == HTTP_SEND_DONE) {
      failures = 0;
      break;
    }
    if (txStat == 0x32) {
      sendContext.state = HTTP_SEND_ERROR;
      ++failures;
      return -EBUSY;
    }
    sendContext.state = HTTP_SEND_WAIT;

    /* Store the next frame ID in context (without incrementing it) */
    temp = env->xbee->frame_id;
    sendContext.frame_id = xbee_next_frame_id(env->xbee);
    env->xbee->frame_id = temp;

    ret = xbee_ipv4_envelope_send(env);
  }
  return 0;
}

/***************************************************************************//**
 * @brief Blocks, waits for incoming/complete http response, has a timeout
 ******************************************************************************/
int httpClientRecv(xbee_ipv4_envelope_t *env, int timeout)
{
  /* If we don't care about response, then skip */
  if (recvContext.thisRepsonse == NULL) {
    recvContext.state = HTTP_RECEIVE_DONE;
    return 0;
  }

  /* Init to empty state */
  recvContext.thisRepsonse->status = 0;
  recvContext.thisRepsonse->type = HTTP_CONTENT_UNKNOWN;
  recvContext.thisRepsonse->contentLength = 0;
  recvContext.bodyLenRead = 0;
  recvContext.state = HTTP_RECEIVE_NONE;

  while (recvContext.state != HTTP_RECEIVE_DONE) {
    /* Check first for timeout */
    if (xbee_seconds_timer() >= (uint32_t) timeout && timeout >= 0) {
      recvContext.state = HTTP_RECEIVE_ERROR;
      recvContext.thisRepsonse->contentLength = 0;
      recvContext.thisRepsonse->type = HTTP_CONTENT_UNKNOWN;
      return -ETIMEDOUT;
      /* Next check for error on receive */
    } else if (recvContext.state == HTTP_RECEIVE_ERROR) {
      recvContext.thisRepsonse->type = HTTP_CONTENT_UNKNOWN;
      recvContext.thisRepsonse->contentLength = 0;
      return -EIO;
    } else {
      xbee_dev_tick(env->xbee);
    }
  }
  return 0;
}

/***************************************************************************//**
 * @brief Properly encodes the authorization field using base64
 ******************************************************************************/
int httpClientCreateAuth(void)
{
  int len = strlen(CREDENTIALS);
  if (len > credLen) {
    return -EINVAL;
  }
  return base64Enc(CREDENTIALS, len, credentials, &credLen);
}

/***************************************************************************//**
 * @brief Fills env with the needed connection data and gets the hostname
 ******************************************************************************/
int httpClientInitConnection(xbee_dev_t *xbee, bool_t secure)
{
  int ret;

  env.xbee = xbee;

  hostName[0] = 0; //Null terminate
  ret = getCloudFQDN(env.xbee, hostName, MAX_URL_SIZE);

  if (ret < 0) {
    return ret;
  }

  env.local_port = 0;

  if (secure) {
    env.protocol = XBEE_IPV4_PROTOCOL_SSL;
    env.remote_port = 443;
  } else {
    env.protocol = XBEE_IPV4_PROTOCOL_TCP;
    env.remote_port = 80;
  }
  env.remote_addr_be = resolveFQDN(env.xbee, hostName);
  /* Could not get IPv4 address from specified EQ field */
  if (env.remote_addr_be == 0) {
    return -EINVAL;
  }
  env.options = 0;
  env.length = 0;

  /* Add authentication */
  if (httpClientCreateAuth()) {
      #ifdef HTTP_CLIENT_VERBOSE
    printf("\f(increase HTTP_AUTHENTICATION_SIZE\n"
           "from: %d\n to: %d", HTTP_AUTHENTICATION_SIZE, credLen);
      #endif
    return -EINVAL;   // Buffer was too small or parameter error
  }
  return 0;
}

/***************************************************************************//**
 * @brief See the header file for documentation
 ******************************************************************************/
int httpClientConnect(xbee_ipv4_envelope_t *env,
                      HttpRequest_t type,
                      const char *hostName,
                      const char *path,
                      HttpDataOut_t *dataOut,
                      HttpDataIn_t *dataIn,
                      int timeout)
{
  char *buffPtr;
  int reqLen = 0;
  int ret;

  const char* typeStr = (type == GET) ? "GET"
                        : (type == POST) ? "POST"
                        : (type == PUT) ? "PUT"
                        : (type == DELETE) ? "DELETE" : "";

  if (typeStr[0] == 0) {
    ret = -EINVAL; // Invalid request type EINVAL
    goto ERR_EXIT;
  }

  if (timeout >= 0) { // if we have a timeout
    timeout += xbee_seconds_timer();
  }
  recvContext.thisRepsonse = dataIn;

  buffPtr = buff;
  reqLen = 0;
  /* Start forming request for HTTP v1.1 */
  ret = snprintf(buffPtr, REQUEST_SIZE - reqLen, "%s %s HTTP/1.1\r\nHost: %s:%d\r\n",
                 typeStr, path, hostName, env->remote_port);
  if (ret < 0 || ret >= REQUEST_SIZE - reqLen) {
    ret = -EINVAL; // Buffer was too small or parameter error
    goto ERR_EXIT;
  } else {
    reqLen += ret;
    buffPtr = buff + reqLen;
  }

  /* Add authentication header if it hasn't been created yet then error */
  if (credentials[0] == 0) {
      #ifdef HTTP_CLIENT_VERBOSE
    printf("\fERROR with credentials\n"
           "buffer: %d\ncredLen: %d", HTTP_AUTHENTICATION_SIZE, credLen);
      #endif
    ret = -EINVAL;   // Buffer was too small or parameter error
    goto ERR_EXIT;
  }
  ret = snprintf(buffPtr, REQUEST_SIZE - reqLen, "Authorization: Basic %.*s\r\n", credLen, credentials);
  if (ret < 0 || ret >= REQUEST_SIZE - reqLen) {
    ret = -EINVAL; // Buffer was too small or parameter error
    goto ERR_EXIT;
  } else {
    reqLen += ret;
    buffPtr = buff + reqLen;
  }

  /* If we have a body to send, then send appropriate headers */
  if (dataOut != NULL && dataOut->contentLength > 0) {
    /* Send body content length header */
    ret = snprintf(buffPtr, REQUEST_SIZE - reqLen,
                   "Content-length: %d\r\n", dataOut->contentLength);
    if (ret < 0 || ret >= REQUEST_SIZE - reqLen) {
      ret = -EINVAL; // Buffer was too small or parameter error
      goto ERR_EXIT;
    } else {
      reqLen += ret;
      buffPtr = buff + reqLen;
    }
    /* Send body content type header */
    switch (dataOut->type) {
      case HTTP_CONTENT_JSON:
        ret = snprintf(buffPtr, REQUEST_SIZE - reqLen,
                       "Content-Type: application/json; charset=ISO-8859-1\r\n"
                       "Accept: application/json\r\n");
        break;
      case HTTP_CONTENT_HTML:
        ret = snprintf(buffPtr, REQUEST_SIZE - reqLen,
                       "Content-Type: text/html; charset=ISO-8859-1\r\n"
                       "Accept: text/html\r\n");
        break;
      case HTTP_CONTENT_XML:
        ret = snprintf(buffPtr, REQUEST_SIZE - reqLen,
                       "Content-Type: text/xml; charset=ISO-8859-1\r\n"
                       "Accept: text/xml\r\n");
        break;
      default:
        #ifdef HTTP_CLIENT_VERBOSE
        printf("\fERROR in choosing data out type");
        #endif
        ret = -EINVAL;
        goto ERR_EXIT;
    }
    if (ret < 0 || ret >= REQUEST_SIZE - reqLen) {
      ret = -EINVAL; // Buffer was too small or parameter error
      goto ERR_EXIT;
    } else {
      reqLen += ret;
      buffPtr = buff + reqLen;
    }
    ret = snprintf(buffPtr, REQUEST_SIZE - reqLen, "\r\n");
    if (ret < 0 || ret >= REQUEST_SIZE - reqLen) {
      ret = -EINVAL; // Buffer was too small or parameter error
      goto ERR_EXIT;
    } else {
      reqLen += ret;
      buffPtr = buff + reqLen;
    }
    /* Send headers in buffer */
    ret = httpClientSend(env, buff, reqLen, 0, timeout);
    if (ret < 0) {
      goto ERR_CLOSE;
    }
    /* Send body of request */
    ret = httpClientSend(env, dataOut->buf, dataOut->contentLength, 0, timeout);
    if (ret < 0) {
      goto ERR_CLOSE;
    }
  } else {
    ret = snprintf(buffPtr, REQUEST_SIZE - reqLen, "\r\n");
    if (ret < 0 || ret >= REQUEST_SIZE - reqLen) {
      ret = -EINVAL; // Buffer was too small or parameter error
      goto ERR_EXIT;
    } else {
      reqLen += ret;
      buffPtr = buff + reqLen;
    }
    /* Send headers in buffer */
    ret = httpClientSend(env, buff, reqLen, 0, timeout);
    if (ret < 0) {
      goto ERR_CLOSE;
    }
  }
  /* Receive HTTP response */
  ret = httpClientRecv(env, timeout);
  if (ret < 0) {
    goto ERR_CLOSE;
  }

  return 0; // Request was successful

  ERR_CLOSE:
  /* Close TCP connection */
  httpClientSend(env, NULL, 0, 2, xbee_seconds_timer() + 5);
  ERR_EXIT:
  return ret;
}

static char encoding[] = {  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                            'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                            'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                            'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                            'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                            'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                            'w', 'x', 'y', 'z', '0', '1', '2', '3',
                            '4', '5', '6', '7', '8', '9', '+', '/' };

/***************************************************************************//**
 * @brief Encodes given character string to base 64
 * @note outputLength is the size of the output buffer but then
 *       it changes to be length of encoded character array
 ******************************************************************************/
int base64Enc(const char *input,
              int inputLength,
              char *output,
              int *outputLength)
{
  uint32_t octA, octB, octC, acc;
  int i, j;
  // The power of truncation removes the need for modular division
  size_t outputSize = 4 * ((inputLength + 2) / 3);
  /* Check if the resulting string is larger than the allocated buffer */
  if (outputSize > (size_t) (*outputLength)) {
    return -EINVAL;
  } else {
    *outputLength = outputSize;
  }

  for (i = 0, j = 0; i < inputLength; ) {
    octA = i < inputLength ? (uint8_t) input[i++] : 0;
    octB = i < inputLength ? (uint8_t) input[i++] : 0;
    octC = i < inputLength ? (uint8_t) input[i++] : 0;
    acc = (octA << 0x10) + (octB << 0x08) + (octC);
    output[j++] = encoding[((acc >> (18)) & BASE_64_MASK)];
    output[j++] = encoding[((acc >> (12)) & BASE_64_MASK)];
    output[j++] = encoding[((acc >> (6)) & BASE_64_MASK)];
    output[j++] = encoding[(acc & BASE_64_MASK)];
  }
  if (inputLength % 3 == 1) {
    output[*outputLength - 2] = '=';
    output[*outputLength - 1] = '=';
  } else if (inputLength % 3 == 2) {
    output[*outputLength - 1] = '=';
  }
  return 0;
}

int getDataFromCloud(char *data, const char *streamID, int timeout)
{
  HttpDataIn_t recvData;
  int ret;
  char *startP;
  char *endP;
  if ((streamID == NULL) || (data == NULL)) {
    return -EINVAL;
  }
  /* Use the sendBuff to generate path */
  ret = snprintf(sendBuff, HTTP_BUFFER_LEN, "/ws/v1/streams/inventory/%s", streamID);

  if (ret < 0 || ret >= HTTP_BUFFER_LEN) {
    #ifdef HTTP_CLIENT_VERBOSE
    printf("\fERROR formating send\n");
    #endif
    return -EINVAL;
  }
  recvData.buf = recvBuff;
  recvData.bufLength = HTTP_BUFFER_LEN;
  recvData.contentLength = 0;

  ret = httpClientConnect(&env, GET, hostName, sendBuff, NULL, &recvData, timeout);
  if (ret < 0) {
    return ret;
  }
  if (recvData.status < 200 || recvData.status >= 300) {
    #ifdef HTTP_CLIENT_VERBOSE
    printf("\fstatus:%3d", recvData.status);
    #endif
    return -EPERM;
  }
  recvData.buf[recvData.contentLength] = '\0'; //NULL terminate so we can use string functions
  startP = strstr(recvData.buf, "\"value\"");
  if (startP == NULL) {
    #ifdef HTTP_CLIENT_VERBOSE
    printf("\fFailed to parse incoming value from %s\n", streamID);
    #endif
    return -EINVAL;
  }
  startP += 7;
  startP = strchr(startP, '\"');
  startP += 1;
  endP = strchr(startP, '\"');
  *endP = '\0';
  strcpy(data, startP);
  return 0;
}

int sendDataToCloud(char *data,
                    const char *streamID,
                    const char *type,
                    char * pos,
                    int timeout)
{
  HttpDataOut_t sendData;
  HttpDataIn_t recvData;
  int ret;
  if ((streamID == NULL) || (data == NULL) || (type == NULL)) {
    return -EINVAL;
  }
  /* check for viable GNSS data */
  if (pos != NULL) {
    ret = snprintf(sendBuff, HTTP_BUFFER_LEN,
                   "{\"stream_id\":\"%s\","
                   "\"stream_type\":\"%s\","
                   "\"value\":\"%s\","
                   "\"geoposition\":{\"coordinates\": %s}"
                   "}",
                   streamID,
                   type,
                   data,
                   pos);
  } else {
    ret = snprintf(sendBuff, HTTP_BUFFER_LEN,
                   "{\"stream_id\":\"%s\","
                   "\"stream_type\":\"%s\","
                   "\"value\":\"%s\""
                   "}",
                   streamID,
                   type,
                   data);
  }

  if (ret < 0 || ret >= HTTP_BUFFER_LEN) {
    #ifdef HTTP_CLIENT_VERBOSE
    printf("\fERROR formating send\n");
    #endif
    return -EINVAL;
  }
  sendData.type = HTTP_CONTENT_JSON;
  sendData.contentLength = strlen(sendBuff);
  sendData.buf = sendBuff;

  recvData.buf = recvBuff;
  recvData.bufLength = HTTP_BUFFER_LEN;
  recvData.contentLength = 0;

  ret = httpClientConnect(&env, POST, hostName, "/ws/v1/streams/history", &sendData, &recvData, timeout);
  if (ret < 0) {
    return ret;
  }
  if (recvData.status < 200 || recvData.status >= 300) {
    #ifdef HTTP_CLIENT_VERBOSE
    printf("\fstatus:%3d", recvData.status);
    #endif
    return -EPERM;
  }
  return 0;
}

int initCloudDataStream(const char *streamID,
                        const char *type,
                        const char* units,
                        int timeout)
{
  HttpDataOut_t sendData;
  HttpDataIn_t recvData;
  int ret;
  if ((streamID == NULL) || (type == NULL) || (units == NULL)) {
    return -EINVAL;
  }
  ret = snprintf(sendBuff, HTTP_BUFFER_LEN,
                 "{\"id\":\"%s\","
                 "\"type\":\"%s\","
                 "\"units\":\"%s\""
                 "}",
                 streamID, type, units);

  if (ret < 0 || ret >= HTTP_BUFFER_LEN) {
    #ifdef HTTP_CLIENT_VERBOSE
    printf("\fERROR formating send\n");
    #endif
    return -EINVAL;
  }
  sendData.type = HTTP_CONTENT_JSON;
  sendData.contentLength = strlen(sendBuff);
  sendData.buf = sendBuff;

  recvData.buf = recvBuff;
  recvData.bufLength = HTTP_BUFFER_LEN;
  recvData.contentLength = 0;

  ret = httpClientConnect(&env, POST, hostName, "/ws/v1/streams/inventory", &sendData, &recvData, timeout);
  if (ret < 0) {
    return ret;
  }
  if (recvData.status < 200 || recvData.status >= 300) {
    #ifdef HTTP_CLIENT_VERBOSE
    printf("\fstatus:%3d", recvData.status);
    #endif
    return -EPERM;
  }
  return ret;
}

char * httpGetHostName(void)
{
  return hostName;
}
uint32_t httpGetIpadr(void)
{
  return env.remote_addr_be;
}
int getConsecutiveFailures(void)
{
  return failures;
}
int getLastTXStatus(void)
{
  return txStat;
}
