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
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/*
*********************************************************************************************************
*
*                                          USB HOST EXAMPLE
*
*                                   USB Host module initialization
*********************************************************************************************************
* This example shows how to initialize the USB Host module and how to add a Host Controller. It also
* initializes all the available class drivers.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/rtos_err.h>

#include  <usb/include/host/usbh_core.h>

#ifdef RTOS_MODULE_USB_HOST_PBHCI_AVAIL
#include  <usb/include/host/usbh_pbhci.h>
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                         CONFIGURATION ERRORS
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef RTOS_MODULE_USB_HOST_AVAIL
#error "RTOS_MODULE_USB_HOST_AVAIL must be defined in rtos_description.h and present in your project to use this example."
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      USB HOST CONTROLLER NAME
*
* Note(s) : (1) usb1 is used by default. It may not be possible to use this one as host. Change it if
*               needed.
*********************************************************************************************************
*/

#define  EX_USBH_CTRLR_NAME                           "usb0"    /* TODO: Modify usb controller name.                    */


/*
*********************************************************************************************************
*                                               LOGGING
*
* Note(s) : (1) This example outputs information to the console via the function printf() via a macro
*               called EX_TRACE(). This can be modified or disabled if printf() is not supported.
*********************************************************************************************************
*/

#include  <stdio.h>
#define  EX_TRACE(...)                                      printf(__VA_ARGS__)


/*
*********************************************************************************************************
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

#if defined(RTOS_MODULE_USB_HOST_AOAP_AVAIL)
void         Ex_USBH_AOAP_Init      (void);
#endif

#if (defined(RTOS_MODULE_USB_HOST_CDC_AVAIL) && \
     defined(RTOS_MODULE_USB_HOST_ACM_AVAIL))
void         Ex_USBH_CDC_ACM_Init   (void);
#endif

#if defined(RTOS_MODULE_USB_HOST_HID_AVAIL)
void         Ex_USBH_HID_Init       (void);
#endif

#if defined(RTOS_MODULE_USB_HOST_MSC_AVAIL)
void         Ex_USBH_MSC_Init       (void);
#endif

#if defined(RTOS_MODULE_USB_HOST_USB2SER_AVAIL)
void         Ex_USBH_USB2SER_Init   (void);
#endif

CPU_BOOLEAN  Ex_USBH_DevConnAccept  (void);

CPU_BOOLEAN  Ex_USBH_DevConfigAccept(void);

void         Ex_USBH_DevConfig      (USBH_DEV_HANDLE   dev_handle,
                                     CPU_INT08U        config_nbr,
                                     RTOS_ERR          err);

void         Ex_USBH_FnctConnFail   (USBH_FNCT_HANDLE  fnct_handle,
                                     RTOS_ERR          err);

void         Ex_USBH_DevConnFail    (CPU_INT08U        hub_addr,
                                     CPU_INT08U        port_nbr,
                                     RTOS_ERR          err);

void         Ex_USBH_DevDisconn     (USBH_DEV_HANDLE   dev_handle);

/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/

static  USBH_EVENT_FNCTS  Ex_USBH_EventFncts = {
    .DevConnAccept   = Ex_USBH_DevConnAccept,
    .DevConfigAccept = Ex_USBH_DevConfigAccept,
    .DevConfig       = Ex_USBH_DevConfig,
    .FnctConnFail    = Ex_USBH_FnctConnFail,
    .DevConnFail     = Ex_USBH_DevConnFail,
    .DevResume       = DEF_NULL,                                /* Not yet supported. For futur considerations.         */
    .DevDisconn      = Ex_USBH_DevDisconn
};


/*
*********************************************************************************************************
*********************************************************************************************************
*                                          LOCAL CONSTANTS
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                          GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           Ex_USBH_Init()
*
* Description : Example of initialization of the USB host module. Also initializes the available class
*               drivers.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  Ex_USBH_Init (void)
{
    RTOS_ERR  err;


    USBH_ConfigureEventFncts(&Ex_USBH_EventFncts);

    USBH_Init(1u, &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

#ifdef RTOS_MODULE_USB_HOST_PBHCI_AVAIL
    USBH_PBHCI_Init(&err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);
#endif

    (void)USBH_HC_Add(EX_USBH_CTRLR_NAME,
                      DEF_NULL,
                     &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);

#if defined(RTOS_MODULE_USB_HOST_AOAP_AVAIL)
    Ex_USBH_AOAP_Init();
#endif

#if (defined(RTOS_MODULE_USB_HOST_CDC_AVAIL) && \
     defined(RTOS_MODULE_USB_HOST_ACM_AVAIL))
    Ex_USBH_CDC_ACM_Init();
#endif

#if defined(RTOS_MODULE_USB_HOST_HID_AVAIL)
    Ex_USBH_HID_Init();
#endif

#if defined(RTOS_MODULE_USB_HOST_MSC_AVAIL)
    Ex_USBH_MSC_Init();
#endif

#if defined(RTOS_MODULE_USB_HOST_USB2SER_AVAIL)
    Ex_USBH_USB2SER_Init();
#endif
}


/*
*********************************************************************************************************
*                                           Ex_USBH_Start()
*
* Description : Example of host controller start.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  Ex_USBH_Start (void)
{
    USBH_HC_HANDLE  hc_handle;
    RTOS_ERR        err;


    hc_handle = USBH_HC_HandleGetFromName(EX_USBH_CTRLR_NAME);

    USBH_HC_Start(hc_handle, &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE, ;);
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                        Ex_USBH_DevConnAccept()
*
* Description : Accepts a device connection.
*
* Argument(s) : None.
*
* Return(s)   : DEF_YES, Device connection accepted by application.
*               DEF_NO,  Device connection declined by application.
*
* Note(s)     : (1) Basic information on the connected device can be retrieved within this function via the
*                   standard USB Host dev API using the special handle value: USBH_DEV_HANDLE_NOTIFICATION.
*
*               (2) If device connection is declined, the device will be disconnected.
*********************************************************************************************************
*/

CPU_BOOLEAN  Ex_USBH_DevConnAccept (void)
{
    return (DEF_YES);
}


/*
*********************************************************************************************************
*                                       Ex_USBH_DevConfigAccept()
*
* Description : Accepts a device configuration.
*
* Argument(s) : None.
*
* Return(s)   : DEF_YES, Device configuration accepted by application.
*               DEF_NO,  Device configuration declined by application.
*
* Note(s)     : (1) Basic information on the connected device can be retrieved within this function via the
*                   standard USB Host dev API using the special handle value: USBH_DEV_HANDLE_NOTIFICATION.
*
*               (2) If configuration is declined, the device will remain in addressed state. The
*                   application is free to call the function USBH_DevConfigSet() later on for this device.
*********************************************************************************************************
*/

CPU_BOOLEAN  Ex_USBH_DevConfigAccept (void)
{
    return (DEF_YES);
}


/*
*********************************************************************************************************
*                                          Ex_USBH_DevConfig()
*
* Description : Notifies application that a device is now in the configured state and ready for
*               communication.
*
* Argument(s) : dev_handle  Handle to USB device.
*
*               config_nbr  Configuration number that was set.
*
*               err         Error code. If error code is not RTOS_ERR_NONE, device configuration failed.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  Ex_USBH_DevConfig (USBH_DEV_HANDLE  dev_handle,
                         CPU_INT08U       config_nbr,
                         RTOS_ERR         err)
{
    PP_UNUSED_PARAM(dev_handle);
    PP_UNUSED_PARAM(config_nbr);

    if (err.Code == RTOS_ERR_NONE) {
        EX_TRACE("Device configured.\r\n");
    } else {
        EX_TRACE("Device configuration failed with error: %s.\r\n", RTOS_ERR_DESC_STR_GET(err.Code));
    }
}


/*
*********************************************************************************************************
*                                        Ex_USBH_FnctConnFail()
*
* Description : Notifies that one of the device's function connection has failed.
*
* Argument(s) : fnct_handle     Handle to the function.
*
*               err             Error code.
*
* Return(s)   : None.
*
* Note(s)     : (1) If the function is part of a composite device, the device connection may succeed if
*                   at least one function connects successfuly.
*********************************************************************************************************
*/

void  Ex_USBH_FnctConnFail (USBH_FNCT_HANDLE  fnct_handle,
                            RTOS_ERR          err)
{
    PP_UNUSED_PARAM(fnct_handle);
    PP_UNUSED_PARAM(err);

    EX_TRACE("Function connection failed with err: %s\r\n", RTOS_ERR_DESC_STR_GET(err.Code));
}


/*
*********************************************************************************************************
*                                         Ex_USBH_DevConnFail()
*
* Description : Notifies that a device connection has failed.
*
* Argument(s) : hub_addr    Hub address on which the device was connected.
*
*               port_nbr    Hub port number on which the device was connected.
*
*               err         Error code.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  Ex_USBH_DevConnFail (CPU_INT08U  hub_addr,
                           CPU_INT08U  port_nbr,
                           RTOS_ERR    err)
{
    PP_UNUSED_PARAM(hub_addr);
    PP_UNUSED_PARAM(port_nbr);
    PP_UNUSED_PARAM(err);

    EX_TRACE("Device connection failed with err: %s\r\n", RTOS_ERR_DESC_STR_GET(err.Code));
}


/*
*********************************************************************************************************
*                                         Ex_USBH_DevDisconn()
*
* Description : Notifies a device disconnection.
*
* Argument(s) : dev_handle  Handle to the USB device.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  Ex_USBH_DevDisconn (USBH_DEV_HANDLE  dev_handle)
{
    PP_UNUSED_PARAM(dev_handle);

    EX_TRACE("Device disconnected\r\n");
}

