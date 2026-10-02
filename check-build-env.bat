@echo off
REM RadPro Build Environment Check Script for Windows
REM Verifies that all required tools are installed and properly configured

setlocal enabledelayedexpansion

set ERRORS=0
set WARNINGS=0

echo ==========================================
echo RadPro Build Environment Check
echo ==========================================
echo.

REM Check CMake
echo [1/8]
echo Checking CMake...
where cmake >nul 2>&1
if %errorlevel% equ 0 (
    for /f "tokens=3" %%v in ('cmake --version ^| findstr /R "^cmake"') do (
        echo   Current version: %%v
        echo   [OK] CMake found
    )
) else (
    echo   [ERROR] CMake not found
    set /a ERRORS+=1
)
echo.

REM Check Ninja
echo [2/8]
echo Checking Ninja...
where ninja >nul 2>&1
if %errorlevel% equ 0 (
    for /f %%v in ('ninja --version') do (
        echo   Current version: %%v
        echo   [OK] Ninja found
    )
) else (
    echo   [WARNING] Ninja not found ^(optional, can use NMake or MSBuild^)
    set /a WARNINGS+=1
)
echo.

REM Check ARM GCC
echo [3/8]
echo Checking ARM GCC...
where arm-none-eabi-gcc >nul 2>&1
if %errorlevel% equ 0 (
    for /f "tokens=*" %%v in ('arm-none-eabi-gcc --version ^| findstr /R /C:"arm-none-eabi-gcc"') do (
        echo   Current version: %%v
        echo   [OK] ARM GCC found
    )
) else (
    echo   [ERROR] arm-none-eabi-gcc not found
    echo   Install from: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm
    set /a ERRORS+=1
)
echo.

REM Check ARM objcopy
echo [4/8]
echo Checking ARM objcopy...
where arm-none-eabi-objcopy >nul 2>&1
if %errorlevel% equ 0 (
    echo   [OK] ARM objcopy found
) else (
    echo   [ERROR] arm-none-eabi-objcopy not found
    set /a ERRORS+=1
)
echo.

REM Check ARM size
echo [5/8]
echo Checking ARM size...
where arm-none-eabi-size >nul 2>&1
if %errorlevel% equ 0 (
    echo   [OK] ARM size found
) else (
    echo   [WARNING] arm-none-eabi-size not found ^(optional^)
    set /a WARNINGS+=1
)
echo.

REM Check SDL2 (for simulator)
echo [6/8]
echo Checking SDL2 ^(for simulator^)...
REM SDL2 check is complex on Windows, just warn
echo   [WARNING] SDL2 check not automated on Windows
echo   If you want to build the simulator, install SDL2 via vcpkg:
echo     vcpkg install sdl2:x64-windows
set /a WARNINGS+=1
echo.

REM Check Git
echo [7/8]
echo Checking Git...
where git >nul 2>&1
if %errorlevel% equ 0 (
    for /f "tokens=3" %%v in ('git --version') do (
        echo   Current version: %%v
        echo   [OK] Git found
    )
) else (
    echo   [WARNING] Git not found ^(optional^)
    set /a WARNINGS+=1
)
echo.

REM Check board definitions
echo [8/8]
echo Checking board definitions...
if exist "platform.io\boards" (
    dir /b "platform.io\boards\*.json" 2>nul | find /c ".json" > temp_count.txt
    set /p BOARD_COUNT=<temp_count.txt
    del temp_count.txt
    echo   Found !BOARD_COUNT! board definition^(s^)
    echo   [OK] Board definitions found
) else (
    echo   [ERROR] Board directory not found: platform.io\boards
    set /a ERRORS+=1
)
echo.

REM Summary
echo ==========================================
echo Summary
echo ==========================================
if %ERRORS% equ 0 (
    echo [OK] All required tools are installed!
    if %WARNINGS% gtr 0 (
        echo [WARNING] %WARNINGS% optional tool^(s^) missing
    )
    echo.
    echo You can now build RadPro firmware:
    echo   cmake-build.bat
    echo   OR
    echo   cmake --preset fnirsi-gc01_ch32f103r8
    echo   cmake --build --preset fnirsi-gc01_ch32f103r8
    exit /b 0
) else (
    echo [ERROR] %ERRORS% required tool^(s^) missing!
    if %WARNINGS% gtr 0 (
        echo [WARNING] %WARNINGS% optional tool^(s^) missing
    )
    echo.
    echo Please install the missing tools before building.
    echo See BUILD.md for installation instructions.
    exit /b 1
)

endlocal
