# AudioForge v1.3.0 Linux Packages Guide

Complete guide for building .deb, .rpm, and AppImage packages for Linux distributions.

---

## Overview

AudioForge provides three Linux package formats:

| Format | Distributions | Installation |
|--------|---------------|--------------|
| **.deb** | Debian, Ubuntu, Linux Mint, Pop!_OS | `sudo dpkg -i audioforge-1.3.0-amd64.deb` |
| **.rpm** | Fedora, RHEL, CentOS, openSUSE | `sudo rpm -i audioforge-1.3.0-1.fc39.x86_64.rpm` |
| **AppImage** | Universal (any distro) | `chmod +x AudioForge-1.3.0-x86_64.AppImage && ./AudioForge...` |

**All packages install 13 VST3 plugins** - no code signing required on Linux!

---

## Prerequisites

### Ubuntu/Debian 22.04+

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    git \
    dpkg-dev \
    rpm \
    fakeroot \
    wget \
    curl
```

### Fedora 39+

```bash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    git \
    rpm-build \
    dpkg \
    wget \
    curl
```

### Arch Linux

```bash
sudo pacman -S base-devel cmake git rpm-tools wget curl
```

---

## Quick Build Process (4-6 hours first time, 1 hour subsequent)

### Step 1: Clone Repository (5 min)

```bash
cd ~/Projects
git clone --recursive https://github.com/kentin0-fiz0l/AudioForge.git
cd AudioForge
```

**Verify JUCE submodule:**
```bash
git submodule update --init --recursive
```

### Step 2: Build All Plugins (30-45 min on 4-core machine)

```bash
# Use the macOS/Linux build script
./build-installer.sh
```

**What it does:**
- Builds all 13 plugins in Release mode
- Collects VST3 plugins to `releases/staging/`
- Creates README and install scripts
- Takes ~2-3 minutes per plugin

**Expected output:**
```
======================================
Build Complete!
======================================

Staging directory: releases/staging

Next steps:
1. Review contents in: releases/staging
2. Test Install AudioForge.command
3. Create DMG with: ./create-dmg.sh
```

**Plugin verification:**
```bash
ls -1 releases/staging/AudioForge\ Plugins/VST3/*.vst3
```

Should show 13 plugins.

### Step 3: Build Linux Packages (2-5 min)

```bash
cd installers/linux
chmod +x build-packages.sh
./build-packages.sh
```

**What gets built:**
- ✅ `audioforge-1.3.0-amd64.deb` (always built)
- ✅ `audioforge-1.3.0-1.fc39.x86_64.rpm` (if rpmbuild available)
- ✅ `AudioForge-1.3.0-x86_64.AppImage` (if appimagetool available)

**Expected output:**
```
========================================
Linux packages built successfully!
========================================

Output directory: /path/to/AudioForge/releases/installers

-rw-r--r-- 1 user user  45M audioforge-1.3.0-amd64.deb
-rw-r--r-- 1 user user  46M audioforge-1.3.0-1.fc39.x86_64.rpm
-rwxr-xr-x 1 user user  47M AudioForge-1.3.0-x86_64.AppImage
```

---

## Package Details

### .deb Package (Debian/Ubuntu)

**Installation:**
```bash
sudo dpkg -i audioforge-1.3.0-amd64.deb
```

**Installs to:**
- Plugins: `/usr/lib/vst3/*.vst3` (13 plugins)
- Docs: `/usr/share/doc/audioforge/README.txt`

**Uninstall:**
```bash
sudo apt remove audioforge
```

**Package info:**
```bash
dpkg -L audioforge  # List installed files
dpkg -s audioforge  # Show package status
```

### .rpm Package (Fedora/RHEL)

**Installation:**
```bash
sudo rpm -i audioforge-1.3.0-1.fc39.x86_64.rpm
```

**Or with dnf:**
```bash
sudo dnf install audioforge-1.3.0-1.fc39.x86_64.rpm
```

**Installs to:**
- Plugins: `/usr/lib64/vst3/*.vst3` (13 plugins)
- Docs: `/usr/share/doc/audioforge/README.txt`

**Uninstall:**
```bash
sudo rpm -e audioforge
# or
sudo dnf remove audioforge
```

**Package info:**
```bash
rpm -ql audioforge  # List installed files
rpm -qi audioforge  # Show package info
```

### AppImage (Universal)

**Usage:**
```bash
# Make executable
chmod +x AudioForge-1.3.0-x86_64.AppImage

# Run to install plugins
./AudioForge-1.3.0-x86_64.AppImage
```

**What it does:**
- Copies all 13 VST3 plugins to `~/.vst3/`
- User-specific installation (no sudo required)
- Portable - works on any Linux distro

**Manual installation from AppImage:**
```bash
# Extract AppImage
./AudioForge-1.3.0-x86_64.AppImage --appimage-extract

# Copy plugins manually
cp -r squashfs-root/usr/lib/vst3/*.vst3 ~/.vst3/
```

**Uninstall:**
```bash
rm -rf ~/.vst3/SimpleGain.vst3
rm -rf ~/.vst3/PanUtil.vst3
# ... repeat for all 13 plugins
# Or remove all AudioForge plugins:
rm -rf ~/.vst3/{SimpleGain,PanUtil,BasicSynth,CleanDelay,SimpleEQ,SimpleComp,WaveShaper,Saturation,ChorusFlanger,Reverb,FreezeFX,GranularEngine,SpectralFreeze}.vst3
```

---

## Testing

### Test 1: Install on Clean VM

**Recommended Test VMs:**
- Ubuntu 22.04 LTS (Jammy)
- Ubuntu 24.04 LTS (Noble)
- Fedora 39/40
- Debian 12 (Bookworm)

**Test .deb on Ubuntu 22.04:**
```bash
# 1. Copy package to VM
scp audioforge-1.3.0-amd64.deb user@vm:~/

# 2. Install on VM
ssh user@vm
sudo dpkg -i audioforge-1.3.0-amd64.deb

# 3. Verify installation
ls /usr/lib/vst3/*.vst3 | wc -l
# Expected: 13

# 4. List all plugins
dpkg -L audioforge | grep vst3

# 5. Test in DAW (see below)
```

**Test .rpm on Fedora 40:**
```bash
# 1. Copy package
scp audioforge-1.3.0-1.fc40.x86_64.rpm user@fedora-vm:~/

# 2. Install
ssh user@fedora-vm
sudo dnf install audioforge-1.3.0-1.fc40.x86_64.rpm

# 3. Verify
ls /usr/lib64/vst3/*.vst3 | wc -l
# Expected: 13
```

**Test AppImage on any distro:**
```bash
# 1. Copy AppImage
scp AudioForge-1.3.0-x86_64.AppImage user@vm:~/

# 2. Run
chmod +x AudioForge-1.3.0-x86_64.AppImage
./AudioForge-1.3.0-x86_64.AppImage

# 3. Verify
ls ~/.vst3/*.vst3 | wc -l
# Expected: 13
```

### Test 2: Load in DAW

**Recommended Linux DAWs:**
- **Reaper** (free trial, excellent VST3 support): https://www.reaper.fm/download.php
- **Ardour** (open source): https://ardour.org/download.html
- **Bitwig Studio** (trial): https://www.bitwig.com/download/
- **LMMS** (free): https://lmms.io/download

**Test in Reaper:**
```bash
# 1. Install Reaper
wget https://www.reaper.fm/files/7.x/reaper720_linux_x86_64.tar.xz
tar xf reaper720_linux_x86_64.tar.xz
cd reaper_linux_x86_64
./install-reaper.sh

# 2. Launch Reaper
reaper

# 3. Scan plugins
Options → Preferences → Plug-ins → VST3
Click "Re-scan" → Wait for scan to complete

# 4. Load AudioForge plugins
Insert new track → Insert → FX → VST3
Search for "AudioForge" or plugin names:
  - SpectralFreeze (newest advanced plugin)
  - GranularEngine (newest advanced plugin)
  - SimpleGain (test core plugin)
  - Reverb (test core plugin)

# 5. Verify functionality
- Move sliders and verify parameter changes
- Process audio through plugins
- Check for crashes or errors
```

### Test 3: Verify Plugin Count

```bash
# For .deb/.rpm packages
dpkg -L audioforge 2>/dev/null | grep "\.vst3$" | wc -l  # Debian/Ubuntu
rpm -ql audioforge 2>/dev/null | grep "\.vst3$" | wc -l  # Fedora/RHEL
# Expected: 13

# For AppImage
ls ~/.vst3/*.vst3 2>/dev/null | wc -l
# Expected: 13

# List all installed plugins
ls -1 /usr/lib/vst3/*.vst3 2>/dev/null || ls -1 /usr/lib64/vst3/*.vst3 2>/dev/null || ls -1 ~/.vst3/*.vst3
```

### Test 4: Uninstall

**Test .deb uninstall:**
```bash
sudo apt remove audioforge
ls /usr/lib/vst3/*.vst3 2>/dev/null | wc -l
# Expected: 0 (or only non-AudioForge plugins if others installed)
```

**Test .rpm uninstall:**
```bash
sudo dnf remove audioforge
ls /usr/lib64/vst3/*.vst3 2>/dev/null | wc -l
# Expected: 0
```

---

## Distribution

### Upload to GitHub Release

```bash
# Upload all three package types to v1.3.0 release
cd releases/installers

gh release upload v1.3.0 \
    audioforge-1.3.0-amd64.deb \
    audioforge-1.3.0-1.fc39.x86_64.rpm \
    AudioForge-1.3.0-x86_64.AppImage
```

**Or upload manually:**
1. Go to https://github.com/kentin0-fiz0l/AudioForge/releases/tag/v1.3.0
2. Click "Edit release"
3. Drag and drop all three files to assets
4. Click "Update release"

### Download Links

**Direct downloads:**
```
.deb: https://github.com/kentin0-fiz0l/AudioForge/releases/download/v1.3.0/audioforge-1.3.0-amd64.deb
.rpm: https://github.com/kentin0-fiz0l/AudioForge/releases/download/v1.3.0/audioforge-1.3.0-1.fc39.x86_64.rpm
AppImage: https://github.com/kentin0-fiz0l/AudioForge/releases/download/v1.3.0/AudioForge-1.3.0-x86_64.AppImage
```

### Website Integration

```html
<!-- Debian/Ubuntu -->
<a href="https://github.com/kentin0-fiz0l/AudioForge/releases/download/v1.3.0/audioforge-1.3.0-amd64.deb">
  Download for Debian/Ubuntu (.deb - 45MB)
</a>

<!-- Fedora/RHEL -->
<a href="https://github.com/kentin0-fiz0l/AudioForge/releases/download/v1.3.0/audioforge-1.3.0-1.fc39.x86_64.rpm">
  Download for Fedora/RHEL (.rpm - 46MB)
</a>

<!-- Universal Linux -->
<a href="https://github.com/kentin0-fiz0l/AudioForge/releases/download/v1.3.0/AudioForge-1.3.0-x86_64.AppImage">
  Download for All Linux (AppImage - 47MB)
</a>
```

---

## Troubleshooting

### Build Errors

**Error:** `cmake: command not found`
```bash
# Ubuntu/Debian
sudo apt install cmake

# Fedora
sudo dnf install cmake
```

**Error:** `JUCE modules not found`
```bash
# Initialize JUCE submodule
git submodule update --init --recursive
```

**Error:** `dpkg-deb: command not found`
```bash
sudo apt install dpkg-dev  # Ubuntu/Debian
sudo dnf install dpkg      # Fedora
```

### Package Installation Errors

**Error:** `.deb` dependency issues
```bash
# Fix missing dependencies
sudo apt --fix-broken install
```

**Error:** `.rpm` conflicts
```bash
# Force installation (use with caution)
sudo rpm -i --force audioforge-1.3.0-1.fc39.x86_64.rpm
```

**Error:** `AppImage` won't run
```bash
# Check FUSE is installed
sudo apt install fuse libfuse2  # Ubuntu/Debian
sudo dnf install fuse fuse-libs  # Fedora

# Or extract and run manually
./AudioForge-1.3.0-x86_64.AppImage --appimage-extract
cd squashfs-root
./AppRun
```

### Plugin Not Found in DAW

**Issue:** Plugins don't appear after installation

**Solution:**
```bash
# 1. Verify plugins are installed
ls /usr/lib/vst3/*.vst3 2>/dev/null || \
ls /usr/lib64/vst3/*.vst3 2>/dev/null || \
ls ~/.vst3/*.vst3

# 2. Check VST3 path in DAW
# Most DAWs scan these locations by default:
# - /usr/lib/vst3/
# - /usr/lib64/vst3/
# - ~/.vst3/

# 3. Add custom path in DAW if needed
# Reaper: Preferences → Plug-ins → VST3 → Add
# Ardour: Edit → Preferences → Plugins → VST3

# 4. Force rescan in DAW
# Reaper: Preferences → Plug-ins → VST3 → Re-scan
# Ardour: Edit → Preferences → Plugins → Scan for Plugins
```

---

## File Checklist

After building, you should have:

```
AudioForge/
└── releases/
    └── installers/
        ├── audioforge-1.3.0-amd64.deb           (~45 MB)
        ├── audioforge-1.3.0-1.fc39.x86_64.rpm   (~46 MB)
        └── AudioForge-1.3.0-x86_64.AppImage     (~47 MB)
```

**Verify package contents:**

```bash
# Check .deb contents
dpkg-deb -c audioforge-1.3.0-amd64.deb | grep vst3

# Check .rpm contents
rpm -qpl audioforge-1.3.0-1.fc39.x86_64.rpm | grep vst3

# Check AppImage contents
./AudioForge-1.3.0-x86_64.AppImage --appimage-extract
ls squashfs-root/usr/lib/vst3/
```

All should show 13 `.vst3` plugins.

---

## Comparison: Linux vs macOS vs Windows

| Feature | Linux | macOS | Windows |
|---------|-------|-------|---------|
| **Package formats** | .deb, .rpm, AppImage | .dmg | .exe (NSIS) |
| **Plugin formats** | VST3 only | VST3 + AU | VST3 only |
| **Plugin count** | 13 | 13 + 13 (AU) | 13 |
| **Code signing** | ❌ Not required | ✅ Required ($99/yr) | ⚠️ Optional ($200-400/yr) |
| **Build time** | 30-45 min | 5-10 min | 30-45 min |
| **User experience** | Excellent | Excellent (signed) | Good (signed) / OK (unsigned) |
| **No warnings** | ✅ Always | ✅ After notarization | ✅ With EV cert only |

**Linux advantage:** No code signing required - zero security warnings!

---

## Quick Reference Commands

**Full build workflow:**
```bash
# 1. Clone and build plugins
git clone --recursive https://github.com/kentin0-fiz0l/AudioForge.git
cd AudioForge
./build-installer.sh

# 2. Build Linux packages
cd installers/linux
./build-packages.sh

# 3. Verify packages
ls -lh ../../releases/installers/audioforge*

# 4. Test .deb locally (Ubuntu/Debian)
sudo dpkg -i ../../releases/installers/audioforge-1.3.0-amd64.deb
ls /usr/lib/vst3/*.vst3 | wc -l  # Should show 13
```

**Quick test in Reaper:**
```bash
# 1. Install package
sudo dpkg -i audioforge-1.3.0-amd64.deb

# 2. Launch Reaper
reaper

# 3. Insert track → FX → VST3 → Search "Spectral" or "Granular"
# 4. Load SpectralFreeze or GranularEngine
# 5. Test with audio
```

---

## Time Estimates

| Task | First Time | Subsequent |
|------|-----------|------------|
| Setup development environment | 30 min | - |
| Clone repository | 5 min | - |
| Build all 13 plugins | 30-45 min | 30-45 min |
| Build Linux packages | 5 min | 5 min |
| Test on VM (single distro) | 30 min | 15 min |
| Test on 3 VMs (Ubuntu/Fedora/Debian) | 1.5 hours | 45 min |
| Upload to GitHub | 2 min | 2 min |
| **Total** | **4-6 hours** | **1-2 hours** |

---

## Next Steps After Linux Packages

Once Linux packages are complete:
1. ✅ Upload all three formats to GitHub Release v1.3.0
2. ✅ Test on Ubuntu 22.04/24.04 and Fedora 39/40 VMs
3. ✅ Update website with Linux download links
4. ✅ Share on Linux audio communities (r/linuxaudio, Linux Audio Users mailing list)
5. ✅ Consider creating AUR package for Arch Linux users

---

**Last Updated:** 2026-08-30
**Version:** 1.3.0
**Author:** AudioForge Team
