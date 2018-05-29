/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2011 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   usb_touch_if.h                                                                                        */
/*            This file contains USB touch interface                                                       */
/* Project:                                                                                                */
/*            Multi-Touch project                                                                          */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef  USB_TOUCH_H
#define  USB_TOUCH_H

#ifdef  USB_TOUCH_GLOBALS
#define USB_TOUCH_EXT
#else
#define USB_TOUCH_EXT extern
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                INCLUDES                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include "hid.h"
#include "transport_if.h"
#include "hal.h"
#include "MaxUSB.h"

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                               Interfaces                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#if defined DIAG_BULK || defined NMTP_BULK
#define BULK_MODE
#endif


#if !defined BULK_MODE
enum
{
    DSC_INTERFACE_HID0,
    DSC_INTERFACE_HID1,
    DSC_NUM_INTERFACE
};
#else
enum
{
    DSC_INTERFACE_BULK,
    DSC_NUM_INTERFACE
};
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                ENDPOINTS                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#define ENDPOINT_IN(ep)                     ((ep) + 0x80)
#define ENDPOINT_OUT(ep)                    (ep)

#define BULK_EP_OUT                         ENDPOINT_OUT(MAXUSB_EP1)
#define BULK_EP_IN                          ENDPOINT_IN(MAXUSB_EP2)
#define INTERRUPT_EP_TOUCH                  ENDPOINT_IN(MAXUSB_EP3)



/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                               DEVICE IDs                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#define NUVOTON_DEVICE_ID                0x0416

#if     defined MT_HID                   /* Multi Touch firmware via Win7 MT HID protocol                  */
#define MULTI_TOUCH_PRODUCT_ID           0xc130
#elif   defined NMTP_BRIDGE_HID          /* Multi Touch bridge via Win7 MT HID protocol                    */
#define MULTI_TOUCH_PRODUCT_ID           0xc132
#elif   defined DIAG_BULK                /* Diagnostic (legacy) data via USB bulk                          */
#define MULTI_TOUCH_PRODUCT_ID           0xc131
#elif   defined NMTP_BULK                /* NMTP data via USB bulk                                         */
#define MULTI_TOUCH_PRODUCT_ID           0xc133
#endif

#define BCD_VERSION                      0x0002

/*---------------------------------------------------------------------------------------------------------*/
/* USB Constants                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
/* Surpress PC lint warining on LSB/MSB shifting                                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*lint -emacro(572, LSB)                                                                                   */
/*lint -emacro(572, MSB)                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/

#define  CONTROL_MAX_PACKET_SIZE         (64)
#define  TOUCH_MAX_PACKET_SIZE           (64)
#define  BULK_IN_PACKET_SIZE             (64)
#define  BULK_OUT_PACKET_SIZE            (64)

/*---------------------------------------------------------------------------------------------------------*/
/* Device Descriptor                                                                                       */
/*                                                                                                         */
/* lint -emacro(572, DEVICE_DESCRIPTIOR)                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define DEVICE_DESCRIPTOR_SIZE 18
#define DEVICE_DESCRIPTIOR                                                                      \
    DEVICE_DESCRIPTOR_SIZE,                 /* bLength                                      */  \
    0x01,                                   /* bDescriptorType (DEVICE)                     */  \
    LSB(0x0110),MSB(0x0110),                /* bcdUSB                                       */  \
                                                                                                \
    0x00,                                   /* bDeviceClass                                 */  \
    0x00,                                   /* bDeviceSubClass                              */  \
    0x00,                                   /* bDeviceProtocol                              */  \
    CONTROL_MAX_PACKET_SIZE,                /* bMaxPacketSize0                              */  \
    LSB(NUVOTON_DEVICE_ID), MSB(NUVOTON_DEVICE_ID), /*idVendor                              */  \
    LSB(MULTI_TOUCH_PRODUCT_ID), MSB(MULTI_TOUCH_PRODUCT_ID),/*idProduct                    */  \
    LSB(BCD_VERSION), MSB(BCD_VERSION),     /* bcdDevice                                    */  \
    0x01,                                   /* iManufacturer                                */  \
    0x02,                                   /* iProduct                                     */  \
    0x00,                                   /* iSerialNumber                                */  \
    0x01                                    /* bNumConfigurations                           */  \

/*---------------------------------------------------------------------------------------------------------*/
/* Device qualifier                                                                                        */
/*                                                                                                         */
/*lint -emacro(572,DEVICE_QUALIFIER)                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#define DEVICE_QUALIFIER_DESCRIPTOR_SIZE  (UINT16)10
#define DEVICE_QUALIFIER                                                                        \
                                                                                                \
    DEVICE_QUALIFIER_DESCRIPTOR_SIZE,       /* bLength Length of this descriptor            */  \
    6,                                      /* bDescType This is a DEVICE Qualifier descr   */  \
    0,2,                                    /* bcdUSB USB revision 2.0                      */  \
    0,                                      /* bDeviceClass                                 */  \
    0,                                      /* bDeviceSubClass                              */  \
    0,                                      /* bDeviceProtocol                              */  \
    CONTROL_MAX_PACKET_SIZE,                /* bMaxPacketSize0                              */  \
    0x01,                                   /* bNumConfigurations                           */  \
    0

/*---------------------------------------------------------------------------------------------------------*/
/* Configuration descriptror                                                                               */
/*                                                                                                         */
/* lint -emacro(572, CONFIG_DESCRITOR)                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#if defined BULK_MODE
/*---------------------------------------------------------------------------------------------------------*/
/* This must be counted manually and updated with the descriptor                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define CONFIG_DESC_SIZE                (UINT16)(9 + (9+7+7))
#define CONFIG_DESCRITOR                                                                        \
    /*Config Descriptor */                                                                      \
    0x09,                                   /* bLength                                      */  \
    0x02,                                   /* bDescriptorType (CONFIGURATION)              */  \
    LSB(CONFIG_DESC_SIZE),                  /* wTotalLength                                 */  \
    MSB(CONFIG_DESC_SIZE),                                                                      \
    DSC_NUM_INTERFACE,                      /* bNumInterfaces                               */  \
    0x01,                                   /* bConfigurationValue                          */  \
    0x00,                                   /* iConfiguration                               */  \
    0xA0,                                   /* bmAttributes (Remote Wakeup Enabled)         */  \
    0xc8,                                   /* MaxPower (400mA)                             */  \
                                                                                                \
    /* Interface Descriptor 2 */                                                                \
    0x09,                                   /* bLength                                      */  \
    0x04,                                   /* bDescriptorType (INTERFACE)                  */  \
    DSC_INTERFACE_BULK,                     /* bInterfaceNumber                             */  \
    0x00,                                   /* bAlternateSetting                            */  \
    0x02,                                   /* bNumEndpoints                                */  \
    0xFF,                                   /* bInterfaceClass (USER DEFINED)               */  \
    0x01,                                   /* bInterfaceSubClass                           */  \
    0x01,                                   /* bInterfaceProtocol                           */  \
    0x03,                                   /* iInterface                                   */  \
                                                                                                \
    /* Endpoint 1 descriptor - Output*/                                                         \
    0x07,                                   /* bLength                                      */  \
    0x05,                                   /* bDescriptorType (ENDPOINT)                   */  \
    BULK_EP_OUT,                            /* bEndpointAddress                             */  \
    0x02,                                   /* bmAttributes (BULK)                          */  \
    LSB(BULK_OUT_PACKET_SIZE),              /* wMaxPacketSize                               */  \
    MSB(BULK_OUT_PACKET_SIZE),                                                                  \
    0x01,                                   /* bInterval                                    */  \
                                                                                                \
    /* Endpoint 2 descriptor - Input*/                                                          \
    0x07,                                   /* bLength                                      */  \
    0x05,                                   /* bDescriptorType (ENDPOINT)                   */  \
    BULK_EP_IN,                             /* bEndpointAddress                             */  \
    0x02,                                   /* bmAttributes (BULK)                          */  \
    LSB(BULK_IN_PACKET_SIZE),               /* wMaxPacketSize                               */  \
    MSB(BULK_IN_PACKET_SIZE),                                                                   \
    0x01                                    /* bInterval                                    */  \

#else
#define CONFIG_DESC_SIZE                (9 + (9+9+7)*2)
#define CONFIG_DESCRITOR                                                                        \
    /*Config Descriptor */                                                                      \
    0x09,                                   /* bLength                                      */  \
    0x02,                                   /* bDescriptorType (CONFIGURATION)              */  \
    LSB(CONFIG_DESC_SIZE),                  /* wTotalLength                                 */  \
    MSB(CONFIG_DESC_SIZE),                                                                      \
    DSC_NUM_INTERFACE,                      /* bNumInterfaces                               */  \
    0x01,                                   /* bConfigurationValue                          */  \
    0x00,                                   /* iConfiguration                               */  \
    0xA0,                                   /* bmAttributes (Remote Wakeup Enabled)         */  \
    0xc8,                                   /* MaxPower (400mA)                             */  \
                                                                                                \
    /* Interface Descriptor 0 */                                                                \
    0x09,                                   /* bLength                                      */  \
    0x04,                                   /* bDescriptorType (INTERFACE)                  */  \
    DSC_INTERFACE_HID0,                     /* bInterfaceNumber                             */  \
    0x00,                                   /* bAlternateSetting                            */  \
    0x01,                                   /* bNumEndpoints                                */  \
    0x03,                                   /* bInterfaceClass (HID)                        */  \
    0x00,                                   /* bInterfaceSubClass                           */  \
    0x02,                                   /* bInterfaceProtocol                           */  \
    0x00,                                   /* iInterface                                   */  \
                                                                                                \
    /* HID descriptor */                                                                        \
    0x09,                                   /* bLength                                      */  \
    0x21,                                   /* bDescriptorType (HID)                        */  \
    LSB(0x0110),                            /* bcdHID (HID specification version 1.00)      */  \
    MSB(0x0110),                                                                                \
    0x00,                                   /* bCountryCode                                 */  \
    0x01,                                   /* bNumDescriptors                              */  \
    0x22,                                   /* bDescriptorType (Report descriptor)          */  \
    LSB(TOUCH_REPORT_DESC_SIZE),            /* wDescriptorlength                            */  \
    MSB(TOUCH_REPORT_DESC_SIZE),                                                                \
                                                                                                \
    /*Endpoint descriptor */                                                                    \
    0x07,                                   /* bLength                                      */  \
    0x05,                                   /* bDescriptorType (ENDPOINT)                   */  \
    INTERRUPT_EP_TOUCH,                     /* bEndpointAddress                             */  \
    0x03,                                   /* bmAttributes (INTERRUPT)                     */  \
    LSB(TOUCH_MAX_PACKET_SIZE),             /* wMaxPacketSize                               */  \
    MSB(TOUCH_MAX_PACKET_SIZE),                                                                 \
    0x07,                                   /* bInterval                                    */  \
                                                                                                \
    /* Interface Descriptor 1 */                                                                \
    0x09,                                   /* bLength                                      */  \
    0x04,                                   /* bDescriptorType (INTERFACE)                  */  \
    DSC_INTERFACE_HID1,                     /* bInterfaceNumber                             */  \
    0x00,                                   /* bAlternateSetting                            */  \
    0x01,                                   /* bNumEndpoints                                */  \
    0x03,                                   /* bInterfaceClass (HID)                        */  \
    0x00,                                   /* bInterfaceSubClass                           */  \
    0x02,                                   /* bInterfaceProtocol                           */  \
    0x00,                                   /* iInterface                                   */  \
                                                                                                \
    /* HID descriptor */                                                                        \
    0x09,                                   /* bLength                                      */  \
    0x21,                                   /* bDescriptorType (HID)                        */  \
    LSB(0x0110),                            /* bcdHID (HID specification version 1.00)      */  \
    MSB(0x0110),                                                                                \
    0x00,                                   /* bCountryCode                                 */  \
    0x01,                                   /* bNumDescriptors                              */  \
    0x22,                                   /* bDescriptorType (Report descriptor)          */  \
    LSB(TOUCH_REPORT_DESC_SIZE),            /* wDescriptorlength                            */  \
    MSB(TOUCH_REPORT_DESC_SIZE),                                                                \
                                                                                                \
    /*Endpoint descriptor */                                                                    \
    0x07,                                   /* bLength                                      */  \
    0x05,                                   /* bDescriptorType (ENDPOINT)                   */  \
    INTERRUPT_EP_TOUCH,                     /* bEndpointAddress                             */  \
    0x03,                                   /* bmAttributes (INTERRUPT)                     */  \
    LSB(TOUCH_MAX_PACKET_SIZE),             /* wMaxPacketSize                               */  \
    MSB(TOUCH_MAX_PACKET_SIZE),                                                                 \
    0x07                                    /* bInterval                                    */  \


#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Strings                                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define USB_STRING_ARRAY_SIZE  8


#define STRING1  'N','u','v','o','t','o','n',' ','E','l','e','c','t','r','o','n','i','c','s',' ','C','o','r','p'
#define STRING2  'M','u','l','t','i','-','t','o','u','c','h',' ','B','r','i','d','g','e'
#define STRING3  'D','i','a','g','n','o','s','t','i','c','s',' ','M','T','-','F','W'
#define STRING4  'D','e','m','o'
#define STRING5  '_','0','1'
#define STRING6  'N','u','v','o','t','o','n'
#define STRINGN  'B','A','D',' ','S','T','R','I','N','G',' ','I','n','d','e','x'




/*---------------------------------------------------------------------------------------------------------*/
/* USB Callbacks                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
typedef UINT16   (*BULK_GET_DATA_PTR)(void** data, UINT16 size);
typedef void     (*BULK_SET_DATA_PTR)(void*  data, UINT16 size);
typedef UINT16   (*INTERRUPT_GET_DATA_PTR)(void** data, UINT16 size);
typedef void     (*INTERRUPT_READ_FINISHED_PTR)(UINT16 size);

typedef struct
{
    INTERRUPT_GET_DATA_PTR          interruptGetData;
    BULK_GET_DATA_PTR               bulkGetData;
    INTERRUPT_READ_FINISHED_PTR     bulkGetDataFinished;
    BULK_SET_DATA_PTR               bulkSetData;
} USB_TOUCH_CALLBACKS;

#endif

