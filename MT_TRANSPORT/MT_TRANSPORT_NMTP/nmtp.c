/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   nmtp.c                                                                                                */
/*            This file contains implementation of Nuvoton Multi-Touch protocol transport layer            */
/* Project:                                                                                                */
/*            MFTS                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                INCLUDES                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include "../transport_if.h"
#include "hal.h"
#include "nmtp.h"
#include <string.h>

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                              DEBUG SUPPORT                                              */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#ifdef DEBUG
#define ASSERT_RET(func)        ASSERT(func == DEFS_STATUS_OK)
#else
#define ASSERT_RET(func)        (void)func
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                               DEFINITIONS                                               */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#define TRANSPORT_INPUT_BUFFER_SIZE                 sizeof(TRANSPORT_COMMAND_T)
#define TRANSPORT_OUTPUT_BUFFER_SIZE                (ROUND_UP(sizeof(TRANSPORT_REPLY_T), 4)              +   \
                                                    (ROUND_UP(TRANSPORT_REPORT_COORDINATES_SIZE, 4) * 2) +   \
                                                    (ROUND_UP(TRANSPORT_REPORT_BUTTONS_SIZE, 4) * 2)     +   \
                                                    ROUND_UP(sizeof(TRANSPORT_REPORT_T), 4)              +   \
                                                    256)



/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                  TYPES                                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Output packet types                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    TRANSPORT_OUT_REPLY                         = 0,
    TRANSPORT_OUT_FW_READY_REPORT,
    TRANSPORT_OUT_COORDINATE_REPORT_A,
    TRANSPORT_OUT_COORDINATE_REPORT_B,
    TRANSPORT_OUT_BUTTONS_REPORT_A,
    TRANSPORT_OUT_BUTTONS_REPORT_B,
    TRANSPORT_OUT_RAW_DATA_REPORT,
    TRANSPORT_OUT_NONE,
    TRANSPORT_OUT_NUMBER_OF_TYPES
} TRANSPORT_OUT_PACKET;

/*---------------------------------------------------------------------------------------------------------*/
/* Parameters getters/setters                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
typedef void (*TRANSPORT_PARAM_GET_T)(UINT8* data);
typedef void (*TRANSPORT_PARAM_SET_T)(UINT8  data);


/*---------------------------------------------------------------------------------------------------------*/
/* Parameter type                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    TRANSPORT_PARAM_GET_T   getParam;
    TRANSPORT_PARAM_SET_T   setParam;
    UINT8                   paramSize;
} TRANSPORT_PARAM_T;


/*---------------------------------------------------------------------------------------------------------*/
/* Commands callbacks                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
typedef void  (*TRANSPORT_COMMAND_HANDLER_T)    (TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply);




/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 MACROS                                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Host transaction macro                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define HOST_TRANSACTION(addr, size, callback, type)                                                       \
/*lint -e{160,155}                                                                                      */ \
({                                                                                                         \
        DEFS_STATUS __ret_status = DEFS_STATUS_FAIL;                                                       \
                                                                                                           \
        if (TRANSPORT_initParams_l->hostInterface.type)                                                    \
        {                                                                                                  \
            __ret_status = TRANSPORT_initParams_l->hostInterface.type(addr, size, callback);               \
                                                                                                           \
            ASSERT(__ret_status == DEFS_STATUS_OK);                                                        \
        }                                                                                                  \
        else                                                                                               \
        {                                                                                                  \
            ASSERT(FALSE);                                                                                 \
        }                                                                                                  \
        ;                                                                                                  \
        __ret_status;                                                                                      \
})

/*---------------------------------------------------------------------------------------------------------*/
/* Host Read                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define HOST_READ(addr, size)       HOST_TRANSACTION(addr, size, TRANSPORT_readFinished_L, hostRead)

/*---------------------------------------------------------------------------------------------------------*/
/* Host Write                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define HOST_WRITE(addr, size)      HOST_TRANSACTION(addr, size, TRANSPORT_writeFinished_L, hostWrite)


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                       LOCAL FUNCTIONS DEFINITIONS                                       */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
static void         TRANSPORT_readFinished_L                (UINT16 actualSize, DEFS_STATUS status);
static void         TRANSPORT_writeFinished_L               (UINT16 actualSize, DEFS_STATUS status);
static void         TRANSPORT_fillCompactRawDataReport_L    (void);
static UINT16       TRANSPORT_getSensorValue_L              (UINT16 sensorNumber);
static void         TRANSPORT_dummyError_L                  (DEFS_STATUS err);
static DEFS_STATUS  TRANSPORT_sendPackage_L                 (TRANSPORT_OUT_PACKET packageType);
static void         TRANSPORT_COMMANDS_getParam_L           (TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply);
static void         TRANSPORT_COMMANDS_setParam_L           (TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply);
static void         TRANSPORT_COMMANDS_getFWVersion_L       (UINT8* data);
static void         TRANSPORT_COMMANDS_getFWChecksum_L      (UINT8* data);
static void         TRANSPORT_COMMANDS_getUpgradeModeState_L(UINT8* data);
static void         TRANSPORT_COMMANDS_getRawDataType_L     (UINT8* data);
static void         TRANSPORT_COMMANDS_setRawDataType_L     (UINT8  data);
#if defined DEBUG
static void         TRANSPORT_COMMANDS_getJTAGState_L       (UINT8* data);
static void         TRANSPORT_COMMANDS_setJTAGState_L       (UINT8  data);
#endif
static void         TRANSPORT_COMMANDS_getStackSize_L       (UINT8* data);
static void         TRANSPORT_COMMANDS_getIStackSize_L      (UINT8* data);
static void         TRANSPORT_COMMANDS_getTraps_L           (UINT8* data);
static void         TRANSPORT_COMMANDS_setPowerState_L      (TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply);
static void         TRANSPORT_COMMANDS_EnterUpgradeMode_L   (TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply);
static void         TRANSPORT_COMMANDS_ExitUpgradeMode_L    (TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply);
static void         TRANSPORT_COMMANDS_fwUpgradeBlock_L     (TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply);
static void         TRANSPORT_COMMANDS_calibrate_L          (TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply);
static void         TRANSPORT_COMMANDS_setFirmwareConfig_L  (TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply);
static void         TRANSPORT_COMMANDS_reset_L              (TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply);
static void         TRANSPORT_COMMANDS_baseline_L           (TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply);
static void         TRANSPORT_COMMANDS_getFirmwareConfig_L  (TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply);
static void         TRANSPORT_COMMANDS_hostReady_L          (TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply);

static DEFS_STATUS  TRANSPORT_PMC_Callback_L                (void);


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                  CONST                                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Shift of packets inside output buffer                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
static const UINT16 TRANSPORT_packetShift_l[TRANSPORT_OUT_NUMBER_OF_TYPES] =
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Reply shift                                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    0,

    /*-----------------------------------------------------------------------------------------------------*/
    /* Report FW ready shift                                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    ROUND_UP(sizeof(TRANSPORT_REPLY_T), 4),

    /*-----------------------------------------------------------------------------------------------------*/
    /* Touch report A shift                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    ROUND_UP(sizeof(TRANSPORT_REPLY_T), 4)                  +                                               \
    ROUND_UP(TRANSPORT_REPORT_FW_READY_SIZE, 4),

    /*-----------------------------------------------------------------------------------------------------*/
    /* Touch report B shift                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    ROUND_UP(sizeof(TRANSPORT_REPLY_T), 4)                  +                                               \
    ROUND_UP(TRANSPORT_REPORT_FW_READY_SIZE, 4)             +                                               \
    ROUND_UP(TRANSPORT_REPORT_COORDINATES_SIZE, 4),

    /*-----------------------------------------------------------------------------------------------------*/
    /* Buttons report A shift                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    ROUND_UP(sizeof(TRANSPORT_REPLY_T), 4)                  +                                               \
    ROUND_UP(TRANSPORT_REPORT_FW_READY_SIZE, 4)             +                                               \
    (ROUND_UP(TRANSPORT_REPORT_COORDINATES_SIZE, 4) * 2),

    /*-----------------------------------------------------------------------------------------------------*/
    /* Buttons report B shift                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    ROUND_UP(sizeof(TRANSPORT_REPLY_T), 4)                  +                                               \
    ROUND_UP(TRANSPORT_REPORT_FW_READY_SIZE, 4)             +                                               \
    (ROUND_UP(TRANSPORT_REPORT_COORDINATES_SIZE, 4) * 2)    +                                               \
    ROUND_UP(TRANSPORT_REPORT_BUTTONS_SIZE, 4),

    /*-----------------------------------------------------------------------------------------------------*/
    /* Raw data report shift                                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    ROUND_UP(sizeof(TRANSPORT_REPLY_T), 4)                  +                                               \
    ROUND_UP(TRANSPORT_REPORT_FW_READY_SIZE, 4)             +                                               \
    (ROUND_UP(TRANSPORT_REPORT_COORDINATES_SIZE, 4) * 2)    +                                               \
    (ROUND_UP(TRANSPORT_REPORT_BUTTONS_SIZE, 4) * 2),

    0,
};

/*---------------------------------------------------------------------------------------------------------*/
/* Command handlers                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
TRANSPORT_COMMAND_HANDLER_T  TRANSPORT_commandHandler[TRANSPORT_COMMAND_NUM_OF_COMMANDS] =
{
/* TRANSPORT_COMMAND_SET_PARAMETER      */  TRANSPORT_COMMANDS_setParam_L,
/* TRANSPORT_COMMAND_GET_PARAMETER      */  TRANSPORT_COMMANDS_getParam_L,
/* TRANSPORT_COMMAND_POWER_DOWN         */  TRANSPORT_COMMANDS_setPowerState_L,
/* TRANSPORT_COMMAND_ENTER_UPGRADE_MODE */  TRANSPORT_COMMANDS_EnterUpgradeMode_L,
/* TRANSPORT_COMMAND_UPGRADE_FW         */  TRANSPORT_COMMANDS_fwUpgradeBlock_L,
/* TRANSPORT_COMMAND_EXIT_UPGRADE_MODE  */  TRANSPORT_COMMANDS_ExitUpgradeMode_L,
/* TRANSPORT_COMMAND_SET_FIRMWARE_CONFIG*/  TRANSPORT_COMMANDS_setFirmwareConfig_L,
/* TRANSPORT_COMMAND_START_CALIBRATION  */  TRANSPORT_COMMANDS_calibrate_L,
/* TRANSPORT_COMMAND_RESET              */  TRANSPORT_COMMANDS_reset_L,
/* TRANSPORT_COMMAND_GET_BASELINE       */  TRANSPORT_COMMANDS_baseline_L,
/* TRANSPORT_COMMAND_GET_FIRMWARE_CONFIG*/  TRANSPORT_COMMANDS_getFirmwareConfig_L,
/* TRANSPORT_COMMAND_HOST_READY         */  TRANSPORT_COMMANDS_hostReady_L,
};

/*---------------------------------------------------------------------------------------------------------*/
/* Param handlers                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
TRANSPORT_PARAM_T TRANSPORT_paramHandler[TRANSPORT_FW_PARAM_NUM_OF_PARAMS] =
{
/* TRANSPORT_FW_PARAM_VER                   */  { TRANSPORT_COMMANDS_getFWVersion_L,            NULL,                                       4   },
/* TRANSPORT_FW_PARAM_CHECKSUM_STATE        */  { TRANSPORT_COMMANDS_getFWChecksum_L,           NULL,                                       1   },
/* TRANSPORT_FW_PARAM_UPGRADE_MODE_STATE    */  { TRANSPORT_COMMANDS_getUpgradeModeState_L,     NULL,                                       1   },
/* TRANSPORT_FW_PARAM_RAW_DATA_TYPE         */  { TRANSPORT_COMMANDS_getRawDataType_L,          TRANSPORT_COMMANDS_setRawDataType_L,        1   },
/* RESERVED                                 */  { NULL,                                         NULL,                                       1   },
#if defined DEBUG
/* TRANSPORT_FW_PARAM_JTAG_LOCK_STATE       */  { TRANSPORT_COMMANDS_getJTAGState_L,            TRANSPORT_COMMANDS_setJTAGState_L,          1   },
#else
/* TRANSPORT_FW_PARAM_JTAG_LOCK_STATE       */  { NULL,                                         NULL,                                       1   },
#endif
/* TRANSPORT_FW_PARAM_STACK_SIZE            */  { TRANSPORT_COMMANDS_getStackSize_L,            NULL,                                       2   },
/* TRANSPORT_FW_PARAM_ISTACK_SIZE           */  { TRANSPORT_COMMANDS_getIStackSize_L,           NULL,                                       2   },
/* TRANSPORT_FW_PARAM_TRAPS                 */  { TRANSPORT_COMMANDS_getTraps_L,                NULL,                                       2   },

};


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                             LOCAL VARIABLES DEFINITION                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
static TRANSPORT_INIT_PARAMS_T*         TRANSPORT_initParams_l;
static UINT8                            TRANSPORT_inputBuffer_l[TRANSPORT_INPUT_BUFFER_SIZE]   __attribute__ ((aligned (4)));
static UINT8                            TRANSPORT_outputBuffer_l[TRANSPORT_OUTPUT_BUFFER_SIZE] __attribute__ ((aligned (4)));
static BOOLEAN                          TRANSPORT_packetReady_l[TRANSPORT_OUT_NUMBER_OF_TYPES];
static UINT16                           TRANSPORT_packageSize_l[TRANSPORT_OUT_NUMBER_OF_TYPES];
static TRANSPORT_OUT_PACKET             TRANSPORT_packetOnWriting_l;
static UINT16                           TRANSPORT_numBytesReadFromHost_l;
static UINT16                           TRANSPORT_numBytesWritenToHost_l;
static TRANSPORT_RAW_DATA_TYPE          TRANSPORT_rawDataType_l;
static BOOLEAN                          TRANSPORT_hostReady_l   = FALSE;
static PMC_CALLBACK_T                   TRANSPORT_powerManagementCallback_l =
{
    TRANSPORT_PMC_Callback_L,
    NULL,
};

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 MACROS                                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#define TRANSPORT_COMMAND_ID_IS_VALID(id)               (IS_EVEN(id))
#define TRANSPORT_GET_REPORT_PTR(type)                  /*lint -e{826}*/ ((TRANSPORT_REPORT_T*)(&(TRANSPORT_outputBuffer_l[TRANSPORT_packetShift_l[type]])))
#define TRANSPORT_GET_COMMAND_PTR()                     /*lint -e{826}*/ ((TRANSPORT_COMMAND_T *)TRANSPORT_inputBuffer_l)
#define TRANSPORT_GET_REPLY_PTR()                       /*lint -e{826}*/ ((TRANSPORT_REPLY_T*)(&(TRANSPORT_outputBuffer_l[TRANSPORT_packetShift_l[TRANSPORT_OUT_REPLY]])))

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                              API FUNCTIONS                                              */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_Init                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  params  - init params                                                                  */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success, other DEFS_STATUS_x on failure                              */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initialize the Transport Layer                                            */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS TRANSPORT_Init (void* params)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Error checking                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_COND_CHECK(params != NULL, DEFS_STATUS_INVALID_PARAMETER);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Init variables                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    TRANSPORT_numBytesReadFromHost_l    = 0;
    TRANSPORT_numBytesWritenToHost_l    = 0;
    TRANSPORT_packetOnWriting_l         = TRANSPORT_OUT_NONE;
    TRANSPORT_rawDataType_l             = TRANSPORT_RAW_DATA_NO;
    TRANSPORT_hostReady_l               = FALSE;

    memset(&TRANSPORT_inputBuffer_l[0],     0,  sizeof(TRANSPORT_inputBuffer_l));
    memset(&TRANSPORT_outputBuffer_l[0],    0,  sizeof(TRANSPORT_outputBuffer_l));
    memset(&TRANSPORT_packetReady_l[0],     0,  sizeof(TRANSPORT_packetReady_l));
    memset(&TRANSPORT_packageSize_l[0],     0,  sizeof(TRANSPORT_packageSize_l));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Init params                                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    TRANSPORT_initParams_l = (TRANSPORT_INIT_PARAMS_T*)params;

    /*-----------------------------------------------------------------------------------------------------*/
    /* More error checking                                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_COND_CHECK(TRANSPORT_initParams_l->hostInterface.hostRead  != NULL, DEFS_STATUS_INVALID_PARAMETER);
    DEFS_STATUS_COND_CHECK(TRANSPORT_initParams_l->hostInterface.hostWrite != NULL, DEFS_STATUS_INVALID_PARAMETER);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check if we given the Error callback                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    if (TRANSPORT_initParams_l->protocol.error == NULL)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Set ptr to point to a dummy function                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        TRANSPORT_initParams_l->protocol.error = TRANSPORT_dummyError_L;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Register Power Management callback                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    PMC_RegisterPrerequisite(&TRANSPORT_powerManagementCallback_l);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Start reading process                                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    return HOST_READ(TRANSPORT_inputBuffer_l, (UINT16)TRANSPORT_INPUT_BUFFER_SIZE);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_SendTouchReport                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  pointsToReport  - Array of points to report                                            */
/*                  numPoints       - Number of points to report                                           */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success, other DEFS_STATUS_x on failure                              */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prepares and sends touch report                                           */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS TRANSPORT_SendTouchReport(TOUCH_POINT_T* pointsToReport, UINT numPoints)
{
    TRANSPORT_REPORT_T*         reportPtr           = NULL;
    TRANSPORT_OUT_PACKET        currentReportType   = TRANSPORT_OUT_NONE;
    UINT                        i                   = 0;
    DEFS_STATUS                 ret                 = DEFS_STATUS_OK;
    UINT                        saveInt             = 0;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Error checking                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_COND_CHECK(pointsToReport != NULL,                  DEFS_STATUS_INVALID_PARAMETER);
    DEFS_STATUS_COND_CHECK(numPoints <= TRANSPORT_MAX_POINTS_NUM,   DEFS_STATUS_INVALID_DATA_SIZE);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check if host is ready                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    if (!TRANSPORT_hostReady_l)
    {
        return DEFS_STATUS_OK;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Disable interrupts                                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    INTERRUPTS_SAVE_DISABLE(saveInt);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Find empty report slot                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    if (TRANSPORT_packetReady_l[TRANSPORT_OUT_COORDINATE_REPORT_A])
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Report A slot is occupied                                                                       */
        /*-------------------------------------------------------------------------------------------------*/

        /*-------------------------------------------------------------------------------------------------*/
        /* Check if report B report is currently busy                                                      */
        /*-------------------------------------------------------------------------------------------------*/
        if (TRANSPORT_packetOnWriting_l == TRANSPORT_OUT_COORDINATE_REPORT_B)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Report B is busy, we will overwrite report A (and report overrun error)                     */
            /*---------------------------------------------------------------------------------------------*/
            currentReportType = TRANSPORT_OUT_COORDINATE_REPORT_A;
            TRANSPORT_initParams_l->protocol.error(DEFS_STATUS_REPORT_OVERRUN);
        }
        else
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Report B is not busy                                                                        */
            /*---------------------------------------------------------------------------------------------*/
            currentReportType = TRANSPORT_OUT_COORDINATE_REPORT_B;

            /*---------------------------------------------------------------------------------------------*/
            /* Report B is occupied as well, we will overwrite report B (and report overrun error)         */
            /*---------------------------------------------------------------------------------------------*/
            if (TRANSPORT_packetReady_l[TRANSPORT_OUT_COORDINATE_REPORT_B])
            {
                /*-----------------------------------------------------------------------------------------*/
                /* Mark B is free                                                                          */
                /*-----------------------------------------------------------------------------------------*/
                TRANSPORT_packetReady_l[TRANSPORT_OUT_COORDINATE_REPORT_B] = FALSE;
                TRANSPORT_initParams_l->protocol.error(DEFS_STATUS_REPORT_OVERRUN);
            }
        }
    }
    else
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Report A slot is not occupied. We use report A                                                  */
        /*-------------------------------------------------------------------------------------------------*/
        currentReportType = TRANSPORT_OUT_COORDINATE_REPORT_A;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Map report to output buffer                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    reportPtr = TRANSPORT_GET_REPORT_PTR(currentReportType);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Initialize header                                                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    reportPtr->size     = (UINT16)(TRANSPORT_REPORT_COORDINATES_HEADER_SIZE + (numPoints * sizeof(TRANSPORT_CONTACT_INFO_T)));
    reportPtr->reportID = (UINT8)TRANSPORT_REPORT_COORDINATES;
    reportPtr->reportData.touchReport.numOfContacts = (UINT8)numPoints;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Add all points                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    for (i=0; i<numPoints; ++i)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Extract point info                                                                              */
        /*-------------------------------------------------------------------------------------------------*/
        reportPtr->reportData.touchReport.contacts[i].contactId =      pointsToReport[i].contactId;
        reportPtr->reportData.touchReport.contacts[i].contactStatus =  pointsToReport[i].status;
        reportPtr->reportData.touchReport.contacts[i].contactX =       pointsToReport[i].coordinates.X;
        reportPtr->reportData.touchReport.contacts[i].contactY =       pointsToReport[i].coordinates.Y;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Try to send report                                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    ret = TRANSPORT_sendPackage_L(currentReportType);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Re-enable the interrupts                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    INTERRUPTS_RESTORE(saveInt);

    return ret;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_SendButtonsReport                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  newPressed  - Bit array for pressed buttons                                            */
/*                  newReleased - Bit array for released buttons                                           */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success, other DEFS_STATUS_x on failure                              */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends buttons report                                                      */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS TRANSPORT_SendButtonsReport(UINT8 newPressed, UINT8 newReleased)
{
    TRANSPORT_REPORT_T*         reportPtr           = NULL;
    TRANSPORT_OUT_PACKET        currentReportType   = TRANSPORT_OUT_NONE;
    DEFS_STATUS                 ret                 = DEFS_STATUS_OK;
    UINT                        saveInt             = 0;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check if host is ready                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    if (!TRANSPORT_hostReady_l)
    {
        return DEFS_STATUS_OK;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Disable interrupts                                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    INTERRUPTS_SAVE_DISABLE(saveInt);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Calculate current report type                                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    if (TRANSPORT_packetReady_l[TRANSPORT_OUT_BUTTONS_REPORT_A])
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Check if report B report is currently busy                                                      */
        /*-------------------------------------------------------------------------------------------------*/
        if (TRANSPORT_packetOnWriting_l == TRANSPORT_OUT_BUTTONS_REPORT_B)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Report B is busy, we will overwrite report A (and report overrun error)                     */
            /*---------------------------------------------------------------------------------------------*/
            currentReportType = TRANSPORT_OUT_BUTTONS_REPORT_A;
            TRANSPORT_initParams_l->protocol.error(DEFS_STATUS_REPORT_OVERRUN);
        }
        else
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Report A slot is occupied, we will use report B                                             */
            /*---------------------------------------------------------------------------------------------*/
            currentReportType = TRANSPORT_OUT_BUTTONS_REPORT_B;

            /*---------------------------------------------------------------------------------------------*/
            /* Report B is occupied as well, we will overwrite report B (and report overrun error)         */
            /*---------------------------------------------------------------------------------------------*/
            if (TRANSPORT_packetReady_l[TRANSPORT_OUT_BUTTONS_REPORT_B])
            {
               TRANSPORT_packetReady_l[TRANSPORT_OUT_BUTTONS_REPORT_B] = FALSE;
               TRANSPORT_initParams_l->protocol.error(DEFS_STATUS_REPORT_OVERRUN);
            }
        }
    }
    else
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Report A is not occupied. We use report A                                                       */
        /*-------------------------------------------------------------------------------------------------*/
        currentReportType = TRANSPORT_OUT_BUTTONS_REPORT_A;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Map report to output buffer                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    reportPtr = TRANSPORT_GET_REPORT_PTR(currentReportType);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Fill report                                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    reportPtr->size = TRANSPORT_REPORT_BUTTONS_SIZE;
    reportPtr->reportID = TRANSPORT_REPORT_BUTTONS;
    reportPtr->reportData.buttonsReport.pressed     = newPressed;
    reportPtr->reportData.buttonsReport.released    = newReleased;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Try to send report                                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    ret = TRANSPORT_sendPackage_L(currentReportType);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Re-enable the interrupts                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    INTERRUPTS_RESTORE(saveInt);

    return ret;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_SendRawDataReport                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         DEFS_STATUS_OK on success, other DEFS_STATUS_x on failure                              */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine triggers raw data send                                                    */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS TRANSPORT_SendRawDataReport(void)
{
    DEFS_STATUS             ret         = DEFS_STATUS_OK;
    UINT                    saveInt     = 0;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check if host is ready                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    if (!TRANSPORT_hostReady_l)
    {
        return DEFS_STATUS_OK;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check if raw data send is enabled                                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    if (TRANSPORT_rawDataType_l == TRANSPORT_RAW_DATA_NO)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Raw data send is not enabled. Nothing to do                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        return DEFS_STATUS_OK;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check that older raw data report is not sent yet                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (TRANSPORT_packetReady_l[TRANSPORT_OUT_RAW_DATA_REPORT] == TRUE)
    {
        return DEFS_STATUS_SYSTEM_BUSY;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Disable interrupts                                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    INTERRUPTS_SAVE_DISABLE(saveInt);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Fill Compact Raw Data                                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    TRANSPORT_fillCompactRawDataReport_L();

    /*-----------------------------------------------------------------------------------------------------*/
    /* Try to send report                                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    ret = TRANSPORT_sendPackage_L(TRANSPORT_OUT_RAW_DATA_REPORT);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Re-enable the interrupts                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    INTERRUPTS_RESTORE(saveInt);

    return ret;
}



/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                             LOCAL FUNCTIONS                                             */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_readFinished_L                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  actualSize - actual number of bytes read                                               */
/*                  status - indicates wether the reading succeeded                                        */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles fw read finished event                                            */
/*                                                                                                         */
/*lint -esym(715, status)                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_readFinished_L(UINT16 actualSize, DEFS_STATUS status)
{
    TRANSPORT_COMMAND_T *           commandPtr;
    TRANSPORT_REPLY_T*              replyPtr;
    UINT8                           commandID;
    UINT8                           commandIndex;
    UINT16                          packetSize;
    void*                           readAddr;
    UINT16                          readSize;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check if error was found                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(status == DEFS_STATUS_OK);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Get command                                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    commandPtr = TRANSPORT_GET_COMMAND_PTR();

    /*-----------------------------------------------------------------------------------------------------*/
    /* Adjust number of read bytes                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    TRANSPORT_numBytesReadFromHost_l += actualSize;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Get packet size                                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    packetSize = commandPtr->size + sizeof(commandPtr->size);

    /*-----------------------------------------------------------------------------------------------------*/
    /* If current read packet transaction  is not completed                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    if (packetSize > TRANSPORT_numBytesReadFromHost_l)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Read next buffer                                                                                */
        /*-------------------------------------------------------------------------------------------------*/
        readAddr    = &TRANSPORT_inputBuffer_l[TRANSPORT_numBytesReadFromHost_l];
        readSize    = (UINT16)TRANSPORT_INPUT_BUFFER_SIZE;

        ASSERT_RET(HOST_READ(readAddr, readSize));

        return;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Extract packet information                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    commandID       = commandPtr->commandID;
    commandIndex    = TRANSPORT_GET_COMMAND_INDEX(commandID);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Prepare reply structure                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    replyPtr        = TRANSPORT_GET_REPLY_PTR();

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check that read ended successfully and Command ID is valid                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    if ((commandIndex < TRANSPORT_COMMAND_NUM_OF_COMMANDS) && TRANSPORT_COMMAND_ID_IS_VALID(commandID))
    {
        TRANSPORT_COMMAND_HANDLER_T cmdHandler = TRANSPORT_commandHandler[commandIndex];

        /*-------------------------------------------------------------------------------------------------*/
        /* Handle command                                                                                  */
        /*-------------------------------------------------------------------------------------------------*/
        if (cmdHandler)
        {
            cmdHandler(commandPtr, replyPtr);
        }
        else
        {
            ASSERT(FALSE);
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Set reply                                                                                       */
        /*-------------------------------------------------------------------------------------------------*/
        replyPtr->replyID = commandID + TRANSPORT_COMMAND_REPLY_MASK;

        /*-------------------------------------------------------------------------------------------------*/
        /* Try to send reply                                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        ASSERT_RET(TRANSPORT_sendPackage_L(TRANSPORT_OUT_REPLY));

        /*-------------------------------------------------------------------------------------------------*/
        /* Init read transaction vars                                                                      */
        /*-------------------------------------------------------------------------------------------------*/
        TRANSPORT_numBytesReadFromHost_l = 0;

        /*-------------------------------------------------------------------------------------------------*/
        /* Read next command                                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        ASSERT_RET(HOST_READ(TRANSPORT_inputBuffer_l, (UINT16)TRANSPORT_INPUT_BUFFER_SIZE));
    }
    else
    {
        TRANSPORT_initParams_l->protocol.error(DEFS_STATUS_COMMUNICATION_ERROR);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_writeFinished_L                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  actualSize - actual number of bytes read                                               */
/*                  status - indicates wether the reading succeeded                                        */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles fw report write finished event                                    */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_writeFinished_L(UINT16 actualSize, DEFS_STATUS status)
{
    void*   writeAddr;
    UINT16  writeSize;
    UINT16  writeIndex;

    TRANSPORT_numBytesWritenToHost_l += actualSize;

    /*-----------------------------------------------------------------------------------------------------*/
    /* if found error while writing packet                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(status == DEFS_STATUS_OK);

    /*-----------------------------------------------------------------------------------------------------*/
    /* If current write packet transaction  is not completed                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    if (TRANSPORT_packageSize_l[TRANSPORT_packetOnWriting_l] > TRANSPORT_numBytesWritenToHost_l)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Write next buffer                                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        if (TRANSPORT_packetOnWriting_l != TRANSPORT_OUT_NONE)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Calculate write buffer/size                                                                 */
            /*---------------------------------------------------------------------------------------------*/
            writeIndex  = (UINT16)(TRANSPORT_packetShift_l[TRANSPORT_packetOnWriting_l]) + TRANSPORT_numBytesWritenToHost_l;
            writeAddr   = &TRANSPORT_outputBuffer_l[writeIndex];
            writeSize   = (UINT16)(TRANSPORT_packageSize_l[TRANSPORT_packetOnWriting_l] - TRANSPORT_numBytesWritenToHost_l);

            /*---------------------------------------------------------------------------------------------*/
            /* Execute write                                                                               */
            /*---------------------------------------------------------------------------------------------*/
            ASSERT_RET(HOST_WRITE(writeAddr, writeSize));

            return;
        }
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check if writing succedded                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    if (status != DEFS_STATUS_OK)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Call error callback according to packet type                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        switch (TRANSPORT_packetOnWriting_l)
        {
            case TRANSPORT_OUT_NONE:
            case TRANSPORT_OUT_NUMBER_OF_TYPES:
                TRANSPORT_initParams_l->protocol.error(DEFS_STATUS_SYSTEM_IN_INCORRECT_STATE);
            break;

            case TRANSPORT_OUT_REPLY:
                TRANSPORT_initParams_l->protocol.error(DEFS_STATUS_REPLY_FAILED);
            break;

            case TRANSPORT_OUT_COORDINATE_REPORT_A:
            case TRANSPORT_OUT_COORDINATE_REPORT_B:
                TRANSPORT_initParams_l->protocol.error(DEFS_STATUS_REPORT_COORDS_FAILED);
            break;

            case TRANSPORT_OUT_RAW_DATA_REPORT:
                TRANSPORT_initParams_l->protocol.error(DEFS_STATUS_REPORT_RAW_DATA_FAILED);
            break;

            case TRANSPORT_OUT_BUTTONS_REPORT_A:
            case TRANSPORT_OUT_BUTTONS_REPORT_B:
                TRANSPORT_initParams_l->protocol.error(DEFS_STATUS_REPORT_BUTTONS_FAILED);
            break;

            case TRANSPORT_OUT_FW_READY_REPORT:
            break;
        }
    }

    TRANSPORT_numBytesWritenToHost_l = 0;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Mark sending is over                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    TRANSPORT_packetReady_l[TRANSPORT_packetOnWriting_l] = FALSE;
    TRANSPORT_packetOnWriting_l = TRANSPORT_OUT_NONE;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Send next waiting packet                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    (void)TRANSPORT_sendPackage_L(TRANSPORT_packetOnWriting_l);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_fillCompactRawDataReport_L                                                   */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine fill raw data compact(changed) report                                     */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_fillCompactRawDataReport_L(void)
{
    TRANSPORT_REPORT_T* report = TRANSPORT_GET_REPORT_PTR(TRANSPORT_OUT_RAW_DATA_REPORT);
    UINT8               i       = 0;
    UINT16              count   = 0;
    UINT16              value   = 0;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Fill report header                                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    report->reportID                            = TRANSPORT_REPORT_COMPACT_RAW_DATA;
    report->reportData.rawReport.rawDataType    = (UINT8)TRANSPORT_rawDataType_l;
    report->size                                = TRANSPORT_REPORT_COMPACT_RAW_HEADER_SIZE;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Clean bit array                                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    memset(&report->reportData.rawReport.rawData.rawDataCompact.sensorBitArray[0], 0, (UINT32)(TRANSPORT_MAX_SENSOR_NUM/8));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Clean data array                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    memset(&report->reportData.rawReport.rawData.rawDataCompact.sensorData[0], 0, (UINT32)TRANSPORT_MAX_SENSOR_NUM*2);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Copy raw data to compact buffer according to data type                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    for (i = 0; i < TRANSPORT_initParams_l->numOfSensors; i++)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Read sensor value                                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        value = TRANSPORT_getSensorValue_L(i);

        /*-------------------------------------------------------------------------------------------------*/
        /* Check if value is OK for sending                                                                */
        /*-------------------------------------------------------------------------------------------------*/
        if (value != 0)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Set bit in reported bit array                                                               */
            /*---------------------------------------------------------------------------------------------*/
            SET_VAR_BIT(report->reportData.rawReport.rawData.rawDataCompact.sensorBitArray[i / 8], i % 8);

            /*---------------------------------------------------------------------------------------------*/
            /* Add new value to array                                                                      */
            /*---------------------------------------------------------------------------------------------*/
            report->reportData.rawReport.rawData.rawDataCompact.sensorData[count++] = value;

            /*---------------------------------------------------------------------------------------------*/
            /* Update size                                                                                 */
            /*---------------------------------------------------------------------------------------------*/
            report->size += 2;
        }
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_getSensorValue_L                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  sensorNumber - number of sensor                                                        */
/*                                                                                                         */
/* Returns:         sensor value according to data type                                                    */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns sensor value according to data type                               */
/*---------------------------------------------------------------------------------------------------------*/
UINT16 TRANSPORT_getSensorValue_L(UINT16 sensorNumber)
{
    UINT16 returnValue = 0;

    switch(TRANSPORT_rawDataType_l)
    {
        case TRANSPORT_RAW_DATA_BASELINED:
            returnValue = TRANSPORT_initParams_l->sensorReading.getBaselinedZeroedSensor(sensorNumber);
        break;

        case TRANSPORT_RAW_DATA_NOT_BASELINED:
            returnValue = TRANSPORT_initParams_l->sensorReading.getRawSensor(sensorNumber);
        break;

        case TRANSPORT_RAW_DATA_FIXED_BASELINED:
            returnValue = TRANSPORT_initParams_l->sensorReading.getFixedSensor(sensorNumber);
        break;

        case TRANSPORT_RAW_DATA_BASELINE:
            returnValue = TRANSPORT_initParams_l->sensorReading.getBaseline(sensorNumber);
        break;

        case TRANSPORT_RAW_DATA_BASELINED_WITH_NEGATIVE:
            returnValue = (UINT16)(TRANSPORT_initParams_l->sensorReading.getBaselinedSensor(sensorNumber));
        break;

        default:
        break;
    }
    return returnValue;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_dummyError_L                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  TRANSPORT_ERROR                                                                        */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine does nothing                                                              */
/*                                                                                                         */
/*lint -esym(715, err)                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_dummyError_L(DEFS_STATUS err)
{
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_sendPackage_L                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  packageType - package to be send                                                       */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK on success, other DEFS_STATUS_x on failure                              */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine send older packet or this packet only if no current packet on the air.    */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS TRANSPORT_sendPackage_L(TRANSPORT_OUT_PACKET packageType)
{
    TRANSPORT_REPORT_T*     report          = NULL;
    UINT8                   i               = 0;
    DEFS_STATUS             ret             = DEFS_STATUS_OK;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Mark package is ready to be send                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    TRANSPORT_packetReady_l[packageType] = TRUE;

    /*-----------------------------------------------------------------------------------------------------*/
    /* If no packet is in send process                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    if (TRANSPORT_packetOnWriting_l == TRANSPORT_OUT_NONE)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Assume we are going to send the given package type                                              */
        /*-------------------------------------------------------------------------------------------------*/
        TRANSPORT_packetOnWriting_l = packageType;

        /*-------------------------------------------------------------------------------------------------*/
        /* Look for old packet waiting to be sent                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        for (i = 0; i < TRANSPORT_OUT_NUMBER_OF_TYPES; i++)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* If found older/higher priority packet, shift priority to it                                 */
            /*---------------------------------------------------------------------------------------------*/
            if ((i != TRANSPORT_OUT_NONE) && (i != packageType) && (TRANSPORT_packetReady_l[i] == TRUE))
            {
                TRANSPORT_packetOnWriting_l = i;
            }
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Check if our package it real                                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        if (TRANSPORT_packetOnWriting_l != TRANSPORT_OUT_NONE)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Get report for sending                                                                      */
            /*---------------------------------------------------------------------------------------------*/
            report = TRANSPORT_GET_REPORT_PTR(TRANSPORT_packetOnWriting_l);

            /*---------------------------------------------------------------------------------------------*/
            /* Update packet size for sending                                                              */
            /*---------------------------------------------------------------------------------------------*/
            TRANSPORT_packageSize_l[TRANSPORT_packetOnWriting_l] = report->size + sizeof(report->size);

            /*---------------------------------------------------------------------------------------------*/
            /* Host Write                                                                                  */
            /*---------------------------------------------------------------------------------------------*/
            ret = HOST_WRITE(&(TRANSPORT_outputBuffer_l[TRANSPORT_packetShift_l[TRANSPORT_packetOnWriting_l]]), TRANSPORT_packageSize_l[TRANSPORT_packetOnWriting_l]);

            /*---------------------------------------------------------------------------------------------*/
            /* Check if writing succeeded                                                                  */
            /*---------------------------------------------------------------------------------------------*/
            if (ret != DEFS_STATUS_OK)
            {
                TRANSPORT_packetOnWriting_l = TRANSPORT_OUT_NONE;
            }
        }
    }
    else
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Currently there is packet in progress, we wait till packet sending will finish and retry        */
        /*-------------------------------------------------------------------------------------------------*/
        ret = DEFS_STATUS_OK;
    }

    return ret;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_COMMANDS_getParam_L                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  command     - Command to handle                                                        */
/*                  reply       - Reply to return                                                          */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles get parameter commands                                            */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_COMMANDS_getParam_L(TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply)
{
    UINT8                   paramID         = command->commandInfo.fwParam.paramId;
    TRANSPORT_PARAM_GET_T   getHandler      = TRANSPORT_paramHandler[paramID].getParam;
    UINT8*                  dataPtr         = &reply->replyInfo.fwParam.replyData.paramData[0];
    UINT8                   dataSize        = TRANSPORT_paramHandler[paramID].paramSize;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set reply size                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    reply->size = TRANSPORT_REPLY_GET_PARAM_HEADER_SIZE + dataSize;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Call get parameter handler                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    if (getHandler != NULL)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Execute handler                                                                                 */
        /*-------------------------------------------------------------------------------------------------*/
        getHandler(dataPtr);
    }
    else
    {
        reply->replyInfo.fwParam.replyData.status = DEFS_STATUS_SYSTEM_NOT_INITIALIZED;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set reply param ID                                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    reply->replyInfo.fwParam.paramId = paramID;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_COMMANDS_setParam_L                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  command     - Command to handle                                                        */
/*                  reply       - Reply to return                                                          */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles set parameter commands                                            */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_COMMANDS_setParam_L(TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply)
{
    UINT8                   paramID         = command->commandInfo.fwParam.paramId;
    TRANSPORT_PARAM_SET_T   setHandler      = TRANSPORT_paramHandler[paramID].setParam;
    UINT8                   data            = command->commandInfo.fwParam.paramData;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Call set parameter handler                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    if (setHandler != NULL)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Execute handler                                                                                 */
        /*-------------------------------------------------------------------------------------------------*/
        setHandler(data);

        /*-------------------------------------------------------------------------------------------------*/
        /* Set status                                                                                      */
        /*-------------------------------------------------------------------------------------------------*/
        reply->replyInfo.fwParam.replyData.status = DEFS_STATUS_OK;
    }
    else
    {
        reply->replyInfo.fwParam.replyData.status = DEFS_STATUS_SYSTEM_NOT_INITIALIZED;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set reply ID                                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    reply->replyInfo.fwParam.paramId = paramID;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set reply size                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    reply->size = TRANSPORT_REPLY_SET_PARAM_SIZE; // Reply ID + Param ID + status
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_COMMANDS_getFWVersion_L                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  data    - Placeholder for output data                                                  */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns the current FW version                                            */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_COMMANDS_getFWVersion_L(UINT8* data)
{
    ASSERT(TRANSPORT_initParams_l != NULL);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Copy FW version                                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    memcpy(data, &TRANSPORT_initParams_l->protocol.fwVersion, sizeof(UINT32));
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_COMMANDS_getFWChecksum_L                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  data    - Placeholder for output data                                                  */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns upgradable FW checksum                                            */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_COMMANDS_getFWChecksum_L(UINT8* data)
{
    UINT32 checksum;

    ASSERT(TRANSPORT_initParams_l != NULL);
    ASSERT(TRANSPORT_initParams_l->protocol.getFwChecksumCallback != NULL);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Get checksum                                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    checksum = TRANSPORT_initParams_l->protocol.getFwChecksumCallback();

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set reply                                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    memcpy((void*)data, (void*)&checksum, sizeof(checksum));
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_COMMANDS_getUpgradeModeState_L                                               */
/*                                                                                                         */
/*                  data    - Placeholder for output data                                                  */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns TRUE if working in FW Upgrade mode                                */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_COMMANDS_getUpgradeModeState_L(UINT8* data)
{
    ASSERT(TRANSPORT_initParams_l != NULL);
    ASSERT(TRANSPORT_initParams_l->protocol.getFwUpgradeModeCallback != NULL);

    *data = TRANSPORT_initParams_l->protocol.getFwUpgradeModeCallback();
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_COMMANDS_getRawDataType_L                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  data    - Placeholder for output data                                                  */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns raw data type                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_COMMANDS_getRawDataType_L(UINT8* data)
{
    *data = (UINT8)TRANSPORT_rawDataType_l;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_COMMANDS_setRawDataType_L                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  data    - Data to set                                                                  */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets new raw data type                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_COMMANDS_setRawDataType_L(UINT8 data)
{
    TRANSPORT_rawDataType_l = (TRANSPORT_RAW_DATA_TYPE)data;
}


#if defined DEBUG
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_COMMANDS_getJTAGState_L                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  data    - Placeholder for output data                                                  */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns the JTAG state                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_COMMANDS_getJTAGState_L(UINT8* data)
{
    ASSERT(TRANSPORT_initParams_l != NULL);
    ASSERT(TRANSPORT_initParams_l->protocol.getJTagStateCallback != NULL);

    *data = TRANSPORT_initParams_l->protocol.getJTagStateCallback();
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_COMMANDS_setJTAGState_L                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  data   - New JTAG state                                                                */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets new JTAG state                                                       */
/*                                                                                                         */
/*lint -esym(715, data)                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_COMMANDS_setJTAGState_L(UINT8 data)
{
    ASSERT(TRANSPORT_initParams_l != NULL);
    ASSERT(TRANSPORT_initParams_l->protocol.setJTagStateCallback != NULL);

    (void)TRANSPORT_initParams_l->protocol.setJTagStateCallback(data);
}
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_COMMANDS_setPowerState_L                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  command     - The recieved command                                                     */
/*                  reply       - Reply that the function should fill                                      */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets new power mode state                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_COMMANDS_setPowerState_L(TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Error checking                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(TRANSPORT_initParams_l != NULL);
    ASSERT(TRANSPORT_initParams_l->protocol.powerDownCallback != NULL);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Extract command parameter                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    UINT8 sleepOn = command->commandInfo.fwParam.paramId;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set new power state                                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    reply->replyInfo.general.status = (UINT8)TRANSPORT_initParams_l->protocol.powerDownCallback(sleepOn);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set reply size                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    reply->size = 2;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_COMMANDS_enterUpgradeMode_L                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  command     - The recieved command                                                     */
/*                  reply       - Reply that the function should fill                                      */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine starts FW Upgrade Mode                                                    */
/*                                                                                                         */
/*lint -esym(715, command)                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_COMMANDS_EnterUpgradeMode_L(TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Error checking                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(TRANSPORT_initParams_l != NULL);
    ASSERT(TRANSPORT_initParams_l->protocol.enterUpgradeModeCallback != NULL);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Jump to upgrade mode                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    reply->replyInfo.general.status = (UINT8)TRANSPORT_initParams_l->protocol.enterUpgradeModeCallback();

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set reply size                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    reply->size = 2;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_COMMANDS_exitUpgradeMode_L                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  command     - The recieved command                                                     */
/*                  reply       - Reply that the function should fill                                      */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine exists from FW Upgrade Mode                                               */
/*                                                                                                         */
/*lint -esym(715, command)                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_COMMANDS_ExitUpgradeMode_L(TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Error checking                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(TRANSPORT_initParams_l != NULL);
    ASSERT(TRANSPORT_initParams_l->protocol.exitUpgradeModeCallback != NULL);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Jump to upgrade mode                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    reply->replyInfo.general.status = (UINT8)TRANSPORT_initParams_l->protocol.exitUpgradeModeCallback();

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set reply size                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    reply->size = 2;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_COMMANDS_fwUpgradeBlock_L                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  command     - The recieved command                                                     */
/*                  reply       - Reply that the function should fill                                      */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles FW Upgrade encrypted data block                                   */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_COMMANDS_fwUpgradeBlock_L(TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Error checking                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(TRANSPORT_initParams_l != NULL);
    ASSERT(TRANSPORT_initParams_l->protocol.writeFwUpgradeBlockCallback != NULL);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Extract parameters from command                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    UINT32 offset   = command->commandInfo.fwUpgrade.flashOffset;
    UINT16 size     = command->commandInfo.fwUpgrade.numOfBytes;
    UINT8* data     = &command->commandInfo.fwUpgrade.fwData[0];

    /*-----------------------------------------------------------------------------------------------------*/
    /* Jump to upgrade mode                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    reply->replyInfo.general.status = (UINT8)TRANSPORT_initParams_l->protocol.writeFwUpgradeBlockCallback(offset, size, data);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set reply size                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    reply->size = 2;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_COMMANDS_calibrate_L                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  command     - The recieved command                                                     */
/*                  reply       - Reply that the function should fill                                      */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine starts sensor baseline calibration                                        */
/*                                                                                                         */
/*lint -esym(715, command)                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_COMMANDS_calibrate_L(TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Error checking                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(TRANSPORT_initParams_l != NULL);
    ASSERT(TRANSPORT_initParams_l->protocol.startCalibrationCallback != NULL);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Get forceUpdate                                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    BOOLEAN forceUpdate = (BOOLEAN)command->commandInfo.data;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Jump to upgrade mode                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    reply->replyInfo.general.status = (UINT8)TRANSPORT_initParams_l->protocol.startCalibrationCallback(forceUpdate);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set reply size                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    reply->size = 2;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_COMMANDS_setFirmwareConfig_L                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  command     - The recieved command                                                     */
/*                  reply       - Reply that the function should fill                                      */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles FW configuration update                                           */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_COMMANDS_setFirmwareConfig_L(TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Error checking                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(TRANSPORT_initParams_l != NULL);
    ASSERT(TRANSPORT_initParams_l->protocol.setFirmwareConfigCallback != NULL);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Extract parameters from command                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    UINT8 structID      = command->commandInfo.fwConfig.structId;
    void* structData    = &command->commandInfo.fwConfig.structData[0];

    /*-----------------------------------------------------------------------------------------------------*/
    /* Jump to upgrade mode                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    reply->replyInfo.general.status = (UINT8)TRANSPORT_initParams_l->protocol.setFirmwareConfigCallback(structID, structData);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set reply size                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    reply->size = 2;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_COMMANDS_reset_L                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  command     - The recieved command                                                     */
/*                  reply       - Reply that the function should fill                                      */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles FW reset command                                                  */
/*                                                                                                         */
/*lint -esym(715, command)                                                                                 */
/*lint -esym(715, reply)                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_COMMANDS_reset_L(TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Error checking                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(TRANSPORT_initParams_l != NULL);
    ASSERT(TRANSPORT_initParams_l->protocol.fwResetCallback != NULL);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Jump to upgrade mode                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    (void)TRANSPORT_initParams_l->protocol.fwResetCallback();
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_COMMANDS_getStackSize_L                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  data    - Placeholder for output data                                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles stack size request                                                */
/*---------------------------------------------------------------------------------------------------------*/
static void TRANSPORT_COMMANDS_getStackSize_L(UINT8* data)
{
    UINT16 size;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Error checking                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(TRANSPORT_initParams_l != NULL);
    ASSERT(TRANSPORT_initParams_l->protocol.getStackSize != NULL);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Get stack size                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    size = TRANSPORT_initParams_l->protocol.getStackSize();

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set data                                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    memcpy(data, &size, sizeof(UINT16));
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_COMMANDS_getIStackSize_L                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  data    - Placeholder for output data                                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles istack size request                                               */
/*---------------------------------------------------------------------------------------------------------*/
static void TRANSPORT_COMMANDS_getIStackSize_L(UINT8* data)
{
    UINT16 size;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Error checking                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(TRANSPORT_initParams_l != NULL);
    ASSERT(TRANSPORT_initParams_l->protocol.getIStackSize != NULL);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Get stack size                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    size = TRANSPORT_initParams_l->protocol.getIStackSize();

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set data                                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    memcpy(data, &size, sizeof(UINT16));
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_COMMANDS_getTraps_L                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  data    - Placeholder for output data                                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles traps request                                                     */
/*---------------------------------------------------------------------------------------------------------*/
static void TRANSPORT_COMMANDS_getTraps_L(UINT8* data)
{
    UINT16 traps;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Error checking                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(TRANSPORT_initParams_l != NULL);
    ASSERT(TRANSPORT_initParams_l->protocol.getTraps!= NULL);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Get stack size                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    traps = TRANSPORT_initParams_l->protocol.getTraps();

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set data                                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    memcpy(data, &traps, sizeof(UINT16));
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_COMMANDS_baseline_L                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  command     - The recieved command                                                     */
/*                  reply       - Reply that the function should fill                                      */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles FW reset command                                                  */
/*                                                                                                         */
/*lint -esym(715, command)                                                                                 */
/*lint -esym(715, reply)                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_COMMANDS_baseline_L(TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply)
{
    UINT16 i;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Error checking                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(TRANSPORT_initParams_l != NULL);
    if (TRANSPORT_initParams_l->sensorReading.getBaseline != NULL)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Read sensor baselines                                                                           */
        /*-------------------------------------------------------------------------------------------------*/
        for (i=0; i<TRANSPORT_initParams_l->numOfSensors; ++i)
        {
            reply->replyInfo.baseline.baselineData[i] = TRANSPORT_initParams_l->sensorReading.getBaseline(i);
        }
    }
    reply->size = (TRANSPORT_initParams_l->numOfSensors*2) + 2;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_COMMANDS_getFirmwareConfig_L                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  command    -  The recieved command                                                     */
/*                  reply      -  Reply that the function should fill                                      */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles Get Firmware Configuration command                                */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_COMMANDS_getFirmwareConfig_L(TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply)
{
    TRANSPORT_FW_CONFIG_T   type = (TRANSPORT_FW_CONFIG_T)command->commandInfo.fwConfig.structId;
    void*                   data = (void*)&reply->replyInfo.fwConfig.structData[0];
    UINT16                  size = 0;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Error checking                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(TRANSPORT_initParams_l != NULL);
    ASSERT(TRANSPORT_initParams_l->protocol.getFirmwareConfigCallback != NULL);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read firmware configuration                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    (void)TRANSPORT_initParams_l->protocol.getFirmwareConfigCallback(type, data, &size);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set firmware configuration ID                                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    reply->replyInfo.fwConfig.structId = (UINT8)type;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set reply size                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    reply->size = 2 + size;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_COMMANDS_hostReady_L                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  command    -  The recieved command                                                     */
/*                  reply      -  Reply that the function should fill                                      */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine handles Host Ready command                                                */
/*---------------------------------------------------------------------------------------------------------*/
void TRANSPORT_COMMANDS_hostReady_L(TRANSPORT_COMMAND_T* command, TRANSPORT_REPLY_T* reply)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Update host ready                                                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    TRANSPORT_hostReady_l = command->commandInfo.data;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set reply status                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    reply->replyInfo.general.status = DEFS_STATUS_OK;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set reply size                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    reply->size = 2;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_PMC_Callback_L                                                               */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine checks whenever NMPT can allow PMC to put the chip to sleep               */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS TRANSPORT_PMC_Callback_L(void)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Check if we in raw data sending mode                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    if (TRANSPORT_rawDataType_l != TRANSPORT_RAW_DATA_NO)
    {
        return DEFS_STATUS_SYSTEM_BUSY;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check if we in the middle of read transaction                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    if (TRANSPORT_numBytesReadFromHost_l != 0)
    {
        return DEFS_STATUS_SYSTEM_BUSY;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check if we in the middle of write transaction                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    if (TRANSPORT_packageSize_l[TRANSPORT_packetOnWriting_l] > TRANSPORT_numBytesWritenToHost_l)
    {
        return DEFS_STATUS_SYSTEM_BUSY;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* No transaction is on-going, we can go to sleep                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    return DEFS_STATUS_OK;

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_isBusy                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         returns true if packet is being sent/recieved, false otherwise.                        */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns true if packet is being sent/recieved, false otherwise.           */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN TRANSPORT_isBusy(void)
{
    if ((TRANSPORT_packetOnWriting_l != TRANSPORT_OUT_NONE) || (TRANSPORT_numBytesReadFromHost_l > 0))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

