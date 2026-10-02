@echo off
REM RadPro CMake Build Script for Windows
REM This script simplifies building RadPro firmware with CMake

setlocal enabledelayedexpansion

REM Default values
set BOARD=fnirsi-gc01_ch32f103r8
set LANGUAGE=en
set BUILD_TYPE=Release
set CLEAN=0
set SIMULATOR=0
set SHOW_HELP=0

REM Parse command line arguments
:parse_args
if "%~1"=="" goto :check_args
if /i "%~1"=="-b" (
    set BOARD=%~2
    shift
    shift
    goto :parse_args
)
if /i "%~1"=="--board" (
    set BOARD=%~2
    shift
    shift
    goto :parse_args
)
if /i "%~1"=="-l" (
    set LANGUAGE=%~2
    shift
    shift
    goto :parse_args
)
if /i "%~1"=="--language" (
    set LANGUAGE=%~2
    shift
    shift
    goto :parse_args
)
if /i "%~1"=="-s" (
    set SIMULATOR=1
    shift
    goto :parse_args
)
if /i "%~1"=="--simulator" (
    set SIMULATOR=1
    shift
    goto :parse_args
)
if /i "%~1"=="-c" (
    set CLEAN=1
    shift
    goto :parse_args
)
if /i "%~1"=="--clean" (
    set CLEAN=1
    shift
    goto :parse_args
)
if /i "%~1"=="-h" (
    set SHOW_HELP=1
    shift
    goto :parse_args
)
if /i "%~1"=="--help" (
    set SHOW_HELP=1
    shift
    goto :parse_args
)
echo Unknown option: %~1
set SHOW_HELP=1
goto :check_args

:check_args
if %SHOW_HELP%==1 (
    echo Usage: %~nx0 [OPTIONS]
    echo.
    echo Options:
    echo   -b, --board BOARD       Target board ^(default: fnirsi-gc01_ch32f103r8^)
    echo   -l, --language LANG     UI language ^(default: en^)
    echo   -s, --simulator         Build SDL simulator instead of firmware
    echo   -c, --clean             Clean build directory before building
    echo   -h, --help              Show this help message
    echo.
    echo Supported boards:
    echo   fnirsi-gc01_ch32f103r8, fnirsi-gc01_apm32f103rb, fnirsi-gc03
    echo   fs2011-stm32f051c8, fs2011-gd32f150c8, fs2011-gd32f103c8
    echo   bosean-fs600, bosean-fs1000, bosean-fs5000, bosean-fs5000_landscape
    echo   gq-gmc800, gq-gmc800_landscape
    echo.
    echo Examples:
    echo   %~nx0                                       # Build default board
    echo   %~nx0 --board fnirsi-gc03 --language de    # Build with German UI
    echo   %~nx0 --simulator                           # Build SDL simulator
    echo   %~nx0 --clean --board fnirsi-gc01_ch32f103r8  # Clean build
    exit /b 0
)

REM Set build directory
if %SIMULATOR%==1 (
    set BUILD_DIR=build\simulator
    set BUILD_NAME=simulator
) else (
    set BUILD_DIR=build\%BOARD%
    set BUILD_NAME=%BOARD%
)

echo ========================================
echo RadPro CMake Build
echo ========================================
if %SIMULATOR%==1 (
    echo Target: SDL Simulator
) else (
    echo Board: %BOARD%
)
echo Language: %LANGUAGE%
echo Build Dir: %BUILD_DIR%
echo ========================================
echo.

REM Clean if requested
if %CLEAN%==1 (
    echo Cleaning build directory...
    if exist "%BUILD_DIR%" rd /s /q "%BUILD_DIR%"
)

REM Create build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

REM Configure
echo Configuring...
if %SIMULATOR%==1 (
    cmake -G Ninja -B "%BUILD_DIR%" ^
        -DRADPRO_SIMULATOR=ON ^
        -DRADPRO_LANGUAGE=%LANGUAGE% ^
        -DCMAKE_BUILD_TYPE=Debug
) else (
    cmake -G Ninja -B "%BUILD_DIR%" ^
        -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake ^
        -DRADPRO_BOARD=%BOARD% ^
        -DRADPRO_LANGUAGE=%LANGUAGE% ^
        -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
)

if errorlevel 1 (
    echo.
    echo Configuration failed!
    exit /b 1
)

REM Build
echo.
echo Building...
cmake --build "%BUILD_DIR%" --verbose

if errorlevel 1 (
    echo.
    echo Build failed!
    exit /b 1
)

REM Success message
echo.
echo ========================================
echo Build Successful!
echo ========================================
if %SIMULATOR%==1 (
    echo Executable: %BUILD_DIR%\radpro-simulator.exe
) else (
    echo Firmware files:
    echo   %BUILD_DIR%\%BUILD_NAME%.elf
    echo   %BUILD_DIR%\%BUILD_NAME%.bin
    echo   %BUILD_DIR%\%BUILD_NAME%.hex
    echo   %BUILD_DIR%\radpro.map
)
echo ========================================

endlocal
