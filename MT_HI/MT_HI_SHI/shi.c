/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   shi.c                                                                                                 */
/*            This file contains Serial Host Interface implementation                                      */
/* Project:                                                                                                */
/*            MFTS                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 INCLUDE                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include "../hi_if.h"
#include "../../SWC_HAL/hal.h"

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                              API FUNCTIONS                                              */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        HI_Init                                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  hwParams    - hardware configuration parameters                                        */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK if no error occurred or other DEFS_STATUS_x on error                    */
/* Side effects:                                                                                           */
/* Description:     This routine initializes the SHI module                                                */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS HI_Init(void* hwParams)
{
    return SHI_Init((SHI_PARAMS_T *)hwParams);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        HI_Read                                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  buffer          - buffer for input data                                                */
/*                  size            - size of buffer                                                       */
/*                  readFinished    - function pointer for read finished event                             */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK if no error occurred or other DEFS_STATUS_x on error                    */
/* Side effects:                                                                                           */
/* Description:     This routine initiate a read transaction                                               */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS HI_Read(void* buffer, UINT16 size, HI_EVENT_FINISHED_T readFinished)
{
    return SHI_Read(buffer, size, (SHI_EVENT_FINISHED_T)readFinished);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        HI_Write                                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  buffer          - buffer to write to output buffer                                     */
/*                  size            - size of buffer                                                       */
/*                  writeFinished   - function pointer for write finished event                            */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK if no error occurred or other DEFS_STATUS_x on error                    */
/* Side effects:                                                                                           */
/* Description:     This routine initiate a write transaction                                              */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS HI_Write(void* buffer, UINT16 size, HI_EVENT_FINISHED_T writeFinished)
{
    return SHI_Write(buffer, size, (SHI_EVENT_FINISHED_T)writeFinished);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        HI_WriteReset                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine stops the write transaction                                               */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS HI_WriteStop(void)
{
    return SHI_WriteStop();
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        HI_ReadStop                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine stops the read transaction                                                */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS HI_ReadStop(void)
{
    return SHI_ReadStop();
}



