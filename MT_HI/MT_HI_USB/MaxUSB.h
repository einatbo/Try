/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2011 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/* File Contents:                                                                                          */
/*   MaxUSB.h                                                                                              */
/*            This file contains interface of a driver for Maxim Peripheral USB devive                     */
/* Project:                                                                                                */
/*           Multi Touch                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef  MAXUSB_H
#define  MAXUSB_H

#include "defs.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Endpoints                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define MAXUSB_EP1                          1
#define MAXUSB_EP2                          2
#define MAXUSB_EP3                          3

/*---------------------------------------------------------------------------------------------------------*/
/* Module definitions                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define MAXUSB_SPI_ACKSTAT                  0,  1
#define MAXUSB_SPI_DIR                      1,  1
#define MAXUSB_SPI_ADDRESS                  3,  5


#define MAXUSB_SPI_READ_COMMAND             0
#define MAXUSB_SPI_WRITE_COMMAND            1


/*---------------------------------------------------------------------------------------------------------*/
/* Macros                                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define MAXUSB_WRITE_COMMAND(address)                                 \
  ( BUILD_FIELD_VAL(MAXUSB_SPI_DIR,       MAXUSB_SPI_WRITE_COMMAND) | \
    BUILD_FIELD_VAL(MAXUSB_SPI_ADDRESS,   address                 ))

#define MAXUSB_WRITE_ACKSTAT(address)                                 \
  ( BUILD_FIELD_VAL(MAXUSB_SPI_DIR,       MAXUSB_SPI_WRITE_COMMAND) | \
    BUILD_FIELD_VAL(MAXUSB_SPI_ACKSTAT,   1)                        | \
    BUILD_FIELD_VAL(MAXUSB_SPI_ADDRESS,   address                 ))




#define MAXUSB_READ_COMMAND(address)                                  \
  ( BUILD_FIELD_VAL(MAXUSB_SPI_DIR,   MAXUSB_SPI_READ_COMMAND )  |    \
    BUILD_FIELD_VAL(MAXUSB_SPI_ADDRESS,   address                 ))


/*---------------------------------------------------------------------------------------------------------*/
/* Register access redirections                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define MAXUSB_ACCESS                        MAXUSB_

#define MAXUSB_R8                            MAXUSB_ReadReg
#define MAXUSB_W8                            MAXUSB_WriteReg


/*---------------------------------------------------------------------------------------------------------*/
/* MaxUSB registers                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define MAXUSB_REVISION_VAL                  0x04

#define MAXUSB_EP0FIFO                       (0),  MAXUSB_ACCESS, 8
#define MAXUSB_EP1OUTFIFO                    (1),  MAXUSB_ACCESS, 8
#define MAXUSB_EP2INFIFO                     (2),  MAXUSB_ACCESS, 8
#define MAXUSB_EP3INFIFO                     (3),  MAXUSB_ACCESS, 8
#define MAXUSB_SUDFIFO                       (4),  MAXUSB_ACCESS, 8
#define MAXUSB_EP0BC                         (5),  MAXUSB_ACCESS, 8
#define MAXUSB_EP1OUTBC                      (6),  MAXUSB_ACCESS, 8
#define MAXUSB_EP2INBC                       (7),  MAXUSB_ACCESS, 8
#define MAXUSB_EP3INBC                       (8),  MAXUSB_ACCESS, 8
#define MAXUSB_EPSTALLS                      (9),  MAXUSB_ACCESS, 8
#define MAXUSB_CLRTOGS                       (10), MAXUSB_ACCESS, 8
#define MAXUSB_EPIRQ                         (11), MAXUSB_ACCESS, 8
#define MAXUSB_EPIEN                         (12), MAXUSB_ACCESS, 8
#define MAXUSB_USBIRQ                        (13), MAXUSB_ACCESS, 8
#define MAXUSB_USBIEN                        (14), MAXUSB_ACCESS, 8
#define MAXUSB_USBCTL                        (15), MAXUSB_ACCESS, 8
#define MAXUSB_CPUCTL                        (16), MAXUSB_ACCESS, 8
#define MAXUSB_PINCTL                        (17), MAXUSB_ACCESS, 8
#define MAXUSB_REVISION                      (18), MAXUSB_ACCESS, 8
#define MAXUSB_FNADDR                        (19), MAXUSB_ACCESS, 8
#define MAXUSB_IOPINS                        (20), MAXUSB_ACCESS, 8

/*---------------------------------------------------------------------------------------------------------*/
/* EPSTALLS Fields                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define MAXUSB_EPSTALLS_STLEP0IN              0, 1
#define MAXUSB_EPSTALLS_STLEP0OUT             1, 1
#define MAXUSB_EPSTALLS_STLEP1OUT             2, 1
#define MAXUSB_EPSTALLS_STLEP2IN              3, 1
#define MAXUSB_EPSTALLS_STLEP3IN              4, 1
#define MAXUSB_EPSTALLS_STLSTAT               5, 1
#define MAXUSB_EPSTALLS_ACKSTAT               6, 1

/*---------------------------------------------------------------------------------------------------------*/
/* CLRTOGS Fields                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define MAXUSB_CLRTOGS_CTGEP1OUT              2, 1
#define MAXUSB_CLRTOGS_CTGEP2IN               3, 1
#define MAXUSB_CLRTOGS_CTGEP3IN               4, 1
#define MAXUSB_CLRTOGS_EP1DISAB               5, 1
#define MAXUSB_CLRTOGS_EP2DISAB               6, 1
#define MAXUSB_CLRTOGS_EP3DISAB               7, 1

/*---------------------------------------------------------------------------------------------------------*/
/* EPIRQ Fields                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define MAXUSB_EPIRQ_IN0BAVIRQ                0, 1
#define MAXUSB_EPIRQ_OUT0DAVIRQ               1, 1
#define MAXUSB_EPIRQ_OUT1DAVIRQ               2, 1
#define MAXUSB_EPIRQ_IN2BAVIRQ                3, 1
#define MAXUSB_EPIRQ_IN3BAVIRQ                4, 1
#define MAXUSB_EPIRQ_SUDAVIRQ                 5, 1


/*---------------------------------------------------------------------------------------------------------*/
/* EPIEN Fields                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define MAXUSB_EPIEN_IN0BAVIE                 0, 1
#define MAXUSB_EPIEN_OUT0DAVIE                1, 1
#define MAXUSB_EPIEN_OUT1DAVIE                2, 1
#define MAXUSB_EPIEN_IN2BAVIE                 3, 1
#define MAXUSB_EPIEN_IN3BAVIE                 4, 1
#define MAXUSB_EPIEN_SUDAVIE                  5, 1


/*---------------------------------------------------------------------------------------------------------*/
/* USBIRQ Fields                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define MAXUSB_USBIRQ_OSCOKIRQ                0, 1
#define MAXUSB_USBIRQ_RWUDNIRQ                1, 1
#define MAXUSB_USBIRQ_BUSACTIRQ               2, 1
#define MAXUSB_USBIRQ_URESIRQ                 3, 1
#define MAXUSB_USBIRQ_SUSPIRQ                 4, 1
#define MAXUSB_USBIRQ_NOVBUSIRQ               5, 1
#define MAXUSB_USBIRQ_VBUSIRQ                 6, 1
#define MAXUSB_USBIRQ_URESDNIRQ               7, 1

/*---------------------------------------------------------------------------------------------------------*/
/* USBIEN Fields                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define MAXUSB_USBIEN_OSCOKIE                 0, 1
#define MAXUSB_USBIEN_RWUDNIE                 1, 1
#define MAXUSB_USBIEN_BUSACTIE                2, 1
#define MAXUSB_USBIEN_URESIE                  3, 1
#define MAXUSB_USBIEN_SUSPIE                  4, 1
#define MAXUSB_USBIEN_NOVBUSIE                5, 1
#define MAXUSB_USBIEN_VBUSIE                  6, 1
#define MAXUSB_USBIEN_URESDNIE                7, 1


/*---------------------------------------------------------------------------------------------------------*/
/* USBCTL Fields                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define MAXUSB_USBCTL_SIGRWU                  2, 1
#define MAXUSB_USBCTL_CONNECT                 3, 1
#define MAXUSB_USBCTL_PWRDOWN                 4, 1
#define MAXUSB_USBCTL_CHIPRES                 5, 1
#define MAXUSB_USBCTL_VBGATE                  6, 1
#define MAXUSB_USBCTL_HOSCSTEN                7, 1


/*---------------------------------------------------------------------------------------------------------*/
/* CPUCTL Fields                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define MAXUSB_CPUCTL_IE                      0, 1

/*---------------------------------------------------------------------------------------------------------*/
/* PINCTL Fields                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define MAXUSB_PINCTL_GPXA                    0, 1
#define MAXUSB_PINCTL_GPXB                    1, 1
#define MAXUSB_PINCTL_POSINT                  2, 1
#define MAXUSB_PINCTL_INTLEVEL                3, 1
#define MAXUSB_PINCTL_FDUPSPI                 4, 1
#define MAXUSB_PINCTL_EP0INAK                 5, 1
#define MAXUSB_PINCTL_EP2INAK                 6, 1
#define MAXUSB_PINCTL_EP3INAK                 7, 1

/*---------------------------------------------------------------------------------------------------------*/
/* REVISION Fields                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define MAXUSB_REVISION_REV0                  0, 1
#define MAXUSB_REVISION_REV1                  1, 1
#define MAXUSB_REVISION_REV2                  2, 1
#define MAXUSB_REVISION_REV3                  3, 1

/*---------------------------------------------------------------------------------------------------------*/
/* IOPINS Fields                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define MAXUSB_IOPINS_GPOUT0                  0, 1
#define MAXUSB_IOPINS_GPOUT1                  1, 1
#define MAXUSB_IOPINS_GPOUT2                  2, 1
#define MAXUSB_IOPINS_GPOUT3                  3, 1
#define MAXUSB_IOPINS_GPIN0                   4, 1
#define MAXUSB_IOPINS_GPIN1                   5, 1
#define MAXUSB_IOPINS_GPIN2                   6, 1
#define MAXUSB_IOPINS_GPIN3                   7, 1


#endif
