/*------------------------------------------------------------------------------
 * Copyright (c) 2005-2008 by Nuvoton Technology Corporation
 * All rights reserved.
 *<<<---------------------------------------------------------------------------
 * File Contents:
 *     header.c - uRider header definition and initialization
 *
 * Project:
 *     Driver Set for WPCE78nx Notebook Embedded Controller Peripherals
 *------------------------------------------------------------------------->>>*/

#include "hal.h"

#pragma section(".header",flash_header)

/*lint -esym( 754, * ) */
typedef struct
{
/*-----------------------------------------------------------------------------
* Flash Signature
*----------------------------------------------------------------------------*/
/* 00 */ UINT16  signature; // Flash Signature
/* 02 */ UINT16  reserved1;

/*-----------------------------------------------------------------------------
* MSWC Core Registers Configuration
*----------------------------------------------------------------------------*/
/* 04 */ UINT8   HCBAL_DAT; // Host Configuration Base Address Low
/* 05 */ UINT8   HCBAH_DAT; // Host Configuration Base Address High
/* 06 */ UINT16  reserved2;

/*-----------------------------------------------------------------------------
* High Frequency Clock Generator (HFCG) Module Configuration
*----------------------------------------------------------------------------*/
/* 08 */ UINT8   HFCGML_DAT; // HFCG, M value, Low
/* 09 */ UINT8   HFCGMH_DAT; // HFCG, M value, High
/* 0A */ UINT8   HFCGN_DAT;  // HFCG, N value
/* 0B */ UINT8   HFCGP_DAT;  // bits 7-4 - Flash Clock Divider
                             // bits 3-0 - Core Clock Divider
/* 0C */ UINT16  reserved3;

/*-----------------------------------------------------------------------------
* Flash Interface Unit (FIU) Module Configuration
*----------------------------------------------------------------------------*/
/* 0E */ UINT8   FL_SIZE_DAT; // Flash size
/* 0F */ UINT8   BURST_DAT;   // bits 5-4 - Flash Write Burst
                              // bits 1-0 - Flash Read Burst
/* 10 */ UINT8   SPI_FL_DAT;  // bit    7 - Number of Flash Devices
                              // bit    6 - Flash Fast Read
                              // bits 5-0 - Flash Device Size
/* 11 */ UINT8   MISC_CTL;    // bit 7    - Bypass on bad Firmware Checksum
/* 12 */ UINT16  Reserved4;

/*-----------------------------------------------------------------------------
* General Settings
*----------------------------------------------------------------------------*/
/* 14 */ UINT32  FIRMW_START;  // Firmware Start Address
/* 18 */ UINT32  FIRMW_SIZE;   // Firmware Size (in words)
/* 1C */ UINT16  Reserved5[18];
/* 40 */ UINT16  HEADER_CKSM;  // Checksum of Header (bytes 0h to 40h)
/* 42 */ UINT16  Reserved6;
/* 44 */ UINT32  FIRMW_CKSM;   // Checksum of Firmware
/* 48 */ UINT16  Reserved7[2]; // Dummy bytes 
} Flash_header_t;

/*---------------------------------------------------------------------------------------------------------*/
/* uRider Flash header                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
/*lint -esym(752, flash_header)                                                                            */
const Flash_header_t flash_header =
{
/*-----------------------------------------------------------------------------
* Flash Signature
*----------------------------------------------------------------------------*/
/* 00 */ 0x1678, // Signature 0x8761 - A1, 0x1678 - A2
/* 02 */ 0x0000, // reserved1

/*-----------------------------------------------------------------------------
* MSWC Core Registers Configuration
*----------------------------------------------------------------------------*/
/* 04 */ 0x4e,   // HCBAL_DAT
/* 05 */ 0x16,   // HCBAH_DAT
/* 06 */ 0x0000, // reserved2

/*-----------------------------------------------------------------------------
* High Frequency Clock Generator (HFCG) Module Configuraion
* Please don't change the clock registers values here. The values are defined
* in hfcg.h and they depend on the values of HFCG_FMCLK, HFCG_FLASH_CLK and
* HFCG_CORE_CLK which may be customized.
*----------------------------------------------------------------------------*/
/* 08 */ HFCG_FMCLK_HFCGML,   // HFCGML_DAT
/* 09 */ HFCG_FMCLK_HFCGMH,   // HFCGMH_DAT
/* 0A */ HFCG_FMCLK_HFCGN,    // HFCGN_DAT
/* 0B */ HFCG_HFCGP_VAL,      // HFCGP_DAT   bits 7-4 - FPRED
                              //             bits 3-0 - CPRED
/* 0C */ 0x0000,              // reserved3

/*-----------------------------------------------------------------------------
* Flash Interface Unit (FIU) Module Configuration
*----------------------------------------------------------------------------*/
/* 0E */ 0x14,   // FL_SIZE_P1
/* 0F */ 0x03,   // bits 5-4 - W_BURST
                 // bits 1-0 - R_BURST
/* 10 */ 0x50,   // bit    7 - FL_DEV
                 // bit    6 - FREAD
                 // bits 0-5 - DEV_SIZE
/* 11 */ 0x7f,   // MISC_CTL
                 // bit 7    - BAD_FCKSM_BYP
/* 12 */ 0x0000, // Reserved4

/*-----------------------------------------------------------------------------
* General Settings
*----------------------------------------------------------------------------*/
/* 14 */ 0x00020100,        // FIRMW_START
/* 18 */ 0x00000000,        // FIRMW_SIZE
/* 1C */ {0x0000},          // Reserved5[18]
/* 40 */ 0x0000,            // HEADER_CKSM
/* 42 */ 0x0000,            // Reserved6;
/* 44 */ 0x00000000,        // FIRMW_CKSM
/* 48 */ {0xFFFF, 0xFFFF},  // Calibration values space
};

/*-----------------------------------------------------------------------------
* OEM/ODM  Model name and FW version
*----------------------------------------------------------------------------*/ 
/*lint -esym(752, MODEL_NAME, FW_VERSION)                                    */
const unsigned char MODEL_NAME[] __attribute__ ((section (".header_2"))) = "$MLXXXX$";       // MODEL_NAME XXXX; 
const unsigned char FW_VERSION[] __attribute__ ((section (".header_2"))) = "$VR01.00.00$";   // FW_VERSION  1.0.0




