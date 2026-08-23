# Linux Installation Guide

This guide covers installation of AudioForge plugins on Linux distributions using .deb packages (Ubuntu/Debian), .rpm packages (Fedora/RHEL), or AppImage (universal).

## Quick Start

### Ubuntu / Debian (.deb)

```bash
# Download .deb package
wget https://github.com/username/AudioForge/releases/download/v1.1.0/audioforge-1.1.0-amd64.deb

# Install
sudo dpkg -i audioforge-1.1.0-amd64.deb

# Rescan plugins in your DAW
```

### Fedora / RHEL (.rpm)

```bash
# Download .rpm package
wget https://github.com/username/AudioForge/releases/download/v1.1.0/audioforge-1.1.0-1.fc39.x86_64.rpm

# Install
sudo rpm -ivh audioforge-1.1.0-1.fc39.x86_64.rpm

# Or with dnf
sudo dnf install audioforge-1.1.0-1.fc39.x86_64.rpm

# Rescan plugins in your DAW
```

### Universal (AppImage)

```bash
# Download AppImage
wget https://github.com/username/AudioForge/releases/download/v1.1.0/AudioForge-1.1.0-x86_64.AppImage

# Make executable
chmod +x AudioForge-1.1.0-x86_64.AppImage

# Run installer
./AudioForge-1.1.0-x86_64.AppImage

# Rescan plugins in your DAW
```

---

## Detailed Installation

### Prerequisites

**Supported Distributions**:
- Ubuntu 20.04 LTS, 22.04 LTS, 24.04 LTS
- Debian 11 (Bullseye), 12 (Bookworm)
- Fedora 39, 40
- RHEL 8, 9
- Arch Linux (manual installation)
- Other distros (use AppImage)

**System Requirements**:
- 64-bit x86_64 architecture
- GLIBC 2.31+ (Ubuntu 20.04+, Fedora 32+)
- VST3-compatible DAW (Reaper, Bitwig, Ardour, etc.)

### Installation Methods

#### Method 1: .deb Package (Ubuntu/Debian)

**Download**:
```bash
cd ~/Downloads
wget https://github.com/username/AudioForge/releases/download/v1.1.0/audioforge-1.1.0-amd64.deb
```

**Install**:
```bash
sudo dpkg -i audioforge-1.1.0-amd64.deb
```

**Verify installation**:
```bash
dpkg -L audioforge
ls /usr/lib/vst3/
```

**Expected output**:
```
/usr/lib/vst3/SimpleGain.vst3
/usr/lib/vst3/PanUtil.vst3
/usr/lib/vst3/BasicSynth.vst3
/usr/lib/vst3/CleanDelay.vst3
/usr/lib/vst3/SimpleEQ.vst3
/usr/lib/vst3/SimpleComp.vst3
/usr/lib/vst3/WaveShaper.vst3
/usr/lib/vst3/Saturation.vst3
/usr/lib/vst3/ChorusFlanger.vst3
/usr/lib/vst3/Reverb.vst3
```

**Uninstall**:
```bash
sudo dpkg -r audioforge
```

#### Method 2: .rpm Package (Fedora/RHEL)

**Download**:
```bash
cd ~/Downloads
wget https://github.com/username/AudioForge/releases/download/v1.1.0/audioforge-1.1.0-1.fc39.x86_64.rpm
```

**Install with rpm**:
```bash
sudo rpm -ivh audioforge-1.1.0-1.fc39.x86_64.rpm
```

**Or install with dnf** (recommended):
```bash
sudo dnf install audioforge-1.1.0-1.fc39.x86_64.rpm
```

**Verify installation**:
```bash
rpm -ql audioforge
ls /usr/lib64/vst3/
```

**Uninstall**:
```bash
sudo dnf remove audioforge
# or
sudo rpm -e audioforge
```

#### Method 3: AppImage (Universal)

**Download and run**:
```bash
cd ~/Downloads
wget https://github.com/username/AudioForge/releases/download/v1.1.0/AudioForge-1.1.0-x86_64.AppImage
chmod +x AudioForge-1.1.0-x86_64.AppImage
./AudioForge-1.1.0-x86_64.AppImage
```

**What it does**:
- Installs plugins to `~/.vst3/`
- No root/sudo required
- User-specific installation

**Verify installation**:
```bash
ls ~/.vst3/
```

**Uninstall**:
```bash
rm -rf ~/.vst3/SimpleGain.vst3
rm -rf ~/.vst3/PanUtil.vst3
# ... (remove other plugins)
# or remove all:
rm -rf ~/.vst3/*.vst3
```

#### Method 4: Manual Installation

**From GitHub releases**:
```bash
# Download source or release artifacts
wget https://github.com/username/AudioForge/releases/download/v1.1.0/AudioForge-v1.1.0-Linux.tar.gz

# Extract
tar xzf AudioForge-v1.1.0-Linux.tar.gz

# Copy plugins (system-wide, requires sudo)
sudo cp -r AudioForge-v1.1.0/VST3/*.vst3 /usr/lib/vst3/

# Or user-specific (no sudo)
mkdir -p ~/.vst3
cp -r AudioForge-v1.1.0/VST3/*.vst3 ~/.vst3/
```

---

## DAW Configuration

### Reaper

1. **Set VST3 path** (if needed):
   - Go to **Options** → **Preferences** → **Plug-ins** → **VST**
   - Add path: `/usr/lib/vst3` or `~/.vst3`

2. **Rescan plugins**:
   - Click **Re-scan**
   - Wait for scan to complete

3. **Load plugin**:
   - Insert effect on track
   - Search for "AudioForge" or plugin name (e.g., "SimpleGain")

### Bitwig Studio

1. **Set VST3 path**:
   - Go to **Settings** → **Locations** → **VST3 Plug-ins**
   - Add `/usr/lib/vst3` or `~/.vst3`

2. **Rescan plugins**:
   - Click **Rescan** in Locations settings

3. **Load plugin**:
   - Browser → **Plug-ins** → **VST3** → AudioForge

### Ardour

1. **Rescan plugins**:
   - Go to **Window** → **Plugin Manager**
   - Click **Discover**

2. **Load plugin**:
   - Right-click mixer strip → **New Plugin**
   - Search for plugin name

### Qtractor

1. **Rescan plugins**:
   - Go to **View** → **Plugins**
   - Click **Rescan**

2. **Load plugin**:
   - Right-click track → **Add Plugin** → VST3

### LMMS

1. **Set VST3 path**:
   - Go to **Edit** → **Settings** → **Paths** → **VST-plugin directories**
   - Add `/usr/lib/vst3` or `~/.vst3`

2. **Rescan and load**:
   - Restart LMMS
   - Plugins appear in **Instrument plugins** / **Effect plugins**

---

## VST3 Plugin Paths

Linux VST3 plugins are searched in these locations (in order):

### System-wide (all users, requires sudo):
- `/usr/lib/vst3/`
- `/usr/local/lib/vst3/`
- `/usr/lib64/vst3/` (Fedora/RHEL)

### User-specific (no sudo required):
- `~/.vst3/`

**Recommendation**: Use system-wide installation (`.deb`/`.rpm`) for single-user systems, or user-specific (`~/.vst3`) for multi-user systems.

---

## Troubleshooting

### Plugins not showing in DAW

**Cause**: VST3 path not configured, or plugins not in search path

**Fix**:
```bash
# Verify plugins exist
ls /usr/lib/vst3/*.vst3
# or
ls ~/.vst3/*.vst3

# Check DAW's VST3 search paths
# Most DAWs: Preferences → Plug-ins → VST3

# Force rescan in DAW
```

### Permission denied when installing .deb/.rpm

**Cause**: Missing `sudo` for system-wide installation

**Fix**:
```bash
# Use sudo
sudo dpkg -i audioforge-1.1.0-amd64.deb
# or
sudo rpm -ivh audioforge-1.1.0-1.fc39.x86_64.rpm

# Or use AppImage (no sudo required)
./AudioForge-1.1.0-x86_64.AppImage
```

### GLIBC version error

**Error**:
```
./SimpleGain.vst3: /lib/x86_64-linux-gnu/libc.so.6: version `GLIBC_2.34' not found
```

**Cause**: Your distribution has an older GLIBC version

**Fix**:
```bash
# Check your GLIBC version
ldd --version

# AudioForge requires GLIBC 2.31+
# Upgrade your distribution or build from source
```

### Plugins crash or don't load

**Cause**: Missing dependencies or library conflicts

**Fix**:
```bash
# Check plugin dependencies
ldd /usr/lib/vst3/SimpleGain.vst3/Contents/x86_64-linux/SimpleGain.so

# Install missing dependencies
# Ubuntu/Debian:
sudo apt install libstdc++6 libgcc-s1

# Fedora:
sudo dnf install libstdc++ gcc
```

### Audio crackling or latency issues

**Cause**: JACK/ALSA buffer size too small

**Fix**:
```bash
# For JACK users
qjackctl
# Settings → Frames/Period: 512 or 1024

# For PipeWire users
pw-metadata -n settings 0 clock.force-quantum 1024
```

---

## Building from Source

For advanced users who want to build from source:

### Prerequisites

```bash
# Ubuntu/Debian
sudo apt install build-essential cmake git \
    libasound2-dev libfreetype6-dev \
    libx11-dev libxrandr-dev libxinerama-dev \
    libxcursor-dev

# Fedora
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake git alsa-lib-devel \
    freetype-devel libX11-devel
```

### Build Steps

```bash
# Clone repository
git clone https://github.com/username/AudioForge.git
cd AudioForge

# Initialize JUCE submodule
git submodule update --init --recursive

# Build all plugins
./build-all.sh

# Install system-wide
sudo ./install-plugins.sh

# Or install user-specific
./install-plugins.sh --user
```

---

## Uninstallation

### Remove .deb package

```bash
sudo dpkg -r audioforge
```

### Remove .rpm package

```bash
sudo dnf remove audioforge
# or
sudo rpm -e audioforge
```

### Remove AppImage installation

```bash
rm -rf ~/.vst3/SimpleGain.vst3
rm -rf ~/.vst3/PanUtil.vst3
rm -rf ~/.vst3/BasicSynth.vst3
rm -rf ~/.vst3/CleanDelay.vst3
rm -rf ~/.vst3/SimpleEQ.vst3
rm -rf ~/.vst3/SimpleComp.vst3
rm -rf ~/.vst3/WaveShaper.vst3
rm -rf ~/.vst3/Saturation.vst3
rm -rf ~/.vst3/ChorusFlanger.vst3
rm -rf ~/.vst3/Reverb.vst3
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
- ✅ Cross-platform (Linux, macOS, Windows)
- ✅ Tested with major DAWs

---

## Support

**Website**: https://audioforge.fluxstudio.art
**GitHub**: https://github.com/username/AudioForge
**Issues**: https://github.com/username/AudioForge/issues
**Discussions**: https://github.com/username/AudioForge/discussions

---

**Last Updated**: 2026-08-23
