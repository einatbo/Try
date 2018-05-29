/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2011 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/* File Contents:                                                                                          */
/*   touch.h                                                                                               */
/*            This file contains common touch structures and definitions                                   */
/* Project:                                                                                                */
/*            Multi Touch                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef  TOUCH_H
#define  TOUCH_H

#include "defs.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Touch coordinates structure                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct TOUCH_COORDINATES_T
{
    INT16   X;
    INT16   Y;

} TOUCH_COORDINATES_T;


/*---------------------------------------------------------------------------------------------------------*/
/* Touch point structure                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct TOUCH_POINT_T
{
    TOUCH_COORDINATES_T       coordinates;
    UINT8                     status;
    UINT8                     contactId;
} TOUCH_POINT_T;


/*---------------------------------------------------------------------------------------------------------*/
/* Touch axis structure                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    TOUCH_AXIS_X,
    TOUCH_AXIS_Y
} TOUCH_AXIS_T;




#endif
