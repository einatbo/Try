/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2010 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   serial_touch.h                                                                                        */
/*            This file contains protocol definitions for the Nuvoton Serial Touch protocol                */
/* Project:                                                                                                */
/*            Multi Touch                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef  SERIAL_TOUCH_H
#define  SERIAL_TOUCH_H

#include "defs.h"

#ifdef  SERIAL_TOUCH_GLOBALS
#define SERIAL_TOUCH_H_EXT
#else
#define SERIAL_TOUCH_H_EXT extern
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Non-HID transport protocol definitions                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define MAX_TOUCH_POINTS                   2

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/* Devce-to-host report types                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
typedef enum 
{
    REPORT_ID_REPORT_VERSION               = 0x80,
    REPORT_ID_REPORT_SELF_TEST_RESULTS     = 0x82,    
    REPORT_ID_REPORT_RESOLUTION            = 0x84,
    REPORT_ID_REPORT_PARAMETER             = 0x86,    
    REPORT_ID_REPORT_COORDINATES_ABSOLUTE  = 0x88 
    
} SERIAL_TOUCH_REPORT_T;

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/* Host-to-device commands types                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    HOST_COMMAND_PERFORM_SET_PARAMETER    = 0x02,
    HOST_COMMAND_PERFORM_SELF_TEST        = 0x04,
    HOST_COMMAND_ENTER_FW_UPGRADE_MODE    = 0x06,
    HOST_COMMAND_WRITE_FW_UPGRADE_BLOCK   = 0x08,
    HOST_COMMAND_EXIT_FW_UPGRADE_BLOCK    = 0x0A,

    HOST_COMMAND_GET_VERSION              = REPORT_ID_REPORT_VERSION,
    HOST_COMMAND_GET_SELF_TEST_RESULTS    = REPORT_ID_REPORT_SELF_TEST_RESULTS,
    HOST_COMMAND_GET_RESOLUTION           = REPORT_ID_REPORT_RESOLUTION,
    HOST_COMMAND_GET_PARAMETER            = REPORT_ID_REPORT_PARAMETER,
    HOST_COMMAND_GET_COORDINATES_REPORT   = REPORT_ID_REPORT_COORDINATES_ABSOLUTE

} SERIAL_TOUCH_HOST_COMMAND_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Coordinates report structure                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    UINT8  contactID;
    UINT16 X;
    UINT16 Y;
    UINT8  width;
    UINT8  status;
    
} SERIAL_TOUCH_ContactStructure_t;

typedef struct
{
    UINT8   reportID;
    UINT8   numContacts;
    SERIAL_TOUCH_ContactStructure_t contact[MAX_TOUCH_POINTS];
} SERIAL_TOUCH_GetCoordinatesReport_t;

/*---------------------------------------------------------------------------------------------------------*/
/* Version report structure                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    UINT8   reportID;
    UINT8   minor;
    UINT8   major;
    UINT8   revision;
} SERIAL_TOUCH_GetFirmwareVersionReport_t;

/*---------------------------------------------------------------------------------------------------------*/
/* Resolution report structure                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    UINT8   reportID;
    UINT16  Xmax;
    UINT16  Ymax;
} SERIAL_TOUCH_ResolutionReport_t;


/*---------------------------------------------------------------------------------------------------------*/
/* Set/Get Parameter command structure                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    UINT8   commmandID;
    UINT8   parameterNum;
} SERIAL_TOUCH_GetParameterCommand_t;

/*---------------------------------------------------------------------------------------------------------*/
/* Set Parameter command structure                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    UINT8   commmandID;
    UINT8   parameterNum;
    UINT16  parameter;    
} SERIAL_TOUCH_SetParameterCommand_t;


/*---------------------------------------------------------------------------------------------------------*/
/* Write Firmware block command structure                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    UINT8  commmandID;
    UINT8  offset;
    UINT8  size;    
} SERIAL_TOUCH_WriteBlockCommand_t;

/*---------------------------------------------------------------------------------------------------------*/
/* Other commands contain only commandID field, no structure for them                                      */
/*---------------------------------------------------------------------------------------------------------*/



#endif
