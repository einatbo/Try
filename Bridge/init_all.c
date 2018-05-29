/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2010 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   init_all.c                                                                                            */
/*            This file contains CR sections initialization                                                */
/* Project:                                                                                                */
/*            <my project>                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef NO_LIBC
    #include <string.h>
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Type of pointer to function returning void                                                              */
/*---------------------------------------------------------------------------------------------------------*/
 typedef void (*func_ptr) (void);


/*---------------------------------------------------------------------------------------------------------*/
/* External symbols from linker definition file                                                            */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Default Model                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
extern const char _DATA_IMAGE_START;
extern char _DATA_START, _DATA_END, _BSS_START, _BSS_END;

/*---------------------------------------------------------------------------------------------------------*/
/* Near Model                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
extern const char _NDATA_IMAGE_START;
extern char _NDATA_START, _NDATA_END, _NBSS_START, _NBSS_END;

/*---------------------------------------------------------------------------------------------------------*/
/* Far Model                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#if defined (__CR16CP__) && defined (__DATA_FAR__)
    extern const char _FDATA_IMAGE_START;
    extern char _FDATA_START, _FDATA_END, _FBSS_START, _FBSS_END;
#endif



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        init_bss_data                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine Copies data from source address (ROM) to destination address (RAM), and   */
/*                  clears the bss section of uninitialized data (RAM)                                     */
/*---------------------------------------------------------------------------------------------------------*/
void CR_InitBssData(void)
{
#ifdef NO_LIBC
    long i;
    char * mem_ptr;
#endif

#ifndef NO_LIBC     //  use libc routines

    /*-----------------------------------------------------------------------------------------------------*/
    /* Copy data from ROM to RAM                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------------------------------------*/
    /* Default Model                                                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    memcpy((void *)&_DATA_START, (void *)&_DATA_IMAGE_START,
                        (size_t)(&_DATA_END - &_DATA_START));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Near Model                                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    memcpy((void *)&_NDATA_START, (void *)&_NDATA_IMAGE_START,
                        (size_t)(&_NDATA_END - &_NDATA_START));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Far Model                                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    #if defined (__CR16CP__) && defined (__DATA_FAR__)
        memcpy((void *)&_FDATA_START, (void *)&_FDATA_IMAGE_START,
            (size_t)(&_FDATA_END - &_FDATA_START));
    #endif

    /*-----------------------------------------------------------------------------------------------------*/
    /* Fill the uninitialized data section with 0                                                          */
    /*-----------------------------------------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------------------------------------*/
    /* Default Model                                                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    memset((void *)&_BSS_START, 0, (size_t)(&_BSS_END - &_BSS_START));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Near Model                                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    memset((void *)&_NBSS_START, 0, (size_t)(&_NBSS_END - &_NBSS_START));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Far Model                                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    #if defined (__CR16CP__) && defined (__DATA_FAR__)
        memset((void *)&_FBSS_START, 0, (size_t)(&_FBSS_END - &_FBSS_START));
    #endif


#else   /* don't use libc routines */

    /*-----------------------------------------------------------------------------------------------------*/
    /* Copy data from ROM to RAM                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------------------------------------*/
    /* Default Model                                                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    for (i = 0; i < &_DATA_END - &_DATA_START; i++)
    {
        *(&_DATA_START + i) = *(&_DATA_IMAGE_START + i);
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Near Model                                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    for (i = 0; i < &_NDATA_END - &_NDATA_START; i++)
    {
        *(&_NDATA_START + i) = *(&_NDATA_IMAGE_START + i);
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Far Model                                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    #if defined (__CR16CP__) && defined (__DATA_FAR__)
        for (i = 0; i < &_FDATA_END - &_FDATA_START; i++)
        {
            *(&_FDATA_START + i) = *(&_FDATA_IMAGE_START + i);
        }
    #endif
    
    /*-----------------------------------------------------------------------------------------------------*/
    /* Fill the uninitialized data section with 0                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    
    /*-----------------------------------------------------------------------------------------------------*/
    /* Default Model                                                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    for (mem_ptr = &_BSS_START; mem_ptr <= &_BSS_END; mem_ptr++)
    {
        *mem_ptr = 0;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Near Model                                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    for (mem_ptr = &_NBSS_START; mem_ptr <= &_NBSS_END; mem_ptr++)
    {
        *mem_ptr = 0;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Far Model                                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    #if defined (__CR16CP__) && defined (__DATA_FAR__)
        for (mem_ptr = &_FBSS_START; mem_ptr <= &_FBSS_END; mem_ptr++)
        {
            *mem_ptr = 0;
        }
    #endif
    
#endif /* NO_LIBC */
}

