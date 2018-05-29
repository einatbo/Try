#/*---------------------------------------------------------------------------------------------------------*/
#/*  Nuvoton Technology Corporation confidential                                                            */
#/*                                                                                                         */
#/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
#/*  All rights reserved                                                                                    */
#/*                                                                                                         */
#/*<<<------------------------------------------------------------------------------------------------------*/
#/* File Contents:                                                                                          */
#/*   NATIONAL.inc                                                                                          */
#/*            This file tools list used with National toolsef                                              */
#/* Project:                                                                                                */
#/*            Multi Touch project                                                                          */
#/*---------------------------------------------------------------------------------------------------------*/

#/*---------------------------------------------------------------------------------------------------------*/
#/* Tools used in makefile                                                                                  */
#/*---------------------------------------------------------------------------------------------------------*/

CC            = $(CRDIR)\crcc
CPP           = $(CRDIR)\lib\cpp
LD            = $(CRDIR)\crld
AS            = $(CRDIR)\crasm

#/*--------------------------------------------------------------------------------------------------------*/
#/* Compiler Flags                                                                                         */
#/*--------------------------------------------------------------------------------------------------------*/
CFLAGS_TOOLSET = -mcr16c -g -Wall -Os -mdata-model=near -ffunction-sections -fdata-sections
COMPILE_NO_LINK_TOOLSET = -c
LFLAGS = -T $(FILE_LNK_OUT) -L$(CRDIR)\lib16c_near_Os --gc-sections -e start -u _flash_header -u _MODEL_NAME -u FW_VERSION  -print-gc-sections

#/*--------------------------------------------------------------------------------------------------------*/
#/* Assembler Flags                                                                                        */
#/*--------------------------------------------------------------------------------------------------------*/
ASFLAGS_TOOLSET = -mcr16c -g
