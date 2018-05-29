
/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2011 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/* File Contents:                                                                                          */
/*   touch_utils.c                                                                                         */
/*            This file implementation of utilities useful it touch applications                           */
/* Project:                                                                                                */
/*            Multi Touch                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                INCLUDES                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include "touch_utils.h"
#include <string.h>



/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                               DEFINITIONS                                               */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Touch Utilities module definitions                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define MAX_NUM_ELEMENTS                        25


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           INTERFACE FUNCTIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TOUCH_UTILS_WeightedAverageInterpolation                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  numElements -                                                                          */
/*                  valuesBuffer -                                                                         */
/*                  weight -                                                                               */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs a weighter average interpolation given a buffer of values        */
/*---------------------------------------------------------------------------------------------------------*/
UINT16 TOUCH_UTILS_WeightedAverageInterpolation(UINT16* valuesBuffer, UINT numElements, UINT weight)
{
    UINT i;
    UINT16 result;
    UINT32 counterAcc = 0, weightedCounterAcc = 0;

    for (i = 0; i < numElements; i++)
    {
        counterAcc         += (UINT32)valuesBuffer[i];
        weightedCounterAcc += (UINT32)(valuesBuffer[i]*(i*((UINT32)weight) + weight/2));
    }

    if (counterAcc == 0)
    {
        result = 0;
    }
    else
    {
        result = (UINT16)TOUCH_UTILS_UDIV(weightedCounterAcc, counterAcc);
    }

    return result;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TOUCH_UTILS_Average                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  numElements -                                                                          */
/*                  takeZeroElements - boolean indicating whether to take in account zero elements         */
/*                  valuesBuffer -                                                                         */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine calculates average either of a buffer of values or of non-zero elemetns   */
/*---------------------------------------------------------------------------------------------------------*/
UINT16  TOUCH_UTILS_Average(UINT16* valuesBuffer, UINT numElements, UINT minimalValue)
{
    UINT i;
    UINT16 result = 0;
    UINT32 counterAcc = 0, weightedCounterAcc = 0;

    for (i = 0; i < numElements; i++)
    {
        if (valuesBuffer[i] >= minimalValue)
        {
           weightedCounterAcc += valuesBuffer[i];
           counterAcc++;
        }
    }
    if (counterAcc != 0)
    {
        result = (UINT16)TOUCH_UTILS_UDIV(weightedCounterAcc, counterAcc);
    }

    return result;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TOUCH_UTILS_Median                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  numElements -                                                                          */
/*                  valuesBuffer -                                                                         */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine finds a median of series of values                                        */
/*---------------------------------------------------------------------------------------------------------*/
UINT16 TOUCH_UTILS_Median(UINT16* valuesBuffer, UINT numElements)
{
    UINT16 sortedValuesBuffer[MAX_NUM_ELEMENTS];

    if (numElements == 1)
    {
        return valuesBuffer[0];
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Sort the values                                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    memcpy((void*)sortedValuesBuffer, (void*)valuesBuffer, numElements*sizeof(UINT16));
    TOUCH_UTILS_Sort(sortedValuesBuffer, numElements);

    /*-----------------------------------------------------------------------------------------------------*/
    /* If number of elements is odd, take middle value, otherwise take average of 2 middle values          */
    /*-----------------------------------------------------------------------------------------------------*/
    if (numElements & 1)
    {
        return sortedValuesBuffer[numElements/2];
    }
    else
    {
        return AVG(sortedValuesBuffer[numElements/2], sortedValuesBuffer[numElements/2-1]);
    }

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TOUCH_UTILS_Sort                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  numElements -                                                                          */
/*                  valuesBuffer -                                                                         */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:    The routine changes the input "valuesBuffer"                                           */
/* Description:                                                                                            */
/*                  This routine sorts the buffer of values                                                */
/* PC Lint messages surpression:                                                                           */
/*lint -e{679} Suspicious Truncation in arithmetic expression combining with pointer                       */
/*---------------------------------------------------------------------------------------------------------*/
void TOUCH_UTILS_Sort(UINT16* valuesBuffer, UINT numElements)
{
    UINT i, n = numElements - 1;
    UINT16 res;
    BOOLEAN changed = TRUE;

    while (changed)
    {
        changed = FALSE;
        for (i = 0; i < n; i++)
        {
            if (valuesBuffer[i] > valuesBuffer[i+1])
            {
                res = valuesBuffer[i];
                valuesBuffer[i] = valuesBuffer[i+1];
                valuesBuffer[i+1] = res;
                changed = TRUE;
            }
        }
        n--;
    }
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TOUCH_UTILS_SortIndices                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  indicesBuffer -                                                                        */
/*                  numElements -                                                                          */
/*                  valuesBuffer -                                                                         */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:    The routine changes the input "indicesBuffer"                                          */
/* Description:                                                                                            */
/*                  This routine sorts indiciesBuffer according to values of valuesBuffer                  */
/*---------------------------------------------------------------------------------------------------------*/
void TOUCH_UTILS_SortIndices(UINT16* valuesBuffer, UINT16* indicesBuffer, UINT numElements)
{
    UINT i, n = numElements - 1;
    UINT16 res;
    BOOLEAN changed = TRUE;

    while (changed)
    {
        changed = FALSE;
        for (i = 0; i < n; i++)
        {
            if (valuesBuffer[indicesBuffer[i]] < valuesBuffer[indicesBuffer[(UINT)(i+1)]])
            {
                res = indicesBuffer[i];
                indicesBuffer[i] = indicesBuffer[(UINT)(i+1)];
                indicesBuffer[(UINT)(i+1)] = res;
                changed = TRUE;
            }
        }
        n--;
    }
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TOUCH_UTILS_Max                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  numElements -                                                                          */
/*                  valuesBuffer -                                                                         */
/*                                                                                                         */
/* Returns:         the index of a maximal element                                                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine finds a maximal value out of series of unsigned values                    */
/*                  If there is a plateu of maximum values, it returns the center of the plateu            */
/* PC Lint messages surpression:                                                                           */
/*lint -e{679} Suspicious Truncation in arithmetic expression combining with pointer                       */
/*---------------------------------------------------------------------------------------------------------*/
UINT16 TOUCH_UTILS_Max(UINT16* valuesBuffer, UINT numElements)
{
    UINT i, maxIndex = 0, plateuStart = 0, plateuEnd = 0;
    UINT16  max = 0;
    BOOLEAN plateu = FALSE;


    for (i = 0; i < numElements; i++)
    {
        if (valuesBuffer[i] > max)
        {
            max = valuesBuffer[i];
            maxIndex = i;
            plateuStart = i;
        }
        else if (valuesBuffer[i] == max)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Is it a plateu?                                                                             */
            /*---------------------------------------------------------------------------------------------*/
            if (i == maxIndex + 1)
            {
                plateu = TRUE;
                maxIndex = i;
            }
        }
        else
        {
            plateuEnd = i;
        }
    }

    if (plateu)
    {
        if (plateuEnd < plateuStart)
        {
            plateuEnd = i;
        }
        maxIndex = plateuStart + (plateuEnd - plateuStart)/2;
    }

    return maxIndex;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TOUCH_UTILS_LocalMinimum                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  numElements -                                                                          */
/*                  valuesBuffer -                                                                         */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine finds first local minumum in a series of values                           */
/* PC Lint messages surpression:                                                                           */
/*lint -e{679} Suspicious Truncation in arithmetic expression combining with pointer                       */
/*---------------------------------------------------------------------------------------------------------*/
UINT16 TOUCH_UTILS_LocalMinimum(UINT16* valuesBuffer, UINT numElements)
{
    UINT i,localMinIndex = 0;

    for (i = 1; i < numElements - 1; i++)
    {
        if ((valuesBuffer[i] < valuesBuffer[i - 1]) && (valuesBuffer[i] < valuesBuffer[i + 1]))
        {
            localMinIndex = i;
        }
    }

    return localMinIndex;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TOUCH_UTILS_ZerosBuffer                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  numElements -                                                                          */
/*                  valuesBuffer -                                                                         */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine examines a buffer and checks whether it consists of zero values only      */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN TOUCH_UTILS_ZerosBuffer(UINT16* valuesBuffer, UINT numElements)
{
    UINT i;
    BOOLEAN zerosBuffer = TRUE;

    for (i = 0; i < numElements; i++)
    {
        if  (valuesBuffer[i] != 0)
        {
            zerosBuffer = FALSE;
        }
    }

    return zerosBuffer;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TOUCH_Parabolic3PointsInterpolation                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  numElements -                                                                          */
/*                  valuesBuffer -                                                                         */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs 3 points interpolation of series of values                       */
/* PC Lint messages surpression:                                                                           */
/*lint -e{679} Suspicious Truncation in arithmetic expression combining with pointer                       */
/*lint -e{771}  Symbol 'parabolicFix' conceivably not initialized                                          */
/*---------------------------------------------------------------------------------------------------------*/
UINT16 TOUCH_UTILS_Parabolic3PointsInterpolation(UINT16* valuesBuffer, UINT numElements, UINT weight, UINT startIndex)
{
    UINT16  bestFit = 1, constant, numFixes = numElements - 2, i;
    INT16   fix = 0;
    INT16   parabolicFix[MAX_NUM_ELEMENTS];
    UINT16  parabolaRatings[MAX_NUM_ELEMENTS];


    if (numFixes > MAX_NUM_ELEMENTS) numFixes = MAX_NUM_ELEMENTS;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Perform 3 parabolic interpolation aroud the maximum                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    for (i = 0; i < numFixes; i++)
    {
        parabolicFix[i] = TOUCH_UTILS_ParabolaCenter((INT16)(valuesBuffer[i]),
                                     (INT16)(valuesBuffer[i+1]), (INT16)(valuesBuffer[i+2]),
                                     weight, &parabolaRatings[i]);
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Find best parabola and take its fit                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    bestFit = TOUCH_UTILS_Max(parabolaRatings, numFixes);
    fix     = parabolicFix[bestFit];

    /*-----------------------------------------------------------------------------------------------------*/
    /* Calculate the coordinate of the parabola center                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    constant = (UINT)((startIndex + bestFit + 1)*weight - weight/2);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Return fixed center coordinate                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    return (UINT16)((INT)constant + fix);

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TOUCH_UTILS_ParabolaCenter                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  A -                                                                                    */
/*                  B -                                                                                    */
/*                  C -                                                                                    */
/*                  factor88 -                                                                             */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/* This routine performs a parabolic approximation out of 3 points A, B and C and returns                  */
/* a fix offset from the center                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
INT16 TOUCH_UTILS_ParabolaCenter(INT16 f_nminus1, INT16 f_n0, INT16 f_nplus1, UINT16 unitWeight, UINT16* rating)
{

    INT32 div, xMax, sumExternal, doublePeak;
    INT32 z1, z2, z3;

    z1 = (INT32)f_nminus1;
    z2 = (INT32)f_n0;
    z3 = (INT32)f_nplus1;

    sumExternal = z1 + z3 + 1;
    doublePeak  = 2*z2;

    div = (sumExternal - doublePeak);
    if (div == 0) div = 1;

    *rating = (UINT16)TOUCH_UTILS_SDIV(doublePeak, sumExternal);


    xMax = (z1 - z3);
    xMax*= (INT32)unitWeight;
    xMax = DIV_CEILING(xMax, div);
    xMax = xMax/2;


    if (xMax < -(INT16)unitWeight/2)
        xMax = -(INT16)unitWeight/2;

    if (xMax > (INT16)unitWeight/2)
        xMax = (INT16)unitWeight/2;

    return (INT16)xMax;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TOUCH_UTILS_SQRT                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  a -  number to take a square root from                                                 */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine implements integer square root                                            */
/*---------------------------------------------------------------------------------------------------------*/
UINT16   TOUCH_UTILS_SQRT(UINT32 a)
{
    register INT i;
    register UINT32 rem  = 0;
    register UINT32 root = 0;

    for (i = 0; i < 16; i++)
    {
        root <<= 1;
        rem = ((rem << 2) + (a >> 30));
        a <<= 2;
        root++;

        if (root <= rem)
        {
            rem -= root;
            root++;
        }
        else
        {
            root--;
        }
    }
    return (UINT16) (root >> 1);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TOUCH_UTILS_UDIV                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  n   - Divident                                                                         */
/*                  d   - Divisor                                                                          */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs unsigned 32->32 division                                         */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 TOUCH_UTILS_UDIV(UINT32 n, UINT32 d)
{
    register UINT32 shifts      = 0;
    register UINT32 rquo        = 0;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check if d is zero and generate a DVZ trap                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    if (d == 0)
    {
        return 0;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check if d > n                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    if (d > n)
    {
        return 0;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check if d == 1                                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    if (d == 1)
    {
        return n;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check if divisor is power of 2                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    if ((d & (d-1)) == 0)
    {
        return n >> TOUCH_UTILS_CTZ(d);
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Shift the divisor left until it is either greater than the dividend or has a one in its top bit     */
    /*-----------------------------------------------------------------------------------------------------*/
    shifts = TOUCH_UTILS_CLZ(d) - TOUCH_UTILS_CLZ(n);
    d <<= shifts;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Test for the case where the divisor has a one in its top bit but is still less than the             */
    /* dividend                                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    if (d <= n)
    {
        n -= d;
        rquo++;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* For each shift performed earlier, shift the divisor right one bit, shift the quotient left one      */
    /* bit, and if the dividend is greater than or equal to the divisor, subtract the divisor from the     */
    /* dividend and increment the quotient                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    while (shifts > 0)
    {
        shifts--;
        d >>= 1;
        rquo <<= 1;

        if (n >= d)
        {
            n -= d;
            rquo++;
        }
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Return quotient                                                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    return rquo;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TOUCH_UTILS_SDIV                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  n   - Divident                                                                         */
/*                  d   - Divisor                                                                          */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs signed 32->32 division                                           */
/*---------------------------------------------------------------------------------------------------------*/
INT32 TOUCH_UTILS_SDIV(INT32 n, INT32 d)
{
    register BOOLEAN neg_n  = FALSE;
    register BOOLEAN neg_d  = FALSE;
    register INT32   ret    = 0;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check if N is negative                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    if (n < 0)
    {
        neg_n = TRUE;
        n = -n;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check if D is negative                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    if (d < 0)
    {
        neg_d = TRUE;
        d = -d;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Calculate unsigned division                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    ret = (INT32)TOUCH_UTILS_UDIV((UINT32)n, (UINT32)d);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Add sign                                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    if (neg_n != neg_d)
    {
        ret = -ret;
    }

    return ret;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TOUCH_UTILS_CLZ                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  x -                                                                                    */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns number of leading zeros in 32bit number                           */
/*---------------------------------------------------------------------------------------------------------*/
UINT8 TOUCH_UTILS_CLZ(UINT32 x)
{
    register UINT8 num;

    static UINT8 clzlut[256] =
    {
        8,7,6,6,5,5,5,5,
        4,4,4,4,4,4,4,4,
        3,3,3,3,3,3,3,3,
        3,3,3,3,3,3,3,3,
        2,2,2,2,2,2,2,2,
        2,2,2,2,2,2,2,2,
        2,2,2,2,2,2,2,2,
        2,2,2,2,2,2,2,2,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0
    };

    register UINT8 b0 = LSB0(x);
    register UINT8 b1 = LSB1(x);
    register UINT8 b2 = LSB2(x);
    register UINT8 b3 = LSB3(x);

    if ((num = clzlut[b3]) == 8)
    {
        if ((num += clzlut[b2]) == 16)
        {
            if ((num += clzlut[b1]) == 24)
            {
                num += clzlut[b0];
            }
        }
    }

    return num;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        TOUCH_UTILS_CTZ                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  x -                                                                                    */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns nubmer of trailing zeros                                          */
/*---------------------------------------------------------------------------------------------------------*/
UINT8 TOUCH_UTILS_CTZ(UINT32 x)
{
    register UINT8 c;

    if (x & 0x1)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* special case for odd X (assumed to happen half of the time)                                     */
        /*-------------------------------------------------------------------------------------------------*/
        c = 0;
    }
    else
    {
        c = 1;

        if ((x & 0xffff) == 0)
        {
            x >>= 16;
            c += 16;
        }

        if ((x & 0xff) == 0)
        {
            x >>= 8;
            c += 8;
        }

        if ((x & 0xf) == 0)
        {
            x >>= 4;
            c += 4;
        }

        if ((x & 0x3) == 0)
        {
            x >>= 2;
            c += 2;
        }

        c -= x & 0x1;
    }

    return c;
}


