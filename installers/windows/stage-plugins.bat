@echo off
REM AudioForge Windows Plugin Staging Script
REM Copies built plugins to staging directory for installer creation
REM Version: 1.1.1

echo ======================================
echo AudioForge Plugin Staging Script
echo Version: 1.1.1
echo ======================================
echo.

REM Check if we're in the AudioForge directory
if not exist "plugins\" (
    echo ERROR: Run this script from the AudioForge root directory
    exit /b 1
)

REM Staging directory
set STAGING_DIR=releases\staging\AudioForge_Plugins
set VST3_DIR=%STAGING_DIR%\VST3

REM Clean and create staging directory
echo Cleaning staging directory...
if exist "%STAGING_DIR%\" rmdir /s /q "%STAGING_DIR%"
mkdir "%VST3_DIR%"

echo.
echo Copying plugins to staging...
echo.

REM Plugin list
set PLUGINS=SimpleGain PanUtil BasicSynth CleanDelay SimpleEQ SimpleComp WaveShaper Saturation ChorusFlanger Reverb

REM Copy counter
set COPIED=0
set MISSING=0

REM Copy each plugin
for %%P in (%PLUGINS%) do (
    set PLUGIN_PATH=plugins\%%P\build\%%P_artefacts\Release\VST3\%%P.vst3

    if exist "!PLUGIN_PATH!" (
        echo [OK] Copying %%P.vst3...
        xcopy "!PLUGIN_PATH!" "%VST3_DIR%\%%P.vst3\" /E /I /Q
        if errorlevel 1 (
            echo [ERROR] Failed to copy %%P
            set /a MISSING+=1
        ) else (
            set /a COPIED+=1
        )
    ) else (
        echo [MISSING] %%P.vst3 not found - did you build it?
        set /a MISSING+=1
    )
)

REM Copy README
echo.
echo Copying README...
if exist "releases\staging\README.txt" (
    copy "releases\staging\README.txt" "%STAGING_DIR%\"
) else (
    echo WARNING: README.txt not found
)

echo.
echo ======================================
echo Staging Summary
echo ======================================
echo Copied: %COPIED%/10 plugins
echo Missing: %MISSING%/10 plugins
echo Location: %STAGING_DIR%
echo ======================================

if %MISSING% gtr 0 (
    echo.
    echo WARNING: Some plugins are missing. Build them first with:
    echo   installers\windows\build-all-plugins.bat
    exit /b 1
)

echo.
echo All plugins staged successfully!
echo.
echo Next step:
echo   cd installers\windows
echo   makensis audioforge.nsi
echo.

exit /b 0
