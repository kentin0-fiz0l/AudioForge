# Windows Installation Guide

This guide covers installation of AudioForge plugins on Windows 10 and Windows 11.

## Quick Start

### Option 1: Automatic Installation (Recommended)

1. **Download** the installer:
   - Go to https://github.com/username/AudioForge/releases/latest
   - Download `AudioForge-Setup-v1.1.0.exe`

2. **Run installer**:
   - Double-click `AudioForge-Setup-v1.1.0.exe`
   - Click "Install"
   - Wait for installation to complete

3. **Rescan plugins** in your DAW

**That's it!** All 10 plugins are now installed.

### Option 2: Manual Installation

1. **Download** the ZIP archive:
   - Go to https://github.com/username/AudioForge/releases/latest
   - Download `AudioForge-v1.1.0-Windows.zip`

2. **Extract** the ZIP file

3. **Copy plugins** to VST3 folder:
   ```powershell
   # Default VST3 path (64-bit)
   C:\Program Files\Common Files\VST3\
   ```

4. **Rescan plugins** in your DAW

---

## Detailed Installation

### System Requirements

**Operating System**:
- Windows 10 (version 1809 or later)
- Windows 11 (all versions)

**Architecture**:
- 64-bit (x86_64) - required
- 32-bit not supported

**DAW Requirements**:
- VST3-compatible DAW (Ableton Live, FL Studio, Reaper, Cubase, etc.)

### Installation Methods

#### Method 1: Automatic Installer (NSIS)

**Download installer**:
- Visit https://github.com/username/AudioForge/releases/latest
- Download `AudioForge-Setup-v1.1.0.exe` (~25 MB)

**Run installer**:
1. Double-click `AudioForge-Setup-v1.1.0.exe`

2. **Windows SmartScreen warning** (may appear if certificate is new):
   - Click **"More info"**
   - Click **"Run anyway"**

   *(This warning will disappear after our code signing certificate builds reputation)*

3. **User Account Control (UAC) prompt**:
   - Click **"Yes"** to allow installation

4. **Installer wizard**:
   - Click **"Install"**
   - Wait for plugins to be copied
   - Click **"Finish"**

**What it does**:
- Installs 10 VST3 plugins to `C:\Program Files\Common Files\VST3\`
- Creates uninstaller in `C:\Program Files\AudioForge\`
- Adds AudioForge to **Add/Remove Programs**

**Verify installation**:
```powershell
# Open PowerShell and run:
dir "C:\Program Files\Common Files\VST3\*.vst3"
```

Expected output:
```
SimpleGain.vst3
PanUtil.vst3
BasicSynth.vst3
CleanDelay.vst3
SimpleEQ.vst3
SimpleComp.vst3
WaveShaper.vst3
Saturation.vst3
ChorusFlanger.vst3
Reverb.vst3
```

#### Method 2: Silent Installation (Command Line)

**For IT administrators or automated deployment**:

```powershell
# Download installer
Invoke-WebRequest -Uri "https://github.com/username/AudioForge/releases/download/v1.1.0/AudioForge-Setup-v1.1.0.exe" -OutFile "AudioForge-Setup.exe"

# Run silent installation
.\AudioForge-Setup.exe /S

# Verify installation
Test-Path "C:\Program Files\Common Files\VST3\SimpleGain.vst3"
```

**Silent uninstall**:
```powershell
"C:\Program Files\AudioForge\Uninstall.exe" /S
```

#### Method 3: Manual Installation (ZIP)

**Download ZIP**:
- Visit https://github.com/username/AudioForge/releases/latest
- Download `AudioForge-v1.1.0-Windows.zip`

**Extract ZIP**:
1. Right-click `AudioForge-v1.1.0-Windows.zip`
2. Select **"Extract All..."**
3. Choose destination (e.g., `C:\Temp\AudioForge\`)

**Copy plugins**:

**Option A: Using File Explorer**:
1. Navigate to extracted folder → `VST3\`
2. Select all `.vst3` files
3. Copy (Ctrl+C)
4. Navigate to `C:\Program Files\Common Files\VST3\`
5. Paste (Ctrl+V)
   - If prompted for admin permission, click **"Continue"**

**Option B: Using PowerShell** (as Administrator):
```powershell
# Navigate to extracted folder
cd C:\Temp\AudioForge

# Copy all VST3 plugins
Copy-Item -Path "VST3\*.vst3" -Destination "C:\Program Files\Common Files\VST3\" -Recurse -Force
```

**Verify**:
```powershell
dir "C:\Program Files\Common Files\VST3\*.vst3"
```

---

## DAW Configuration

### Ableton Live

1. **Set VST3 path** (usually auto-detected):
   - Go to **Options** → **Preferences** → **Plug-ins**
   - Check **"Use VST3 Plug-in System Folders"**
   - Or manually add: `C:\Program Files\Common Files\VST3`

2. **Rescan plugins**:
   - Click **"Rescan"** in Plug-ins preferences
   - Wait for scan to complete

3. **Load plugin**:
   - Browser → **Plug-ins** → **VST3** → **AudioForge**
   - Drag plugin to track

### FL Studio

1. **Set VST3 path**:
   - Go to **Options** → **Manage plugins**
   - Click **"Find more plugins"** (folder icon)
   - FL Studio will scan `C:\Program Files\Common Files\VST3` automatically

2. **Rescan**:
   - Click **"Start scan"** → **"Fast scan"**

3. **Load plugin**:
   - Mixer → Insert slot → More → VST3 → AudioForge

### Reaper

1. **Set VST3 path** (usually auto-detected):
   - Go to **Options** → **Preferences** → **Plug-ins** → **VST**
   - Add path if needed: `C:\Program Files\Common Files\VST3`

2. **Rescan**:
   - Click **"Re-scan"**

3. **Load plugin**:
   - Insert → **FX** → search for plugin name

### Cubase

1. **VST3 path** (auto-detected by default)
   - Cubase automatically scans `C:\Program Files\Common Files\VST3`

2. **Rescan**:
   - Go to **Studio** → **VST Plug-in Manager**
   - Click **"Update"**

3. **Load plugin**:
   - Insert → **Audio Effect** → **AudioForge**

### Studio One

1. **VST3 path** (auto-detected):
   - Go to **Studio One** → **Options** → **Locations** → **VST Plug-ins**
   - Verify `C:\Program Files\Common Files\VST3` is listed

2. **Rescan**:
   - Click **"Reset"** → **"Reset and Re-scan"**

3. **Load plugin**:
   - Browser → **Effects** → **VST3** → **AudioForge**

---

## VST3 Plugin Paths

Windows VST3 plugins are searched in these locations:

### System-wide (recommended):
- `C:\Program Files\Common Files\VST3\` (64-bit)

### User-specific:
- `C:\Users\<Username>\AppData\Local\Programs\Common\VST3\`

**Note**: AudioForge installer uses system-wide path for all users.

---

## Troubleshooting

### Installer shows SmartScreen warning

**Warning message**:
```
"Windows protected your PC"
"Microsoft Defender SmartScreen prevented an unrecognized app from starting"
```

**Why this happens**:
- Our code signing certificate is new and hasn't built reputation yet
- This is normal for new software publishers

**How to proceed** (safe):
1. Click **"More info"**
2. Click **"Run anyway"**
3. Click **"Yes"** on UAC prompt

**Future**: This warning will disappear after our certificate builds reputation (100+ downloads over 2-4 weeks)

### Plugins not showing in DAW

**Cause**: VST3 path not configured, or plugins not in search path

**Fix**:
```powershell
# Verify plugins exist
dir "C:\Program Files\Common Files\VST3\*.vst3"

# Check DAW's VST3 search paths
# Most DAWs: Preferences → Plug-ins → VST3

# Force rescan in DAW
```

### Access Denied when copying plugins manually

**Error**:
```
Access is denied
```

**Cause**: Missing administrator permissions

**Fix**:
```powershell
# Run PowerShell as Administrator
# Right-click PowerShell → "Run as administrator"

# Then copy plugins
Copy-Item -Path "C:\Temp\AudioForge\VST3\*.vst3" -Destination "C:\Program Files\Common Files\VST3\" -Recurse -Force
```

### Plugins crash or don't load

**Cause**: Missing Visual C++ Redistributable

**Fix**:
```powershell
# Download and install Visual C++ Redistributable
# https://aka.ms/vs/17/release/vc_redist.x64.exe

# Or via PowerShell (as Administrator):
Invoke-WebRequest -Uri "https://aka.ms/vs/17/release/vc_redist.x64.exe" -OutFile "vc_redist.x64.exe"
.\vc_redist.x64.exe /install /quiet /norestart
```

### Audio crackling or latency issues

**Cause**: ASIO buffer size too small

**Fix**:
- Open your DAW's audio settings
- Increase **ASIO buffer size** to 512 or 1024 samples
- Example (Ableton): Options → Preferences → Audio → Buffer Size

---

## Uninstallation

### Method 1: Add/Remove Programs (Recommended)

1. Open **Settings** → **Apps** → **Installed apps**
2. Search for **"AudioForge"**
3. Click **"..."** → **"Uninstall"**
4. Click **"Yes"** to confirm

### Method 2: Uninstaller Executable

```powershell
# Run uninstaller
"C:\Program Files\AudioForge\Uninstall.exe"

# Or silent uninstall
"C:\Program Files\AudioForge\Uninstall.exe" /S
```

### Method 3: Manual Removal

```powershell
# Remove plugins (as Administrator)
Remove-Item "C:\Program Files\Common Files\VST3\SimpleGain.vst3" -Recurse -Force
Remove-Item "C:\Program Files\Common Files\VST3\PanUtil.vst3" -Recurse -Force
Remove-Item "C:\Program Files\Common Files\VST3\BasicSynth.vst3" -Recurse -Force
Remove-Item "C:\Program Files\Common Files\VST3\CleanDelay.vst3" -Recurse -Force
Remove-Item "C:\Program Files\Common Files\VST3\SimpleEQ.vst3" -Recurse -Force
Remove-Item "C:\Program Files\Common Files\VST3\SimpleComp.vst3" -Recurse -Force
Remove-Item "C:\Program Files\Common Files\VST3\WaveShaper.vst3" -Recurse -Force
Remove-Item "C:\Program Files\Common Files\VST3\Saturation.vst3" -Recurse -Force
Remove-Item "C:\Program Files\Common Files\VST3\ChorusFlanger.vst3" -Recurse -Force
Remove-Item "C:\Program Files\Common Files\VST3\Reverb.vst3" -Recurse -Force

# Remove program files
Remove-Item "C:\Program Files\AudioForge" -Recurse -Force
```

---

## Included Plugins

AudioForge v1.1.0 includes 10 professional VST3 plugins:

1. **SimpleGain** - Clean gain/volume control with metering
2. **PanUtil** - Stereo panning and width control
3. **BasicSynth** - Subtractive synthesizer (3 oscillators, ADSR, filter)
4. **CleanDelay** - Stereo delay with ping-pong mode
5. **SimpleEQ** - 3-band parametric equalizer
6. **SimpleComp** - Compressor with auto makeup gain
7. **WaveShaper** - Waveshaping distortion
8. **Saturation** - Multi-mode analog saturation
9. **ChorusFlanger** - LFO-based modulation effects
10. **Reverb** - Freeverb-inspired algorithmic reverb

All plugins are:
- ✅ Free and open source (GPL-3.0)
- ✅ VST3 format
- ✅ Cross-platform (Windows, macOS, Linux)
- ✅ Tested with major DAWs

---

## Advanced: Building from Source

For developers who want to build from source on Windows:

### Prerequisites

```powershell
# Install Visual Studio 2022 (Community Edition)
# https://visualstudio.microsoft.com/downloads/
# Select "Desktop development with C++"

# Install CMake
winget install Kitware.CMake

# Install Git
winget install Git.Git
```

### Build Steps

```powershell
# Clone repository
git clone https://github.com/username/AudioForge.git
cd AudioForge

# Initialize JUCE submodule
git submodule update --init --recursive

# Build all plugins (PowerShell)
.\build-all.ps1

# Or build with CMake manually
cd plugins\SimpleGain
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release

# Install plugins
Copy-Item -Path "build\SimpleGain_artefacts\Release\VST3\SimpleGain.vst3" -Destination "C:\Program Files\Common Files\VST3\" -Recurse -Force
```

---

## Support

**Website**: https://audioforge.fluxstudio.art
**GitHub**: https://github.com/username/AudioForge
**Issues**: https://github.com/username/AudioForge/issues
**Discussions**: https://github.com/username/AudioForge/discussions

---

**Last Updated**: 2026-08-23
