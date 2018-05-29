/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   defs_os_windows.h                                                                                     */
/*            This file contains definitions for Windows OS Applications                                   */
/*  Project:                                                                                               */
/*            SWC DEFS                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef _DEFS_WINDOWS_H_
#define _DEFS_WINDOWS_H_

#include <windows.h>

/*---------------------------------------------------------------------------------------------------------*/
/* OS dependent types                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
typedef unsigned int        UINT;                       /* Native type of the core that fits the core's    */
typedef int                 INT;                        /* internal registers                              */

/* Windows already define BOOLEAN basic type - no need to define it here                                   */

/*---------------------------------------------------------------------------------------------------------*/
/* OS dependent PTR definitions                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define PTR8                (volatile  UINT8  *) 
#define PTR16               (volatile  UINT16 *)
#define PTR32               (volatile  UINT32 *)

#endif /* _DEFS_WINDOWS_H_ */
