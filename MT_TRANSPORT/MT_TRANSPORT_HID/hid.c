/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2011 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   hid.c                                                                                                 */
/*            HID specific, communication independent functionality                                        */
/* Project:                                                                                                */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

#define HID_GLOBALS
#include "transport_if.h"
#include "hid.h"
#include "hal.h"
#include <string.h>

/*---------------------------------------------------------------------------------------------------------*/
/* HID module definitions                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef  X_OFFSET
#define X_OFFSET                        200
#endif
#ifndef  Y_OFFSET
#define Y_OFFSET                        200
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_Init                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initializes HID logic                                                     */
/* PC Lint  messages surpression:                                                                          */
/*lint -efunc(715, TRANSPORT_Init)                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS TRANSPORT_Init (void* params)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Initialize globals                                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    memset((void*)&HID_TouchData, 0, sizeof(HID_TouchData));

    return DEFS_STATUS_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_Start                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initializes a set of points to be reported                                */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS TRANSPORT_SendTouchReport(TOUCH_POINT_T* pointsToReport, UINT numPoints)
{
    UINT i, ie = 0;

    if (numPoints > MAX_TOUCH_POINTS)
    {
        numPoints = MAX_TOUCH_POINTS;
    }

    INTERRUPTS_SAVE_DISABLE(ie);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Fill HID report                                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    memset((void *)&HID_TouchData, 0, sizeof(HID_TouchDataStruct));

    HID_TouchData.numContacts = (UINT8)numPoints;

    for (i = 0; i < numPoints; i++)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Adjust X, Y coordinates to the desired orienation and screen resolution                         */
        /*-------------------------------------------------------------------------------------------------*/
        UINT16 xCoord = (UINT16)pointsToReport[i].coordinates.X;
        UINT16 yCoord = (UINT16)pointsToReport[i].coordinates.Y;

#if defined PANEL_X_SIZE && defined HOST_X_SIZE && (PANEL_X_SIZE != HOST_X_SIZE)
        xCoord = xCoord*PANEL_X_SIZE/HOST_X_SIZE;
#endif
#if defined PANEL_Y_SIZE && defined HOST_Y_SIZE && (PANEL_Y_SIZE != HOST_Y_SIZE)
        yCoord = yCoord*PANEL_Y_SIZE/HOST_Y_SIZE;
#endif
        xCoord += X_OFFSET;
        yCoord += Y_OFFSET;

        HID_TouchData.contactReport[i].finger_status = pointsToReport[i].status;
        HID_TouchData.contactReport[i].finger_id     = pointsToReport[i].contactId;
        HID_TouchData.contactReport[i].finger_x_low  = LSB(xCoord);
        HID_TouchData.contactReport[i].finger_x_high = MSB(xCoord);
        HID_TouchData.contactReport[i].finger_y_low  = LSB(yCoord);
        HID_TouchData.contactReport[i].finger_y_high = MSB(yCoord);
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Report is ready                                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    HID_TouchData.readyToSend = TRUE;

    INTERRUPTS_RESTORE(ie);

    return DEFS_STATUS_OK;
}

#undef HID_GLOBALS
