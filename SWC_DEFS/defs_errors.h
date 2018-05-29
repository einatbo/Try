/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2006-2010 by Nuvoton Technology Corporation                                              */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*    defs_errors.h                                                                                        */
/*            This file contains NTIL standard error codes set                                             */
/* Project:                                                                                                */
/*            SWC DEFS                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef __ERRORS_H__
#define __ERRORS_H__


/*---------------------------------------------------------------------------------------------------------*/
/*                                               ERROR CODES                                               */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
   DEFS_STATUS_OK                                = 0x00,
   DEFS_STATUS_FAIL                              = 0x01,

   /*------------------------------------------------------------------------------------------------------*/
   /* Parameters validity                                                                                  */
   /*------------------------------------------------------------------------------------------------------*/
   DEFS_STATUS_INVALID_PARAMETER                 = 0x10,

   DEFS_STATUS_INVALID_DATA_SIZE                 = 0x11,
   DEFS_STATUS_PARAMETER_OUT_OF_RANGE            = 0x12,
   DEFS_STATUS_INVALID_DATA_FIELD                = 0x13,

   /*------------------------------------------------------------------------------------------------------*/
   /* Response                                                                                             */
   /*------------------------------------------------------------------------------------------------------*/
   DEFS_STATUS_RESPONSE_CANT_BE_PROVIDED         = 0x20,

   DEFS_STATUS_SYSTEM_BUSY                       = 0x21,
   DEFS_STATUS_SYSTEM_NOT_INITIALIZED            = 0x22,
   DEFS_STATUS_SYSTEM_IN_INCORRECT_STATE         = 0x23,
   DEFS_STATUS_RESPONSE_TIMEOUT                  = 0x24,
   DEFS_STATUS_RESPONSE_ABORT                    = 0x25,

   /*------------------------------------------------------------------------------------------------------*/
   /* Security                                                                                             */
   /*------------------------------------------------------------------------------------------------------*/
   DEFS_STATUS_SECURITY_ERROR                    = 0x30,

   DEFS_STATUS_UNSUFFIENT_PRIVILEDGE_LEVEL       = 0x31,
   DEFS_STATUS_AUTHENTICATION_FAIL               = 0x32,
   DEFS_STATUS_BAD_SIGNATURE                     = 0x33,

   /*------------------------------------------------------------------------------------------------------*/
   /* Communication                                                                                        */
   /*------------------------------------------------------------------------------------------------------*/
   DEFS_STATUS_COMMUNICATION_ERROR               = 0x40,

   DEFS_STATUS_NO_CONNECTION                     = 0x41,
   DEFS_STATUS_CANT_OPEN_CONNECTION              = 0x42,
   DEFS_STATUS_CONNECTION_ALREADY_OPEN           = 0x43,

   /*------------------------------------------------------------------------------------------------------*/
   /* Hardware                                                                                             */
   /*------------------------------------------------------------------------------------------------------*/
   DEFS_STATUS_HARDWARE_ERROR                    = 0x50,
   
   DEFS_STATUS_IO_ERROR                          = 0x51,
   DEFS_STATUS_CLK_ERROR                         = 0x52,
   
   /*------------------------------------------------------------------------------------------------------*/
   /* Custom error codes                                                                                   */
   /*------------------------------------------------------------------------------------------------------*/
   DEFS_STATUS_CUSTOM_ERROR_00                   = 0xF0,   
   DEFS_STATUS_CUSTOM_ERROR_01                   = 0xF1,
   DEFS_STATUS_CUSTOM_ERROR_02                   = 0xF2,
   DEFS_STATUS_CUSTOM_ERROR_03                   = 0xF3,
   DEFS_STATUS_CUSTOM_ERROR_04                   = 0xF4,
   DEFS_STATUS_CUSTOM_ERROR_05                   = 0xF5,
   DEFS_STATUS_CUSTOM_ERROR_06                   = 0xF6,
   DEFS_STATUS_CUSTOM_ERROR_07                   = 0xF7,
   DEFS_STATUS_CUSTOM_ERROR_08                   = 0xF8,
   DEFS_STATUS_CUSTOM_ERROR_09                   = 0xF9,
   DEFS_STATUS_CUSTOM_ERROR_10                   = 0xFA,
   DEFS_STATUS_CUSTOM_ERROR_11                   = 0xFB,
   DEFS_STATUS_CUSTOM_ERROR_12                   = 0xFC,
   DEFS_STATUS_CUSTOM_ERROR_13                   = 0xFD,
   DEFS_STATUS_CUSTOM_ERROR_14                   = 0xFE,
   DEFS_STATUS_CUSTOM_ERROR_15                   = 0xFF
} DEFS_STATUS;

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                              ERROR MACROS                                               */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#define DEFS_STATUS_RET_CHECK(func)                                 \
{                                                                   \
    DEFS_STATUS ret;                                                \
                                                                    \
    if ((ret = func) != DEFS_STATUS_OK)                             \
    {                                                               \
       return ret;                                                  \
    }                                                               \
}

#define DEFS_STATUS_COND_CHECK(cond, err)                           \
{                                                                   \
    if (!(cond))                                                    \
    {                                                               \
        return err;                                                 \
    }                                                               \
}

#endif /* __ERRORS_H__ */
