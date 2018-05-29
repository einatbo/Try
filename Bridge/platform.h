
/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2012 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   platform.h                                                                                            */
/*            This file contains board specific defitions for Hannsar Zipper 7 inch prototype panel        */
/* Project:                                                                                                */
/*            Multi Touch                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef  BOARD_H
#define  BOARD_H

#define EINAT_LAB

#include "defs.h"
#include "host.h"


#define PANEL_X_SIZE               160L
#define PANEL_Y_SIZE               103L

#define X_OFFSET                   0
#define Y_OFFSET                   0

#ifdef EINAT_LAB
#define HOST_X_RESOLUTION          1280L
#define HOST_X_SIZE                370L


#define HOST_Y_RESOLUTION          1024L
#define HOST_Y_SIZE                300L


#elif defined EINAT_LAPTOP
#define HOST_X_RESOLUTION          1024L
#define HOST_X_SIZE                300L

#define HOST_Y_RESOLUTION          768L
#define HOST_Y_SIZE                190L

#elif defined MAX_LAPTOP
#define HOST_X_RESOLUTION          1280L
#define HOST_X_SIZE                300L

#define HOST_Y_RESOLUTION          800L
#define HOST_Y_SIZE                190L

#elif defined DELL_NTRIG_LAPTOP
#define HOST_X_RESOLUTION          1280L
#define HOST_X_SIZE                260L

#define HOST_Y_RESOLUTION          800L
#define HOST_Y_SIZE                165L

#elif defined HANNSTAR_LCM
#define HOST_X_RESOLUTION          1024L
#define HOST_X_SIZE                PANEL_X_SIZE

#define HOST_Y_RESOLUTION          600L
#define HOST_Y_SIZE                PANEL_Y_SIZE

#endif


#endif
