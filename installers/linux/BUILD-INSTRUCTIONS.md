# Linux Package Build Instructions

This guide covers building .deb (Debian/Ubuntu), .rpm (Fedora/RHEL), and AppImage packages for AudioForge.

## Prerequisites

### Ubuntu/Debian

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    git \
    dpkg-dev \
    rpm \
    fakeroot \
    wget
```

### Fedora/RHEL

```bash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    git \
    rpm-build \
    dpkg \
    wget
```

## Building Plugins from Source

### 1. Clone the repository

```bash
git clone https://github.com/kentin0-fiz0l/AudioForge.git
cd AudioForge
```

### 2. Build all plugins

AudioForge uses JUCE framework and CMake. Each plugin must be built separately:

```bash
# Build all 10 plugins
for plugin in SimpleGain PanUtil BasicSynth CleanDelay SimpleEQ SimpleComp WaveShaper Saturation ChorusFlanger Reverb; do
    echo "Building $plugin..."
    cd plugins/$plugin
    mkdir -p build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    cmake --build . --config Release
    cd ../../..
done
```

### 3. Collect plugins to staging directory

```bash
# Run the build-installer.sh script to collect all plugins
./build-installer.sh
```

This creates:
- `releases/staging/AudioForge Plugins/VST3/` - All 10 VST3 plugins
- `releases/staging/README.txt` - Installation instructions

## Building Packages

### Quick Start: Build All Packages

```bash
cd installers/linux
chmod +x build-packages.sh
./build-packages.sh
```

This builds (if tools are available):
- ✅ `.deb` package (always built, requires dpkg-deb)
- ⚠️ `.rpm` package (requires rpmbuild)
- ⚠️ `AppImage` (requires appimagetool)

Output: `releases/installers/`

---

## Manual Package Building

### Build .deb Package (Debian/Ubuntu)

```bash
cd installers/linux

# Create package structure
DEB_DIR="audioforge-1.1.0"
mkdir -p $DEB_DIR/DEBIAN
mkdir -p $DEB_DIR/usr/lib/vst3
mkdir -p $DEB_DIR/usr/share/doc/audioforge

# Copy plugins
cp -r ../../releases/staging/AudioForge\ Plugins/VST3/*.vst3 $DEB_DIR/usr/lib/vst3/

# Copy README
cp ../../releases/staging/README.txt $DEB_DIR/usr/share/doc/audioforge/

# Create control file (package metadata)
cat > $DEB_DIR/DEBIAN/control << EOF
Package: audioforge
Version: 1.1.0
Section: sound
Priority: optional
Architecture: amd64
Maintainer: AudioForge <contact@audioforge.art>
Homepage: https://audioforge.fluxstudio.art
Description: Professional audio plugins (VST3)
 AudioForge is a collection of 10 professional-quality audio plugins
 for music production, including gain control, EQ, compression, delay,
 reverb, saturation, and synthesis tools.
 .
 All plugins are free and open source (GPL-3.0).
EOF

# Create post-installation script
cat > $DEB_DIR/DEBIAN/postinst << 'EOF'
#!/bin/bash
echo "AudioForge plugins installed to /usr/lib/vst3/"
echo "Please rescan plugins in your DAW."
exit 0
EOF
chmod 755 $DEB_DIR/DEBIAN/postinst

# Build .deb
dpkg-deb --build --root-owner-group $DEB_DIR

# Move to output directory
mkdir -p ../../releases/installers
mv audioforge-1.1.0.deb ../../releases/installers/audioforge-1.1.0-amd64.deb

# Cleanup
rm -rf $DEB_DIR
```

**Verify package**:
```bash
dpkg -I ../../releases/installers/audioforge-1.1.0-amd64.deb
dpkg -c ../../releases/installers/audioforge-1.1.0-amd64.deb
```

---

### Build .rpm Package (Fedora/RHEL)

**Install rpmbuild** (if not already installed):

```bash
# Fedora
sudo dnf install rpm-build

# Ubuntu/Debian
sudo apt install rpm
```

**Build RPM**:

```bash
cd installers/linux

# Set up RPM build environment
RPM_ROOT="$HOME/rpmbuild"
mkdir -p $RPM_ROOT/{BUILD,RPMS,SOURCES,SPECS,SRPMS}

# Create source tarball
TARBALL_DIR="audioforge-1.1.0"
mkdir -p $TARBALL_DIR/usr/lib64/vst3
mkdir -p $TARBALL_DIR/usr/share/doc/audioforge

cp -r ../../releases/staging/AudioForge\ Plugins/VST3/*.vst3 $TARBALL_DIR/usr/lib64/vst3/
cp ../../releases/staging/README.txt $TARBALL_DIR/usr/share/doc/audioforge/

tar czf $RPM_ROOT/SOURCES/audioforge-1.1.0.tar.gz $TARBALL_DIR
rm -rf $TARBALL_DIR

# Use the existing spec file
cp audioforge.spec $RPM_ROOT/SPECS/

# Build RPM
rpmbuild -ba $RPM_ROOT/SPECS/audioforge.spec

# Copy to output directory
mkdir -p ../../releases/installers
cp $RPM_ROOT/RPMS/x86_64/audioforge-1.1.0-1.*.rpm ../../releases/installers/
```

**Verify package**:
```bash
rpm -qpi ../../releases/installers/audioforge-1.1.0-1.*.rpm
rpm -qpl ../../releases/installers/audioforge-1.1.0-1.*.rpm
```

---

### Build AppImage (Universal Linux)

**Install appimagetool**:

```bash
wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage
chmod +x appimagetool-x86_64.AppImage
sudo mv appimagetool-x86_64.AppImage /usr/local/bin/appimagetool
```

**Build AppImage**:

```bash
cd installers/linux

# Create AppDir structure
APPDIR="AudioForge.AppDir"
mkdir -p $APPDIR/usr/lib/vst3
mkdir -p $APPDIR/usr/share/applications
mkdir -p $APPDIR/usr/share/icons/hicolor/256x256/apps

# Copy plugins
cp -r ../../releases/staging/AudioForge\ Plugins/VST3/*.vst3 $APPDIR/usr/lib/vst3/

# Create desktop entry
cat > $APPDIR/audioforge.desktop << EOF
[Desktop Entry]
Type=Application
Name=AudioForge Plugins
Comment=Professional audio plugins
Icon=audioforge
Exec=echo "AudioForge plugins installed. Please rescan in your DAW."
Categories=Audio;AudioVideo;
Terminal=true
EOF

# Create AppRun script (installer)
cat > $APPDIR/AppRun << 'EOF'
#!/bin/bash
# AudioForge AppImage installer
VST3_DIR="$HOME/.vst3"
mkdir -p "$VST3_DIR"

echo "Installing AudioForge plugins to $VST3_DIR..."
cp -r "$APPDIR/usr/lib/vst3/"*.vst3 "$VST3_DIR/"

echo ""
echo "✓ AudioForge plugins installed successfully!"
echo "Plugins installed to: $VST3_DIR"
echo ""
echo "Please rescan plugins in your DAW."
EOF
chmod +x $APPDIR/AppRun

# TODO: Add actual AudioForge logo
# For now, create a placeholder
echo "Replace with actual logo" > $APPDIR/audioforge.png

# Build AppImage
mkdir -p ../../releases/installers
appimagetool $APPDIR ../../releases/installers/AudioForge-1.1.0-x86_64.AppImage

# Cleanup
rm -rf $APPDIR
```

**Make AppImage executable**:
```bash
chmod +x ../../releases/installers/AudioForge-1.1.0-x86_64.AppImage
```

---

## Testing Packages

### Test .deb on Ubuntu/Debian

```bash
# Install
sudo dpkg -i ../../releases/installers/audioforge-1.1.0-amd64.deb

# Verify installation
ls -l /usr/lib/vst3/*.vst3 | wc -l
# Expected: 10

# Test in a DAW
reaper  # or bitwig-studio, ardour, etc.
# Rescan plugins, load a few to verify they work

# Uninstall
sudo apt remove audioforge
```

### Test .rpm on Fedora/RHEL

```bash
# Install
sudo rpm -ivh ../../releases/installers/audioforge-1.1.0-1.*.rpm

# Verify installation
ls -l /usr/lib64/vst3/*.vst3 | wc -l
# Expected: 10

# Test in a DAW
reaper  # or ardour, qtractor, etc.

# Uninstall
sudo rpm -e audioforge
```

### Test AppImage

```bash
# Run the AppImage (it's an installer)
../../releases/installers/AudioForge-1.1.0-x86_64.AppImage

# Verify installation
ls -l ~/.vst3/*.vst3 | wc -l
# Expected: 10

# Test in a DAW
reaper

# Uninstall (manual)
rm -rf ~/.vst3/*.vst3
```

---

## Testing Matrix

Test on multiple distributions to ensure compatibility:

### Ubuntu/Debian
- ✅ Ubuntu 22.04 LTS (Jammy)
- ✅ Ubuntu 24.04 LTS (Noble)
- ✅ Debian 12 (Bookworm)

### Fedora/RHEL
- ✅ Fedora 39
- ✅ Fedora 40
- ✅ Rocky Linux 9

### Universal (AppImage)
- ✅ Any Linux distro with glibc 2.31+ (2020 or later)

---

## Distribution

### Upload to GitHub Release

1. Go to https://github.com/kentin0-fiz0l/AudioForge/releases/tag/v1.1.0
2. Click "Edit release"
3. Upload packages:
   - `audioforge-1.1.0-amd64.deb`
   - `audioforge-1.1.0-1.fc40.x86_64.rpm` (or similar)
   - `AudioForge-1.1.0-x86_64.AppImage`
4. Update release notes

### Direct Download Links

```markdown
**Linux**:
- [.deb (Debian/Ubuntu)](https://github.com/kentin0-fiz0l/AudioForge/releases/download/v1.1.0/audioforge-1.1.0-amd64.deb)
- [.rpm (Fedora/RHEL)](https://github.com/kentin0-fiz0l/AudioForge/releases/download/v1.1.0/audioforge-1.1.0-1.fc40.x86_64.rpm)
- [AppImage (Universal)](https://github.com/kentin0-fiz0l/AudioForge/releases/download/v1.1.0/AudioForge-1.1.0-x86_64.AppImage)
```

### Installation Instructions (for users)

**Debian/Ubuntu**:
```bash
wget https://github.com/kentin0-fiz0l/AudioForge/releases/download/v1.1.0/audioforge-1.1.0-amd64.deb
sudo dpkg -i audioforge-1.1.0-amd64.deb
```

**Fedora/RHEL**:
```bash
wget https://github.com/kentin0-fiz0l/AudioForge/releases/download/v1.1.0/audioforge-1.1.0-1.fc40.x86_64.rpm
sudo rpm -ivh audioforge-1.1.0-1.fc40.x86_64.rpm
```

**AppImage (Any Linux)**:
```bash
wget https://github.com/kentin0-fiz0l/AudioForge/releases/download/v1.1.0/AudioForge-1.1.0-x86_64.AppImage
chmod +x AudioForge-1.1.0-x86_64.AppImage
./AudioForge-1.1.0-x86_64.AppImage
```

---

## Troubleshooting

### dpkg-deb: command not found

**Solution**:
```bash
sudo apt install dpkg-dev  # Ubuntu/Debian
sudo dnf install dpkg      # Fedora
```

### rpmbuild: command not found

**Solution**:
```bash
sudo dnf install rpm-build  # Fedora
sudo apt install rpm        # Ubuntu/Debian
```

### Plugins not loading in DAW

**Check plugin path**:
```bash
# Debian/Ubuntu
ls -l /usr/lib/vst3/

# Fedora/RHEL
ls -l /usr/lib64/vst3/

# AppImage
ls -l ~/.vst3/
```

**Verify DAW is scanning the correct path**:
- Reaper: Options → Preferences → VST
- Bitwig: Settings → Locations → VST3
- Ardour: Edit → Preferences → Plugins

### Dependencies missing

AudioForge plugins have minimal dependencies, but if you encounter issues:

```bash
# Ubuntu/Debian
sudo apt install libc6 libgcc-s1 libstdc++6

# Fedora
sudo dnf install glibc gcc-c++
```

---

## Notes

- All packages install to standard VST3 directories
- No root required for AppImage (installs to `~/.vst3/`)
- Package size: ~50MB (all 10 plugins)
- Build time: ~5 minutes per package type
- Linux plugins are currently macOS-built; for native Linux, build from source on Linux

---

**Last Updated**: 2026-08-27
