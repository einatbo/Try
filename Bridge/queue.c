/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2010 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/* File Contents:                                                                                          */
/*   queue.c                                                                                               */
/*            This file contains queue module implementation                                               */
/* Project:                                                                                                */
/*            Multi Touch bridge                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

#include "queue.h"

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                               LOCAL VARS                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
static UINT8  QUEUE_buffer[QUEUE_BUFFER_MAX];
static UINT16 QUEUE_nextByteToBeWritten_l;
static UINT16 QUEUE_nextByteToBeRead_l;
static UINT16 QUEUE_lastHighBoundary_l;
static UINT16 QUEUE_capacity_l;


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           INTERFACE FUNCTIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        QUEUE_Init                                                                             */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initialize queue.                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void QUEUE_Init(void)
{
    QUEUE_nextByteToBeWritten_l = 0;
    QUEUE_nextByteToBeRead_l = 0;
    QUEUE_lastHighBoundary_l = 0;
    QUEUE_capacity_l = 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        QUEUE_Allocate                                                                         */
/*                                                                                                         */
/* Parameters:      size - packet size that should be written to the buffer.                               */
/* Returns:         next byte to be written pointer or NULL in case of overflow.                           */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns next byte to be written pointer or NULL in case of overflow.      */
/*---------------------------------------------------------------------------------------------------------*/
void * QUEUE_Allocate(UINT16 size)
{
    BOOLEAN overlap = (size >  (QUEUE_BUFFER_MAX - QUEUE_nextByteToBeWritten_l));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check if overflow goign to occur                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (((QUEUE_nextByteToBeWritten_l < QUEUE_nextByteToBeRead_l) &&
        (QUEUE_nextByteToBeRead_l - QUEUE_nextByteToBeWritten_l) < size)
        ||
        ((QUEUE_nextByteToBeWritten_l > QUEUE_nextByteToBeRead_l) &&
        (overlap) && (size > QUEUE_nextByteToBeRead_l))
        ||
        ((QUEUE_nextByteToBeWritten_l == QUEUE_nextByteToBeRead_l) &&
        (QUEUE_capacity_l > 0)))

    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Overflow occurred                                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        return NULL;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* If found overlap                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (overlap)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Save last boundary                                                                              */
        /*-------------------------------------------------------------------------------------------------*/
        QUEUE_lastHighBoundary_l  = QUEUE_nextByteToBeWritten_l;
        /*-------------------------------------------------------------------------------------------------*/
        /* Update bytes written pointer                                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        QUEUE_nextByteToBeWritten_l = 0;

        /*-------------------------------------------------------------------------------------------------*/
        /* If next byte to be read pointer reach buffer boundary                                           */
        /*-------------------------------------------------------------------------------------------------*/
        if (QUEUE_nextByteToBeRead_l == QUEUE_lastHighBoundary_l)
        {
            QUEUE_nextByteToBeRead_l = 0;
        }
    }

    return (&QUEUE_buffer[QUEUE_nextByteToBeWritten_l]);

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        QUEUE_Peek                                                                             */
/*                                                                                                         */
/* Parameters:      size - number of bytes to read from buffer.                                            */
/*                  buff - address of next byte to be read pointer                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns number of bytes available , and update buff according to next     */
/*                  byte to be read.                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
UINT16 QUEUE_Peek(UINT16 size, void ** buff)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* If no new data is available                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    if (QUEUE_capacity_l == 0)
    {
        *buff = NULL;
        return 0;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Returned size is the minumum between the requested size and the remained size                       */
    /*-----------------------------------------------------------------------------------------------------*/
    if (QUEUE_nextByteToBeWritten_l > QUEUE_nextByteToBeRead_l)
    {
        size = MIN((QUEUE_nextByteToBeWritten_l - QUEUE_nextByteToBeRead_l), size);
    }
    else
    {
        size = MIN((QUEUE_lastHighBoundary_l - QUEUE_nextByteToBeRead_l), size);
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Update pointer according to next byte to be read                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    *buff = (&QUEUE_buffer[QUEUE_nextByteToBeRead_l]);
    return size;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        QUEUE_Pop                                                                              */
/*                                                                                                         */
/* Parameters:      size - number of bytes read from buffer.                                               */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine update next byte to be read according to size reported.                   */
/*---------------------------------------------------------------------------------------------------------*/
void QUEUE_Pop(UINT16 size)
{
    ASSERT(QUEUE_capacity_l >= size);
    QUEUE_capacity_l -= size;

    QUEUE_nextByteToBeRead_l += size;
    ASSERT(QUEUE_nextByteToBeRead_l <= QUEUE_lastHighBoundary_l);

    /*-----------------------------------------------------------------------------------------------------*/
    /* If next byte to be read pointer reach buffer boundary                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    if ((QUEUE_nextByteToBeRead_l == QUEUE_lastHighBoundary_l) &&
        (QUEUE_lastHighBoundary_l != QUEUE_nextByteToBeWritten_l))
    {
        QUEUE_nextByteToBeRead_l = 0;
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        QUEUE_Push                                                                             */
/*                                                                                                         */
/* Parameters:      size - number of bytes wrote to buffer.                                                */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine update next byte to be written according to size reported.                */
/*---------------------------------------------------------------------------------------------------------*/
void * QUEUE_Push(UINT16 size)
{
    QUEUE_nextByteToBeWritten_l += size;
    QUEUE_capacity_l += size;
    /*-----------------------------------------------------------------------------------------------------*/
    /* Update boundary if needed                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    if (QUEUE_nextByteToBeWritten_l > QUEUE_lastHighBoundary_l)
    {
        QUEUE_lastHighBoundary_l = QUEUE_nextByteToBeWritten_l;
    }

    return (&QUEUE_buffer[QUEUE_nextByteToBeWritten_l]);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        QUEUE_GetSize                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns the queue size (number of written bytes - number of read bytes)   */
/*---------------------------------------------------------------------------------------------------------*/
UINT16 QUEUE_GetSize(void)
{
    return QUEUE_capacity_l;
}

