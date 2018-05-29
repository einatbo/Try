/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2011 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   timer.c                                                                                               */
/*            This file contains timer specific functionality                                              */
/* Project:                                                                                                */
/*           Mulit Touch                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define  TIMER_GLOBALS

#include "defs.h"
#include "hal.h"
#include "timer.h"

static UINT32  TIMER_MainSysTimestamp[TIMER_MAX];

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_Init                                                                             */
/*                                                                                                         */
/* Parameters:      TimerID   - the mudule we use                                                          */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initializes Timer counting                                                */
/*---------------------------------------------------------------------------------------------------------*/
void TIMER_Init(TIMER_MODULE TimerID)
{
    static BOOLEAN MFT_Init_called = FALSE;
    TIMER_MainSysTimestamp[TimerID] = 0;

    if (MFT_Init_called == FALSE)
    {
        MFT_Init();
        MFT_Init_called = TRUE;
    }
    MFT_Stop(TimerID, MFT_TIMER_1);
    if (!MFT_ConfigModule(TimerID, MFT_MODE_3, MFT_C_FLAG, 0, NULL, MFT_CKC_PRESCALE, MFT_CKC_PRESCALE, 0x1F))
    {
        return;
    }
    MFT_SetReload(TimerID, MFT_RELOAD_A, 0xFFFF);
    MFT_SetCounter(TimerID, MFT_TIMER_1, 0xFFFF);
    MFT_Start(TimerID, MFT_TIMER_1);

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_Restart                                                                          */
/*                                                                                                         */
/* Parameters:      TimerID   - the mudule we use                                                          */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This restarts Timer counting                                                           */
/*---------------------------------------------------------------------------------------------------------*/
void TIMER_Restart(TIMER_MODULE TimerID)
{
    MFT_Stop(TimerID, MFT_TIMER_1);
    MFT_SetCounter(TimerID, MFT_TIMER_1, 0xFFFF);
    MFT_Start(TimerID, MFT_TIMER_1);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_GetClockAndRestart                                                               */
/*                                                                                                         */
/* Parameters:      TimerID   - the mudule we use                                                          */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns Timer value and restarts Timer                                    */
/*---------------------------------------------------------------------------------------------------------*/
UINT16 TIMER_GetClockAndRestart(TIMER_MODULE TimerID)
{
    UINT16 GetClock = TIMER_GetClock(TimerID);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Restart timer for next measurement                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    TIMER_Restart(TimerID);

    return GetClock;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_GetClock                                                                         */
/*                                                                                                         */
/* Parameters:      TimerID   - the mudule we use                                                          */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns Timer value                                                       */
/*---------------------------------------------------------------------------------------------------------*/
UINT16 TIMER_GetClock(TIMER_MODULE TimerID)
{
    MFT_STS_T Timer_status;

    MFT_GetStatus(TimerID, &Timer_status);

    return (UINT16)TIMER_CONVERT_TICKS_TO_MS(~((UINT32)Timer_status.current_TnCnt1));
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_Delay                                                                            */
/*                                                                                                         */
/* Parameters:      TimerID   - the mudule we use                                                          */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns implements a delay using MFT timer                                */
/*---------------------------------------------------------------------------------------------------------*/
void TIMER_Delay(TIMER_MODULE TimerID, UINT numUS)
{
    MFT_STS_T Timer_status;

    UINT numTicks = (UINT)TIMER_CONVERT_US_TO_TICKS(numUS);

    MFT_Stop(TimerID, MFT_TIMER_2);
    MFT_SetCounter(TimerID, MFT_TIMER_2, numTicks);
    MFT_Start(TimerID, MFT_TIMER_2);

   do
   {
        MFT_GetStatus(TimerID, &Timer_status);
   }

   while (Timer_status.current_TnCnt2 < numTicks);
}




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
void   TIMER_UpdateRoundTimestamp(void)
{
    TIMER_MainSysTimestamp[TIMER_MAIN] += TIMER_CONVERT_TICKS_TO_MS((UINT32)TIMER_GetClockAndRestart(TIMER_MAIN));
}


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
UINT32 TIMER_GetRoundTimestamp(void)
{
    return TIMER_MainSysTimestamp[TIMER_MAIN];
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TIMER_LoopDelay                                                                        */
/*                                                                                                         */
/* Parameters:      MicroSeconds   - micro seconds to delay                                                */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine implements a delay using a while loop                                     */
/*---------------------------------------------------------------------------------------------------------*/
void TIMER_LoopDelay(UINT16 MicroSeconds)
{
    UINT32 nLoop = LOOPS_IN_MICROSECOND((UINT32)MicroSeconds);

    while(nLoop--) ;
}

static PERIODIC_CALLBACK PeriodicCallback_L;
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PeriodicHandler_L                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  arg -                                                                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is a periodic handler to be called each timer tick                        */
/*lint -efunc(715, PeriodicHandler_L)                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
static void PeriodicHandler_L(void* arg)
{
    PeriodicCallback_L();
}

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
void  PeriodicHandler_us(PERIODIC_CALLBACK PeriodicCallback, UINT16 us)
{
    (void)ITIM8_Init(0, ITIM8_SOURCE_CLOCK_2);
    PeriodicCallback_L = PeriodicCallback;
    (void)ITIM8_StartPeriodicEvent(0, 1000000L/us, PeriodicHandler_L, NULL);
}


#undef  TIMER_GLOBALS


