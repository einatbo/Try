/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2010 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/* File Contents:                                                                                          */
/*   queue.h                                                                                               */
/*            This file is a header of queue module                                                        */
/* Project:                                                                                                */
/*            Multi Touch bridge                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

#include "defs.h"

#define QUEUE_BUFFER_MAX             1024


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                         QUEUE modulw functions                                          */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
void QUEUE_Init(void);
void * QUEUE_Allocate(UINT16 size);
UINT16 QUEUE_Peek(UINT16 size, void ** buff);
void QUEUE_Pop(UINT16 size);
void * QUEUE_Push(UINT16 size);
UINT16 QUEUE_GetSize(void);

