/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2011 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   hid.h                                                                                                 */
/*            This file contains ...                                                                       */
/* Project:                                                                                                */
/*            Timer specific defintions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef  TIMER_H
#define  TIMER_H

#include "defs.h"



#define PLATFORM_TIMER_CONVERT_TICKS_TO_MS(x)          ((UINT32)32UL*(x)/(HFCG_CORE_CLK/1000UL))
#define PLATFORM_TIMER_CONVERT_TICKS_TO_US(x)          ((UINT32)32UL*(x)/(HFCG_CORE_CLK/1000000UL))
#define PLATFORM_TIMER_CONVERT_US_TO_TICKS(x)          ((UINT32)(HFCG_CORE_CLK/1000000UL)*(x)/32UL)
#define PLATFORM_SYSTEM_LOOPS_IN_MICROSECOND(x)        ((UINT32)((((x)>3)? (x): 3)-3)*8333/1000)

#define TIMER_CONVERT_TICKS_TO_MS(x)          PLATFORM_TIMER_CONVERT_TICKS_TO_MS(x)
#define TIMER_CONVERT_TICKS_TO_US(x)          PLATFORM_TIMER_CONVERT_TICKS_TO_US(x)
#define TIMER_CONVERT_US_TO_TICKS(x)          PLATFORM_TIMER_CONVERT_US_TO_TICKS(x)
#define SYSTEM_LOOPS_IN_MICROSECOND(x)        PLATFORM_SYSTEM_LOOPS_IN_MICROSECOND(x)


typedef enum {
    TIMER_MAIN = 0,
    TIMER_LOCAL,
    TIMER_MAX
} TIMER_MODULE;

/*---------------------------------------------------------------------------------------------------------*/
/* Interface macros                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define LOOPS_IN_MICROSECOND(x)        SYSTEM_LOOPS_IN_MICROSECOND(x)

/*---------------------------------------------------------------------------------------------------------*/
/* Timer API                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_Init                                                                             */
/*                                                                                                         */
/* Parameters:      TimerID   - the mudule we use                                                          */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initializes Timer counting                                                */
/*---------------------------------------------------------------------------------------------------------*/
void   TIMER_Init(TIMER_MODULE TimerID);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_Restart                                                                          */
/*                                                                                                         */
/* Parameters:      TimerID   - the mudule we use                                                          */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This restarts Timer counting                                                           */
/*---------------------------------------------------------------------------------------------------------*/
void   TIMER_Restart(TIMER_MODULE TimerID);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_GetClock                                                                         */
/*                                                                                                         */
/* Parameters:      TimerID   - the mudule we use                                                          */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns Timer value                                                       */
/*---------------------------------------------------------------------------------------------------------*/
UINT16 TIMER_GetClock(TIMER_MODULE TimerID);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_GetClockAndRestart                                                               */
/*                                                                                                         */
/* Parameters:      TimerID   - the mudule we use                                                          */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns Timer value and restarts Timer                                    */
/*---------------------------------------------------------------------------------------------------------*/
UINT16 TIMER_GetClockAndRestart(TIMER_MODULE TimerID);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_UpdateRoundTimestamp                                                             */
/*                                                                                                         */
/* Parameters:      TimerID   - the mudule we use                                                          */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                                                                         */
/*                  This routine updates per-round system variables representing main system clock and     */
/*                  number of ticks per round                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void   TIMER_UpdateRoundTimestamp(void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_GetRoundTimestamp                                                                */
/*                                                                                                         */
/* Parameters:      TimerID   - the mudule we use                                                          */
/* Returns:         main system clock tick                                                                 */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                                                                                                         */
/*                  This return the main system clock tick at the time TIMER_UpdateRoundTimestamp() was called       */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 TIMER_GetRoundTimestamp(void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_Delay                                                                            */
/*                                                                                                         */
/* Parameters:      TimerID   - the mudule we use                                                          */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns implements a delay using MFT timer                                */
/*---------------------------------------------------------------------------------------------------------*/
void   TIMER_Delay(TIMER_MODULE TimerID, UINT numTicks);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_LoopDelay                                                                        */
/*                                                                                                         */
/* Parameters:      MicroSeconds   - micro seconds to delay                                                */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine implements a delay using a while loop                                     */
/*---------------------------------------------------------------------------------------------------------*/
void   TIMER_LoopDelay(UINT16 MicroSeconds);


typedef void (*PERIODIC_CALLBACK)(void);
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PeriodicHandler_us                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  PeriodicCallback - Application callback to be called periodically.                     */
/*                  us               - desired peridic in micro seconds                                    */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine makes a periodic handler that calls 'PeriodicCallback' every 'us' micro   */
/*                  seconds                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
void  PeriodicHandler_us(PERIODIC_CALLBACK PeriodicCallback, UINT16 us);

#endif

