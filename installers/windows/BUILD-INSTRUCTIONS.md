# Windows Installer Build Instructions

## Prerequisites

### 1. Install NSIS (Nullsoft Scriptable Install System)

**Download**: https://nsis.sourceforge.io/Download

**Installation**:
1. Download NSIS 3.x installer
2. Run the installer
3. Add NSIS to PATH (optional, for command-line use)

**Verify installation**:
```cmd
makensis /VERSION
```

Expected output: `v3.x`

### 2. Prepare Plugin Files

**Option A: Build from source on Windows**

```cmd
cd AudioForge
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

**Option B: Copy pre-built macOS plugins to Windows**

If plugins are already built on macOS, you need to rebuild them for Windows:

```cmd
REM Clone the repository on Windows
git clone https://github.com/kentin0-fiz0l/AudioForge.git
cd AudioForge

REM Build all plugins
cd plugins\SimpleGain
mkdir build && cd build
cmake ..
cmake --build . --config Release
cd ..\..\..\

REM Repeat for all 10 plugins, or use build script:
REM TODO: Create build-all-plugins.bat for Windows
```

### 3. Run build-installer.sh Equivalent on Windows

The `build-installer.sh` script is for macOS/Linux. On Windows, you need to manually collect the plugins:

```cmd
REM Create staging directory
mkdir releases\staging\AudioForge_Plugins\VST3

REM Copy all VST3 plugins from builds
xcopy plugins\SimpleGain\build\SimpleGain_artefacts\VST3\SimpleGain.vst3 releases\staging\AudioForge_Plugins\VST3\ /E /I
xcopy plugins\PanUtil\build\PanUtil_artefacts\VST3\PanUtil.vst3 releases\staging\AudioForge_Plugins\VST3\ /E /I
xcopy plugins\BasicSynth\build\BasicSynth_artefacts\VST3\BasicSynth.vst3 releases\staging\AudioForge_Plugins\VST3\ /E /I
xcopy plugins\CleanDelay\build\CleanDelay_artefacts\VST3\CleanDelay.vst3 releases\staging\AudioForge_Plugins\VST3\ /E /I
xcopy plugins\SimpleEQ\build\SimpleEQ_artefacts\VST3\SimpleEQ.vst3 releases\staging\AudioForge_Plugins\VST3\ /E /I
xcopy plugins\SimpleComp\build\SimpleComp_artefacts\VST3\SimpleComp.vst3 releases\staging\AudioForge_Plugins\VST3\ /E /I
xcopy plugins\WaveShaper\build\WaveShaper_artefacts\VST3\WaveShaper.vst3 releases\staging\AudioForge_Plugins\VST3\ /E /I
xcopy plugins\Saturation\build\Saturation_artefacts\VST3\Saturation.vst3 releases\staging\AudioForge_Plugins\VST3\ /E /I
xcopy plugins\ChorusFlanger\build\ChorusFlanger_artefacts\VST3\ChorusFlanger.vst3 releases\staging\AudioForge_Plugins\VST3\ /E /I
xcopy plugins\Reverb\build\Reverb_artefacts\VST3\Reverb.vst3 releases\staging\AudioForge_Plugins\VST3\ /E /I

REM Copy README
copy releases\staging\README.txt releases\staging\AudioForge_Plugins\
```

## Building the Installer (Unsigned)

### 1. Navigate to installer directory

```cmd
cd installers\windows
```

### 2. Build with NSIS

```cmd
makensis audioforge.nsi
```

Expected output:
```
Output: "AudioForge-Setup-v1.1.0.exe"
Install: 10 files, 50MB
Uninstall: 10 files
```

### 3. Verify the installer

The installer will be created at:
```
installers\windows\AudioForge-Setup-v1.1.0.exe
```

**Test the installer**:
1. Double-click the .exe
2. Follow installation wizard
3. Verify plugins are installed to: `C:\Program Files\Common Files\VST3\`

## Code Signing (Eliminates SmartScreen Warnings)

### 1. Purchase Code Signing Certificate

**Recommended**: EV (Extended Validation) Certificate - $400/year
- Instant SmartScreen trust
- No reputation-building period
- Best user experience

**Alternative**: Standard OV Certificate - $200/year
- Initial SmartScreen warnings
- Builds reputation over time (weeks/months)

**Vendors**:
- DigiCert: https://www.digicert.com/signing/code-signing-certificates
- GlobalSign: https://www.globalsign.com/en/code-signing-certificate
- Sectigo (formerly Comodo): https://sectigo.com/ssl-certificates-tls/code-signing
- SSL.com: https://www.ssl.com/code-signing/

### 2. Install Certificate

**For EV Certificate** (USB token):
1. Insert USB token
2. Install certificate drivers from vendor
3. Certificate is on the token, no installation needed

**For Standard Certificate** (file-based):
1. Download certificate from vendor
2. Double-click .pfx or .p12 file
3. Import to Windows Certificate Store
4. Enter password when prompted

**Verify certificate is installed**:
```powershell
Get-ChildItem Cert:\CurrentUser\My -CodeSigningCert
```

### 3. Sign the Installer

**Using the provided PowerShell script**:

```powershell
cd installers\windows
.\sign.ps1
```

**Manual signing**:

```powershell
# Sign the installer executable
signtool sign /a /fd SHA256 /tr http://timestamp.digicert.com /td SHA256 AudioForge-Setup-v1.1.0.exe

# Sign individual plugins (optional, but recommended)
signtool sign /a /fd SHA256 /tr http://timestamp.digicert.com /td SHA256 "..\..\releases\staging\AudioForge Plugins\VST3\SimpleGain.vst3\Contents\x86_64-win\SimpleGain.vst3"
# Repeat for all 10 plugins
```

### 4. Verify Signature

```powershell
signtool verify /pa /v AudioForge-Setup-v1.1.0.exe
```

Expected output:
```
Successfully verified: AudioForge-Setup-v1.1.0.exe
Number of signatures successfully Verified: 1
```

## Testing

### 1. Test on Clean Windows VM

**Recommended VMs**:
- Windows 10 21H2 (64-bit)
- Windows 10 22H2 (64-bit)
- Windows 11 (64-bit)

**Test Steps**:
1. Download installer to VM
2. Double-click to run
3. **Expected (unsigned)**: SmartScreen warning appears
   - Click "More info" → "Run anyway"
4. **Expected (EV signed)**: No SmartScreen warning
5. Follow installation wizard
6. Verify plugins installed to: `C:\Program Files\Common Files\VST3\`
7. Open a DAW (Reaper, FL Studio, Ableton Live)
8. Rescan plugins
9. Load and test at least 3 plugins

### 2. Test Uninstaller

1. Open Settings → Apps → Installed apps
2. Find "AudioForge v1.1.0"
3. Click Uninstall
4. Verify all files removed from `C:\Program Files\Common Files\VST3\`

## Troubleshooting

### makensis not found

**Solution**: Add NSIS to PATH

```cmd
set PATH=%PATH%;C:\Program Files (x86)\NSIS
```

Or restart terminal after NSIS installation.

### Plugins not found during build

**Error**: `File: "..\..\releases\staging\AudioForge Plugins\VST3\*.vst3" -> no files found.`

**Solution**: Build plugins first or ensure staging directory structure is correct.

### Code signing fails: "No certificates found"

**Solution**: Verify certificate is installed in the Windows Certificate Store:

```powershell
Get-ChildItem Cert:\CurrentUser\My -CodeSigningCert
```

If empty, re-import the certificate.

### SmartScreen still warns after signing

**Cause**: Standard OV certificate hasn't built reputation yet.

**Solution**:
1. Wait 2-4 weeks for reputation to build (based on download volume)
2. OR upgrade to EV certificate for instant trust

## Distribution

### Upload to GitHub Release

1. Go to https://github.com/kentin0-fiz0l/AudioForge/releases/tag/v1.1.0
2. Click "Edit release"
3. Drag and drop `AudioForge-Setup-v1.1.0.exe` to assets
4. Update release notes to mention Windows installer

### Share Installer

**Direct download link**:
```
https://github.com/kentin0-fiz0l/AudioForge/releases/download/v1.1.0/AudioForge-Setup-v1.1.0.exe
```

**Website download button**:
```html
<a href="https://github.com/kentin0-fiz0l/AudioForge/releases/download/v1.1.0/AudioForge-Setup-v1.1.0.exe">
  Download for Windows (v1.1.0)
</a>
```

## Notes

- Installer requires Windows 7 or later (64-bit)
- Plugins are installed to the standard VST3 directory
- Uninstaller is registered in Windows Add/Remove Programs
- File size: ~50MB (all 10 plugins)
- Build time: ~2 minutes (unsigned), ~5 minutes (signed)

---

**Last Updated**: 2026-08-27
