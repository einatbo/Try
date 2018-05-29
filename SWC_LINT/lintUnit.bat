@rem Parameters:
@rem %1:    <path to proj-cfg.lnt>
@rem %2:    <file-name>
@rem %3:    <env lint-file (optional)>
@rem %4:    <external lint option (optional)>
@rem %5:    <external lint option (optional)>

@rem Example 1 (SI):    lintUnit.bat . adc.c env-si.lnt -dDEBUG
@rem Example 2 (CMD):   lintUnit.bat .\lnt adc.c -e752

@set params=
@rem Set project path
@set project_path=%1
@rem Skip first parameter
@SHIFT
@rem Set file name
@set filename=%1
@SHIFT

@rem Agregate all other parameters
:loop
@IF [%1]==[] GOTO continue
@set params=%params% %1
@SHIFT
@GOTO loop
:continue

@"lint-nt.exe" +v -i".\SWC_LINT" -i"%project_path%" -os(.\SWC_LINT\_LINT.TXT) --u proj-cfg %params% %filename%
@if errorlevel 1 goto error
@goto success

:error
@type .\SWC_LINT\_LINT.TXT
@echo ---
@echo --- output placed in SWC_LINT\_LINT.TXT
@goto exit

:success
@echo ---
@echo --- PC-lint completed with no messages

:exit
