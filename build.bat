@echo off
@rem/*--------------------------------------------------------------------------------------------------------*/
@rem/*   Nuvoton Technology Corporation confidential                                                          */
@rem/*                                                                                                        */
@rem/*  Copyright (c) 2011 by Nuvoton Technology Corporation                                                  */
@rem/*  All rights reserved                                                                                   */
@rem/*                                                                                                        */
@rem/*--------------------------------------------------------------------------------------------------------*/
@rem/* File Contents:                                                                                         */
@rem/*   build.bat                                                                                            */
@rem/*            Build script for Touch Bridge firmware                                                      */
@rem/*  Project:  Multi-Touch                                                                                 */
@rem/*--------------------------------------------------------------------------------------------------------*/

title Build Bridge Firmware

if "%MTENV%" == "1"  goto start

SET MTENV=1
SET path=\\tanap1\proj_sw\sw_dev_tools\GNU\Gmake\3.80;C:\lint;\\tanap1\sw_prod\aio\Rider\bogCheck\2.0\deliverables;%path%
CALL %CRDIR%\CR16C.bat

:start
SETLOCAL ENABLEDELAYEDEXPANSION

set FLAG=NMTP_BULK
set loadOption=
set cleanOption=
set debugOption=0

if "%1" == "load"      set loadOption=load
if "%2" == "load"      set loadOption=load
if "%3" == "load"      set loadOption=load
if "%4" == "load"      set loadOption=load


if "%1" == "loadonly"  set loadOption=loadonly
if "%2" == "loadonly"  set loadOption=loadonly
if "%3" == "loadonly"  set loadOption=loadonly
if "%4" == "loadonly"  set loadOption=loadonly

if "%1" == "clean"     set cleanOption=clean
if "%2" == "clean"     set cleanOption=clean
if "%3" == "clean"     set cleanOption=clean
if "%4" == "clean"     set cleanOption=clean

if "%1" == "hid"       set FLAG=NMTP_BRIDGE_HID
if "%2" == "hid"       set FLAG=NMTP_BRIDGE_HID
if "%3" == "hid"       set FLAG=NMTP_BRIDGE_HID
if "%4" == "hid"       set FLAG=NMTP_BRIDGE_HID

if "%1" == "debug"     set debugOption=1
if "%2" == "debug"     set debugOption=1
if "%3" == "debug"     set debugOption=1
if "%4" == "debug"     set debugOption=1

SET flash_address=0x1F5000

if "%loadOption%" == "loadonly" (
   SET  resName=PASSED
   SET  loadOption=load
   goto load
)


@rem/*--------------------------------------------------------------------------------------------------------*/
@rem/* Build                                                                                                  */
@rem/*--------------------------------------------------------------------------------------------------------*/
     if "%cleanOption%" == "clean" (
         @rem/*-----------------------------------------------------------------------------------------------*/
         @rem/* Run the PC Lint                                                                               */
         @rem/*-----------------------------------------------------------------------------------------------*/
         echo -dDEBUG=%debugOption% -d%FLAG% > dyn-cfg.lnt
         cmd  /c SWC_LINT\lintAll.bat  Bridge dyn-cfg.lnt
         del dyn-cfg.lnt
         @rem/*-----------------------------------------------------------------------------------------------*/
         @rem/* Run the build                                                                                 */
         @rem/*-----------------------------------------------------------------------------------------------*/
         cmd /c gmake  --directory Bridge --no-print-directory  clean  TOOLSET_TYPE=NATIONAL
     )
     cmd /c gmake   --directory Bridge --no-print-directory  TOOLSET_TYPE=NATIONAL FLASH_ADDRESS=%flash_address%  FLAG=-D%FLAG% DEBUG=%debugOption%
)


@rem/*--------------------------------------------------------------------------------------------------------*/
@rem/* Print summary of compilation result                                                                    */
@rem/*--------------------------------------------------------------------------------------------------------*/

if %ERRORLEVEL% NEQ 0 (
    SET resName=FAILED
 ) else (
    SET resName=PASSED
 )

@echo =======================================================================
@echo        Build of Multi Touch bridge firmware %resName%
@echo =======================================================================
:load
rem @echo 

set FLASH_LOADER_DIR=\\tanap1\proj_sw\sw_dev_tools\Nuvoton\FlashLoader\v4.21.01\Rider
SET path=%path%;%FLASH_LOADER_DIR%

if "%resName%" == "PASSED" (
        if "%loadOption%" == "load" (
            @rem/*---------------------------------------------------------------------------------------------*/
            @rem/* If loading is requested, load the firmware to the device flash using FlashLoader tool       */
            @rem/*---------------------------------------------------------------------------------------------*/
            @echo =======================================================================
            echo        Loading Multi Touch bridge firmware to the device flash
            @echo =======================================================================
            RiderLoader.exe Deliverables\MT_Bridge_FW.1.0.1.bin -partial_erase -offset %flash_address% -i  %FLASH_LOADER_DIR%\spi_cfg\SST_25VF016B.cfg -g
        )
 )

goto end

:cancel
       @echo =======================================================================
       @echo                  Build was cancelled
       @echo =======================================================================

:end

ENDLOCAL


@echo on
