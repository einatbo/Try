/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   defs_core_cr16.h                                                                                      */
/*            This file contains definitions for CR18 core and its compilers                               */
/*  Project:                                                                                               */
/*            SWC DEFS                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef _DEFS_CR16_H_
#define _DEFS_CR16_H_

#include <asm.h>
/*---------------------------------------------------------------------------------------------------------*/
/* PSR register fields                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define PSR_E               9,   1
#define PSR_I               11,  1

/*---------------------------------------------------------------------------------------------------------*/
/* CFG register fields                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define CFG_DC              2,  1                       /* Data Cache bit                                  */
#define CFG_IC              4,  1                       /* Instruction Cache bit                           */


/*---------------------------------------------------------------------------------------------------------*/
/* Core dependent types                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef UINT16              UINT;                       /* Native type of the core that fits the core's    */
typedef INT16               INT;                        /* internal registers                              */
typedef UINT                BOOLEAN;

/*---------------------------------------------------------------------------------------------------------*/
/* Core dependent PTR definitions                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#if (__CRCC_VERSION__ == 31)
#define FAR                 __far
#elif (__CRCC_VERSION__ == 41)
#define FAR
#else
#error "Unsupported CR16 architecture"
#endif

#define PTR8                (FAR volatile UINT8 *) 
#define PTR16               (FAR volatile UINT16 *)
#define PTR32               (FAR volatile UINT32 *)

/*---------------------------------------------------------------------------------------------------------*/
/* Core dependent MEM definitions                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define MEMR8(a)            (*PTR8 (a)) 
#define MEMR16(a)           (*PTR16(a))
#define MEMR32(a)           (*PTR32(a))

#define MEMW8(a,v)          (*(PTR8  (a))) = ((UINT8)(v)) 
#define MEMW16(a,v)         (*(PTR16 (a))) = ((UINT16)(v))
#define MEMW32(a,v)         (*(PTR32 (a))) = ((UINT32)(v))

/*---------------------------------------------------------------------------------------------------------*/
/* Core dependent IO definitions                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define IOR8(a)             MEMR8(a)
#define IOR16(a)            MEMR16(a) 
#define IOR32(a)            MEMR32(a) 

#define IOW8(a,v)           MEMW8(a, v)
#define IOW16(a,v)          MEMW16(a, v) 
#define IOW32(a,v)          MEMW32(a, v) 

/*---------------------------------------------------------------------------------------------------------*/
/* Interrupts macros                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/* Enable interrupts - E and I bits                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define ENABLE_INTERRUPTS()                     \
{                                               \
    UINT16 tpsr = 0;                            \
    _spr_("psr", tpsr);                         \
     SET_VAR_FIELD(tpsr, PSR_E, 1);             \
     SET_VAR_FIELD(tpsr, PSR_I, 1);             \
    _lpr_("psr",tpsr);                          \
}

/*---------------------------------------------------------------------------------------------------------*/
/* Disable interrupts - E and I bits                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#define DISABLE_INTERRUPTS()                    \
{                                               \
    UINT16 tpsr = 0;                            \
    _spr_("psr", tpsr);                         \
    SET_VAR_FIELD(tpsr, PSR_E, 0);              \
    SET_VAR_FIELD(tpsr, PSR_I, 0);              \
    _lpr_("psr",tpsr);                          \
}

/*---------------------------------------------------------------------------------------------------------*/
/* Preserve interrupt state and disable                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define INTERRUPTS_SAVE_DISABLE(var)            \
{                                               \
    _spr_("psr",var);                           \
    _di_();                                     \
}

/*---------------------------------------------------------------------------------------------------------*/
/* Restore saved interrupt state - E bit only                                                              */
/*---------------------------------------------------------------------------------------------------------*/

#define INTERRUPTS_RESTORE(var)                 \
{                                               \
    if (READ_VAR_FIELD(var, PSR_E))             \
    {                                           \
        _ei_();                                 \
    }                                           \
}


/*---------------------------------------------------------------------------------------------------------*/
/* Cache macros utilities                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define __CACHE_SAVE_DISABLE(var, type)         \
{                                               \
    UINT tmp = 0;                               \
    _spr_("cfg",tmp);                           \
    var = READ_VAR_FIELD(tmp, CFG_##type);      \
    SET_VAR_FIELD(tmp, CFG_##type, 0);          \
    _lpr_("cfg",tmp);                           \
}

#define __CACHE_SAVE_ENABLE(var, type)          \
{                                               \
    UINT tmp = 0;                               \
    _spr_("cfg",tmp);                           \
    var = READ_VAR_FIELD(tmp, CFG_##type);      \
    SET_VAR_FIELD(tmp, CFG_##type, 1);          \
    _lpr_("cfg",tmp);                           \
}

#define __CACHE_RESTORE(var, type)              \
{                                               \
    UINT tmp = 0;                               \
    _spr_("cfg",tmp);                           \
    SET_VAR_FIELD(tmp, CFG_##type, var);        \
    _lpr_("cfg",tmp);                           \
}

/*---------------------------------------------------------------------------------------------------------*/
/* Cache macros                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define ICACHE_SAVE_DISABLE(var)        __CACHE_SAVE_DISABLE(var, IC)
#define ICACHE_SAVE_ENABLE(var)         __CACHE_SAVE_ENABLE(var, IC)
#define ICACHE_RESTORE(var)             __CACHE_RESTORE(var, IC)

#define DCACHE_SAVE_DISABLE(var)        __CACHE_SAVE_DISABLE(var, DC)
#define DCACHE_SAVE_ENABLE(var)         __CACHE_SAVE_ENABLE(var, DC)
#define DCACHE_RESTORE(var)             __CACHE_RESTORE(var, DC)


/*---------------------------------------------------------------------------------------------------------*/
/* CPU power management                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define CPU_IDLE()              _eiwait_()

/*---------------------------------------------------------------------------------------------------------*/
/* Assertion macros                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef DEBUG
#define ASSERT(cond)  {if(!(cond)) __asm__("excp bpt"); }
#else
#define ASSERT(cond)
#endif


#endif /*_DEFS_CR16_H_ */
