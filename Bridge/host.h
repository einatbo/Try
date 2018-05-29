/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/* File Contents:                                                                                          */
/*   host_board.h                                                                                          */
/*            This file contains host board (CapKey EC Host module) pins assignments                       */
/* Project:                                                                                                */
/*            Multi-Touch                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef  HOST_H
#define  HOST_H

/*---------------------------------------------------------------------------------------------------------*/
/* Host connector pin assignments                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define CS_SDO                          0x22
#define CS_SCLK                         0x17
#define CS                              0x82
#define CK_INT_GPIO                     6
#define CK_INT                          ICU_INT_7
#define CK_INT_SOURCE                   MIWU_GPI06
#define CK_INTERRUPT_PROVIDER           INTERRUPT_PROVIDER_MIWU
#define CK_INTERRUPT_POLARITY           MIWU_FALLING_EDGE
#define I2C_IF                          SMB_MODULE_1

/*---------------------------------------------------------------------------------------------------------*/
/* Alternative host connector assignment (used for boards with no shift register)                          */
/*---------------------------------------------------------------------------------------------------------*/
#define SPI_CS_CK0                      0x22
#define SPI_CS_CK1                      0x17

/*---------------------------------------------------------------------------------------------------------*/
/* Test GPIOs                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define SCAN_TEST_GPIO                  0x90        /* pin 97 */
#define HID_TEST_GPIO                   0x04        /* pin 96 */

/*---------------------------------------------------------------------------------------------------------*/
/* USB device                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define INTERRUPT_PROVIDER_USB          INTERRUPT_PROVIDER_ICU
#define USB_INT                         19
#define USB_INT_GPIO                    0x05
#define USB_SPI_CS                      0x84





#endif
