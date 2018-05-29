/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2011 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   hid.h                                                                                                 */
/*            This file contains ...                                                                       */
/* Project:                                                                                                */
/*            HID specific but not communication dependent defintions                                      */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef  HID_H
#define  HID_H

#ifdef  HID_GLOBALS
#define HID_H_EXT
#else
#define HID_H_EXT extern
#endif

#include "defs.h"
#include "platform.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Definitions                                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define REPORTID_NONE                                0
#define REPORTID_MOUSE                               1
#define REPORTID_MAXFINGERS                          2
#define REPORTID_MTOUCH                              3
#define REPORTID_FEATURE                             5
#define REPORTID_NUVOTON                             6
#define REPORTID_INVALID                             0xff

#define NUM_OF_FINGERS                               5               /* Max number of contacts             */
#define MULTI_INPUT                                  2               /* Multi-point device                 */

#define SINGLE_TO_MULTI_DELAY                        0

/*---------------------------------------------------------------------------------------------------------*/
/* Multi-Touch report descriptor                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define LOGICAL_MAX_X                                 HOST_X_RESOLUTION
#define LOGICAL_MAX_Y                                 HOST_Y_RESOLUTION

#define PHYSICAL_MAX_X                                HOST_X_RESOLUTION
#define PHYSICAL_MAX_Y                                HOST_Y_RESOLUTION

#define ENABLE_NUVOTON_REPORT                         1


/*---------------------------------------------------------------------------------------------------------*/
/* Multi touch HID report descriptor                                                                       */
/*---------------------------------------------------------------------------------------------------------*/

#define FINGER_DESCRIPTOR                                                                              \
                                                                                                       \
    0xa1, 0x02,                                       /*    COLLECTION (Logical)                    */ \
    0x05, 0x0d,                                       /*       USAGE_PAGE (Digitizers)              */ \
    0x09, 0x42,                                       /*       USAGE (Tip Switch)                   */ \
    0x15, 0x00,                                       /*       LOGICAL_MINIMUM (0)                  */ \
    0x25, 0x01,                                       /*       LOGICAL_MAXIMUM (1)                  */ \
    0x75, 0x01,                                       /*       REPORT_SIZE (1)                      */ \
    0x95, 0x01,                                       /*       REPORT_COUNT (1)                     */ \
    0x81, 0x02,                                       /*       INPUT (Data,Var,Abs)                 */ \
                                                                                                       \
    0x09, 0x32,                                       /*       USAGE (In Range)                     */ \
    0x81, 0x02,                                       /*       INPUT (Data,Var,Abs)                 */ \
                                                                                                       \
    0x09, 0x47,                                       /*       USAGE (Confidence)                   */ \
    0x81, 0x02,                                       /*       INPUT (Data,Var,Abs)                 */ \
                                                                                                       \
    0x95, 0x05,                                       /*       REPORT_COUNT (5)                     */ \
    0x81, 0x03,                                       /*       INPUT (Cnst,Ary,Abs)                 */ \
                                                                                                       \
    0x75, 0x08,                                       /*       REPORT_SIZE (8)                      */ \
    0x09, 0x51,                                       /*       USAGE (Cotact Identifier)            */ \
    0x15, 0x01,                                       /*       LOGICAL_MINIMUM (1)                  */ \
    0x25, 0xFE,                                       /*       LOGICAL_MAXIMUM                      */ \
    0x95, 0x01,                                       /*       REPORT_COUNT (1)                     */ \
    0x81, 0x02,                                       /*       INPUT (Data,Var,Abs)                 */ \
                                                                                                       \
    0x05, 0x01,                                       /*       USAGE_PAGE (Generic Desktop)         */ \
    0x26, LSB(LOGICAL_MAX_X), MSB(LOGICAL_MAX_X),     /*       LOGICAL_MAXIMUM                      */ \
    0x75, 0x10,                                       /*       REPORT_SIZE (16)                     */ \
    0x09, 0x30,                                       /*       USAGE (X)                            */ \
    0x35, 0x00,                                       /*       PHYSICAL_MINIMUM (0)                 */ \
    0x46, LSB(PHYSICAL_MAX_X), MSB(PHYSICAL_MAX_X),   /*       PHYSICAL_MAXIMUM                     */ \
    0x81, 0x02,                                       /*       INPUT (Data,Var,Abs)                 */ \
                                                                                                       \
    0x26, LSB(LOGICAL_MAX_Y), MSB(LOGICAL_MAX_Y),     /*       LOGICAL_MAXIMUM                      */ \
    0x75, 0x10,                                       /*       REPORT_SIZE (16)                     */ \
    0x09, 0x31,                                       /*       USAGE (Y)                            */ \
    0x35, 0x00,                                       /*       PHYSICAL_MINIMUM (0)                 */ \
    0x46, LSB(PHYSICAL_MAX_Y), MSB(PHYSICAL_MAX_Y),   /*       PHYSICAL_MAXIMUM                     */ \
    0x81, 0x02,                                       /*       INPUT (Data,Var,Abs)                 */ \
    0xc0                                              /*    END_COLLECTION                          */



#define FINGER_DESCRIPTOR_SIZE    71

#define DIGITIZER_REPORT_DESC                                                                          \
    0x05, 0x0d,                                       /* USAGE_PAGE (Digitizers)                    */ \
    0x09, 0x04,                                       /* USAGE (Touch Screen)                       */ \
    0xa1, 0x01,                                       /* COLLECTION (Application)                   */ \
    0x85, REPORTID_MTOUCH,                            /*    REPORT_ID (Touch)                       */ \
    0x09, 0x22,                                       /*    USAGE (Finger)                          */ \
    FINGER_DESCRIPTOR,                                /*    Finger 1 desciptor                      */ \
    FINGER_DESCRIPTOR,                                /*    Finger 2 desciptor                      */ \
    FINGER_DESCRIPTOR,                                /*    Finger 3 desciptor                      */ \
    FINGER_DESCRIPTOR,                                /*    Finger 4 desciptor                      */ \
    FINGER_DESCRIPTOR,                                /*    Finger 5 desciptor                      */ \
                                                                                                       \
    0x05, 0x0d,                                       /*    USAGE_PAGE (Digitizers)                 */ \
    0x09, 0x54,                                       /*    USAGE (Contact Count)                   */ \
    0x95, 0x01,                                       /*    REPORT_COUNT (1)                        */ \
    0x75, 0x08,                                       /*    REPORT_SIZE (8)                         */ \
    0x81, 0x02,                                       /*    INPUT (Data,Var,Abs)                    */ \
                                                                                                       \
    0x85, REPORTID_MAXFINGERS,                        /*    REPORT_ID (Feature)                     */ \
    0x09, 0x55,                                       /*    USAGE(Contact Count Maximum)            */ \
    0x25, (NUM_OF_FINGERS+1),                         /*    LOGICAL_MAXIMUM                         */ \
    0xb1, 0x02,                                       /*    FEATURE (Data,Var,Abs)                  */ \
    0xc0,                                             /* END_COLLECTION                             */ \
                                                                                                       \
    0x09, 0x0E,                                       /* USAGE (Configuration)                      */ \
    0xa1, 0x01,                                       /* COLLECTION (Application)                   */ \
    0x85, REPORTID_FEATURE,                           /*   REPORT_ID (Feature)                      */ \
    0x09, 0x22,                                       /*   USAGE (Finger)                           */ \
    0xa1, 0x00,                                       /*   COLLECTION (Physical)                    */ \
    0x09, 0x52,                                       /*    USAGE (Input Mode)                      */ \
    0x09, 0x53,                                       /*    USAGE (Device Index                     */ \
    0x15, 0x00,                                       /*    LOGICAL_MINIMUM (0)                     */ \
    0x25, 0x0a,                                       /*    LOGICAL_MAXIMUM (10)                    */ \
    0x75, 0x08,                                       /*    REPORT_SIZE (8)                         */ \
    0x95, 0x02,                                       /*    REPORT_COUNT (2)                        */ \
    0xb1, 0x02,                                       /*   FEATURE (Data,Var,Abs                    */ \
    0xc0,                                             /*   END_COLLECTION                           */ \
    0xc0                                              /* END_COLLECTION                             */

#define NUVOTON_REPORT_DESC                                                                            \
    0x05, 0x01,                                       /* USAGE_PAGE (Generic Desktop Controls)      */ \
    0x09, 0x0E,                                       /* USAGE (Configuration)                      */ \
    0xa1, 0x01,                                       /* COLLECTION (Application)                   */ \
    0x85, REPORTID_NUVOTON,                           /*   REPORT_ID (Feature)                      */ \
    0xa1, 0x00,                                       /*   COLLECTION (Physical)                    */ \
    0x09, 0x52,                                       /*    USAGE (Input Mode)                      */ \
    0x09, 0x53,                                       /*    USAGE (Device Index)                    */ \
    0x75, 0x08,                                       /*    REPORT_SIZE (8)                         */ \
    0x95, 0x04,                                       /*    REPORT_COUNT (4)                        */ \
    0xb1, 0x02,                                       /*   FEATURE (Data,Var,Abs)                   */ \
    0xc0,                                             /*   END_COLLECTION                           */ \
    0xc0                                              /* END_COLLECTION                             */


#ifdef ENABLE_NUVOTON_REPORT
#define TOUCH_REPORT_DESC  \
    DIGITIZER_REPORT_DESC, \
    NUVOTON_REPORT_DESC

#define TOUCH_REPORT_DESC_SIZE    (FINGER_DESCRIPTOR_SIZE*NUM_OF_FINGERS + 55 + 22)

#else
#define TOUCH_REPORT_DESC  \
    DIGITIZER_REPORT_DESC

#define TOUCH_REPORT_DESC_SIZE    (FINGER_DESCRIPTOR_SIZE*NUM_OF_FINGERS + 55)

#endif


#define MOUSE_REPORT_DESC                                                                              \
    0x05, 0x01,                                       /* USAGE_PAGE (Generic Desktop)               */ \
    0x09, 0x02,                                       /* USAGE (Mouse)                              */ \
    0xa1, 0x01,                                       /* COLLECTION (Application)                   */ \
    0x85, REPORTID_MOUSE,                             /*   REPORT_ID (Mouse)                        */ \
    0x09, 0x01,                                       /*   USAGE (Pointer)                          */ \
    0xa1, 0x00,                                       /*   COLLECTION (Physical)                    */ \
    0x05, 0x09,                                       /*     USAGE_PAGE (Button)                    */ \
    0x19, 0x01,                                       /*     USAGE_MINIMUM (Button 1)               */ \
    0x29, 0x03,                                       /*     USAGE_MAXIMUM (Button 3)               */ \
    0x15, 0x00,                                       /*     LOGICAL_MINIMUM (0)                    */ \
    0x25, 0x01,                                       /*     LOGICAL_MAXIMUM (1)                    */ \
    0x75, 0x01,                                       /*     REPORT_SIZE (1)                        */ \
    0x95, 0x03,                                       /*     REPORT_COUNT (3)                       */ \
    0x81, 0x02,                                       /*     INPUT (Data,Var,Abs)                   */ \
    0x75, 0x01,                                       /*     REPORT_SIZE (1)                        */ \
    0x95, 0x05,                                       /*     REPORT_COUNT (5)                       */ \
    0x81, 0x03,                                       /*     INPUT (Cnst,Var,Abs)                   */ \
    0x05, 0x01,                                       /*     USAGE_PAGE (Generic Desktop)           */ \
    0x09, 0x30,                                       /*     USAGE (X)                              */ \
    0x09, 0x31,                                       /*     USAGE (Y)                              */ \
    0x75, 0x08,                                       /*     REPORT_SIZE (8)                        */ \
    0x95, 0x02,                                       /*     REPORT_COUNT (2)                       */ \
    0x15, 0x81,                                       /*     LOGICAL_MINIMUM (-127)                 */ \
    0x25, 0x7f,                                       /*     LOGICAL_MAXIMUM (127)                  */ \
    0x81, 0x06,                                       /*     INPUT (Data,Var,Rel)                   */ \
    0xc0,                                             /*   END_COLLECTION                           */ \
    0xc0                                              /* END_COLLECTION                             */


#define MOUSE_REPORT_DESC_SIZE    52


/*---------------------------------------------------------------------------------------------------------*/
/* HID Touch structures                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct HID_contactDataStruct
{
    UINT8    finger_status;
    UINT8    finger_id;
    UINT8    finger_x_low;
    UINT8    finger_x_high;
    UINT8    finger_y_low;
    UINT8    finger_y_high;
} HID_contactDataStruct;


/*---------------------------------------------------------------------------------------------------------*/
/* HID module types                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
typedef volatile struct   HID_TouchDataStruct
{
    UINT8                     reportId;
    HID_contactDataStruct     contactReport[NUM_OF_FINGERS];
    UINT8                     numContacts;
    BOOLEAN                   readyToSend;

} HID_TouchDataStruct;


HID_H_EXT HID_TouchDataStruct HID_TouchData;


#endif

