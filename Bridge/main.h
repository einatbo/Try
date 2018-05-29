/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   main.h                                                                                                */
/*            This file contains main definitions of Touch Bridge firmware                                 */
/* Project:                                                                                                */
/*            Touch bridge                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef  MAIN_H
#define  MAIN_H

#ifdef  MAIN_GLOBALS
#define MAIN_EXT
#else
#define MAIN_EXT extern
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*  Firmware version                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#define BRIDGE_FW_MAJOR                BRIDGE_MAJOR                                  /* (1 byte)                                      */
#define BRIDGE_FW_MINOR                BRIDGE_MINOR                                  /* (4 bits)                                      */
#define BRIDGE_FW_REVISION             BRIDGE_REVISION                               /* (4 bits)                                      */





#endif
