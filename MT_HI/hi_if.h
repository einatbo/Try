/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2011 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   hi_if.h                                                                                               */
/*            This file contains Host Interface API                                                        */
/* Project:                                                                                                */
/*            MFTS                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef  HI_IF_H
#define  HI_IF_H

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                INCLUDES                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include "defs.h"

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                  TYPES                                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
typedef void (*HI_EVENT_FINISHED_T)(UINT16 actualSize, DEFS_STATUS status);


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           Host Interface API                                            */
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
/* Description:     This routine initializes the Host Interface Layer                                      */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS HI_Init (void* hwParams);

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
DEFS_STATUS HI_Read (void* buffer, UINT16 size, HI_EVENT_FINISHED_T readFinished);

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
DEFS_STATUS HI_Write (void* buffer, UINT16 size, HI_EVENT_FINISHED_T writeFinished);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        HI_WriteReset                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine stops the write transaction                                               */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS HI_WriteStop(void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        HI_ReadStop                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine stops the read transaction                                                */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS HI_ReadStop(void);


#endif

