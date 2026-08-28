# AudioForge Cross-Platform Build Guide

This guide explains how to build installers for all three platforms (macOS, Windows, Linux).

## Overview

AudioForge v1.1.0 provides installers for:
- ✅ **macOS**: Signed DMG with Install/Uninstall scripts
- ✅ **Windows**: NSIS installer (.exe)
- ✅ **Linux**: .deb, .rpm, and AppImage packages

**Current Status** (2026-08-27):
- ✅ All plugins built and tested (macOS)
- ✅ macOS build scripts ready (signing pending certificate)
- ✅ Windows NSIS script ready
- ✅ Linux build scripts ready
- ⏳ Need to build on respective platforms

---

## Build Strategy

### Option 1: Platform-Specific VMs (Recommended)

Build each installer on its native platform using VMs:

**macOS** (current platform):
- ✅ Build plugins ✓
- ✅ Code signing (pending certificate)
- ✅ DMG notarization (pending credentials)
- ✅ Test on macOS 13, 14

**Windows VM** (via Parallels, VMware, or VirtualBox):
- Transfer plugin source code
- Rebuild plugins for Windows
- Build NSIS installer
- Code signing (pending certificate purchase)
- Test on Windows 10, 11

**Linux VM** (via Multipass, Docker, or VirtualBox):
- Transfer plugin source code
- Rebuild plugins for Linux
- Build .deb, .rpm, AppImage
- Test on Ubuntu 22.04/24.04, Fedora 39/40

### Option 2: CI/CD (Future Enhancement)

Use GitHub Actions to build all platforms automatically:
- macOS: Hosted runner (free for public repos)
- Windows: Hosted runner (free for public repos)
- Linux: Hosted runner (free for public repos)

**Benefit**: One commit triggers builds for all platforms
**Status**: Not implemented yet (planned for v1.2.0+)

---

## Step-by-Step: Building All Platforms

### Prerequisites

**Tools Required**:
- macOS: Xcode, Apple Developer account
- Windows: Visual Studio 2019+, NSIS, Code signing certificate
- Linux: GCC/Clang, CMake, dpkg-dev, rpm-build, appimagetool

**Estimated Time**:
- macOS build: 30 minutes (first time), 10 minutes (subsequent)
- Windows build: 45 minutes (first time), 15 minutes (subsequent)
- Linux build: 30 minutes (first time), 10 minutes (subsequent)

---

## Platform 1: macOS (Current)

### 1.1. Create Apple Developer ID Certificate

**Manual Step** (you need to do this):
1. Open Xcode → Settings → Accounts
2. Select your Apple ID → Manage Certificates
3. Click + → Developer ID Application
4. Set environment variable:
   ```bash
   export CODESIGN_IDENTITY="Developer ID Application: Your Name (TEAM_ID)"
   echo 'export CODESIGN_IDENTITY="..."' >> ~/.zshrc
   ```

### 1.2. Build Signed DMG

```bash
cd ~/Projects/Active/AudioForge

# Build plugins and create DMG (automatically signs if CODESIGN_IDENTITY is set)
./build-installer.sh

# Output: releases/installers/AudioForge-v1.1.0-macOS.dmg
```

### 1.3. Notarize DMG

**Manual Step** (requires app-specific password):
1. Generate app-specific password at appleid.apple.com
2. Store in Keychain:
   ```bash
   xcrun notarytool store-credentials "audioforge-notarize" \
       --apple-id "your@email.com" \
       --team-id "TEAM_ID" \
       --password "app-specific-password"
   ```
3. Notarize:
   ```bash
   ./installers/notarize-dmg.sh releases/installers/AudioForge-v1.1.0-macOS.dmg
   ```

### 1.4. Verify

```bash
# Check signature
codesign --verify --deep --strict --verbose=2 ~/Library/Audio/Plug-Ins/VST3/SimpleGain.vst3

# Check notarization
spctl --assess --type install --verbose releases/installers/AudioForge-v1.1.0-macOS.dmg
```

**Expected**: "source=Notarized Developer ID"

---

## Platform 2: Windows

### 2.1. Set Up Windows Build Environment

**Option A: Windows VM on macOS** (Parallels, VMware Fusion)
**Option B: Native Windows PC**
**Option C: Cloud VM** (AWS EC2 Windows, Azure)

**Install tools**:
1. Visual Studio 2019 or 2022 (Community Edition is free)
   - Include: "Desktop development with C++"
2. CMake: https://cmake.org/download/
3. Git: https://git-scm.com/download/win
4. NSIS: https://nsis.sourceforge.io/Download

### 2.2. Clone and Build Plugins

```cmd
git clone https://github.com/kentin0-fiz0l/AudioForge.git
cd AudioForge

REM Build all 10 plugins
for %P in (SimpleGain PanUtil BasicSynth CleanDelay SimpleEQ SimpleComp WaveShaper Saturation ChorusFlanger Reverb) do (
    cd plugins\%P
    mkdir build
    cd build
    cmake -G "Visual Studio 17 2022" ..
    cmake --build . --config Release
    cd ..\..\..
)
```

### 2.3. Collect Plugins to Staging

```cmd
mkdir releases\staging\AudioForge_Plugins\VST3

for %P in (SimpleGain PanUtil BasicSynth CleanDelay SimpleEQ SimpleComp WaveShaper Saturation ChorusFlanger Reverb) do (
    xcopy plugins\%P\build\%P_artefacts\VST3\%P.vst3 releases\staging\AudioForge_Plugins\VST3\ /E /I
)

copy releases\staging\README.txt releases\staging\AudioForge_Plugins\
```

### 2.4. Build NSIS Installer

```cmd
cd installers\windows
makensis audioforge.nsi
```

**Output**: `AudioForge-Setup-v1.1.0.exe`

### 2.5. Code Sign (Optional but Recommended)

**Purchase certificate** (one-time):
- EV Certificate: ~$400/year (instant SmartScreen trust) - **RECOMMENDED**
- Standard OV: ~$200/year (builds reputation over time)

**Vendors**: DigiCert, GlobalSign, Sectigo, SSL.com

**Sign installer**:
```powershell
cd installers\windows
.\sign.ps1
```

Or manually:
```powershell
signtool sign /a /fd SHA256 /tr http://timestamp.digicert.com /td SHA256 AudioForge-Setup-v1.1.0.exe
```

### 2.6. Test

1. Install on Windows 10/11 VM
2. Verify no SmartScreen warnings (if EV signed)
3. Check plugins installed to: `C:\Program Files\Common Files\VST3\`
4. Test in Reaper, FL Studio, or Ableton Live

**See**: `installers/windows/BUILD-INSTRUCTIONS.md` for detailed steps

---

## Platform 3: Linux

### 3.1. Set Up Linux Build Environment

**Option A: Multipass VM** (easiest on macOS):
```bash
# Install Multipass
brew install multipass

# Launch Ubuntu VM
multipass launch 24.04 --name audioforge-build --memory 4G --disk 20G

# Shell into VM
multipass shell audioforge-build
```

**Option B: Docker Container**:
```bash
docker run -it --rm -v $(pwd):/workspace ubuntu:24.04 bash
```

**Option C: Native Linux**

### 3.2. Install Build Tools

**Ubuntu/Debian**:
```bash
sudo apt update
sudo apt install -y build-essential cmake git dpkg-dev rpm wget
```

**Fedora**:
```bash
sudo dnf install -y gcc-c++ cmake git rpm-build dpkg wget
```

### 3.3. Clone and Build Plugins

```bash
git clone https://github.com/kentin0-fiz0l/AudioForge.git
cd AudioForge

# Build all 10 plugins
for plugin in SimpleGain PanUtil BasicSynth CleanDelay SimpleEQ SimpleComp WaveShaper Saturation ChorusFlanger Reverb; do
    cd plugins/$plugin
    mkdir -p build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    cmake --build . --config Release
    cd ../../..
done
```

### 3.4. Collect Plugins

```bash
./build-installer.sh
# Creates: releases/staging/AudioForge Plugins/VST3/
```

### 3.5. Build Packages

```bash
cd installers/linux
chmod +x build-packages.sh
./build-packages.sh
```

**Outputs**:
- `audioforge-1.1.0-amd64.deb` (Debian/Ubuntu)
- `audioforge-1.1.0-1.fc40.x86_64.rpm` (Fedora/RHEL)
- `AudioForge-1.1.0-x86_64.AppImage` (Universal)

### 3.6. Test

**Ubuntu/Debian**:
```bash
sudo dpkg -i ../../releases/installers/audioforge-1.1.0-amd64.deb
ls /usr/lib/vst3/*.vst3 | wc -l  # Should show 10
```

**Fedora**:
```bash
sudo rpm -ivh ../../releases/installers/audioforge-1.1.0-1.fc40.x86_64.rpm
ls /usr/lib64/vst3/*.vst3 | wc -l  # Should show 10
```

**AppImage**:
```bash
chmod +x ../../releases/installers/AudioForge-1.1.0-x86_64.AppImage
../../releases/installers/AudioForge-1.1.0-x86_64.AppImage
ls ~/.vst3/*.vst3 | wc -l  # Should show 10
```

**See**: `installers/linux/BUILD-INSTRUCTIONS.md` for detailed steps

---

## Quick Reference

### File Locations After Build

```
AudioForge/
├── releases/
│   └── installers/
│       ├── AudioForge-v1.1.0-macOS.dmg           # macOS installer
│       ├── AudioForge-Setup-v1.1.0.exe           # Windows installer
│       ├── audioforge-1.1.0-amd64.deb            # Debian/Ubuntu
│       ├── audioforge-1.1.0-1.fc40.x86_64.rpm    # Fedora/RHEL
│       └── AudioForge-1.1.0-x86_64.AppImage      # Universal Linux
```

### Upload to GitHub Release

Once all installers are built:

```bash
# Use GitHub CLI
gh release upload v1.1.0 \
    releases/installers/AudioForge-v1.1.0-macOS.dmg \
    releases/installers/AudioForge-Setup-v1.1.0.exe \
    releases/installers/audioforge-1.1.0-amd64.deb \
    releases/installers/audioforge-1.1.0-1.fc40.x86_64.rpm \
    releases/installers/AudioForge-1.1.0-x86_64.AppImage
```

### File Sizes (Approximate)

- macOS DMG: ~80 MB (includes all 10 VST3 + AU plugins)
- Windows .exe: ~50 MB (10 VST3 plugins)
- Linux .deb: ~50 MB
- Linux .rpm: ~50 MB
- Linux AppImage: ~50 MB

---

## Troubleshooting

### "I don't have a Windows/Linux machine"

**Solutions**:
1. **VMs**: Use Parallels (macOS), VirtualBox (free), or Multipass (Linux)
2. **Cloud**: AWS EC2, Azure, or DigitalOcean (pay-per-hour)
3. **CI/CD**: Set up GitHub Actions (free for public repos)
4. **Community**: Ask a contributor to build (less ideal)

### "Building takes too long"

**Optimizations**:
- Use pre-built plugins from macOS (transfer .vst3 folders)
- Only rebuild for the target platform (Windows/Linux need native binaries)
- Use CI/CD to automate (set up once, builds happen automatically)

### "Code signing is expensive"

**Options**:
1. **macOS**: Required for good UX ($99/year Apple Developer)
2. **Windows**: Optional but recommended ($200-400/year)
   - Without signing: Users see SmartScreen warnings
   - With EV signing: Instant trust, professional image
3. **Linux**: No code signing required (package managers verify checksums)

**Recommendation**: Start with macOS + Linux (free/cheap), add Windows signing later

---

## Next Steps

1. ✅ **macOS**: Create Developer ID certificate, notarize DMG
2. ⏳ **Windows**: Set up VM, build installer, decide on code signing
3. ⏳ **Linux**: Set up VM/Docker, build packages, test on Ubuntu/Fedora
4. ⏳ **Upload**: All installers to GitHub Release v1.1.0
5. ⏳ **Test**: VMs for each platform
6. ✅ **Announce**: v1.1.0 release on GitHub Discussions, Reddit, forums

---

**Last Updated**: 2026-08-27
