# AudioForge v1.3.0 Windows Installer Guide

Complete guide for building and signing the Windows installer with all 13 plugins.

---

## Prerequisites

### 1. Development Tools

**Visual Studio 2022** (Community Edition - Free)
- Download: https://visualstudio.microsoft.com/downloads/
- Workload: "Desktop development with C++"
- Components: CMake tools, MSVC v143, Windows SDK

**NSIS 3.x** (Nullsoft Scriptable Install System)
- Download: https://nsis.sourceforge.io/Download
- Install and add to PATH

**Git for Windows**
- Download: https://git-scm.com/download/win
- For cloning the repository

### 2. Code Signing Certificate (Optional but Recommended)

**Option A: EV Certificate** ($400/year) - **Recommended**
- ✅ Instant SmartScreen trust
- ✅ Zero security warnings on first install
- ✅ Best user experience
- ✅ Professional appearance

**Vendors:**
- DigiCert: https://www.digicert.com/signing/code-signing-certificates
- GlobalSign: https://www.globalsign.com/en/code-signing-certificate

**Option B: Standard OV Certificate** ($200/year)
- ⚠️ Initial SmartScreen warnings
- ⏳ Builds reputation over 2-4 weeks
- ✅ Eventually gets trusted
- ✅ More affordable

**Vendors:**
- Sectigo: https://sectigo.com/ssl-certificates-tls/code-signing
- SSL.com: https://www.ssl.com/code-signing/

**Option C: No Certificate** (Free)
- ❌ SmartScreen warnings on all installs
- ❌ Users must click "More info" → "Run anyway"
- ❌ Poor user experience
- ✅ OK for testing/development

---

## Build Process (6-8 hours first time, 1-2 hours subsequent)

### Step 1: Clone Repository (5 min)

```cmd
cd C:\Projects
git clone --recursive https://github.com/kentin0-fiz0l/AudioForge.git
cd AudioForge
```

### Step 2: Build All Plugins (30-45 min)

```cmd
REM Run the automated build script
installers\windows\build-all-plugins.bat
```

**What it does:**
- Builds all 13 plugins in Release mode (VST3 format)
- Uses CMake + Visual Studio 2022
- Takes ~2-3 minutes per plugin

**Expected output:**
```
======================================
Build Summary
======================================
Successful: 13/13
Failed: 0/13
======================================

All plugins built successfully!
```

**If errors occur:**
- Check Visual Studio 2022 is installed
- Verify CMake is in PATH
- Check JUCE submodule is initialized: `git submodule update --init`

### Step 3: Stage Plugins (2 min)

```cmd
REM Copy built plugins to staging directory
installers\windows\stage-plugins.bat
```

**What it does:**
- Collects all VST3 plugins from build directories
- Copies to `releases\staging\AudioForge_Plugins\VST3\`
- Prepares for installer packaging

**Expected output:**
```
======================================
Staging Summary
======================================
Copied: 13/13 plugins
Missing: 0/13 plugins
Location: releases\staging\AudioForge_Plugins
======================================

All plugins staged successfully!
```

### Step 4: Create Installer Executable (1 min)

```cmd
cd installers\windows
makensis audioforge.nsi
```

**Output:**
```
Output: "AudioForge-Setup-v1.3.0.exe"
Size: ~60-80 MB (all 13 plugins)
```

**File created:**
```
installers\windows\AudioForge-Setup-v1.3.0.exe
```

---

## Code Signing (If you have a certificate)

### Setup (One-Time)

**For EV Certificate (USB Token):**
1. Insert USB token into computer
2. Install vendor-provided drivers
3. Certificate is on token (no installation needed)

**For Standard Certificate (.pfx file):**
1. Download certificate from vendor
2. Double-click `.pfx` file
3. Import to Windows Certificate Store
4. Enter password when prompted

**Verify certificate:**
```powershell
Get-ChildItem Cert:\CurrentUser\My -CodeSigningCert
```

### Sign Plugins and Installer

**Using the automated script:**
```powershell
cd installers\windows
.\sign.ps1 -CertificatePath "C:\path\to\certificate.pfx" -CertificatePassword "your-password"
```

**Or set environment variables:**
```powershell
$env:WINDOWS_CERT_PATH = "C:\path\to\certificate.pfx"
$env:WINDOWS_CERT_PASSWORD = "your-password"
.\sign.ps1
```

**What gets signed:**
- All 13 VST3 plugin binaries
- AudioForge-Setup-v1.3.0.exe installer

**Verify signatures:**
```powershell
signtool verify /pa /v AudioForge-Setup-v1.3.0.exe
```

**Expected output:**
```
Successfully verified: AudioForge-Setup-v1.3.0.exe
Number of signatures successfully Verified: 1
```

---

## Testing (30 min - 1 hour)

### Test 1: Install on Clean Windows VM

**Recommended VMs:**
- Windows 10 22H2 (64-bit)
- Windows 11 23H2 (64-bit)

**Test steps:**
1. Copy `AudioForge-Setup-v1.3.0.exe` to VM
2. Double-click installer
3. **Expected (EV signed):** No SmartScreen warnings
4. **Expected (unsigned):** SmartScreen warning → "More info" → "Run anyway"
5. Complete installation wizard
6. Verify plugins installed to: `C:\Program Files\Common Files\VST3\`
7. Check all 13 plugins present:
   - Core Suite (11): SimpleGain, PanUtil, BasicSynth, CleanDelay, SimpleEQ, SimpleComp, WaveShaper, Saturation, ChorusFlanger, Reverb, FreezeFX
   - Advanced Suite (2): GranularEngine, SpectralFreeze

### Test 2: Load in DAW

**Recommended DAWs:**
- Reaper (free trial): https://www.reaper.fm/download.php
- FL Studio (demo): https://www.image-line.com/fl-studio-download/
- Ableton Live (trial): https://www.ableton.com/en/trial/

**Test steps:**
1. Open DAW
2. Rescan plugins (Settings → Audio → VST3 scan)
3. Load SpectralFreeze (newest plugin)
4. Load GranularEngine
5. Load at least 2 core plugins (e.g., SimpleGain, Reverb)
6. Verify all plugins load without errors
7. Test basic functionality (move sliders, hear audio processing)

### Test 3: Uninstall

1. Open Settings → Apps → Installed apps
2. Find "AudioForge v1.3.0"
3. Click Uninstall
4. Verify all 13 plugins removed from VST3 directory
5. Verify uninstaller removed from Program Files

---

## Distribution

### Upload to GitHub Release

```bash
# Via GitHub CLI (gh)
gh release upload v1.3.0 installers\windows\AudioForge-Setup-v1.3.0.exe

# Or manually:
# 1. Go to https://github.com/kentin0-fiz0l/AudioForge/releases/tag/v1.3.0
# 2. Click "Edit release"
# 3. Drag and drop AudioForge-Setup-v1.3.0.exe to assets
# 4. Click "Update release"
```

### Download Link

Direct download:
```
https://github.com/kentin0-fiz0l/AudioForge/releases/download/v1.3.0/AudioForge-Setup-v1.3.0.exe
```

Website integration:
```html
<a href="https://github.com/kentin0-fiz0l/AudioForge/releases/download/v1.3.0/AudioForge-Setup-v1.3.0.exe">
  Download for Windows (v1.3.0 - 80MB)
</a>
```

---

## File Checklist

After completing the build process, you should have:

```
AudioForge/
├── installers/
│   └── windows/
│       └── AudioForge-Setup-v1.3.0.exe  ← Installer (signed or unsigned)
└── releases/
    └── staging/
        └── AudioForge_Plugins/
            ├── VST3/
            │   ├── SimpleGain.vst3
            │   ├── PanUtil.vst3
            │   ├── BasicSynth.vst3
            │   ├── CleanDelay.vst3
            │   ├── SimpleEQ.vst3
            │   ├── SimpleComp.vst3
            │   ├── WaveShaper.vst3
            │   ├── Saturation.vst3
            │   ├── ChorusFlanger.vst3
            │   ├── Reverb.vst3
            │   ├── FreezeFX.vst3
            │   ├── GranularEngine.vst3
            │   └── SpectralFreeze.vst3
            └── README.txt
```

---

## Troubleshooting

### Build Errors

**Error:** `CMake not found`
**Solution:** Install Visual Studio 2022 with CMake tools, or add CMake to PATH

**Error:** `JUCE modules not found`
**Solution:** Initialize submodules: `git submodule update --init --recursive`

**Error:** `Build failed for [Plugin]`
**Solution:** Check Visual Studio 2022 is installed with C++ desktop development workload

### Signing Errors

**Error:** `No certificates found`
**Solution:** Verify certificate is in Windows Certificate Store:
```powershell
Get-ChildItem Cert:\CurrentUser\My -CodeSigningCert
```

**Error:** `Invalid certificate password`
**Solution:** Verify password is correct for .pfx file

**Error:** `Timestamp server unreachable`
**Solution:** Check internet connection, or use alternate timestamp server:
```powershell
.\sign.ps1 -TimestampServer "http://timestamp.comodoca.com"
```

### Installer Errors

**Error:** `makensis: command not found`
**Solution:** Add NSIS to PATH:
```cmd
set PATH=%PATH%;C:\Program Files (x86)\NSIS
```

**Error:** `File: "*.vst3" -> no files found`
**Solution:** Run `installers\windows\stage-plugins.bat` first to collect plugins

### SmartScreen Warnings

**Issue:** SmartScreen still warns after signing with Standard OV certificate
**Cause:** Certificate hasn't built reputation yet
**Solution:**
- Wait 2-4 weeks for reputation to build (based on download volume)
- OR upgrade to EV certificate for instant trust

---

## Time Estimates

| Task | First Time | Subsequent |
|------|-----------|------------|
| Setup development environment | 1-2 hours | - |
| Clone repository | 5 min | - |
| Build all 13 plugins | 30-45 min | 30-45 min |
| Stage plugins | 2 min | 2 min |
| Create installer | 1 min | 1 min |
| Sign plugins + installer | 5 min | 5 min |
| Test on VM | 30 min | 15 min |
| Upload to GitHub | 2 min | 2 min |
| **Total** | **6-8 hours** | **1-2 hours** |

---

## Comparison: Windows vs macOS

| Feature | Windows (v1.3.0) | macOS (v1.3.0) |
|---------|------------------|----------------|
| **Plugins** | 13 VST3 | 13 VST3 + 13 AU |
| **Installer** | .exe (NSIS) | .dmg |
| **Signing** | Authenticode | Developer ID |
| **Notarization** | Not needed | Required |
| **Build time** | 30-45 min | 5-10 min |
| **Cert cost** | $200-400/year | $99/year |
| **SmartScreen** | Yes (without EV cert) | No (after notarization) |
| **User experience** | Good (EV cert) / OK (unsigned) | Excellent (signed + notarized) |

---

## Quick Reference Commands

**Full build workflow:**
```cmd
REM 1. Build all plugins
installers\windows\build-all-plugins.bat

REM 2. Stage plugins
installers\windows\stage-plugins.bat

REM 3. Create installer
cd installers\windows
makensis audioforge.nsi

REM 4. Sign (if certificate available)
.\sign.ps1 -CertificatePath "path\to\cert.pfx" -CertificatePassword "password"

REM 5. Verify
signtool verify /pa /v AudioForge-Setup-v1.3.0.exe
```

**Quick test:**
```cmd
REM Install
AudioForge-Setup-v1.3.0.exe

REM Verify installation
dir "C:\Program Files\Common Files\VST3\*.vst3"

REM Count plugins
dir "C:\Program Files\Common Files\VST3\*.vst3" /b | find /c /v ""
REM Expected: 13
```

---

## Next Steps After Windows Installer

Once Windows installer is complete:
1. ✅ Update GitHub Release v1.3.0 with Windows installer
2. ✅ Test on Windows 10/11 VMs
3. ⏭️ Build Linux packages (.deb, .rpm, AppImage)
4. ⏭️ Update website with download links
5. ⏭️ Share on Windows audio production communities

---

**Last Updated:** 2026-08-30
**Version:** 1.3.0
**Author:** AudioForge Team
