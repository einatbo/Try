/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2011 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/* File Contents:                                                                                          */
/*   capkey.h                                                                                              */
/*            This file contains header of utilities useful in touch applications                          */
/* Project:                                                                                                */
/*            Multi Touch                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef  TOUCH_UTILES_H
#define  TOUCH_UTILES_H


#include "defs.h"

/*---------------------------------------------------------------------------------------------------------*/
/*                                         Touch Utils interface definitions                               */
/*---------------------------------------------------------------------------------------------------------*/
#define RATIO_MULTIPILER             100

/*---------------------------------------------------------------------------------------------------------*/
/* Utility macros                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/

#define DISTANCE_SQR(X1, X2, Y1, Y2)                     ((UINT32)(SQR((INT32)X1-(INT32)X2) +        \
                                                                 SQR((INT32)Y1-(INT32)Y2)))


#define DISTANCE(X1, X2, Y1, Y2)                        (INT32)TOUCH_UTILS_SQRT(DISTANCE_SQR(X1, X2, Y1, Y2))



#define DELTA(X1, X2)                                   ((INT32)X1-(INT32)X2)

#define DISTANCE_PYTHAGORAS_SQR(deltaX, deltaY)         (SQR((INT32)deltaX) +  SQR((INT32)deltaY))

#define DISTANCE_PYTHAGORAS(deltaX, deltaY)             TOUCH_UTILS_SQRT((UINT32)DISTANCE_PYTHAGORAS_SQR(deltaX, deltaY))

#define DISTANCE_MANHATTAN(deltaX, deltaY)              ((INT32)ABS(deltaX) + (INT32)ABS(deltaY))

#define BASELINE_RATIO_MULTIPILER                       10L
#define IIR(x, y, coef)                               (((BASELINE_RATIO_MULTIPILER * x * (coef - 1))/coef + \
                                                         (BASELINE_RATIO_MULTIPILER * y)/coef + BASELINE_RATIO_MULTIPILER/2)/ \
                                                         BASELINE_RATIO_MULTIPILER)



/*---------------------------------------------------------------------------------------------------------*/
/*                                         Touch Utils interface functions                                 */
/*---------------------------------------------------------------------------------------------------------*/
UINT16  TOUCH_UTILS_Average(UINT16* valuesBuffer, UINT numElements, UINT minimalValue);
UINT16  TOUCH_UTILS_Median (UINT16* valuesBuffer, UINT numElements);
void    TOUCH_UTILS_Sort   (UINT16* valuesBuffer, UINT numElements);
void    TOUCH_UTILS_SortIndices(UINT16* valuesBuffer, UINT16* indicesBuffer, UINT numElements);
UINT16  TOUCH_UTILS_Max    (UINT16* valuesBuffer, UINT numElements);
UINT16  TOUCH_UTILS_LocalMinimum (UINT16* valuesBuffer, UINT numElements);
BOOLEAN TOUCH_UTILS_ZerosBuffer(UINT16* valuesBuffer, UINT numElements);
UINT16  TOUCH_UTILS_WeightedAverageInterpolation (UINT16* valuesBuffer, UINT numElements, UINT weight);
UINT16  TOUCH_UTILS_Parabolic3PointsInterpolation(UINT16* valuesBuffer, UINT numElements, UINT weight, UINT startIndex);
INT16   TOUCH_UTILS_ParabolaCenter(INT16 f_nminus1, INT16 f_n0, INT16 f_nplus1, UINT16 unitWeight, UINT16* rating);
UINT16  TOUCH_UTILS_SQRT(UINT32);
UINT32  TOUCH_UTILS_UDIV(UINT32 n, UINT32 d);
INT32   TOUCH_UTILS_SDIV(INT32 n, INT32 d);
UINT8   TOUCH_UTILS_CLZ(UINT32 x);
UINT8   TOUCH_UTILS_CTZ(UINT32 x);



#endif
