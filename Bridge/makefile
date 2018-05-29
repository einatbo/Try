#/*---------------------------------------------------------------------------------------------------------*/
#/*  Nuvoton Technology Corporation confidential                                                            */
#/*                                                                                                         */
#/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
#/*  All rights reserved                                                                                    */
#/*                                                                                                         */
#/*<<<------------------------------------------------------------------------------------------------------*/
#/* File Contents:                                                                                          */
#/*   makefile                                                                                              */
#/*            This file is a make file                                                                     */
#/* Project:                                                                                                */
#/*            Multi Touch Bridge project                                                                   */
#/*---------------------------------------------------------------------------------------------------------*/

#/*---------------------------------------------------------------------------------------------------------*/
#/* Version                                                                                                 */
#/*---------------------------------------------------------------------------------------------------------*/
BRIDGE_MAJOR    = 1
BRIDGE_MINOR    = 0
BRIDGE_REVISION = 1

#/*---------------------------------------------------------------------------------------------------------*/
#/* Tools and utilities used in makefile                                                                     */
#/*---------------------------------------------------------------------------------------------------------*/

include $(TOOLSET_TYPE).inc

RM            = del /q /f
RMDIR         = rmdir /s /q
MD            = md
CP            = copy
MV            = move
LINT          = lint-nt
lc = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))

#/*--------------------------------------------------------------------------------------------------------*/
#/* Suffixes                                                                                               */
#/*--------------------------------------------------------------------------------------------------------*/

.SUFFIXES:
.SUFFIXES: .h .c .s .sp .o .a

#/*--------------------------------------------------------------------------------------------------------*/
#/* Directories                                                                                            */
#/*--------------------------------------------------------------------------------------------------------*/

ROOT_DIR             =  ..
LINT_DIR             =  SWC_LINT
SW_DEFS_INCLUDE_DIR  =  ..\SWC_DEFS

HAL_DIR              =  ..\SWC_HAL
BRIDGE_DIR           =  .

TRANSPORT_DIR		 =  ..\MT_TRANSPORT
HI_DIR				 =  ..\MT_HI

COMMON_DIR			 =  ..\MT_COMMON

NMTP_DIR			 =  ..\MT_TRANSPORT\MT_TRANSPORT_NMTP
HID_DIR				 =  ..\MT_TRANSPORT\MT_TRANSPORT_HID
USB_DIR				 =  ..\MT_HI\MT_HI_USB
OUTPUT_DIR           =  ..\Deliverables

#/*--------------------------------------------------------------------------------------------------------*/
#/* Files                                                                                                  */
#/*--------------------------------------------------------------------------------------------------------*/
TARGET             =  MT_Bridge_FW.$(BRIDGE_MAJOR).$(BRIDGE_MINOR).$(BRIDGE_REVISION)

#/*--------------------------------------------------------------------------------------------------------*/
#/* Object files of the project                                                                            */
#/*--------------------------------------------------------------------------------------------------------*/

BRIDGE_OBJS  =   queue.o					   \
				 main.o         			   \
                 start.o         			   \
                 header.o       			   \
                 init_all.o       			   \
                 $(USB_DIR)\usb_touch.o        \
                 $(HID_DIR)\hid.o              \
                 bridge.o         			   \
                 $(HAL_DIR)\hal.o 			   \

FILE_LNK_IN          =   linkObj.def
FILE_LNK_OUT         =   linker.def

#/*--------------------------------------------------------------------------------------------------------*/
#/* C compilation flags                                                                                    */
#/*--------------------------------------------------------------------------------------------------------*/
INCLUDE = -I. -I$(HAL_DIR) -I$(SW_DEFS_INCLUDE_DIR) -I$(COMMON_DIR) -I$(TRANSPORT_DIR) -I$(HI_DIR) -I$(NMTP_DIR) -I$(HID_DIR) -I$(USB_DIR)

ifeq ($(DEBUG),1)
DEBUG_FLAG = -DDEBUG
else
DEBUG_FLAG =
endif

CFLAGS = $(CFLAGS_TOOLSET) \
         $(INCLUDE) $(FLAG) -D__CR16C__ $(DEBUG_FLAG)\
         -DCHIP_NAME=npce79nx -DEXTERNAL_INCLUDE_FILE=chip_patch.h -DMAX_TOUCH_POINTS=10 -DNMTP_BRIDGE

#/*--------------------------------------------------------------------------------------------------------*/
#/* Assembler Flags                                                                                        */
#/*--------------------------------------------------------------------------------------------------------*/
ASFLAGS = $(ASFLAGS_TOOLSET)

#/*--------------------------------------------------------------------------------------------------------*/
#/* Linker flags                                                                                           */
#/*--------------------------------------------------------------------------------------------------------*/
LDEXLIB = -lc

#/*--------------------------------------------------------------------------------------------------------*/
#/* Add floating point support to linker if needed                                                         */
#/*--------------------------------------------------------------------------------------------------------*/
LDEXLIB += -lhfp

#/*--------------------------------------------------------------------------------------------------------*/
#/* Explicit rules                                                                                         */
#/*--------------------------------------------------------------------------------------------------------*/

all: $(OUTPUT_DIR)\$(TARGET).elf

$(OUTPUT_DIR)\$(TARGET).elf: $(BRIDGE_OBJS) $(FILE_LNK_OUT)
	$(LD) -o $(OUTPUT_DIR)\$(TARGET).elf $(LFLAGS) \
	 $(CR_OBJS) $(BRIDGE_OBJS) $(LDEXLIB) -M > $(OUTPUT_DIR)\$(TARGET).map
	bogcheck -elf $(OUTPUT_DIR)\$(TARGET).elf
	crobjcopy --gap-fill 0xff -O binary $(OUTPUT_DIR)\$(TARGET).elf $(OUTPUT_DIR)\$(TARGET).bin
	bogcheck -gmrp $(FLASH_ADDRESS) -i $(OUTPUT_DIR)\$(TARGET).bin -v
	bogcheck -bin $(OUTPUT_DIR)\$(TARGET).bin -v
	crobjdump -D -S -l $(OUTPUT_DIR)\$(TARGET).elf > $(OUTPUT_DIR)\$(TARGET).dump
	crsize -t  $(OUTPUT_DIR)\$(TARGET).elf

#/*--------------------------------------------------------------------------------------------------------*/
#/* Implicit rules                                                                                         */
#/*--------------------------------------------------------------------------------------------------------*/

.c.o:
	-@ if  exist $*.o $(RM) $*.o
	-@ if not exist $(OUTPUT_DIR) mkdir $(OUTPUT_DIR)
	$(CC) $*.c $(CFLAGS) $(COMPILE_NO_LINK_TOOLSET) -o $*.o

.s.o:
	-@ if  exist $*.o $(RM) $*.o
	-@ if not exist $(OUTPUT_DIR) mkdir $(OUTPUT_DIR)
	$(AS) $*.s $(ASFLAGS) -o $*.o

.sp.o:
	-@ if  exist $*.o $(RM) $*.o
	-@ if not exist $(OUTPUT_DIR) mkdir $(OUTPUT_DIR)
	$(CC) $*.sp $(ASFLAGS) $(COMPILE_NO_LINK_TOOLSET) -o $*.o

# pre-process linker.def
$(FILE_LNK_OUT): $(FILE_LNK_IN) makefile
	CPP -P $(FILE_LNK_IN) $(CFLAGS) -o $(FILE_LNK_OUT)

#/*--------------------------------------------------------------------------------------------------------*/
#/* Clean                                                                                                  */
#/*--------------------------------------------------------------------------------------------------------*/
clean:
	$(RM) $(BRIDGE_OBJS)
	$(RM) $(FILE_LNK_OUT)
