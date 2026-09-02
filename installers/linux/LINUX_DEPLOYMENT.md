# Linux Deployment Guide - AudioForge v1.5.0

## Overview

AudioForge supports three Linux package formats:
1. **`.deb`** - Debian/Ubuntu (most common)
2. **`.rpm`** - Fedora/RHEL/openSUSE
3. **`AppImage`** - Universal (works on all distros)

All packages install VST3 plugins to standard system directories.

---

## Quick Start

### Option A: Build on Linux (Native)

```bash
# On Ubuntu/Debian
sudo apt install dpkg-dev rpm

# On Fedora/RHEL
sudo dnf install dpkg rpm-build

# Build all packages
cd installers/linux
./build-packages.sh

# Output:
# releases/installers/audioforge-1.5.0-amd64.deb
# releases/installers/audioforge-1.5.0-1.x86_64.rpm
# releases/installers/AudioForge-1.5.0-x86_64.AppImage
```

### Option B: Build from macOS (Docker)

```bash
# Build .deb package
docker run --rm -v "$PWD:/workspace" -w /workspace/installers/linux \
    ubuntu:22.04 bash -c "
        apt update && apt install -y dpkg-dev &&
        ./build-packages.sh deb
    "

# Build .rpm package
docker run --rm -v "$PWD:/workspace" -w /workspace/installers/linux \
    fedora:38 bash -c "
        dnf install -y rpm-build &&
        ./build-packages.sh rpm
    "

# Build AppImage (requires appimage-builder)
docker run --rm -v "$PWD:/workspace" -w /workspace/installers/linux \
    ubuntu:22.04 bash -c "
        apt update && apt install -y wget python3-pip &&
        pip3 install appimage-builder &&
        ./build-packages.sh appimage
    "
```

---

## Package Details

### .deb Package (Debian/Ubuntu)

**File**: `audioforge-1.5.0-amd64.deb`  
**Size**: ~50 MB  
**Install Path**: `/usr/lib/vst3/`  
**Supported**: Ubuntu 20.04+, Debian 11+, Linux Mint 20+

**Installation:**
```bash
sudo dpkg -i audioforge-1.5.0-amd64.deb

# Or double-click in file manager (Ubuntu Software opens)
```

**Uninstallation:**
```bash
sudo apt remove audioforge
```

### .rpm Package (Fedora/RHEL)

**File**: `audioforge-1.5.0-1.x86_64.rpm`  
**Size**: ~50 MB  
**Install Path**: `/usr/lib64/vst3/`  
**Supported**: Fedora 36+, RHEL 9+, openSUSE Leap 15+

**Installation:**
```bash
sudo rpm -i audioforge-1.5.0-1.x86_64.rpm

# Or via dnf
sudo dnf install audioforge-1.5.0-1.x86_64.rpm
```

**Uninstallation:**
```bash
sudo rpm -e audioforge
```

### AppImage (Universal)

**File**: `AudioForge-1.5.0-x86_64.AppImage`  
**Size**: ~60 MB (includes dependencies)  
**Install Path**: User's home directory (no root required)  
**Supported**: All modern Linux distros

**Installation:**
```bash
# Make executable
chmod +x AudioForge-1.5.0-x86_64.AppImage

# Run (extracts to ~/.local/share/vst3/)
./AudioForge-1.5.0-x86_64.AppImage

# Optional: Integrate with system
./AudioForge-1.5.0-x86_64.AppImage --appimage-integrate
```

**Uninstallation:**
```bash
rm AudioForge-1.5.0-x86_64.AppImage
rm -rf ~/.local/share/vst3/AudioForge*
```

---

## Build Script Reference

### Full Build (All Packages)

```bash
cd installers/linux
./build-packages.sh
```

### Individual Package Types

```bash
# .deb only (5-10 minutes)
./build-packages.sh deb

# .rpm only (5-10 minutes)
./build-packages.sh rpm

# AppImage only (10-15 minutes)
./build-packages.sh appimage
```

---

## Testing on Linux

### Recommended Test Distros

1. **Ubuntu 22.04 LTS** (.deb) - Most popular
2. **Fedora 38** (.rpm) - Red Hat ecosystem
3. **Arch Linux** (AppImage) - Rolling release

### Test Procedure

```bash
# 1. Install package
sudo dpkg -i audioforge-1.5.0-amd64.deb

# 2. Verify files installed
ls /usr/lib/vst3/

# Should show:
# BasicSynth.vst3
# ChorusFlanger.vst3
# ... (19 total plugins)

# 3. Test in Reaper
reaper
# → Preferences → VST → Rescan

# 4. Load each plugin
# → Insert → VST3 → AudioForge → [Plugin Name]

# 5. Check for crashes
# → Process audio for 30 seconds
# → Automate parameters
# → Save/load project
```

---

## Known Issues & Workarounds

### Issue 1: dpkg-deb not found (macOS)

**Error**: `bash: dpkg-deb: command not found`

**Solution**: Use Docker build (see Option B above)

### Issue 2: Plugins not showing in DAW

**Symptoms**: Installed successfully but DAW can't find plugins

**Solutions**:
1. **Reaper**: Preferences → VST → Add path `/usr/lib/vst3` → Rescan
2. **Bitwig**: Settings → Plug-ins → Add location → `/usr/lib/vst3`
3. **Ardour**: Preferences → Plugins → VST3 Path → Add `/usr/lib/vst3`

### Issue 3: Permission denied on /usr/lib/vst3

**Error**: `Permission denied: /usr/lib/vst3`

**Solution**: User-local install instead of system-wide:
```bash
# Extract .deb without installing
dpkg-deb -x audioforge-1.5.0-amd64.deb audioforge-extracted

# Copy to user directory
mkdir -p ~/.vst3
cp -r audioforge-extracted/usr/lib/vst3/* ~/.vst3/

# DAWs will scan ~/.vst3 automatically
```

---

## CI/CD Integration

### GitHub Actions Workflow

Add to `.github/workflows/release.yml`:

```yaml
name: Build Linux Packages

on:
  release:
    types: [created]

jobs:
  build-linux:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Build plugins
        run: ./installers/build-installer.sh
      
      - name: Build .deb package
        run: |
          cd installers/linux
          ./build-packages.sh deb
      
      - name: Upload .deb to release
        uses: actions/upload-release-asset@v1
        with:
          upload_url: ${{ github.event.release.upload_url }}
          asset_path: releases/installers/audioforge-1.5.0-amd64.deb
          asset_name: audioforge-1.5.0-amd64.deb
          asset_content_type: application/vnd.debian.binary-package
```

---

## Distribution Checklist

Before releasing Linux packages:

- [ ] Update version in `build-packages.sh`
- [ ] Update plugin count in package descriptions
- [ ] Test .deb on Ubuntu 22.04
- [ ] Test .rpm on Fedora 38
- [ ] Test AppImage on Arch Linux
- [ ] Verify all 19 plugins load in Reaper
- [ ] Check CPU usage (<25% total)
- [ ] Generate SHA256 checksums
- [ ] Upload to GitHub Release
- [ ] Update README with Linux install instructions
- [ ] Announce on r/linuxaudio subreddit

---

## SHA256 Checksums

```bash
# Generate checksums
cd releases/installers
sha256sum audioforge-1.5.0-amd64.deb > SHA256SUMS.txt
sha256sum audioforge-1.5.0-1.x86_64.rpm >> SHA256SUMS.txt
sha256sum AudioForge-1.5.0-x86_64.AppImage >> SHA256SUMS.txt

# Verify checksum
sha256sum -c SHA256SUMS.txt
```

---

## User Statistics (Estimated)

**Linux User Base**: 10-15% of audio producers  
**Distribution Breakdown**:
- Ubuntu/Debian: 60%
- Fedora/RHEL: 20%
- Arch/Other: 20%

**Expected Downloads (Month 1)**:
- .deb: 30-50 downloads
- .rpm: 10-20 downloads
- AppImage: 5-10 downloads
- **Total**: 45-80 Linux users

---

## Support

**Linux-specific Issues**: https://github.com/kentin0-fiz0l/AudioForge/issues  
**Label**: `platform: linux`

**Common Linux DAWs**:
- Reaper (recommended, best VST3 support)
- Bitwig Studio (excellent Linux support)
- Ardour (open source, professional)
- LMMS (beginner-friendly)

---

## Next Steps

1. **Build packages on Linux VM or Docker**
2. **Test on 3 major distros**
3. **Upload to GitHub Release**
4. **Update main README with Linux instructions**
5. **Submit to Linux audio communities**
   - r/linuxaudio (Reddit)
   - linuxmusicians.com (Forum)
   - linux.audio (Directory)

---

**Status**: ✅ **SCRIPTS READY** - Awaiting Linux build environment  
**Timeline**: 2-3 hours to build and test all packages  
**Blocker**: None (can build with Docker from macOS)

*Last Updated: September 1, 2026*
