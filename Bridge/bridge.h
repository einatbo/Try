/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2010 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   bridge.h                                                                                              */
/*            This file is a header of Multi Touch bridge module                                           */
/* Project:                                                                                                */
/*            Multi Touch bridge                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

#include "hal.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Bridge module functions                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void        BRIDGE_Init(void);
DEFS_STATUS BRIDGE_Find(void);
DEFS_STATUS BRIDGE_SendToHost(void);
DEFS_STATUS BRIDGE_SendToMFTS(void);

#if defined BULK_MODE
UINT16      BRIDGE_GetBulkData  (void** data, UINT16 size);
void        BRIDGE_RecieveData  (void*  data, UINT16 size);
void        BRIDGE_GetBulkDataFinished(UINT16 size);
#endif


