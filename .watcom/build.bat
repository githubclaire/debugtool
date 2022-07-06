@echo off

pushd ..\
set CODEPATH=%CD%
popd

rem pushd ..\..\
rem set WATPATH=%CD%
rem popd

rem set WATCOM=%WATPATH%\WATCOM
rem set path=%WATPATH%\WATCOM\binnt

rem if NOT EXIST %WATCOM% goto end

if /I "%2" EQU "x86" (
    set ARCH=x86
) else if /I "%2" EQU "arm" (
    SET ARCH=arm
)

if /I "%3" EQU "win" (
    set OS_TYPE=windows
) else if /I "%3" EQU "dos" (
    set OS_TYPE=dos
) else if /I "%3" EQU "linux" (
    set OS_TYPE=linux
) else (
    set OS_TYPE=dos
)

if /I "%1" EQU "all" (
    nmake -nologo clean
    nmake -nologo all
) else if /I "%1" EQU "clean" (
    nmake -nologo clean
)

:end
echo build complete

