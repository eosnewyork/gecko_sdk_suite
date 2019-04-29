/***************************************************************************//**
 * @file
 * @brief Http client application functions
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
#include  <stdio.h>

#include  "rtos_description.h"

#include  "description.h"
#include  "btlapi.h"
#include  "httpclient.h"

#include  <common/include/rtos_path.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/lib_str.h>
#include  <common/include/lib_utils.h>

#include  <net/include/http_client.h>
#include  <net/include/net_app.h>

#include  "cfg/http_client_cfg.h"

#include  "ssl_certificates.h"
#include  <net/include/net_secure.h>

// -----------------------------------------------------------------------------
// Defines

#define  FULL_URL                                "https://www.silabs.com/..." // TODO ADD resource URL

#define  CONN_NBR_MAX                            5u
#define  REQ_NBR_MAX                             5u
#define  CFG_QUERY_STR_NBR_MAX                   5u
#define  CFG_HDR_NBR_MAX                         5u

#define  HTTP_CON_NR                             0u
#define  HTTPS_CON_NR                            1u

#define  CFG_QUERY_STR_KEY_LEN_MAX               20u
#define  CFG_QUERY_STR_VAL_LEN_MAX               50u
#define  HOSTNAME_LEN_MAX                        67u
#define  CFG_HDR_VAL_LEN_MAX                     100u
#define  RESOURCES_URL_LEN_MAX                   200u
#define  CONN_BUF_SIZE                           512u

typedef  struct  httpc_conn_data {
    CPU_BOOLEAN   Close;
} HTTPc_CONN_DATA;

typedef  struct  http_client_req_data {
    CPU_BOOLEAN      Done;
    CPU_INT08U       QueryStrIx;
    CPU_INT16U       FormIx;
} HTTP_CLIENT_REQ_DATA;

// -----------------------------------------------------------------------------
// Local global Variables
                                                                /* HTTPc Task Stack.                                    */
static  CPU_STK                   httpcTaskStk[HTTP_CLIENT_TASK_STK_SIZE];

static  CPU_BOOLEAN               secureConnection;

static  HTTPc_HDR                 respHdrTbl[CFG_HDR_NBR_MAX];
static  CPU_CHAR                  respHdrValStrTbl[CFG_HDR_NBR_MAX][CFG_HDR_VAL_LEN_MAX];

static  HTTP_CLIENT_REQ_DATA      clientData[REQ_NBR_MAX];

static  HTTPc_CONN_OBJ            connTbl[CONN_NBR_MAX];
static  HTTPc_REQ_OBJ             reqTbl[REQ_NBR_MAX];
static  HTTPc_RESP_OBJ            respTbl[REQ_NBR_MAX];

static  CPU_CHAR                  connBufTbl[CONN_NBR_MAX][CONN_BUF_SIZE];
static  CPU_CHAR                  hostname[HOSTNAME_LEN_MAX];
static  CPU_CHAR                  resources[RESOURCES_URL_LEN_MAX];

static  HTTPc_KEY_VAL             reqQueryStrTbl[CFG_QUERY_STR_NBR_MAX];
static  CPU_CHAR                  reqQueryStrKeyTbl[CFG_QUERY_STR_NBR_MAX][CFG_QUERY_STR_KEY_LEN_MAX];
static  CPU_CHAR                  reqQueryStrValTbl[CFG_QUERY_STR_NBR_MAX][CFG_QUERY_STR_VAL_LEN_MAX];

static  HTTPc_HDR                 reqHdrTbl[CFG_HDR_NBR_MAX];
static  CPU_CHAR                  reqHdrValStrTbl[CFG_HDR_NBR_MAX][CFG_HDR_VAL_LEN_MAX];

static  NET_APP_SOCK_SECURE_CFG   secure_cfg;

static  HTTPc_PARAM_TBL           tbl_obj;

// -----------------------------------------------------------------------------
// Local Function Prototypes

static CPU_VOID       parseURL             (CPU_VOID);

static CPU_VOID       getPrepare           (NET_APP_SOCK_SECURE_CFG          *secure_cfg,
                                            CPU_INT08U                       connect_nr);

static CPU_VOID       respHdrHook          (HTTPc_CONN_OBJ            *p_conn,
                                            HTTPc_REQ_OBJ             *p_req,
                                            HTTP_HDR_FIELD             hdr_field,
                                            CPU_CHAR                  *p_hdr_val,
                                            CPU_INT16U                 val_len);

static CPU_INT32U     respBodyHook         (HTTPc_CONN_OBJ            *p_conn,
                                            HTTPc_REQ_OBJ             *p_req,
                                            HTTP_CONTENT_TYPE          content_type,
                                            CPU_VOID                  *p_data,
                                            CPU_INT16U                 data_len,
                                            CPU_BOOLEAN                last_chunk);

static CPU_VOID       reqPrepareHdrs       (CPU_INT08U                connect_nr);

static CPU_VOID       connCloseCallback    (HTTPc_CONN_OBJ            *p_conn,
                                            HTTPc_CONN_CLOSE_STATUS    close_status,
                                            RTOS_ERR                   err);

static CPU_VOID       transDoneCallback    (HTTPc_CONN_OBJ            *p_conn,
                                            HTTPc_REQ_OBJ             *p_req,
                                            HTTPc_RESP_OBJ            *p_resp,
                                            CPU_BOOLEAN                status);

static CPU_VOID       transErrCallback     (HTTPc_CONN_OBJ            *p_conn,
                                            HTTPc_REQ_OBJ             *p_req,
                                            RTOS_ERR                   err_code);

static CPU_BOOLEAN    secureTrustCallback  (CPU_VOID                           *p_cert_dn,
                                            NET_SOCK_SECURE_UNTRUSTED_REASON   reason);

/*
*********************************************************************************************************
*                                            HTTP_Client_Init()
*
* Description : Initialize the HTTP Client stack for the example application.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  HTTP_Client_Init (void)
{
    RTOS_ERR  err;

    HTTPc_ConfigureTaskStk(HTTP_CLIENT_TASK_STK_SIZE, &httpcTaskStk[0]);

                                                                /* ------------- INITIALIZE CLIENT SUITE -------------- */
    HTTPc_Init(&err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

    parseURL();
    getPrepare(&secure_cfg, HTTPS_CON_NR);
    getPrepare(DEF_NULL, HTTP_CON_NR);
}

/*
*********************************************************************************************************
*                                            HTTP_Client_Get()
*
* Description : Retrieve file from a HTTP server with the given URL.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  HTTP_Client_Get (void)
{
    HTTPc_CONN_OBJ           *p_conn;
    HTTPc_REQ_OBJ            *p_req;
    HTTPc_RESP_OBJ           *p_resp;
    CPU_CHAR                 *p_buf;
    CPU_SIZE_T               str_len;
    RTOS_ERR                 err;
    
    if (secureConnection) {
        p_buf  = &connBufTbl[HTTPS_CON_NR][0u];
        p_resp = &respTbl[HTTPS_CON_NR];
        p_conn = &connTbl[HTTPS_CON_NR];
        p_req  = &reqTbl[HTTPS_CON_NR];
    } else {
        p_buf  = &connBufTbl[HTTP_CON_NR][0u];
        p_resp = &respTbl[HTTP_CON_NR];
        p_conn = &connTbl[HTTP_CON_NR];
        p_req  = &reqTbl[HTTP_CON_NR];
    }

                                                                /* ----------------- OPEN CONNECTION ------------------ */
    printf(("Attempting to connect to HTTP server...\n\r"));
    str_len = Str_Len(hostname);
    (void)HTTPc_ConnOpen(p_conn,
                         p_buf,
                         CONN_BUF_SIZE,
                         hostname,
                         str_len,
                         HTTPc_FLAG_NONE,
                        &err);
    if (err.Code == RTOS_ERR_NONE) {
        printf(("Connection to server succeeded.\n\r"));
    } else {
        printf(("Connection to server failed.\n\r"));
    }

                                                                /* ---------------- SEND HTTP REQUEST ----------------- */
    printf(("Sending GET request and retrieving file...\n\r"));
    str_len = Str_Len(resources);
    (void)HTTPc_ReqSend(p_conn,
                        p_req,
                        p_resp,
                        HTTP_METHOD_GET,
                        resources,
                        str_len,
                        HTTPc_FLAG_NONE,
                       &err);
    if (err.Code != RTOS_ERR_NONE) {
        printf(("The requested file could not be retrieved.\n\r"));
    }
}

// -----------------------------------------------------------------------------
// Local Functions

/*
*********************************************************************************************************
*                                        getPrepare()
*
* Description : Prepare a GET request with a Query String and additional headers.
*
* Argument(s) : secure_cfg    Pointer to the secure socket config param
*               connect_nr    HTTP connection object number
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static CPU_VOID  getPrepare (NET_APP_SOCK_SECURE_CFG  *secure_cfg, CPU_INT08U  connect_nr)
{
    HTTPc_CONN_OBJ            *p_conn;
    HTTPc_REQ_OBJ             *p_req;
    RTOS_ERR                  err;
    HTTPc_KEY_VAL             *p_query_str_tbl;
    HTTPc_HDR                 *p_hdr_tbl;

    p_conn = &connTbl[connect_nr];
    p_req  = &reqTbl[connect_nr];
    p_query_str_tbl = &reqQueryStrTbl[connect_nr];
    p_hdr_tbl = &reqHdrTbl[connect_nr];

    p_req->UserDataPtr = (void *)&clientData[connect_nr];

                                                                /* ----------- INIT NEW CONNECTION & REQUEST ---------- */
    HTTPc_ConnClr(p_conn, &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

    HTTPc_ReqClr(p_req, &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

                                                                /* ------------- SET CONNECTION CALLBACKS ------------- */
    HTTPc_ConnSetParam(        p_conn,
                               HTTPc_PARAM_TYPE_CONN_CLOSE_CALLBACK,
                       (void *)connCloseCallback,
                              &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

                                                                /* -------------- SET SSL TRUST CALLBACK -------------- */
    if (secure_cfg != DEF_NULL){

                                                                /* -------------- INSTALL CA CERTIFICATE -------------- */
        NetSecure_CA_CertInstall(SSL_ROOT_CA, SSL_RootCALen, NET_SOCK_SECURE_CERT_KEY_FMT_PEM, &err);
                                                              
        secure_cfg->CertPtr       = (CPU_CHAR *)SSL_ROOT_CA;
        secure_cfg->CertSize      = SSL_RootCALen;
        secure_cfg->KeyPtr        = (CPU_CHAR *)SSL_CLIENT_KEY;
        secure_cfg->KeySize       = SSL_ClientKeyLen;

        secure_cfg->CertFmt       = NET_SOCK_SECURE_CERT_KEY_FMT_PEM;
        secure_cfg->TrustCallback = secureTrustCallback;
        secure_cfg->CommonName    = hostname;
        secure_cfg->CertChain     = DEF_YES;

        HTTPc_ConnSetParam(        p_conn,
                                   HTTPc_PARAM_TYPE_SECURE_CFG_PTR,
                           (void *)secure_cfg,
                                   &err);
      }
                                                                /* ------------ SET STRING QUERY PARAMETERS ----------- */
    tbl_obj.EntryNbr =  connect_nr;
    tbl_obj.TblPtr   = (void *)p_query_str_tbl;
    HTTPc_ReqSetParam(p_req,
                      HTTPc_PARAM_TYPE_REQ_QUERY_STR_TBL,
                     &tbl_obj,
                     &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

                                                                /* -------------- SET ADDITIONAL HEADERS -------------- */
    reqPrepareHdrs(connect_nr);
                                                                /* ---------- SET REQUEST ADDITIONAL HEADERS ---------- */
    tbl_obj.EntryNbr =  connect_nr;
    tbl_obj.TblPtr   = (void *)p_hdr_tbl;
    HTTPc_ReqSetParam(p_req,
                      HTTPc_PARAM_TYPE_REQ_HDR_TBL,
                     &tbl_obj,
                     &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);


                                                                /* ------------ SET RESP HOOK FUNCTIONS ----------- */
    HTTPc_ReqSetParam(        p_req,
                              HTTPc_PARAM_TYPE_RESP_HDR_HOOK,
                      (void *)respHdrHook,
                             &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

    HTTPc_ReqSetParam(        p_req,
                              HTTPc_PARAM_TYPE_RESP_BODY_HOOK,
                      (void *)respBodyHook,
                             &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

                                                                /* --------- SET REQ/RESP CALLBACK FUNCTIONS ---------- */
    HTTPc_ReqSetParam(        p_req,
                              HTTPc_PARAM_TYPE_TRANS_COMPLETE_CALLBACK,
                      (void *)transDoneCallback,
                             &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

                                                                /* --------- SET REQ/RESP CALLBACK FUNCTIONS ---------- */
    HTTPc_ReqSetParam(        p_req,
                              HTTPc_PARAM_TYPE_TRANS_ERR_CALLBACK,
                      (void *)transErrCallback,
                             &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);
}

/*
*********************************************************************************************************
*                                     parseURL()
*
* Description : Parse the URL string
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  CPU_VOID  parseURL (CPU_VOID)
{
    CPU_CHAR* ret;

    if (Str_Str(FULL_URL, "https")) {
        secureConnection = DEF_TRUE;
    } else if (Str_Str(FULL_URL, "http")) {
        secureConnection = DEF_FALSE;
    } else {
        APP_RTOS_ASSERT_CRITICAL_FAIL(;);
    }

    ret = Str_Str(FULL_URL, "://");
    if (ret) {
        ret = &ret[3];
        (void)Str_Copy_N(hostname, ret, Str_Str(ret, "/")-ret);
    } else {
        APP_RTOS_ASSERT_CRITICAL_FAIL(;);
    }
    
    ret = Str_Str(ret, "/");
    if (ret) {
        if (Str_Str(FULL_URL, "?")) {
            (void)Str_Copy_N(resources, ret, Str_Str(ret, "?")-ret);
        } else {
            (void)Str_Copy_N(resources, ret, Str_Len(ret));
        }
    } else {
        APP_RTOS_ASSERT_CRITICAL_FAIL(;);
    }

    HTTPc_KEY_VAL  *p_kvp;


                                                                /* ----------------- SET FIRST QUERY ------------------ */
    if (secureConnection) {
        p_kvp         = &reqQueryStrTbl[HTTPS_CON_NR];
        p_kvp->KeyPtr = &reqQueryStrKeyTbl[HTTPS_CON_NR][0u];
        p_kvp->ValPtr = &reqQueryStrValTbl[HTTPS_CON_NR][0u];
    } else {
        p_kvp         = &reqQueryStrTbl[HTTP_CON_NR];
        p_kvp->KeyPtr = &reqQueryStrKeyTbl[HTTP_CON_NR][0u];
        p_kvp->ValPtr = &reqQueryStrValTbl[HTTP_CON_NR][0u];
    }
   
   ret = Str_Str(FULL_URL, "?");
   if (ret) {
        ret = &ret[1];
        (void)Str_Copy_N(p_kvp->KeyPtr, ret, Str_Str(ret, "=")-ret);

        ret = Str_Str(ret, "=");
        ret = &ret[1];
        (void)Str_Copy_N(p_kvp->ValPtr, ret, Str_Len(ret));

        p_kvp->KeyLen = Str_Len(p_kvp->KeyPtr);
        p_kvp->ValLen = Str_Len(p_kvp->ValPtr);      
    }
}

/*
*********************************************************************************************************
*                                       reqPrepareHdrs()
*
* Description : Configure the Header Fields Table
*
* Argument(s) : connect_nr    HTTP connection object number.
*
* Return(s)   : Number of fields in the table.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  CPU_VOID  reqPrepareHdrs (CPU_INT08U  connect_nr)
{
    HTTPc_HDR  *p_hdr;


                                                                /* ---------------- ADD ACCEPT HEADER ----------------- */
    p_hdr           = &reqHdrTbl[connect_nr];
    p_hdr->ValPtr   = &reqHdrValStrTbl[connect_nr][0u];
    p_hdr->HdrField = HTTP_HDR_FIELD_ACCEPT_ENCODING;

    Str_Copy_N(p_hdr->ValPtr, "identity", CFG_HDR_VAL_LEN_MAX);

    p_hdr->ValLen = Str_Len("identity");
}


/*
*********************************************************************************************************
*                                    respHdrHook()
*
* Description : Retrieve header fields in the HTTP response received.
*
* Argument(s) : p_conn      Pointer to current HTTPc Connection object.
*
*               p_req       Pointer to current HTTPc Request object.
*
*               hdr_field   HTTP header type of the header field received in the HTTP response.
*
*               p_hdr_val   Pointer to the value string received in the Response header field.
*
*               val_len     Length of the value string.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static CPU_VOID  respHdrHook (HTTPc_CONN_OBJ    *p_conn,
                              HTTPc_REQ_OBJ     *p_req,
                              HTTP_HDR_FIELD     hdr_field,
                              CPU_CHAR          *p_hdr_val,
                              CPU_INT16U         val_len)
{
    HTTPc_HDR  *p_hdr;


    PP_UNUSED_PARAM(p_conn);
    PP_UNUSED_PARAM(p_req);

    p_hdr         = &respHdrTbl[0];
    p_hdr->ValPtr = &respHdrValStrTbl[0][0];

    switch (hdr_field) {
        case HTTP_HDR_FIELD_COOKIE:
             p_hdr->HdrField = hdr_field;
             Str_Copy_N(p_hdr->ValPtr, p_hdr_val, val_len);
             p_hdr->ValLen   = val_len;
             break;

        default:
             break;
    }
}

/*
*********************************************************************************************************
*                                    respBodyHook()
*
* Description : Retrieve data in HTTP Response received.
*
* Argument(s) : p_conn          Pointer to current HTTPc Connection object.
*
*               p_req           Pointer to current HTTPc Request object.
*
*               content_type    HTTP Content Type of the HTTP Response body's data.
*
*               p_data          Pointer to a data piece of the HTTP Response body.
*
*               data_len        Length of the data piece received.
*
*               last_chunk      DEF_YES, if this is the last piece of data.
*                               DEF_NO,  if more data is up coming.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static CPU_INT32U respBodyHook (HTTPc_CONN_OBJ     *p_conn,
                                HTTPc_REQ_OBJ      *p_req,
                                HTTP_CONTENT_TYPE   content_type,
                                CPU_VOID           *p_data,
                                CPU_INT16U          data_len,
                                CPU_BOOLEAN         last_chunk)
{
    PP_UNUSED_PARAM(p_conn);
    PP_UNUSED_PARAM(p_req);
    PP_UNUSED_PARAM(content_type);
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
    BTLAPI_reprogramBtlStorage((CPU_INT08U*)p_data, data_len);
    CPU_CRITICAL_EXIT();
    if (last_chunk == DEF_YES) {
        BTLAPI_reprogramBtlStorage(DEF_NULL, 0);
    }
    return data_len;
}

/*
*********************************************************************************************************
*                                 connCloseCallback()
*
* Description : Callback to notify application that an HTTP connection was closed.
*
* Argument(s) : p_conn          Pointer to current HTTPc Connection.
*
*               close_status    Status of the connection closing:
*                                   HTTPc_CONN_CLOSE_STATUS_ERR_INTERNAL
*                                   HTTPc_CONN_CLOSE_STATUS_SERVER
*                                   HTTPc_CONN_CLOSE_STATUS_NO_PERSISTENT
*                                   HTTPc_CONN_CLOSE_STATUS_APP
*
*               err             Error Code when connection was closed.
*
* Return(s)   : None
*
* Note(s)     : none.
*********************************************************************************************************
*/
static CPU_VOID  connCloseCallback (HTTPc_CONN_OBJ           *p_conn,
                                    HTTPc_CONN_CLOSE_STATUS   close_status,
                                    RTOS_ERR                  err)
{
    PP_UNUSED_PARAM(p_conn);
    PP_UNUSED_PARAM(close_status);
    PP_UNUSED_PARAM(err);

    printf("Connection closed.\n\r");
}

/*
*********************************************************************************************************
*                                 transDoneCallback()
*
* Description : Callback to notify application that an HTTP transaction was completed.
*
* Argument(s) : p_conn  Pointer to current HTTPc Connection object.
*
*               p_req   Pointer to current HTTPc Request object.
*
*               p_resp  Pointer to current HTTPc Response object.
*
*               status  Status of the transaction:
*
*                           DEF_OK,   transaction was successful.
*                           DEF_FAIL, otherwise.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/
static CPU_VOID  transDoneCallback (HTTPc_CONN_OBJ  *p_conn,
                                    HTTPc_REQ_OBJ   *p_req,
                                    HTTPc_RESP_OBJ  *p_resp,
                                    CPU_BOOLEAN      status)
{
    HTTP_CLIENT_REQ_DATA  *p_data;
    CPU_SR_ALLOC();

    PP_UNUSED_PARAM(p_conn);

    if (status == DEF_OK) {
        printf("Transaction Status Code: %s\n\r", p_resp->ReasonPhrasePtr);
    } else {
        printf("Transaction failed\n\r");
    }

    p_data = (HTTP_CLIENT_REQ_DATA *)p_req->UserDataPtr;
    CPU_CRITICAL_ENTER();
    p_data->Done = DEF_YES;
    CPU_CRITICAL_EXIT();
}

/*
*********************************************************************************************************
*                                      transErrCallback()
*
* Description : Callback to notify application that an error occurred during an HTTP transaction.
*
* Argument(s) : p_conn      Pointer to current HTTPc Connection object.
*
*               p_req       Pointer to current HTTPc Request object.
*
*               err_code    Error Code.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/
static CPU_VOID transErrCallback (HTTPc_CONN_OBJ  *p_conn,
                                  HTTPc_REQ_OBJ   *p_req,
                                  RTOS_ERR         err_code)
{
    PP_UNUSED_PARAM(p_conn);
    PP_UNUSED_PARAM(p_req);

    printf("Transaction error code %i, description: %s\n\r",
              err_code.Code,
              RTOS_ERR_STR_GET(err_code.Code));
}

/*
*********************************************************************************************************
*                                   secureTrustCallback()
*
* Description : Hook function called by the TLS layer to let client decide to enforce or ignore server
*               certificate validation errors.
*
* Argument(s) : p_cert_dn   Port-specific representation of the certificate distinguished name.
*
*               reason      Reason why the connection is not secure.
*
* Return(s)   : DEF_OK,   if non-secure connection should be allowed.
*               DEF_FAIL, otherwise.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static CPU_BOOLEAN  secureTrustCallback (CPU_VOID                           *p_cert_dn,
                                         NET_SOCK_SECURE_UNTRUSTED_REASON   reason)
{
    printf("HTTP Client Secure Trust Callback %d\n", reason);
    PP_UNUSED_PARAM(p_cert_dn);
    PP_UNUSED_PARAM(reason);
    return (DEF_FAIL);
}


