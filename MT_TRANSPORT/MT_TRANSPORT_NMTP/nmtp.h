/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   nmtp.h                                                                                                */
/*            This file contains definitions for implementation of Nuvoton Multi-Touch protocol transport  */
/*            layer                                                                                        */
/* Project:                                                                                                */
/*            MFTS                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef NMTP_H
#define NMTP_H

#include "../../SWC_DEFS/defs.h"

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           GENERAL DEFINITIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#define TRANSPORT_COMMAND_REPLY_MASK        0x80
#define TRANSPORT_MAX_FW_UPGRADE_BLOCK      512
#define TRANSPORT_MAX_FW_CFG_BLOCK          1024
#define TRANSPORT_MAX_SENSOR_NUM            256
#define TRANSPORT_MAX_POINTS_NUM            (MAX_TOUCH_POINTS*2)

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 ERRORS                                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Custop NMTP errors                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define DEFS_STATUS_REPORT_OVERRUN              DEFS_STATUS_CUSTOM_ERROR_00
#define DEFS_STATUS_REPLY_FAILED                DEFS_STATUS_CUSTOM_ERROR_01
#define DEFS_STATUS_REPORT_COORDS_FAILED        DEFS_STATUS_CUSTOM_ERROR_02
#define DEFS_STATUS_REPORT_RAW_DATA_FAILED      DEFS_STATUS_CUSTOM_ERROR_03
#define DEFS_STATUS_REPORT_BUTTONS_FAILED       DEFS_STATUS_CUSTOM_ERROR_04


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           PROTOCOL STRUCTURES                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Command structure                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Size of command packet                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    UINT16 __attribute__((packed,aligned(1))) size;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Command ID                                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    UINT8 __attribute__((aligned(1))) commandID;

    union
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Parameters related commands                                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        struct
        {
            UINT8   __attribute__((aligned(1))) paramId;
            UINT8   __attribute__((aligned(1))) paramData;
        } __attribute__((packed,aligned(1))) fwParam;

        /*-------------------------------------------------------------------------------------------------*/
        /* FW upgrade related commands                                                                     */
        /*-------------------------------------------------------------------------------------------------*/
        struct
        {
            UINT8   __attribute__((aligned(1))) RESERVED[1];
            UINT32  __attribute__((packed,aligned(1))) flashOffset;
            UINT16  __attribute__((packed,aligned(1))) numOfBytes;
            UINT8   __attribute__((aligned(1))) fwData[TRANSPORT_MAX_FW_UPGRADE_BLOCK];
        } __attribute__((packed,aligned(1))) fwUpgrade;

        /*-------------------------------------------------------------------------------------------------*/
        /* FW configuration related commands                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        struct
        {
            UINT8   __attribute__((aligned(1))) structId;
            UINT8   __attribute__((aligned(1))) structData[TRANSPORT_MAX_FW_CFG_BLOCK];
        } __attribute__((packed,aligned(1))) fwConfig;

        /*-------------------------------------------------------------------------------------------------*/
        /* General command data                                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        UINT8 __attribute__((aligned(1)))  data;
    } __attribute__((packed,aligned(1))) commandInfo;
} __attribute__((packed,aligned(1))) TRANSPORT_COMMAND_T ;


/*---------------------------------------------------------------------------------------------------------*/
/* Reply structure                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Size of reply packet                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    UINT16 __attribute__((packed,aligned(1))) size;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Reply ID                                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    UINT8 __attribute__((aligned(1))) replyID;

    union
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* FW parameters related replies                                                                   */
        /*-------------------------------------------------------------------------------------------------*/
        struct
        {
            UINT8       __attribute__((aligned(1))) paramId;

            union
            {
                UINT8   __attribute__((aligned(1))) paramData[4];
                UINT8   __attribute__((aligned(1))) status;
            } __attribute__((packed,aligned(1))) replyData;
        } __attribute__((packed,aligned(1))) fwParam;

        /*-------------------------------------------------------------------------------------------------*/
        /* Status related replies                                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        struct
        {
            UINT8       __attribute__((aligned(1))) status;
        } __attribute__((packed,aligned(1))) general;

        /*-------------------------------------------------------------------------------------------------*/
        /* Baselines reply                                                                                 */
        /*-------------------------------------------------------------------------------------------------*/
        struct
        {
            UINT8   __attribute__((aligned(1))) RESERVED;
            UINT16  __attribute__((packed,aligned(1))) baselineData[TRANSPORT_MAX_SENSOR_NUM];
        } __attribute__((packed,aligned(1))) baseline;

        struct
        {
            UINT8   __attribute__((aligned(1))) structId ;
            UINT8   __attribute__((aligned(1))) structData[TRANSPORT_MAX_FW_CFG_BLOCK];
        } __attribute__((packed,aligned(1))) fwConfig;
    } __attribute__((packed,aligned(1))) replyInfo;
} __attribute__((packed,aligned(1))) TRANSPORT_REPLY_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Contact info                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/

typedef struct
{
    UINT8                           __attribute__((aligned(1))) contactId;
    UINT8                           __attribute__((aligned(1))) contactStatus;
    INT16                           __attribute__((packed,aligned(1))) contactX;
    INT16                           __attribute__((packed,aligned(1))) contactY;
} __attribute__((packed,aligned(1))) TRANSPORT_CONTACT_INFO_T;



/*---------------------------------------------------------------------------------------------------------*/
/* Report structure                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Size of command packet                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    UINT16 __attribute__((packed,aligned(1))) size;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Report ID                                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    UINT8 __attribute__((aligned(1))) reportID;

    union
    {
        struct
        {
            UINT8                       __attribute__((aligned(1))) numOfContacts ;
            TRANSPORT_CONTACT_INFO_T    __attribute__((aligned(1))) contacts[TRANSPORT_MAX_POINTS_NUM] __attribute__((aligned(1)));
        } __attribute__((packed,aligned(1))) touchReport;

        struct
        {
            UINT8                       __attribute__((aligned(1))) rawDataType;

            union
            {
                struct
                {
                    UINT8               __attribute__((aligned(1))) sensorBitArray[TRANSPORT_MAX_SENSOR_NUM/8];
                    UINT16              __attribute__((packed,aligned(1))) sensorData[TRANSPORT_MAX_SENSOR_NUM];
                }  __attribute__((packed,aligned(1))) rawDataCompact;
            }  __attribute__((packed,aligned(1))) rawData;
        }  __attribute__((packed,aligned(1))) rawReport;

        struct
        {
            UINT8                       __attribute__((aligned(1))) pressed;
            UINT8                       __attribute__((aligned(1))) released;
        }  __attribute__((packed,aligned(1))) buttonsReport;

    }__attribute__((packed,aligned(1)))  reportData;
} __attribute__((packed,aligned(1))) TRANSPORT_REPORT_T;



/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                              FW PARAMETERS                                              */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Parameters                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    TRANSPORT_FW_PARAM_VER                      = 0,
    TRANSPORT_FW_PARAM_CHECKSUM_STATE,
    TRANSPORT_FW_PARAM_UPGRADE_MODE_STATE,
    TRANSPORT_FW_PARAM_RAW_DATA_TYPE,
    TRANSPORT_FW_PARAM_RAW_DATA_REPORT_TYPE,
    TRANSPORT_FW_PARAM_JTAG_LOCK_STATE,
    TRANSPORT_FW_PARAM_STACK_SIZE,
    TRANSPORT_FW_PARAM_ISTACK_SIZE,
    TRANSPORT_FW_PARAM_TRAPS,

    TRANSPORT_FW_PARAM_NUM_OF_PARAMS,
} TRANSPORT_FW_PARAM;



/*---------------------------------------------------------------------------------------------------------*/
/*                                           Parameter specific                                            */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Raw Data parameter type                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    TRANSPORT_RAW_DATA_NO                       = 0,
    TRANSPORT_RAW_DATA_BASELINED,
    TRANSPORT_RAW_DATA_NOT_BASELINED,
    TRANSPORT_RAW_DATA_FIXED_BASELINED,
    TRANSPORT_RAW_DATA_BASELINED_WITH_NEGATIVE,
    TRANSPORT_RAW_DATA_BASELINE,
} TRANSPORT_RAW_DATA_TYPE;


/*---------------------------------------------------------------------------------------------------------*/
/* JTAG parameters fields                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define TRANSPORT_JTAG_PARAM_TEMP_LOCK              5,  1
#define TRANSPORT_JTAG_PARAM_N2J_TEMP_LOCK          6,  1
#define TRANSPORT_JTAG_PARAM_PERMANENT_LOCK         7,  1




/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                               FW COMMANDS                                               */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Supported commands                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    TRANSPORT_COMMAND_SET_PARAMETER = 0,
    TRANSPORT_COMMAND_GET_PARAMETER,
    TRANSPORT_COMMAND_POWER_DOWN,
    TRANSPORT_COMMAND_ENTER_UPGRADE_MODE,
    TRANSPORT_COMMAND_UPGRADE_FW,
    TRANSPORT_COMMAND_EXIT_UPGRADE_MODE,
    TRANSPORT_COMMAND_SET_FIRMWARE_CONFIG,
    TRANSPORT_COMMAND_START_CALIBRATION,
    TRANSPORT_COMMAND_RESET,
    TRANSPORT_COMMAND_GET_BASELINES,
    TRANSPORT_COMMAND_GET_FIRMWARE_CONFIG,
    TRANSPORT_COMMAND_HOST_READY,

    TRANSPORT_COMMAND_NUM_OF_COMMANDS,
} TRANSPORT_COMMAND;

/*---------------------------------------------------------------------------------------------------------*/
/* Command number to command ID                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define TRANSPORT_GET_COMMAND_ID(cmdNum)        ((cmdNum + 1) * 2)
#define TRANSPORT_GET_COMMAND_INDEX(cmdID)      ((cmdID/2) - 1)

/*---------------------------------------------------------------------------------------------------------*/
/*                                            Commands Specific                                            */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* FW configuration types                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    TRANSPORT_FW_CONFIG_OPERATION           = 0,
    TRANSPORT_FW_CONFIG_HI                  = 1,
    TRANSPORT_FW_CONFIG_PROCESSING          = 2,
    TRANSPORT_FW_CONFIG_DETECTION_TOUCH     = 3,
    TRANSPORT_FW_CONFIG_DETECTION_BUTTON    = 4,
    TRANSPORT_FW_CONFIG_PHYSICAL_HW         = 5,
    TRANSPORT_FW_CONFIG_SENSOR_TABLE        = 6,
} TRANSPORT_FW_CONFIG_T;


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                          FW Commands Protocol                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Callback types                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
typedef UINT8                       (*TRANSPORT_GET_CMD_T)                  (void);
typedef UINT32                      (*TRANSPORT_GET_CRC_T)                  (void);
typedef DEFS_STATUS                 (*TRANSPORT_SET_CMD_T)                  (UINT8 param);
typedef DEFS_STATUS                 (*TRANSPORT_GENERAL_CMD_T)              (void);
typedef DEFS_STATUS                 (*TRANSPORT_UPGRADE_FW_CMD_T)           (UINT32 offset, UINT16 size, UINT8* data);
typedef DEFS_STATUS                 (*TRANSPORT_SET_FIRMWARE_CONFIG_T)      (TRANSPORT_FW_CONFIG_T structID, void* structData);
typedef DEFS_STATUS                 (*TRANSPORT_GET_FIRMWARE_CONFIG_T)      (TRANSPORT_FW_CONFIG_T structID, void* structData, UINT16* size);
typedef void                        (*TRANSPORT_ERROR_HANDLER_T)            (DEFS_STATUS error);
typedef UINT16                      (*TRANSPORT_GET_STACK_SIZE_T)           (void);
typedef UINT16                      (*TRANSPORT_GET_TRAPS_T)                (void);
typedef DEFS_STATUS                 (*TRANSPORT_CALIBRATE_CMD_T)            (BOOLEAN forceUpdate);

/*---------------------------------------------------------------------------------------------------------*/
/* Protocol handling structure                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    UINT32                          fwVersion;
    TRANSPORT_GET_CRC_T             getFwChecksumCallback;
    TRANSPORT_GET_CMD_T             getFwUpgradeModeCallback;
    TRANSPORT_GET_CMD_T             getJTagStateCallback;
    TRANSPORT_SET_CMD_T             setJTagStateCallback;
    TRANSPORT_SET_CMD_T             powerDownCallback;
    TRANSPORT_GENERAL_CMD_T         enterUpgradeModeCallback;
    TRANSPORT_GENERAL_CMD_T         exitUpgradeModeCallback;
    TRANSPORT_CALIBRATE_CMD_T       startCalibrationCallback;
    TRANSPORT_UPGRADE_FW_CMD_T      writeFwUpgradeBlockCallback;
    TRANSPORT_SET_FIRMWARE_CONFIG_T setFirmwareConfigCallback;
    TRANSPORT_GET_FIRMWARE_CONFIG_T getFirmwareConfigCallback;
    TRANSPORT_GENERAL_CMD_T         fwResetCallback;
    TRANSPORT_GET_STACK_SIZE_T      getStackSize;
    TRANSPORT_GET_STACK_SIZE_T      getIStackSize;
    TRANSPORT_GET_TRAPS_T           getTraps;
    TRANSPORT_ERROR_HANDLER_T       error;
} TRANSPORT_PROTOCOL_T;


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                             SENSOR READING                                              */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Sensor reading callbacks                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
typedef UINT16                      (*TRANSPORT_GET_BASELINED_ZEROED_SENSOR_PTR)(UINT16 sensorNum);
typedef UINT16                      (*TRANSPORT_GET_RAW_SENSOR_PTR)             (UINT16 sensorNum);
typedef UINT16                      (*TRANSPORT_GET_FIXED_SENSOR_PTR)           (UINT16 sensorNum);
typedef UINT16                      (*TRANSPORT_GET_BASELINE_PTR)               (UINT16 sensorNum);
typedef  INT16                      (*TRANSPORT_GET_BASELINED_PTR)              (UINT16 sensorNum);

/*---------------------------------------------------------------------------------------------------------*/
/* Sensor reading handling structure                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    TRANSPORT_GET_BASELINED_ZEROED_SENSOR_PTR   getBaselinedZeroedSensor;
    TRANSPORT_GET_RAW_SENSOR_PTR                getRawSensor;
    TRANSPORT_GET_FIXED_SENSOR_PTR              getFixedSensor;
    TRANSPORT_GET_BASELINE_PTR                  getBaseline;
    TRANSPORT_GET_BASELINED_PTR                 getBaselinedSensor;
} TRANSPORT_SENSOR_READING_T;


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                             HOST INTERFACE                                              */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Host interface callbacks                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
typedef void                        (*TRANSPORT_HI_EVENT_FINISHED_T)             (UINT16 actualSize, DEFS_STATUS status);
typedef DEFS_STATUS                 (*TRANSPORT_HI_READ_PTR)                     (void* buffer, UINT16 size, TRANSPORT_HI_EVENT_FINISHED_T readFinished);
typedef DEFS_STATUS                 (*TRANSPORT_HI_WRITE_PTR)                    (void* buffer, UINT16 size, TRANSPORT_HI_EVENT_FINISHED_T writeFinished);
typedef DEFS_STATUS                 (*TRANSPORT_HI_STOP_PTR)                     (void);

/*---------------------------------------------------------------------------------------------------------*/
/* Host Interface handling structure                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    TRANSPORT_HI_READ_PTR       hostRead;
    TRANSPORT_HI_WRITE_PTR      hostWrite;
} TRANSPORT_HI_CALLBACKS_T;



/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                NMTP INIT                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    UINT16                      numOfSensors;
    TRANSPORT_PROTOCOL_T        protocol;
    TRANSPORT_SENSOR_READING_T  sensorReading;
    TRANSPORT_HI_CALLBACKS_T    hostInterface;
} TRANSPORT_INIT_PARAMS_T;

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 REPLIES                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#define TRANSPORT_REPLY_GET_PARAM_HEADER_SIZE   2
#define TRANSPORT_REPLY_SET_PARAM_SIZE          3


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 REPORTS                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Report types                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    TRANSPORT_REPORT_COORDINATES                = 0xA2,
    TRANSPORT_REPORT_COMPACT_RAW_DATA           = 0xA4,
    TRANSPORT_REPORT_FULL_RAW_DATA              = 0xA6,
    TRANSPORT_REPORT_BUTTONS                    = 0xA8,
} TRANSPORT_REPORT_TYPE;

/*---------------------------------------------------------------------------------------------------------*/
/* Report sizes                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define TRANSPORT_REPORT_COORDINATES_HEADER_SIZE    2
#define TRANSPORT_REPORT_COORDINATES_SIZE           (2 + TRANSPORT_REPORT_COORDINATES_HEADER_SIZE + (sizeof(TRANSPORT_CONTACT_INFO_T) * TRANSPORT_MAX_POINTS_NUM))
#define TRANSPORT_REPORT_BUTTONS_SIZE               3
#define TRANSPORT_REPORT_FW_READY_SIZE              3
#define TRANSPORT_REPORT_FULL_RAW_HEADER_SIZE       4
#define TRANSPORT_REPORT_COMPACT_RAW_HEADER_SIZE    (2 + (TRANSPORT_MAX_SENSOR_NUM/8))


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                              API FUNCTIONS                                              */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

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
DEFS_STATUS TRANSPORT_SendButtonsReport(UINT8 newPressed, UINT8 newReleased);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_SendRawDataReport                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         DEFS_STATUS_OK on success, other DEFS_STATUS_x on failure                              */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine triggers raw data send                                                    */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS TRANSPORT_SendRawDataReport(void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TRANSPORT_isBusy                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         returns true if packet is being sent/recieved, false otherwise.                        */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns true if packet is being sent/recieved, false otherwise.           */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN TRANSPORT_isBusy(void);


#endif

