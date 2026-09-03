# Linux Build Dependencies for AudioForge

This document lists all dependencies required to build AudioForge plugins on Linux.

## System Requirements

- **OS**: Ubuntu 22.04+ / Debian 11+ / Fedora 36+
- **CMake**: 3.22 or higher (JUCE requirement)
- **Compiler**: GCC 11+ or Clang 12+
- **Architecture**: x86_64 (amd64)

## Required Packages

### Build Tools
```bash
sudo apt-get install -y \
  build-essential \
  cmake \
  pkg-config \
  fakeroot \
  dpkg-dev \
  rpm
```

### Audio Libraries
```bash
sudo apt-get install -y \
  libasound2-dev
```

### Graphics & UI Libraries
```bash
sudo apt-get install -y \
  libfreetype6-dev \
  libgl1-mesa-dev \
  libglu1-mesa-dev
```

### X11 Development Libraries
```bash
sudo apt-get install -y \
  libx11-dev \
  libxinerama-dev \
  libxrandr-dev \
  libxcursor-dev \
  libxi-dev \
  libxext-dev
```

### Network & Web Libraries
```bash
sudo apt-get install -y \
  libcurl4-openssl-dev \
  libwebkit2gtk-4.0-dev
```

### All-in-One Installation

```bash
sudo apt-get update
sudo apt-get install -y \
  build-essential \
  cmake \
  pkg-config \
  libasound2-dev \
  libfreetype6-dev \
  libx11-dev \
  libxinerama-dev \
  libxrandr-dev \
  libxcursor-dev \
  libxi-dev \
  libxext-dev \
  libgl1-mesa-dev \
  libglu1-mesa-dev \
  libcurl4-openssl-dev \
  libwebkit2gtk-4.0-dev \
  fakeroot \
  dpkg-dev \
  rpm \
  wget \
  fuse
```

## Building Plugins

### 1. Build All Plugins (creates staging directory)

```bash
cd /path/to/AudioForge
./installers/build-installer.sh 1.8.1
```

### 2. Remove Broken Plugins from Staging

```bash
cd releases/staging/AudioForge\ Plugins
rm -rf VST3/FreezeFX.vst3 VST3/SpectralFreeze.vst3 VST3/Sampler.vst3
```

### 3. Build Linux Packages

```bash
cd installers/linux
./build-packages.sh
```

This creates:
- `audioforge-1.8.1-amd64.deb` (Debian/Ubuntu)
- `audioforge-1.8.1-1.x86_64.rpm` (Fedora/RHEL)
- `AudioForge-1.8.1.AppImage` (Universal)

## GitHub Actions Status

**Current State**: Workflow created but requires iterative dependency fixing.

**Workflow File**: `.github/workflows/build-linux-packages.yml`

**Known Issues**:
- Dependencies must be discovered incrementally (each missing library = 8-10 min rebuild)
- All dependencies listed above are now in the workflow
- Should work on next run (untested as of 2026-09-03)

**To Run Manually**:
```bash
gh workflow run build-linux-packages.yml -f version=1.8.1
```

## Testing Packages

### Debian/Ubuntu (.deb)
```bash
sudo dpkg -i audioforge-1.8.1-amd64.deb
# Verify installation
ls ~/.vst3/ | grep -i audioforge
# Or system-wide
ls /usr/lib/vst3/
```

### Fedora/RHEL (.rpm)
```bash
sudo rpm -i audioforge-1.8.1-1.x86_64.rpm
# Verify installation
ls ~/.vst3/ | grep -i audioforge
# Or system-wide
ls /usr/lib64/vst3/
```

### AppImage (Universal)
```bash
chmod +x AudioForge-1.8.1.AppImage
./AudioForge-1.8.1.AppImage
# This installs plugins to ~/.vst3/
```

## Troubleshooting

### "CMake 3.22 or higher required"
- Ubuntu 22.04 ships with CMake 3.22+ by default
- Ubuntu 20.04: Install from Kitware APT repository
- Or download from https://cmake.org/download/

### "fatal error: X11/extensions/XInput2.h"
```bash
sudo apt-get install libxi-dev libxext-dev
```

### "fatal error: curl/curl.h"
```bash
sudo apt-get install libcurl4-openssl-dev
```

### "fatal error: webkit2/webkit2.h"
```bash
sudo apt-get install libwebkit2gtk-4.0-dev
```

## Future Improvements

1. **Docker Build**: Create Dockerfile with all dependencies pre-installed
2. **CI/CD Automation**: Fix GitHub Actions workflow and enable on every release tag
3. **Flatpak**: Consider Flatpak distribution for broader Linux compatibility
4. **Snap**: Consider Snap package for Ubuntu Software distribution

## References

- JUCE Linux Documentation: https://github.com/juce-framework/JUCE/blob/master/docs/Linux%20Dependencies.md
- Debian Packaging Guide: https://www.debian.org/doc/manuals/maint-guide/
- RPM Packaging Guide: https://rpm-packaging-guide.github.io/
- AppImage Best Practices: https://docs.appimage.org/packaging-guide/
