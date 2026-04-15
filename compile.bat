@echo off
echo ==============================================
echo   GMpro87dev Firmware Compiler
echo ==============================================
echo.

cd /d "%~dp0"

if not exist "platformio.ini" (
    echo ERROR: platformio.ini not found!
    echo Make sure you run this script from the project folder.
    pause
    exit /b 1
)

echo Checking PlatformIO installation...
pio --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: PlatformIO is not installed!
    echo Install it first: pip install platformio
    pause
    exit /b 1
)

echo.
echo Compiling firmware...
echo.

pio run

if errorlevel 1 (
    echo.
    echo ==============================================
    echo   COMPILATION FAILED!
    echo ==============================================
    pause
    exit /b 1
)

echo.
echo ==============================================
echo   COMPILATION SUCCESS!
echo ==============================================
echo.
echo Finding firmware file...

for /r ".pio\build" %%f in (*.bin) do (
    echo Found: %%f
    copy "%%f" "GMpro87dev.bin"
    echo.
    echo Copied to: %CD%\GMpro87dev.bin
)

if not exist "GMpro87dev.bin" (
    echo.
    echo NOTE: .bin file location:
    dir /s /b ".pio\build\*.bin" 2>nul
)

echo.
echo Ready to flash with esptool:
echo   esptool.py write_flash 0x00000 .pio\build\d1_mini\firmware.bin
echo.
pause
