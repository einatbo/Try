/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2011 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   usb_touch.c                                                                                           */
/*            This file contains Win7 Multi-Touch USB application built on top of MaxUSB device            */
/* Project:                                                                                                */
/*            Mutli-Touch project                                                                          */
/*---------------------------------------------------------------------------------------------------------*/

#define USB_TOUCH_GLOBALS

#include "hal.h"
#include "usb_touch.h"
#include "hi_if.h"
#include <string.h>


/*---------------------------------------------------------------------------------------------------------*/
/* Macros                                                                                                  */
/* PC Lint  messages surpression:                                                                          */
/*lint -e750                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/


#define SR_GET_STATUS           0x00    // Get Status
#define SR_CLEAR_FEATURE        0x01    // Clear Feature
#define SR_RESERVED             0x02    // Reserved
#define SR_SET_FEATURE          0x03    // Set Feature
#define SR_SET_ADDRESS          0x05    // Set Address
#define SR_GET_DESCRIPTOR       0x06    // Get Descriptor
#define SR_SET_DESCRIPTOR       0x07    // Set Descriptor
#define SR_GET_CONFIGURATION    0x08    // Get Configuration
#define SR_SET_CONFIGURATION    0x09    // Set Configuration
#define SR_GET_INTERFACE        0x0a    // Get Interface
#define SR_SET_INTERFACE        0x0b    // Set Interface

/*---------------------------------------------------------------------------------------------------------*/
/* Get Descriptor codes                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define GD_DEVICE               0x01    // Get device descriptor: Device
#define GD_CONFIGURATION        0x02    // Get device descriptor: Configuration
#define GD_STRING               0x03    // Get device descriptor: String
#define GD_HID                  0x21    // Get descriptor: HID
#define GD_REPORT               0x22    // Get descriptor: Report

/*---------------------------------------------------------------------------------------------------------*/
/* SETUP packet offsets                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define bmRequestType           0
#define bRequest                1
#define wValueL                 2
#define wValueH                 3
#define wIndexL                 4
#define wIndexH                 5
#define wLengthL                6
#define wLengthH                7

/*---------------------------------------------------------------------------------------------------------*/
/* HID bRequest values                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define GET_REPORT              1
#define GET_IDLE                2
#define GET_PROTOCOL            3
#define SET_REPORT              9
#define SET_IDLE                0x0A
#define SET_PROTOCOL            0x0B
#define INPUT_REPORT            1

#define MAX_CONTROL_DATA_PAYLOAD 64
/*---------------------------------------------------------------------------------------------------------*/
/* Local MaxUSB functions                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Physical layer                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
static void  MAXUSB_WriteReg(UINT8 address, UINT8 val);
static void  MAXUSB_WriteACKSTAT(void);
static void  MAXUSB_WriteBuf(UINT8 address, UINT8* buf, UINT16 size);
static UINT8 MAXUSB_ReadReg(UINT8 address);
static void  MAXUSB_ReadBuf(UINT8 address, UINT8* buf, UINT16 size);


/*---------------------------------------------------------------------------------------------------------*/
/* USB Chapter 9 layer                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
static void MAXUSB_Reset(void);
static void MAXUSB_DeviceInit(void);
static void MAXUSB_EnableIRQs(void);
static void MAXUSB_ServiceIRQs(UINT16 intNum);

static void MAXUSB_GetDescription(void);
static void MAXUSB_GetStatus(void);
static void MAXUSB_SetInterface(void);
static void MAXUSB_GetInterface(void);
static void MAXUSB_SetConfiguration(void);
static void MAXUSB_GetConfiguration(void);
static void MAXUSB_SetFeature(void);
static void MAXUSB_ClearFeature(void);
static void MAXUSB_Vendor(void);
static void MAXUSB_Class(void);
static void MAXUSB_GetReport(void);
static void MAXUSB_SetReport(void);
static void MAXUSB_SetIdle(void);

static void MAXUSB_DoSetup(void);
static void MAXUSB_ServiceEPOUT1(void);
static void MAXUSB_ServiceEPIN2(void);
static void MAXUSB_ServiceEPIN3(void);


/*---------------------------------------------------------------------------------------------------------*/
/* Macros                                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define MAXUSB_STALL_EP0()   SET_REG_FIELD(MAXUSB_EPSTALLS, MAXUSB_EPSTALLS_STLSTAT, 1);

/*---------------------------------------------------------------------------------------------------------*/
/* Module variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

UINT8  MAXUSB_SudFifo_l[8];                       /* Local copy of the 8 setup data from MAX3420E SUDFIFO  */
UINT16 MAXUSB_ConfigVal_l;                        /* Set/Get_Configuration value                           */

UINT8  MAXUSB_Ep1stall_l;                         /* Set by Set_Feature, reported back in Get_Status       */
UINT8  MAXUSB_Ep2stall_l;
UINT8  MAXUSB_Ep3stall_l;

UINT16  MAXUSB_CallsLog;

#define MAXUSB_CALLSLOG_GET_CONFIG            0, 1
#define MAXUSB_CALLSLOG_GET_DESCRIPTION       1, 1
#define MAXUSB_CALLSLOG_GET_INTERFACE         2, 1
#define MAXUSB_CALLSLOG_GET_FEATURE           3, 1
#define MAXUSB_CALLSLOG_GET_REPORT            4, 1
#define MAXUSB_CALLSLOG_SET_REPORT            5, 1
#define MAXUSB_CALLSLOG_GET_STATUS            6, 1
#define MAXUSB_CALLSLOG_SET_IDLE              7, 1

#define MAXUSB_CALLSLOG_EPIN2                 8, 1
#define MAXUSB_CALLSLOG_EPIN3                 9, 1
#define MAXUSB_CALLSLOG_EPIN1                 10,1

/*---------------------------------------------------------------------------------------------------------*/
/* Device Descriptor                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
/*lint -e{572} Excessive shift value (precision Integer shifted right by Integer)                          */
/*---------------------------------------------------------------------------------------------------------*/
const UINT8  DevDescData[DEVICE_DESCRIPTOR_SIZE] = {DEVICE_DESCRIPTIOR};

/*---------------------------------------------------------------------------------------------------------*/
/* Config descriptor                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
/*lint -e{572} Excessive shift value (precision Integer shifted right by Integer)                          */
/*---------------------------------------------------------------------------------------------------------*/
const UINT8 ConfigDescData[CONFIG_DESC_SIZE] = {CONFIG_DESCRITOR};

/*---------------------------------------------------------------------------------------------------------*/
/* Multi-Touch report descriptors                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
const UINT8 ReportDescData_Touch[TOUCH_REPORT_DESC_SIZE]     = {TOUCH_REPORT_DESC};

const UINT8 USB_MaxFingers[]       = {REPORTID_MAXFINGERS, NUM_OF_FINGERS};
const UINT8 USB_FeatureData[]      = {REPORTID_FEATURE, MULTI_INPUT, 0};

/*---------------------------------------------------------------------------------------------------------*/
/* If the number of strings changes, look for USB_STR_0 everywhere and make                                */
/* the obvious changes.                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/

const UINT16 USB_STR_0[ 2] = {0x300 + sizeof(USB_STR_0), 0x0409};
const UINT16 USB_STR_1[25] = {0x300 + sizeof(USB_STR_1), STRING1};
const UINT16 USB_STR_2[19] = {0x300 + sizeof(USB_STR_2), STRING2};
const UINT16 USB_STR_3[18] = {0x300 + sizeof(USB_STR_3), STRING3};
const UINT16 USB_STR_4[ 5] = {0x300 + sizeof(USB_STR_4), STRING4};
const UINT16 USB_STR_5[ 4] = {0x300 + sizeof(USB_STR_5), STRING5};
const UINT16 USB_STR_6[ 8] = {0x300 + sizeof(USB_STR_6), STRING6};
const UINT16 USB_STR_n[17] = {0x300 + sizeof(USB_STR_n), STRINGN};

const UINT8* USB_STRING_DESC[] =
{
   (UINT8*)USB_STR_0,
   (UINT8*)USB_STR_1,
   (UINT8*)USB_STR_2,
   (UINT8*)USB_STR_3,
   (UINT8*)USB_STR_4,
   (UINT8*)USB_STR_5,
   (UINT8*)USB_STR_6,
   (UINT8*)USB_STR_n
};

BULK_GET_DATA_PTR           MAXUSB_GetBulkData_L;
INTERRUPT_GET_DATA_PTR      MAXUSB_GetInterruptData_L;
INTERRUPT_READ_FINISHED_PTR MAXUSB_GetBulkDataFinished_L;
BULK_SET_DATA_PTR           MAXUSB_SetBulkData_L;



/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/* USB Communication interface functions implementation for MaxUSB device                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        HI_Init                                                                                */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine Initializes the MaxUSB device and enumerates it for Multi Touch device    */
/*lint -efunc(715, HI_Init)                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS HI_Init (void* hwParams)
{
    MAXUSB_DeviceInit();

    MAXUSB_GetBulkData_L            = ((USB_TOUCH_CALLBACKS*)hwParams)->bulkGetData;
    MAXUSB_GetInterruptData_L       = ((USB_TOUCH_CALLBACKS*)hwParams)->interruptGetData;
    MAXUSB_GetBulkDataFinished_L    = ((USB_TOUCH_CALLBACKS*)hwParams)->bulkGetDataFinished;
    MAXUSB_SetBulkData_L            = ((USB_TOUCH_CALLBACKS*)hwParams)->bulkSetData;

    return DEFS_STATUS_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        HI_Periodic                                                                           */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles USB suspend and building HID report                               */
/*---------------------------------------------------------------------------------------------------------*/
void HI_Periodic (void)
{
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_DeviceInit                                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initializes and configures Maxim USB peripheral device                    */
/*lint -esym(550, revision)                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
void MAXUSB_DeviceInit(void)
{

    UINT8 pinctlval = BUILD_FIELD_VAL(MAXUSB_PINCTL_FDUPSPI, 1) | BUILD_FIELD_VAL(MAXUSB_PINCTL_POSINT, 1);
    UINT8 revision = 0;

    MAXUSB_Ep3stall_l = MAXUSB_Ep2stall_l = 0;
    MAXUSB_ConfigVal_l = 0;
    MAXUSB_CallsLog = 0;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Initialize the SPI bus                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    GPIO_Init(USB_SPI_CS, GPIO_DIR_OUTPUT, GPIO_PULL_NONE, FALSE);
    GPIO_Write(USB_SPI_CS, 1);
    SPI_Init(SPI_MODULE_0, SPI_WRITE_FALLING_EDGE, 12500000);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure SPI mode and interupt polarity                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(MAXUSB_PINCTL, pinctlval);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Reset the device                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    MAXUSB_Reset();

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read chip revision and verify its validity                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    revision  = REG_READ(MAXUSB_REVISION);
    ASSERT(revision == MAXUSB_REVISION_VAL);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Connect USB                                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(MAXUSB_USBCTL, MAXUSB_USBCTL_CONNECT, 1);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Unmask all the required interrupts of the MaxUSB device                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    MAXUSB_EnableIRQs();

    /*-----------------------------------------------------------------------------------------------------*/
    /* Enable INT pin                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(MAXUSB_CPUCTL, MAXUSB_CPUCTL_IE, 1);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Enable the interrupt on host side                                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    INTERRUPT_REGISTER_AND_ENABLE(INTERRUPT_PROVIDER_ICU, USB_INT, MAXUSB_ServiceIRQs, 0);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_EnableIRQs                                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enables MaxUSB interrupts:                                                */
/*                        SETUP Data Available Interrupt                                                   */
/*                        Endpoint 2 and 3 IN Buffer Available Interrupt                                   */
/*                        and Endpoint 1 OUT Buffer Available Interrupt                                    */
/*                                                                                                         */
/*                        USB Reset Interrup                                                               */
/*                        USB Bus Reset Done Interrupt                                                     */
/*                        No VBUS Interrupt                                                                */
/*---------------------------------------------------------------------------------------------------------*/
void MAXUSB_EnableIRQs(void)
{
    UINT8 epien, usbien;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Enable Endpoint interrupts                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    epien =  BUILD_FIELD_VAL(MAXUSB_EPIEN_SUDAVIE, 1)   |
             BUILD_FIELD_VAL(MAXUSB_EPIEN_IN2BAVIE, 1)  |
             BUILD_FIELD_VAL(MAXUSB_EPIEN_IN3BAVIE, 1)  |
             BUILD_FIELD_VAL(MAXUSB_EPIEN_OUT1DAVIE,1)  ;

    REG_WRITE(MAXUSB_EPIEN,  epien);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Enable USB general interrupts                                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    usbien = BUILD_FIELD_VAL(MAXUSB_USBIEN_URESDNIE, 1);

    REG_WRITE(MAXUSB_USBIEN, usbien);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_ServiceIRQs                                                                     */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine services IRQs from the device                                             */
/* PC Lint  messages surpression:                                                                          */
/*lint -esym(715, intNum)                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void MAXUSB_ServiceIRQs(UINT16 intNum)
{
    volatile UINT8 epirq,usbirq;


    /*-----------------------------------------------------------------------------------------------------*/
    /* Read EPIRQ and USBIRQ registers values from the device and parse the flags                          */
    /*-----------------------------------------------------------------------------------------------------*/
    epirq  = REG_READ(MAXUSB_EPIRQ);
    usbirq = REG_READ(MAXUSB_USBIRQ);

    /*-----------------------------------------------------------------------------------------------------*/
    /* SETUP data available Interrupt Request                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    if (READ_VAR_FIELD(epirq, MAXUSB_EPIRQ_SUDAVIRQ))
    {
        MAXUSB_DoSetup();
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Endpoint 1-OUT Buffer Available Interrupt Request                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    if (READ_VAR_FIELD(epirq, MAXUSB_EPIRQ_OUT1DAVIRQ))
    {
        MAXUSB_ServiceEPOUT1();
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Endpoint 2-IN Buffer Available Interrupt Request                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (READ_VAR_FIELD(epirq, MAXUSB_EPIRQ_IN2BAVIRQ))
    {
        MAXUSB_ServiceEPIN2();
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Endpoint 3-IN Buffer Available Interrupt Request                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (READ_VAR_FIELD(epirq, MAXUSB_EPIRQ_IN3BAVIRQ))
    {
        MAXUSB_ServiceEPIN3();
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* USB Bus Reset Done Interrupt Request                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    if (READ_VAR_FIELD(usbirq, MAXUSB_USBIRQ_URESDNIRQ))
    {
        MAXUSB_EnableIRQs();                 /* ...because a bus reset clears the IE bits                  */
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear the interrupt statuses                                                                        */
    /* Bits are genearly write 1 to clear, however the three BAV bits should never be cleared by write 1   */
    /* they are used by the MAX3420E for the lockout mechanism. They are cleared by writing the data size  */
    /* top the count register                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_VAR_FIELD(epirq, MAXUSB_EPIRQ_IN3BAVIRQ, 0);
    SET_VAR_FIELD(epirq, MAXUSB_EPIRQ_IN2BAVIRQ, 0);
    SET_VAR_FIELD(epirq, MAXUSB_EPIRQ_IN0BAVIRQ, 0);

    REG_WRITE(MAXUSB_EPIRQ,  epirq);
    REG_WRITE(MAXUSB_USBIRQ, usbirq);


}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_DoSetup                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine parses the setup packet received from the USB and performs an approproate */
/*                  action                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void MAXUSB_DoSetup(void)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Read 8 Setup bytes from the FIFO                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    MAXUSB_ReadBuf(REG_ADDR(MAXUSB_SUDFIFO), MAXUSB_SudFifo_l, 8);

    switch ((MAXUSB_SudFifo_l[bmRequestType]&0x60))             /* handle standard requests only           */
    {
        case 0x00:
           switch(MAXUSB_SudFifo_l[bRequest])
           {
                case      SR_GET_DESCRIPTOR:      MAXUSB_GetDescription();    break;
                case      SR_SET_FEATURE:         MAXUSB_SetFeature();        break;
                case      SR_CLEAR_FEATURE:       MAXUSB_ClearFeature();      break;
                case      SR_GET_STATUS:          MAXUSB_GetStatus();         break;
                case      SR_SET_INTERFACE:       MAXUSB_SetInterface();      break;
                case      SR_GET_INTERFACE:       MAXUSB_GetInterface();      break;
                case      SR_GET_CONFIGURATION:   MAXUSB_GetConfiguration();  break;
                case      SR_SET_CONFIGURATION:   MAXUSB_SetConfiguration();  break;
                case      SR_SET_ADDRESS:         MAXUSB_WriteACKSTAT();      break;
                default:  MAXUSB_STALL_EP0();
           }
       break;
      case 0x20:   MAXUSB_Class();      break;
      case 0x40:   MAXUSB_Vendor();     break;
      default:     MAXUSB_STALL_EP0();  break;
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_ServiceEPOUT1                                                                   */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine services EP 1 OUT interrupts, used for bulk data                          */
/*---------------------------------------------------------------------------------------------------------*/
void MAXUSB_ServiceEPOUT1(void)
{
    static UINT8    buff[BULK_OUT_PACKET_SIZE];
    UINT16          size = 0;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read bulk data size                                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    size = REG_READ(MAXUSB_EP1OUTBC);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read bulk data                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    MAXUSB_ReadBuf(REG_ADDR(MAXUSB_EP1OUTFIFO), &buff[0], size);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Send bulk data to reciever                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    EXECUTE_FUNC(MAXUSB_SetBulkData_L, (buff, size));
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_ServiceEPIN2                                                                    */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine services EP 2 IN interrupts, used for bulk plain data reports             */
/*---------------------------------------------------------------------------------------------------------*/
void MAXUSB_ServiceEPIN2(void)
{
    UINT16  size = 0;
    void*   data = 0;

#if defined BULK_MODE
    EXECUTE_FUNC_RET(size, MAXUSB_GetBulkData_L, (&data, BULK_IN_PACKET_SIZE))
#endif

    MAXUSB_WriteBuf(REG_ADDR(MAXUSB_EP2INFIFO), (UINT8*)data, size);
    REG_WRITE(MAXUSB_EP2INBC, (UINT8)size);

#if defined BULK_MODE
    EXECUTE_FUNC(MAXUSB_GetBulkDataFinished_L, (size));
#endif
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_ServiceEPIN3                                                                    */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine services EP 3 IN interrupts, used for touch reports                       */
/*---------------------------------------------------------------------------------------------------------*/
void MAXUSB_ServiceEPIN3(void)
{
     UINT8 size = 0;
     static volatile UINT8 serializedTouchData[NUM_OF_FINGERS*6 + 2];

    if (HID_TouchData.readyToSend == TRUE)
    {

         serializedTouchData[0] = REPORTID_MTOUCH;
         memcpy((void *)&serializedTouchData[1], (void *)HID_TouchData.contactReport,
                        sizeof(HID_TouchData.contactReport));
         serializedTouchData[31] = HID_TouchData.numContacts;
         size = sizeof(serializedTouchData);

         /*------------------------------------------------------------------------------------------------*/
         /* Send report data over the USB                                                                  */
         /*------------------------------------------------------------------------------------------------*/
         MAXUSB_WriteBuf(REG_ADDR(MAXUSB_EP3INFIFO), (UINT8*)serializedTouchData, size);

         /*------------------------------------------------------------------------------------------------*/
         /* Mark current report as being sent                                                              */
         /*------------------------------------------------------------------------------------------------*/
         HID_TouchData.readyToSend = FALSE;

    }
    REG_WRITE(MAXUSB_EP3INBC, size);
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_SetConfiguration                                                                */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  TThis routine is Chapter 9 SetConfig command                                           */
/*---------------------------------------------------------------------------------------------------------*/
void MAXUSB_SetConfiguration(void)
{
    MAXUSB_ConfigVal_l = MAKE16(MAXUSB_SudFifo_l[wValueL], MAXUSB_SudFifo_l[wValueH]);
    MAXUSB_WriteACKSTAT();

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_GetConfiguration                                                                */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is Chapter 9 GetConfig command                                            */
/*                  See section 9.4.2 (page 189) of the USB 1.1 Specification.                             */
/*---------------------------------------------------------------------------------------------------------*/
void MAXUSB_GetConfiguration(void)
{
    MAXUSB_WriteBuf(REG_ADDR(MAXUSB_EP0FIFO), (UINT8*)&MAXUSB_ConfigVal_l, (UINT16)sizeof(MAXUSB_ConfigVal_l));
    REG_WRITE(MAXUSB_EP0BC, sizeof(MAXUSB_ConfigVal_l));
    SET_VAR_FIELD(MAXUSB_CallsLog, MAXUSB_CALLSLOG_GET_CONFIG, 1);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_SetInterface                                                                    */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is Chapter 9 SetInterface command                                         */
/*                  See section 9.4.10 (page 195) of the USB 1.1 Specification.                            */
/*---------------------------------------------------------------------------------------------------------*/
void MAXUSB_SetInterface(void)
{
    if (MAXUSB_ConfigVal_l == 0)
    {
        MAXUSB_WriteACKSTAT();
    }
    else
    {
        MAXUSB_STALL_EP0();
    }
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_GetInterface                                                                    */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is Chapter 9 GetInterface command                                         */
/*                  See section 9.4.4 (page 190) of the USB 1.1 Specification                              */
/*                  Check for Interface=0, always report AlternateSetting=0                                */
/*---------------------------------------------------------------------------------------------------------*/
void MAXUSB_GetInterface(void)
{

    SET_VAR_FIELD(MAXUSB_CallsLog, MAXUSB_CALLSLOG_GET_INTERFACE, 1);
    if (MAXUSB_SudFifo_l[wIndexL]==0)
      {
        REG_WRITE(MAXUSB_EP0FIFO, 0);
        REG_WRITE(MAXUSB_EP0BC, 1);         /* Send one byte  */
        MAXUSB_WriteACKSTAT();
      }
    else
    {
        MAXUSB_STALL_EP0();
    }
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_GetStatus                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is a USB Chapter 9 GetStatus command                                      */
/*     See section 9.4.5 (page 190) of the USB 1.1 Specification.                                          */
/*---------------------------------------------------------------------------------------------------------*/
void MAXUSB_GetStatus(void)
{
    UINT8 reqType = MAXUSB_SudFifo_l[bmRequestType];

    switch(reqType)
        {
        /*-------------------------------------------------------------------------------------------------*/
        /* Directed to the DEVICE                                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        case 0x80:
            REG_WRITE(MAXUSB_EP0FIFO, 1);   /* first byte is 000000rs where r=enabled for RWU (not) and    */
                                            /* s=self-powered.                                             */
            REG_WRITE(MAXUSB_EP0FIFO, 0);   /* second byte is always 0                                     */
            REG_WRITE(MAXUSB_EP0BC, 2);     /* load byte count, arm the IN transfer,                       */
            MAXUSB_WriteACKSTAT();          /* ACK the status stage of the CTL transfer                    */
            break;
        /*-------------------------------------------------------------------------------------------------*/
        /* Directed to the INTERFACE                                                                       */
        /*-------------------------------------------------------------------------------------------------*/
        case 0x81:
            REG_WRITE(MAXUSB_EP0FIFO, 0);   /* 2 zero bytes */
            REG_WRITE(MAXUSB_EP0FIFO, 0);
            REG_WRITE(MAXUSB_EP0BC, 2);     /* load byte count, arm the IN transfer,                       */
            MAXUSB_WriteACKSTAT();          /* ACK the status stage of the CTL transfer                    */
            break;
        /*-------------------------------------------------------------------------------------------------*/
        /* Directed to the ENDPOINT                                                                        */
        /*-------------------------------------------------------------------------------------------------*/
        case 0x82:
            if (( MAXUSB_SudFifo_l[wIndexL] == INTERRUPT_EP_TOUCH)  ||
                ( MAXUSB_SudFifo_l[wIndexL] == BULK_EP_IN)          ||
                ( MAXUSB_SudFifo_l[wIndexL] == BULK_EP_OUT))
            {
                if (MAXUSB_SudFifo_l[wIndexL] == BULK_EP_IN)
                {
                    REG_WRITE(MAXUSB_EP0FIFO, MAXUSB_Ep1stall_l);   /* first byte is 0000000h where h is the halt (stall) bit*/
                }
                else if (MAXUSB_SudFifo_l[wIndexL] == BULK_EP_IN)
                {
                   REG_WRITE(MAXUSB_EP0FIFO, MAXUSB_Ep2stall_l);   /* first byte is 0000000h where h is the halt (stall) bit*/
                }
                else if (MAXUSB_SudFifo_l[wIndexL]==INTERRUPT_EP_TOUCH)
                {
                   REG_WRITE(MAXUSB_EP0FIFO, MAXUSB_Ep3stall_l);   /* first byte is 0000000h where h is the halt (stall) bit*/
                }

                REG_WRITE(MAXUSB_EP0FIFO, 0);          /* second byte is always 0                          */
                REG_WRITE(MAXUSB_EP0BC, 2);            /* load byte count, arm the IN transfer,            */
                MAXUSB_WriteACKSTAT();                 /* ACK the status stage of the CTL transfer         */
            }
            else
            {
                MAXUSB_STALL_EP0();                    /* Host tried to stall an invalid endpoint (not 3)  */
            }
           break;
        default:
            MAXUSB_STALL_EP0();       // don't recognize the request
        }

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_SetFeature                                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is USB Chapter 9 SetFeature command                                       */
/*                  See section 9.4.9 (page 194) of the USB 1.1 Specification.                             */
/*---------------------------------------------------------------------------------------------------------*/
void MAXUSB_SetFeature(void)
{
  UINT8 mask;

  if ((MAXUSB_SudFifo_l[bmRequestType]==0x02) // dir=h->p, recipient = ENDPOINT
      &&  (MAXUSB_SudFifo_l[wValueL]==0x00))  // wValueL is feature selector, 00 is EP Halt

  {
      if (MAXUSB_SudFifo_l[wIndexL] == INTERRUPT_EP_TOUCH)
      {
          mask = REG_READ(MAXUSB_EPSTALLS) |
                        BUILD_FIELD_VAL(MAXUSB_EPSTALLS_STLEP3IN, 1)|
                        BUILD_FIELD_VAL(MAXUSB_EPSTALLS_ACKSTAT, 1);

          MAXUSB_Ep3stall_l = 1;
          REG_WRITE(MAXUSB_EPSTALLS, mask);
      }
      else if (MAXUSB_SudFifo_l[wIndexL] == BULK_EP_IN)
      {
          mask = REG_READ(MAXUSB_EPSTALLS) |
                        BUILD_FIELD_VAL(MAXUSB_EPSTALLS_STLEP2IN, 1)|
                        BUILD_FIELD_VAL(MAXUSB_EPSTALLS_ACKSTAT, 1);

          MAXUSB_Ep2stall_l = 1;
          REG_WRITE(MAXUSB_EPSTALLS, mask);
      }
      else if (MAXUSB_SudFifo_l[wIndexL] == BULK_EP_OUT)
      {
          mask = REG_READ(MAXUSB_EPSTALLS) |
                        BUILD_FIELD_VAL(MAXUSB_EPSTALLS_STLEP1OUT, 1)|
                        BUILD_FIELD_VAL(MAXUSB_EPSTALLS_ACKSTAT, 1);

          MAXUSB_Ep1stall_l = 1;
          REG_WRITE(MAXUSB_EPSTALLS, mask);
      }
  }
  else
  {
    MAXUSB_STALL_EP0();
  }
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_ClearFeature                                                                    */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is USB Chapter 9 ClearFeature command.                                    */
/*                  See section 9.4.1 (page 188) of the USB 1.1 Specification.                             */
/*---------------------------------------------------------------------------------------------------------*/
void MAXUSB_ClearFeature(void)
{
  UINT8 mask;

  if ((MAXUSB_SudFifo_l[bmRequestType]==0x02)               /* dir=h->p, recipient = ENDPOINT              */
      &&  (MAXUSB_SudFifo_l[wValueL]==0x00))                /* wValueL is feature selector, 00 is EP Halt  */
  {
      if (MAXUSB_SudFifo_l[wIndexL] == INTERRUPT_EP_TOUCH)
      {
          mask = REG_READ(MAXUSB_EPSTALLS) | BUILD_FIELD_VAL(MAXUSB_EPSTALLS_ACKSTAT, 1);
          SET_VAR_FIELD(mask, MAXUSB_EPSTALLS_STLEP3IN, 0);

          MAXUSB_Ep3stall_l = 0;
          SET_REG_FIELD(MAXUSB_CLRTOGS,MAXUSB_CLRTOGS_CTGEP3IN, 1);       /* clear the EP3 data toggle     */
          REG_WRITE(MAXUSB_EPSTALLS, mask);
      }
      else if (MAXUSB_SudFifo_l[wIndexL] == BULK_EP_IN)
      {
          mask = REG_READ(MAXUSB_EPSTALLS) | BUILD_FIELD_VAL(MAXUSB_EPSTALLS_ACKSTAT, 1);
          SET_VAR_FIELD(mask, MAXUSB_EPSTALLS_STLEP2IN, 0);

          MAXUSB_Ep2stall_l =  0;
          SET_REG_FIELD(MAXUSB_CLRTOGS, MAXUSB_CLRTOGS_CTGEP2IN, 1);      /* clear the EP2 data toggle      */
          REG_WRITE(MAXUSB_EPSTALLS, mask);
      }
      else if (MAXUSB_SudFifo_l[wIndexL] == BULK_EP_OUT)
      {
          mask = REG_READ(MAXUSB_EPSTALLS) | BUILD_FIELD_VAL(MAXUSB_EPSTALLS_ACKSTAT, 1);
          SET_VAR_FIELD(mask, MAXUSB_EPSTALLS_STLEP1OUT, 0);

          MAXUSB_Ep1stall_l =  0;
          SET_REG_FIELD(MAXUSB_CLRTOGS, MAXUSB_CLRTOGS_CTGEP1OUT, 1);      /* clear the EP1 data toggle      */
          REG_WRITE(MAXUSB_EPSTALLS, mask);
      }
  }
  else
  {
    MAXUSB_STALL_EP0();
  }
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_GetDescription                                                                  */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*      The Device Request can ask for Device/Config/string/interface/endpoint                             */
/*      descriptors (via wValue). We then post an IN response to return the                                */
/*      requested descriptor.                                                                              */
/*      And then wait for the OUT which terminates the control transfer.                                   */
/*      See section 9.4.3 (page 189) of the USB 1.1 Specification.                                         */
/* PC Lint  messages surpression:                                                                          */
/*lint -efunc(662, MAXUSB_GetDescription)                                                                  */
/*lint -efunc(670, MAXUSB_WriteBuf)                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

void MAXUSB_GetDescription(void)
{
    INT16 reqlen, sendlen, desclen, curLen;
    UINT8 *pDdata = NULL;
    desclen = 0;
    reqlen = (INT16)MAKE16(MAXUSB_SudFifo_l[wLengthL], MAXUSB_SudFifo_l[wLengthH]);

    switch (MAXUSB_SudFifo_l[wValueH])                                         /* wValueH is descriptor types */
    {
        case  GD_DEVICE:
                  desclen = DEVICE_DESCRIPTOR_SIZE;
                  pDdata = (UINT8*)DevDescData;
                  break;
        case  GD_CONFIGURATION:
                  desclen = CONFIG_DESC_SIZE;
                  pDdata = (UINT8*)ConfigDescData;
                  break;
        case  GD_STRING:
                  desclen = USB_STRING_DESC[MAXUSB_SudFifo_l[wValueL]][0];     /* wValueL=string index,     */
                                                                               /* array[0] is the length    */
                  pDdata = (UINT8*)USB_STRING_DESC[MAXUSB_SudFifo_l[wValueL]]; /* point to first element    */

                  break;
        case  GD_REPORT:
                  if (MAXUSB_SudFifo_l[wIndexL] == 1)
                  {
                      desclen = TOUCH_REPORT_DESC_SIZE;
                      pDdata = (UINT8*)ReportDescData_Touch;
                  }
                  break;
        default:
                  desclen = 0;
                  break;
    }

    if ((desclen != 0) && (pDdata != NULL))                    /* one of the case statements above filled     */
                                                               /* in a value                                  */
    {
        sendlen = MIN(reqlen, desclen);                        /* send the smaller of requested and avaiable  */

        do
        {
            curLen =  MIN(sendlen, CONTROL_MAX_PACKET_SIZE);

            MAXUSB_WriteBuf(REG_ADDR(MAXUSB_EP0FIFO), pDdata, (UINT8)curLen);
            REG_WRITE(MAXUSB_EP0BC, (UINT8)curLen);         /* load EP0BC to arm the EP0-IN transfer       */

            sendlen -= curLen;

            if (sendlen >  0)
            {
                while(READ_REG_FIELD(MAXUSB_EPIRQ, MAXUSB_EPIRQ_IN0BAVIRQ) == 0) ;
                pDdata += curLen;
            }

        } while (sendlen >  0);

        MAXUSB_WriteACKSTAT();

    }
    else
    {
        MAXUSB_STALL_EP0();                                /* none of the descriptor types match         */
    }
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_Vendor                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is  Chapter 9 Vendor specific request                                     */
/*                  See section 9.4.11 (page 195) of the USB 1.1 Specification                             */
/*---------------------------------------------------------------------------------------------------------*/
void MAXUSB_Vendor()
{
#if 0
    switch ((MAXUSB_SudFifo_l[bRequest]))
    {
      case 0x00:       DIAG_Setup(DIAG_MODE_1);   break;
      case 0x01:       DIAG_Setup(DIAG_MODE_2);   break;
      default:         MAXUSB_STALL_EP0();        break;
    }
#endif   /* IF 0 */
    MAXUSB_STALL_EP0();

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_Class                                                                           */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is  Chapter 9 Class specific request                                      */
/*                  See section 9.4.11 (page 195) of the USB 1.1 Specification                             */
/*---------------------------------------------------------------------------------------------------------*/
void MAXUSB_Class()
{
    switch ((MAXUSB_SudFifo_l[bRequest]))
    {
      case 0x01:       MAXUSB_GetReport();        break;
      case 0x09:       MAXUSB_SetReport();        break;
      case 0x0a:       MAXUSB_SetIdle();          break;
      default:         MAXUSB_STALL_EP0();        break;
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_GetReport                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is Chapter 9 Class specific request                                       */
/*                  See section 9.4.11 (page 195) of the USB 1.1 Specification                             */
/*---------------------------------------------------------------------------------------------------------*/
void MAXUSB_GetReport()
{
    UINT8* reportPtr = NULL;
    UINT32 reportSize = 0;

    switch (MAXUSB_SudFifo_l[wValueL])
    {
        case REPORTID_MAXFINGERS:
            reportPtr  = (UINT8*)USB_MaxFingers;
            reportSize = sizeof(USB_MaxFingers);
            break;
        case REPORTID_MTOUCH:
            reportPtr  = (UINT8*)&HID_TouchData;
            reportSize = sizeof(HID_TouchData);
            HID_TouchData.reportId = REPORTID_MTOUCH;
            break;
        case REPORTID_FEATURE:
            reportPtr  = (UINT8*)USB_FeatureData;
            reportSize = sizeof(USB_FeatureData);
            break;
        default:
            MAXUSB_STALL_EP0();
            break;
      }

    if (reportPtr != NULL)
    {
        MAXUSB_WriteBuf(REG_ADDR(MAXUSB_EP0FIFO), reportPtr, (UINT8)reportSize);
        REG_WRITE(MAXUSB_EP0BC, (UINT8)reportSize);             /* load EP0BC to arm the EP0-IN transfer              */
        MAXUSB_WriteACKSTAT();
        SET_VAR_FIELD(MAXUSB_CallsLog, MAXUSB_CALLSLOG_GET_REPORT, 1);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_SetReport                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is Chapter 9 Class specific request                                       */
/*                  See section 9.4.11 (page 195) of the USB 1.1 Specification                             */
/*---------------------------------------------------------------------------------------------------------*/
void  MAXUSB_SetReport()
{
    static UINT8 USB_EP0Data[MAX_CONTROL_DATA_PAYLOAD];
    UINT16 reportSize = MIN(MAXUSB_SudFifo_l[wLengthL],MAX_CONTROL_DATA_PAYLOAD);

    MAXUSB_ReadBuf(REG_ADDR(MAXUSB_EP0FIFO), USB_EP0Data, reportSize);

    MAXUSB_WriteACKSTAT();
    SET_VAR_FIELD(MAXUSB_CallsLog, MAXUSB_CALLSLOG_SET_REPORT, 1);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Handle the Set Report command                                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    if ((MAXUSB_SudFifo_l[wValueL]) == REPORTID_NUVOTON)
    {
        //TRANSPORT_SetReport(USB_EP0Data, reportSize);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_SetIdle                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is Chapter 9 Class specific request                                       */
/*                  See section 9.4.11 (page 195) of the USB 1.1 Specification                             */
/*---------------------------------------------------------------------------------------------------------*/
void MAXUSB_SetIdle()
{

   /*------------------------------------------------------------------------------------------------------*/
   /* SET_IDLE is a No data phase transaction from HID class. All it needsis a terminating IN token        */
   /*------------------------------------------------------------------------------------------------------*/
   MAXUSB_WriteACKSTAT();
   SET_VAR_FIELD(MAXUSB_CallsLog, MAXUSB_CALLSLOG_SET_IDLE, 1);

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_Reset                                                                           */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine resets the Maxim USB peripheral device                                    */
/*---------------------------------------------------------------------------------------------------------*/
void MAXUSB_Reset(void)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Reset the chip                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(MAXUSB_USBCTL,0x20);


    /*-----------------------------------------------------------------------------------------------------*/
    /* Remove the reset                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(MAXUSB_USBCTL,0x00);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Wait for oscillator stabilization                                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    while(READ_REG_FIELD(MAXUSB_USBIRQ, MAXUSB_USBIRQ_OSCOKIRQ) == 0) ;
}



/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/* MaxUSB peripheral device low level access routines                                                      */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_WriteReg                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  address - address of MaxUSB register                                                   */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Writes to MaxUSB register                                                              */
/*---------------------------------------------------------------------------------------------------------*/
static void MAXUSB_WriteReg(UINT8 address, UINT8 val)
{
    UINT16 writeTrans  = MAKE16(MAXUSB_WRITE_COMMAND(address), val);

    SPI_WriteRead(SPI_MODULE_0, USB_SPI_CS,
           (UINT8*)&writeTrans,
           2,
           NULL,
           0);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_WriteACKSTAT                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Writes to ACKSTAT bit of MaxUSB                                                        */
/*---------------------------------------------------------------------------------------------------------*/
static void MAXUSB_WriteACKSTAT()
{
    UINT8 writeTrans  = MAXUSB_WRITE_ACKSTAT(0);

    SPI_WriteRead(SPI_MODULE_0, USB_SPI_CS,
           &writeTrans,
           1,
           NULL,
           0);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_WriteBuf                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  address - address of MaxUSB register                                                   */
/*                  buf     - a buffger to write to MaxUSB internal FIFO                                   */
/*                  size    - size of buffer to write to MaxUSB internal FIFO                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Writes a buffer to MaxUSB internal FIFO                                                */
/*---------------------------------------------------------------------------------------------------------*/
static void MAXUSB_WriteBuf(UINT8 address, UINT8* buf, UINT16 size)
{
    UINT8 tempBuf[size+1];

    if ((buf == NULL) || (size == 0))
        return;


    memcpy((void*)(tempBuf+1), (void*)buf, (size_t)size);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Store the command as a first byte of the buffer to write                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    tempBuf[0] = MAXUSB_WRITE_COMMAND(address);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Perform SPI transaction                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    SPI_WriteRead(SPI_MODULE_0, USB_SPI_CS,
           tempBuf,
           (size + 1),
           NULL,
           0);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CAPKEY_WriteIndex                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  address - address of MaxUSB register                                                   */
/*                                                                                                         */
/* Returns:         value read from the MaxUSB register                                                    */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Reads MaxUSB internal register                                                         */
/*---------------------------------------------------------------------------------------------------------*/
static UINT8 MAXUSB_ReadReg(UINT8 address)
{
    UINT8 writeTrans  = MAXUSB_READ_COMMAND(address);
    UINT8 readVal;

    SPI_WriteRead(SPI_MODULE_0, USB_SPI_CS,
           &writeTrans,
           1,
           &readVal,
           1);
    return readVal;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        MAXUSB_ReadBuf                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  address - address of MaxUSB register                                                   */
/*                  buf     - a buffger to store contents of MaxUSB internal FIFO                          */
/*                  size    - size of buffer to read from MaxUSB internal FIFO                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Reads a buffer from MaxUSB internal FIFO                                               */
/*---------------------------------------------------------------------------------------------------------*/
static void MAXUSB_ReadBuf(UINT8 address, UINT8* buf, UINT16 size)
{
    UINT8 readCmd  = MAXUSB_READ_COMMAND(address);

    SPI_WriteRead(SPI_MODULE_0, USB_SPI_CS,
           &readCmd,
           1,
           buf,
           size);
}

#undef USB_TOUCH_GLOBALS

/*lint +e830                                                                                               */
/* EOF */
