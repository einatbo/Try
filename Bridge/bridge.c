/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2010 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   bridge.c                                                                                              */
/*            This file contains implementation of Touch SPI to USB HID bridge for Multi Touch projects    */
/* Project:                                                                                                */
/*            Multi Touch Bridge                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                INCLUDES                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include <string.h>

#include "bridge.h"
#include "hid.h"
#include "hal.h"
#include "touch.h"
#include "nmtp.h"
#include "platform.h"
#include "transport_if.h"
#include "usb_touch.h"
#include "queue.h"

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                  TYPES                                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
      BRIDGE_IF_I2C,
      BRIDGE_IF_SPI
} BRIDGE_IF_T;

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                               DEFINITIONS                                               */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#define _7_BIT_ADDRESS(x)           (x >> 1)
#define I2C_ADDRESS                 _7_BIT_ADDRESS(BRIDGE_SlaveAddress[BRIDGE_CurI2cAddress])
#define DEV_BUFF_SIZE               64

#define BRIDGE_START                0xFF1A
#define BRIDGE_STOP                 0xFF1B

/*---------------------------------------------------------------------------------------------------------*/
/* Connected LED indicator                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define CONNECTED_LED               0x32
#define CONNECTED_LED_ON            0
#define CONNECTED_LED_OFF           1

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                  CONST                                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
static const UINT8      BRIDGE_SlaveAddress[] = {0x54, 0x44, 0x34, 0x24};


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                VARIABLES                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
static UINT8            BRIDGE_Buffer[DEV_BUFF_SIZE];
static UINT8            BRIDGE_CurI2cAddress;
static BRIDGE_IF_T      BRIDGE_If;
static SMB_STATE_IND_T  BRIDGE_I2cIndicator;
static BOOLEAN          BRIDGE_Stopped;
static BOOLEAN          BRIDGE_ExistPendingCommand;
static UINT8            BRIDGE_CommandBuffer[DEV_BUFF_SIZE];
static UINT16           BRIDGE_CommandBufferSize;

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                             LOCAL FUNCTIONS                                             */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
static void         BRIDGE_I2cCallback  (SMB_MODULE_T module_num, SMB_STATE_IND_T op_status, UINT16 info);
static DEFS_STATUS  BRIDGE_Read         (UINT16 size, UINT8 * buff);
static DEFS_STATUS  BRIDGE_Write        (UINT16 size, UINT8 * buff);
static void         BRIDGE_Start        (BOOLEAN start);


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           INTERFACE FUNCTIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BRIDGE_Init                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initializes bridge logic                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void BRIDGE_Init(void)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Init queue                                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    QUEUE_Init();

    /*-----------------------------------------------------------------------------------------------------*/
    /* Initialize variables                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    BRIDGE_If                  = BRIDGE_IF_I2C;        /* Default interface is I2C                           */
    BRIDGE_I2cIndicator        = SMB_NO_STATUS_IND;
    BRIDGE_Stopped             = FALSE;
    BRIDGE_ExistPendingCommand = FALSE;
    BRIDGE_CommandBufferSize   = 0;
    /*-----------------------------------------------------------------------------------------------------*/
    /* Enable SMBus                                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    SMB_Init(BRIDGE_I2cCallback);
    (void)SMB_InitModule (I2C_IF, 400);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Enable Chip Select                                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    GPIO_Init((UINT8)CS, GPIO_DIR_OUTPUT, GPIO_PULL_NONE, FALSE);
    GPIO_Write((UINT8)CS, 1);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Enable SPI                                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
#if 0
    SPI_Init(SPI_MODULE_0, SPI_WRITE_FALLING_EDGE, 12500000);
#else
    SPI_Init(SPI_MODULE_0, SPI_WRITE_FALLING_EDGE, 1000000);
#endif
    /*-----------------------------------------------------------------------------------------------------*/
    /* Enable HINT                                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    MIWU_ConfigEdgeChannel(CK_INT_SOURCE, MIWU_FALLING_EDGE);
    MIWU_EnableChannel(CK_INT_SOURCE, TRUE);
    ICU_EnableInt(CK_INT, FALSE);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Enable connection GPIO                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    GPIO_Init(CONNECTED_LED, GPIO_DIR_OUTPUT, GPIO_PULL_NONE, FALSE);
    GPIO_Write(CONNECTED_LED, CONNECTED_LED_OFF);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BRIDGE_Find                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine finds bridge connectivity                                                 */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS BRIDGE_Find(void)
{
    TRANSPORT_COMMAND_T*    pCommand    = (TRANSPORT_COMMAND_T*)BRIDGE_Buffer;
    UINT16                  reportSize  = 0;
    UINT16                  size;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Turn OFF connected LED                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    GPIO_Write(CONNECTED_LED, CONNECTED_LED_OFF);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Probe communication and check whether it is I2C or SPI                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    pCommand->commandID         = TRANSPORT_GET_COMMAND_ID(TRANSPORT_COMMAND_HOST_READY);
    pCommand->commandInfo.data  = TRUE;
    pCommand->size              = sizeof(pCommand->commandInfo.data) + sizeof(pCommand->commandID);
    size = pCommand->size + sizeof(pCommand->size);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check if transaction succeed?                                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    for (BRIDGE_CurI2cAddress = 0; BRIDGE_CurI2cAddress < ARRAY_SIZE(BRIDGE_SlaveAddress); BRIDGE_CurI2cAddress++)
    {
        BRIDGE_If = BRIDGE_IF_I2C;

        if (BRIDGE_Write(size, BRIDGE_Buffer) == DEFS_STATUS_OK)
        {
            goto suspected_found;
        }
    }

    if (BRIDGE_CurI2cAddress == ARRAY_SIZE(BRIDGE_SlaveAddress))
    {
        BRIDGE_If = BRIDGE_IF_SPI;

        if (BRIDGE_Write(size, BRIDGE_Buffer) == DEFS_STATUS_OK)
        {
            goto suspected_found;
        }
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Nothing found                                                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    return DEFS_STATUS_FAIL;

suspected_found:
    /*-----------------------------------------------------------------------------------------------------*/
    /* We suspect that we found our device. Lets wait for FW reply                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    BUSY_WAIT_TIMEOUT(MIWU_PendingChannel(CK_INT_SOURCE) != TRUE, 0xFFFF);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear pending interrupt                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    MIWU_ClearChannel(CK_INT_SOURCE);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Get report size                                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    DEFS_STATUS_RET_CHECK(BRIDGE_Read(sizeof(pCommand->size), (UINT8*)&reportSize));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Wait for rest of the package                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    BUSY_WAIT_TIMEOUT(MIWU_PendingChannel(CK_INT_SOURCE) != TRUE, 0xFFFF);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Clear pending interrupt                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    MIWU_ClearChannel(CK_INT_SOURCE);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Get rest of the report                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(reportSize != 0);
    ASSERT(reportSize <= DEV_BUFF_SIZE);
    DEFS_STATUS_RET_CHECK(BRIDGE_Read(reportSize, BRIDGE_Buffer));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check if received data is FW Ready report                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    if ((reportSize == 2)
        && (BRIDGE_Buffer[0] == (TRANSPORT_GET_COMMAND_ID(TRANSPORT_COMMAND_HOST_READY) + TRANSPORT_COMMAND_REPLY_MASK))
        && (BRIDGE_Buffer[1] == DEFS_STATUS_OK))
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* We got reply from FW. Device is found successfully                                              */
        /*-------------------------------------------------------------------------------------------------*/
        GPIO_Write(CONNECTED_LED, CONNECTED_LED_ON);

        return DEFS_STATUS_OK;
    }
    else
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Something is wrong. Re-try next time                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        return DEFS_STATUS_FAIL;
    }
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BRIDGE_SendToHost                                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs a conversion of multi touch data arriving via SPI to USB HID     */
/*                  format                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS BRIDGE_SendToHost(void)
{
    UINT16                  i;
    UINT16                  fullSize;
    TOUCH_POINT_T           points[MAX_TOUCH_POINTS];
    UINT16                  packetSize;
    UINT8*                  buff;
    TRANSPORT_REPORT_T*     pReport = NULL;

    if (BRIDGE_Stopped)
    {
        QUEUE_Init();
        return DEFS_STATUS_OK;
    }

    if (MIWU_PendingChannel(CK_INT_SOURCE) == TRUE)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Clear pending interrupt                                                                         */
        /*-------------------------------------------------------------------------------------------------*/
        MIWU_ClearChannel(CK_INT_SOURCE);

        /*-------------------------------------------------------------------------------------------------*/
        /* Get packet size                                                                                 */
        /*-------------------------------------------------------------------------------------------------*/
        DEFS_STATUS_RET_CHECK(BRIDGE_Read(sizeof(UINT16), (UINT8 *)&packetSize));

        /*-------------------------------------------------------------------------------------------------*/
        /* Calculate size of packet                                                                        */
        /*-------------------------------------------------------------------------------------------------*/
        fullSize = (packetSize + sizeof(UINT16));

        /*-------------------------------------------------------------------------------------------------*/
        /* Sanity check that packet is not bigger than the bridge                                          */
        /*-------------------------------------------------------------------------------------------------*/
        ASSERT(fullSize < QUEUE_BUFFER_MAX)

        /*-------------------------------------------------------------------------------------------------*/
        /* Allocate space in queue for the packet                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        do
        {
            ATOMIC_OP((buff = (UINT8*)QUEUE_Allocate(fullSize)));
        }while (buff == NULL);

        /*-------------------------------------------------------------------------------------------------*/
        /* Save beginning of the report                                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        pReport = (TRANSPORT_REPORT_T*)buff;

        /*-------------------------------------------------------------------------------------------------*/
        /* Insert size to queue                                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        memcpy(buff, &packetSize, sizeof(UINT16));
        ATOMIC_OP((buff = QUEUE_Push(sizeof(UINT16))));

        /*-------------------------------------------------------------------------------------------------*/
        /* Calculate number of transactions needed to read the packet                                      */
        /*-------------------------------------------------------------------------------------------------*/
        while (packetSize > 0)
        {
            UINT16 readSize;

            /*---------------------------------------------------------------------------------------------*/
            /* Wait for the device to signal that the new data is there                                    */
            /*---------------------------------------------------------------------------------------------*/
            BUSY_WAIT_TIMEOUT(MIWU_PendingChannel(CK_INT_SOURCE) == FALSE, 0xFFFF);
            MIWU_ClearChannel(CK_INT_SOURCE);

            /*---------------------------------------------------------------------------------------------*/
            /* Perform the next read                                                                       */
            /*---------------------------------------------------------------------------------------------*/
            readSize = MIN(DEV_BUFF_SIZE, packetSize);
            ASSERT(readSize != 0);
            DEFS_STATUS_RET_CHECK(BRIDGE_Read(readSize, buff));

            /*---------------------------------------------------------------------------------------------*/
            /* Update queue                                                                                */
            /*---------------------------------------------------------------------------------------------*/
            ATOMIC_OP((buff = QUEUE_Push(readSize)));

            /*---------------------------------------------------------------------------------------------*/
            /* Update remaining packet size                                                                */
            /*---------------------------------------------------------------------------------------------*/
            packetSize -= readSize;
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* If slave data is a valid coordinates report, forwared it to USB HID                             */
        /*-------------------------------------------------------------------------------------------------*/
        if ((pReport->reportID == TRANSPORT_REPORT_COORDINATES) &&
            (pReport->reportData.touchReport.numOfContacts <= MAX_TOUCH_POINTS))
        {

            for (i = 0; i < pReport->reportData.touchReport.numOfContacts; i++)
            {
                points[i].contactId     =  pReport->reportData.touchReport.contacts[i].contactId;
                points[i].status        =  pReport->reportData.touchReport.contacts[i].contactStatus;
                points[i].coordinates.X =  pReport->reportData.touchReport.contacts[i].contactX;
                points[i].coordinates.Y =  pReport->reportData.touchReport.contacts[i].contactY;
            }

            /*---------------------------------------------------------------------------------------------*/
            /* This is a coordinates report, forward to HID                                                */
            /*---------------------------------------------------------------------------------------------*/
            (void)TRANSPORT_SendTouchReport(points, pReport->reportData.touchReport.numOfContacts);
        }
    }

    return DEFS_STATUS_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                             LOCAL FUNCTIONS                                             */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BRIDGE_I2cCallback                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  info -                                                                                 */
/*                  module_num -                                                                           */
/*                  op_status -                                                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is a callback for I2C transaction completion                              */
/*lint -esym(715, module_num)                                                                              */
/*lint -esym(715, info)                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void BRIDGE_I2cCallback(SMB_MODULE_T module_num, SMB_STATE_IND_T op_status, UINT16 info)
{
    BRIDGE_I2cIndicator = op_status;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BRIDGE_Read                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine read from the device using the interface that was decided on during       */
/*                  probing                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS BRIDGE_Read(UINT16 size, UINT8 * buff)
{
    DEFS_STATUS status = DEFS_STATUS_OK;

    if (BRIDGE_If == BRIDGE_IF_I2C)
    {
        BRIDGE_I2cIndicator      = SMB_NO_STATUS_IND;

        /*-------------------------------------------------------------------------------------------------*/
        /* Perform Read transaction                                                                        */
        /*-------------------------------------------------------------------------------------------------*/
        if (!SMB_StartMasterTransaction(I2C_IF, I2C_ADDRESS, 0, size, NULL, buff))
            {
                status = DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
                return status;
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Wait for transaction to complete                                                                */
        /*-------------------------------------------------------------------------------------------------*/
        BUSY_WAIT_TIMEOUT(BRIDGE_I2cIndicator == SMB_NO_STATUS_IND, 0xFFFF);

        /*-------------------------------------------------------------------------------------------------*/
        /* Check status                                                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        if (BRIDGE_I2cIndicator == SMB_MASTER_DONE_IND)
        {
            status = DEFS_STATUS_OK;
        }
        else
        {
            status = DEFS_STATUS_COMMUNICATION_ERROR;
        }

    }
    else
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Perform SPI read command with dummy byte                                                        */
        /*-------------------------------------------------------------------------------------------------*/
        UINT8 readCommand = 0x03;
        SPI_WriteRead(SPI_MODULE_0, CS, &readCommand, 1, buff, size);
        status = DEFS_STATUS_OK;
    }

    return status;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BRIDGE_RecieveControlData                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  data - pointer to data                                                                 */
/*                  size - size of data                                                                    */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine recieve data from control end point and pass it on                        */
/*---------------------------------------------------------------------------------------------------------*/
void BRIDGE_RecieveData(void* data, UINT16 size)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Check if we recieved single word command for bridge only                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    if (size == 2)
    {
        UINT8* buff = (UINT8*)data;
        UINT16 cmd  = MAKE16(buff[0], buff[1]);

        if (cmd == BRIDGE_START)
        {
            BRIDGE_Start(TRUE);
            return;
        }
        else if (cmd == BRIDGE_STOP)
        {
            BRIDGE_Start(FALSE);
            return;
        }
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* If we reached here, this is probably a data for MFTS chip                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(BRIDGE_ExistPendingCommand == FALSE);
    ASSERT(size <= DEV_BUFF_SIZE);
    /*-----------------------------------------------------------------------------------------------------*/
    /* Get data                                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    memcpy(&BRIDGE_CommandBuffer[0], data, size);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Get data size                                                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    BRIDGE_CommandBufferSize = size;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Mark command is pending                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    BRIDGE_ExistPendingCommand = TRUE;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BRIDGE_SendToMFTS                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sends data from Host to MFTS                                              */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS BRIDGE_SendToMFTS(void)
{
    DEFS_STATUS ret = DEFS_STATUS_OK;

    if (BRIDGE_ExistPendingCommand)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Send data to MFTS                                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        ret = BRIDGE_Write(BRIDGE_CommandBufferSize, BRIDGE_CommandBuffer);

        /*-------------------------------------------------------------------------------------------------*/
        /* Mark data is sent                                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        BRIDGE_ExistPendingCommand = FALSE;
    }

    return ret;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BRIDGE_Write                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  size -                                                                                 */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine write to device using the interface that was decided on during            */
/*                  probing                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS BRIDGE_Write(UINT16 size, UINT8 * buff)
{
    if (BRIDGE_If == BRIDGE_IF_I2C)
    {
        BRIDGE_I2cIndicator      = SMB_NO_STATUS_IND;

        /*-------------------------------------------------------------------------------------------------*/
        /* Perform Write transaction                                                                       */
        /*-------------------------------------------------------------------------------------------------*/
        if (SMB_StartMasterTransaction(I2C_IF, I2C_ADDRESS, size, 0, buff, NULL))
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Wait for transaction to complete                                                            */
            /*---------------------------------------------------------------------------------------------*/
            BUSY_WAIT_TIMEOUT(BRIDGE_I2cIndicator == SMB_NO_STATUS_IND, 0xFFFF);
        }
        /*-------------------------------------------------------------------------------------------------*/
        /* Check status                                                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        if (BRIDGE_I2cIndicator == SMB_MASTER_DONE_IND)
        {
            return DEFS_STATUS_OK;
        }

        return DEFS_STATUS_COMMUNICATION_ERROR;

    }
    else
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Perform SPI write command                                                                       */
        /*-------------------------------------------------------------------------------------------------*/

        SPI_WriteRead(SPI_MODULE_0, CS, buff, size, NULL, 0);
        return DEFS_STATUS_OK;
    }

}



#if defined BULK_MODE
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BRIDGE_GetBulkData                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  data -                                                                                 */
/*                  size - max size to return                                                              */
/*                                                                                                         */
/* Returns:         size - actual size of data                                                             */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns ready diagnositics data for reporting to the host                 */
/*---------------------------------------------------------------------------------------------------------*/
UINT16    BRIDGE_GetBulkData(void** data, UINT16 size)
{
    UINT16 queueSize;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Get queue size                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    ATOMIC_OP((queueSize = QUEUE_GetSize()));
    /*-----------------------------------------------------------------------------------------------------*/
    /* If new data is available                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    if (queueSize > 0)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Assign pointer to the current point within diagnostics buffer                                   */
        /*-------------------------------------------------------------------------------------------------*/
        ATOMIC_OP((size = QUEUE_Peek(size, data)));
    }
    else
    {
        size = 0;
    }

    return size;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BRIDGE_GetBulkDataFinished                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  size - number of bytes read                                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine updates queue with latest bytes read size                                 */
/*---------------------------------------------------------------------------------------------------------*/
void BRIDGE_GetBulkDataFinished(UINT16 size)
{
    ATOMIC_OP(QUEUE_Pop(size));
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        BRIDGE_Start                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  start - true in order to start the bridge, false in order to stop the bridge           */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enable/disable reading new data from chip.                                */
/*---------------------------------------------------------------------------------------------------------*/
void BRIDGE_Start(BOOLEAN start)
{
    if (start)
    {
        BRIDGE_Stopped = FALSE;
    }
    else
    {
        BRIDGE_Stopped = TRUE;
    }
}

#endif


