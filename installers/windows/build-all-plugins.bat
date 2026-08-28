@echo off
REM AudioForge Windows Plugin Build Script
REM Builds all 10 plugins for Windows (VST3)
REM Version: 1.1.1

echo ======================================
echo AudioForge Windows Build Script
echo Version: 1.1.1
echo ======================================
echo.

REM Check if we're in the AudioForge directory
if not exist "plugins\" (
    echo ERROR: Run this script from the AudioForge root directory
    exit /b 1
)

REM Plugin list
set PLUGINS=SimpleGain PanUtil BasicSynth CleanDelay SimpleEQ SimpleComp WaveShaper Saturation ChorusFlanger Reverb

REM Build counter
set SUCCESS=0
set FAILED=0

echo Starting builds...
echo.

REM Build each plugin
for %%P in (%PLUGINS%) do (
    echo ======================================
    echo Building %%P...
    echo ======================================

    cd plugins\%%P

    REM Create build directory
    if exist build\ rmdir /s /q build
    mkdir build
    cd build

    REM Configure with CMake
    echo [1/2] Configuring with CMake...
    cmake .. -G "Visual Studio 17 2022" -A x64
    if errorlevel 1 (
        echo [ERROR] CMake configuration failed for %%P
        set /a FAILED+=1
        cd ..\..\..
        goto :next_plugin
    )

    REM Build with CMake
    echo [2/2] Building...
    cmake --build . --config Release
    if errorlevel 1 (
        echo [ERROR] Build failed for %%P
        set /a FAILED+=1
    ) else (
        echo [SUCCESS] %%P built successfully
        set /a SUCCESS+=1
    )

    cd ..\..\..

    :next_plugin
    echo.
)

echo ======================================
echo Build Summary
echo ======================================
echo Successful: %SUCCESS%/10
echo Failed: %FAILED%/10
echo ======================================

if %FAILED% gtr 0 (
    echo.
    echo WARNING: Some builds failed. Check error messages above.
    exit /b 1
)

echo.
echo All plugins built successfully!
echo.
echo Next steps:
echo 1. Run: installers\windows\stage-plugins.bat
echo 2. Run: cd installers\windows
echo 3. Run: makensis audioforge.nsi
echo.

exit /b 0
