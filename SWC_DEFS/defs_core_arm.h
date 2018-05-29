/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   defs_core_arm.h                                                                                       */
/*            This file contains definitions for ARM core and its compilers                                */
/*  Project:                                                                                               */
/*            SWC DEFS                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef _DEFS_ARM_H_
#define _DEFS_ARM_H_

/*---------------------------------------------------------------------------------------------------------*/
/* Core dependent types                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef UINT32              UINT;                       /* Native type of the core that fits the core's    */
typedef INT32               INT;                        /* internal registers                              */
typedef UINT                BOOLEAN;

/*---------------------------------------------------------------------------------------------------------*/
/* Core dependent PTR definitions                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define PTR8                (volatile  UINT8  *) 
#define PTR16               (volatile  UINT16 *)
#define PTR32               (volatile  UINT32 *)

/*---------------------------------------------------------------------------------------------------------*/
/* Core dependent MEM definitions                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define MEMR8(a)            (*(volatile UINT8  *)(a)) 
#define MEMR16(a)           (*(volatile UINT16 *)(a))
#define MEMR32(a)           (*(volatile UINT32 *)(a))

#define MEMW8(a,v)          (*((volatile UINT8  *)(a))) = ((UINT8)(v)) 
#define MEMW16(a,v)         (*((volatile UINT16 *)(a))) = ((UINT16)(v))
#define MEMW32(a,v)         (*((volatile UINT32 *)(a))) = ((UINT32)(v))

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
/* Macros for RVDS and ADS windows compilers                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#if defined(__ARMCC_VERSION) 

    /*-----------------------------------------------------------------------------------------------------*/
    /* Interrrupt macros                                                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    #define ENABLE_INTERRUPTS()     {                                                   \
                                        UINT32 __armcc_tmp;                             \
                                        __asm { MRS __armcc_tmp, CPSR;                  \
                                                BIC __armcc_tmp, __armcc_tmp, 0x80;     \
                                                MSR CPSR_c, __armcc_tmp; }              \
                                    }
    
    #define DISABLE_INTERRUPTS()    {                                                   \
                                       UINT32 __armcc_tmp;                              \
                                       __asm { MRS __armcc_tmp, CPSR;                   \
                                               ORR __armcc_tmp, __armcc_tmp, 0x80;      \
                                               MSR CPSR_c, __armcc_tmp; }               \
                                    }

    #define INTERRUPTS_SAVE_DISABLE(var)    {                                           \
                                                UINT32 __armcc_tmp;                     \
                                                __asm { MRS var, CPSR;                  \
                                                        ORR __armcc_tmp, var, 0x80;     \
                                                        MSR CPSR_c, __armcc_tmp;}       \
                                            }
    
    #define INTERRUPTS_RESTORE(var)     __asm{ MSR CPSR_c, var;}    


    /*-----------------------------------------------------------------------------------------------------*/
    /* Cache macros                                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    #define ICACHE_SAVE_DISABLE(var) {                                                  \
                                        UINT32 __armcc_temp_var;                        \
                                        __asm{ MRC  p15, 0, var, c1, c0;                \
                                               BIC  __armcc_temp_var, var, 0x1000;      \
                                               MCR  p15, 0, __armcc_temp_var, c1, c0; } \
                                     }
    
    #define ICACHE_SAVE_ENABLE(var)  {                                                  \
                                        UINT32 __armcc_temp_var;                        \
                                        __asm{ MRC  p15, 0, var, c1, c0;                \
                                               ORR  __armcc_temp_var, var, 0x1000;      \
                                               MCR  p15, 0, __armcc_temp_var, c1, c0; } \
                                     }

    
    #define ICACHE_RESTORE(var)         __asm{ MCR  p15, 0, var, c1, c0 }
    
/*---------------------------------------------------------------------------------------------------------*/
/* Macros for GCC and GCC based compilers                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#elif defined (__GNUC__)

    /*-----------------------------------------------------------------------------------------------------*/
    /* Interrrupt macros                                                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    #define ENABLE_INTERRUPTS()       __asm__ __volatile__(                         \
                                            "MRS    r7, CPSR     \n\t"              \
                                            "BIC    r7,r7,#0x80  \n\t"              \
                                            "MSR    CPSR_c,r7  " ::: "r7", "cc" )

    #define DISABLE_INTERRUPTS()      __asm__ __volatile__(                         \
                                            "MRS    r7, CPSR     \n\t"              \
                                            "ORR    r7, r7,#0x80 \n\t"              \
                                            "MSR    CPSR_c, r7 " ::: "r7", "cc")

    #define INTERRUPTS_SAVE_DISABLE(var)  __asm__ __volatile__(                     \
                                            "MRS    %[val], CPSR     \n\t"          \
                                            "ORR    r7, %[val],#0x80 \n\t"          \
                                            "MSR    CPSR_c,r7  "                    \
                                            :[val] "+r" (var):: "r7", "cc")

    #define INTERRUPTS_RESTORE(var)   __asm__ __volatile__(                         \
                                            "MSR    CPSR_c,%[val]  "                \
                                            : [val]"+r"(var) )
                                                

    /*-----------------------------------------------------------------------------------------------------*/
    /* Cache macros                                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    #define ICACHE_SAVE_DISABLE(var)  __asm__ __volatile__(                         \
                                            "MRC  p15, 0, %[val], c1, c0 \n\t"      \
                                            "BIC  r7, %[val], #0x1000     \n\t"     \
                                            "MCR  p15, 0, r7, c1, c0 "              \
                                            :[val] "+r" (var):: "r7", "cc")
    
    #define ICACHE_SAVE_ENABLE(var)   __asm__ __volatile__(                         \
                                            "MRC  p15, 0, %[val], c1, c0 \n\t"      \
                                            "ORR  r7, %[val], #0x1000     \n\t"     \
                                            "MCR  p15, 0, r7, c1, c0 "              \
                                            :[val] "+r" (var):: "r7", "cc")

    #define ICACHE_RESTORE(var)       __asm__ __volatile__(                         \
                                            "MCR  p15, 0, %[val], c1, c0"           \
                                            : [val]"+r"(var) )
    
#else
    #warning Warning: x_INTERRUPTS and ICHACHE_x macros are not defined for current compiler

    #define ENABLE_INTERRUPTS()
    #define DISABLE_INTERRUPTS()

    #define INTERRUPTS_SAVE_DISABLE(var)
    #define INTERRUPTS_RESTORE(var)
    
    #define ICACHE_SAVE_DISABLE(var)    
    #define ICACHE_SAVE_ENABLE(var)
    #define ICACHE_RESTORE(var)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Assertion macros                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef DEBUG
#define ASSERT(cond)  {if (!(cond)) for(;;) ;}           /* infinite loop                                  */
#else
#define ASSERT(cond)
#endif


#endif /* _DEFS_ARM_H_ */
