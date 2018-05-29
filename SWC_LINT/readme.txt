-----------------------------------------------------------------------
Nuvoton Technology Corporation - Confidential
SWC_LINT PC-Lint Package README
Copyright (c) 2010 Nuvoton Technology Corporation. All Rights Reserved.
-----------------------------------------------------------------------

SWC_LINT PC-Lint Package for NTIL Software Projects
README
Version: 1.0.2
Date:    02 June 2011


PACKAGE CONTENTS
----------------
The SWC_LINT package contains the following files:

COMMON:
- co-cr16c.lnt      - Compiler Options for CR16C.
- lintUnit.bat      - Batch file for running PC-Lint on a single module (unit checkout).
- lintAll.bat       - Batch file for running PC-Lint on all modules.
- Sample\*          - Project specific PC-Lint configuration files sample.
- readme.txt        - This ReadMe.

PROJECT-SPECIFIC:
Each project should create the following files:
- proj-cfg.lnt      - Project-Specific PC-Lint high-level component Configuration.
- proj-opt.lnt      - Project-Specific PC-Lint Options.
- proj-src.lnt      - Project-Specific Source-list to lint (all modules lint).
For example, see the sample directory.

EXTERNAL DEPENDENCIES:
Some SW components contain their own PC-Lint Configuration file:
- swc-defs.lnt      - SWC_DEFS PC-Lint Configuration.
- swc-hal.lnt       - SWC_HAL PC-Lint Configuration.


DESCRIPTION
-----------
This package contains the SWC_LINT PC-Lint Package for NTIL Software Projects.

PC-lint finds quirks, idiosyncrasies, glitches and bugs in C and C++ programs. 
The purpose of such analysis is to determine potential problems in such programs 
before integration or porting, or to reveal unusual constructs that may be a source 
of subtle and, yet, undetected errors.


REQUIREMENTS
------------
Software:
- PC-lint for C/C++ Version 8.00


INSTALLATION PROCEDURE
----------------------
- Run PC-Lint installation program from the root directory of the installation CR-ROM.
- Add PC-Lint executable (lint-nt.exe) location to PATH environment variable.
- Run PC-Lint-v8.00x-update.bat to update from PC-lint Version 8.00q to Version 8.00x.


USAGE
-----
- Command line (single module):
  % cd <workspace>
  % .\SWC_LINT\lintUnit.bat <path to proj-cfg.lnt> <file-name> [env lint-file] [lint options...]

  Example:
  % cd P:\Projects\378H\work\Tomer\workspaces\NPCD378H_FW
  % .\SWC_LINT\lintUnit.bat . adc.c -dDEBUG


- Command line (all modules):
  % cd <workspace>
  % .\SWC_LINT\lintAll.bat <path to proj-cfg.lnt> [env lint-file] [lint options...]

  Example:
  % cd P:\Projects\378H\work\Tomer\workspaces\NPCD378H_FW
  % .\SWC_LINT\lintAll.bat .\lnt env-xml -e752


- Source Insight Custom Command for PC-Lint (single module):
  Run:	.\SWC_LINT\lintUnit.bat <path to proj-cfg.lnt> %f env-si.lnt [lint options...]
  Dir:	<workspace>

  Example:
  Run:    .\SWC_LINT\lintUnit.bat . %f env-si.lnt -dDEBUG
  Dir:    P:\Projects\378H\work\Tomer\workspaces\NPCD378H_FW


- Source Insight Custom Command for PC-Lint (all modules):
  Run:    .\SWC_LINT\lintAll.bat <path to proj-cfg.lnt> env-si.lnt [lint options...]
  Dir:    <workspace>

  Example:
  Run:    .\SWC_LINT\lintAll.bat .\lnt env-si.lnt -e752
  Dir:    P:\Projects\378H\work\Tomer\workspaces\NPCD378H_FW


- output is placed in .\SWC_LINT\_LINT.TXT

