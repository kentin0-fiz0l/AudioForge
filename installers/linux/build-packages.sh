#!/bin/bash
set -e

# AudioForge Linux Package Builder
# Builds .deb (Debian/Ubuntu), .rpm (Fedora/RHEL), and AppImage packages

VERSION="1.1.0"
ARCH="amd64"
PROJECT_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
STAGING_DIR="$PROJECT_ROOT/releases/staging"
OUTPUT_DIR="$PROJECT_ROOT/releases/installers"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}AudioForge Linux Package Builder${NC}"
echo -e "${BLUE}Version: $VERSION${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Check if staging directory exists
if [ ! -d "$STAGING_DIR/AudioForge Plugins/VST3" ]; then
    echo -e "${RED}Error: Staging directory not found${NC}"
    echo "Run build-installer.sh first to build plugins"
    exit 1
fi

mkdir -p "$OUTPUT_DIR"

# =============================================================================
# Build .deb package (Debian/Ubuntu)
# =============================================================================

build_deb() {
    echo -e "${BLUE}Building .deb package...${NC}"

    DEB_DIR="audioforge-${VERSION}"
    rm -rf "$DEB_DIR"
    mkdir -p "$DEB_DIR/DEBIAN"
    mkdir -p "$DEB_DIR/usr/lib/vst3"
    mkdir -p "$DEB_DIR/usr/share/doc/audioforge"

    # Copy VST3 plugins
    cp -r "$STAGING_DIR/AudioForge Plugins/VST3/"*.vst3 "$DEB_DIR/usr/lib/vst3/"
    echo -e "${GREEN}✓ Copied VST3 plugins${NC}"

    # Copy README
    cp "$STAGING_DIR/README.txt" "$DEB_DIR/usr/share/doc/audioforge/"

    # Create control file
    cat > "$DEB_DIR/DEBIAN/control" << EOF
Package: audioforge
Version: ${VERSION}
Section: sound
Priority: optional
Architecture: ${ARCH}
Maintainer: AudioForge <contact@audioforge.art>
Homepage: https://audioforge.fluxstudio.art
Description: Professional audio plugins (VST3)
 AudioForge is a collection of 10 professional-quality audio plugins
 for music production, including:
 - SimpleGain: Gain/volume control
 - PanUtil: Stereo panning and width
 - BasicSynth: Subtractive synthesizer
 - CleanDelay: Stereo delay effect
 - SimpleEQ: 3-band parametric EQ
 - SimpleComp: Compressor
 - WaveShaper: Distortion effect
 - Saturation: Analog-style saturation
 - ChorusFlanger: Modulation effects
 - Reverb: Algorithmic reverb
 .
 All plugins are free and open source (GPL-3.0).
EOF

    # Create postinst script (run after installation)
    cat > "$DEB_DIR/DEBIAN/postinst" << 'EOF'
#!/bin/bash
echo "AudioForge plugins installed to /usr/lib/vst3/"
echo "Please rescan plugins in your DAW."
exit 0
EOF
    chmod 755 "$DEB_DIR/DEBIAN/postinst"

    # Build .deb
    dpkg-deb --build --root-owner-group "$DEB_DIR"
    mv "${DEB_DIR}.deb" "$OUTPUT_DIR/audioforge-${VERSION}-${ARCH}.deb"

    # Cleanup
    rm -rf "$DEB_DIR"

    echo -e "${GREEN}✓ .deb package created: audioforge-${VERSION}-${ARCH}.deb${NC}"
}

# =============================================================================
# Build .rpm package (Fedora/RHEL)
# =============================================================================

build_rpm() {
    echo ""
    echo -e "${BLUE}Building .rpm package...${NC}"

    RPM_ROOT="$HOME/rpmbuild"
    mkdir -p "$RPM_ROOT"/{BUILD,RPMS,SOURCES,SPECS,SRPMS}

    # Create tarball for RPM
    TARBALL_DIR="audioforge-${VERSION}"
    rm -rf "$TARBALL_DIR"
    mkdir -p "$TARBALL_DIR/usr/lib64/vst3"
    mkdir -p "$TARBALL_DIR/usr/share/doc/audioforge"

    # Copy VST3 plugins
    cp -r "$STAGING_DIR/AudioForge Plugins/VST3/"*.vst3 "$TARBALL_DIR/usr/lib64/vst3/"
    cp "$STAGING_DIR/README.txt" "$TARBALL_DIR/usr/share/doc/audioforge/"

    # Create tarball
    tar czf "$RPM_ROOT/SOURCES/audioforge-${VERSION}.tar.gz" "$TARBALL_DIR"
    rm -rf "$TARBALL_DIR"
    echo -e "${GREEN}✓ Created source tarball${NC}"

    # Create .spec file
    cat > "$RPM_ROOT/SPECS/audioforge.spec" << EOF
Name:           audioforge
Version:        ${VERSION}
Release:        1%{?dist}
Summary:        Professional audio plugins (VST3)

License:        GPL-3.0
URL:            https://audioforge.fluxstudio.art
Source0:        %{name}-%{version}.tar.gz

BuildArch:      x86_64
Requires:       glibc

%description
AudioForge is a collection of 10 professional-quality audio plugins
for music production, including gain control, EQ, compression, delay,
reverb, saturation, and synthesis tools.

All plugins are free and open source (GPL-3.0).

%prep
%setup -q

%build
# No build needed - precompiled binaries

%install
mkdir -p %{buildroot}/usr/lib64/vst3
mkdir -p %{buildroot}/usr/share/doc/%{name}
cp -r usr/lib64/vst3/* %{buildroot}/usr/lib64/vst3/
cp usr/share/doc/audioforge/* %{buildroot}/usr/share/doc/%{name}/

%files
/usr/lib64/vst3/*.vst3
/usr/share/doc/%{name}/README.txt

%post
echo "AudioForge plugins installed to /usr/lib64/vst3/"
echo "Please rescan plugins in your DAW."

%changelog
* $(date "+%a %b %d %Y") AudioForge <contact@audioforge.art> - ${VERSION}-1
- Initial release of AudioForge v1.1.0
- 10 professional audio plugins
EOF

    # Build RPM
    rpmbuild -ba "$RPM_ROOT/SPECS/audioforge.spec"

    # Copy to output directory
    cp "$RPM_ROOT/RPMS/x86_64/audioforge-${VERSION}-1."*.rpm "$OUTPUT_DIR/" 2>/dev/null || {
        echo -e "${YELLOW}⚠ RPM build failed - rpmbuild may not be installed${NC}"
        echo "  Install rpmbuild: sudo apt install rpm (Debian) or sudo dnf install rpm-build (Fedora)"
        return 1
    }

    echo -e "${GREEN}✓ .rpm package created${NC}"
}

# =============================================================================
# Build AppImage (Universal Linux)
# =============================================================================

build_appimage() {
    echo ""
    echo -e "${BLUE}Building AppImage...${NC}"

    APPDIR="AudioForge.AppDir"
    rm -rf "$APPDIR"
    mkdir -p "$APPDIR/usr/lib/vst3"
    mkdir -p "$APPDIR/usr/share/applications"
    mkdir -p "$APPDIR/usr/share/icons/hicolor/256x256/apps"

    # Copy VST3 plugins
    cp -r "$STAGING_DIR/AudioForge Plugins/VST3/"*.vst3 "$APPDIR/usr/lib/vst3/"
    echo -e "${GREEN}✓ Copied VST3 plugins${NC}"

    # Create desktop entry
    cat > "$APPDIR/audioforge.desktop" << EOF
[Desktop Entry]
Type=Application
Name=AudioForge Plugins
Comment=Professional audio plugins
Icon=audioforge
Exec=echo "AudioForge plugins installed. Please rescan in your DAW."
Categories=Audio;AudioVideo;
Terminal=true
EOF

    # Create AppRun script
    cat > "$APPDIR/AppRun" << 'EOF'
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
    chmod +x "$APPDIR/AppRun"

    # Create placeholder icon (32x32 PNG in base64)
    # This is a minimal placeholder - replace with actual logo later
    echo "iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAAAXNSR0IArs4c6QAA" | base64 -d > "$APPDIR/audioforge.png" 2>/dev/null || true

    # Check if appimagetool is available
    if command -v appimagetool &> /dev/null; then
        appimagetool "$APPDIR" "$OUTPUT_DIR/AudioForge-${VERSION}-x86_64.AppImage"
        echo -e "${GREEN}✓ AppImage created: AudioForge-${VERSION}-x86_64.AppImage${NC}"
    else
        echo -e "${YELLOW}⚠ appimagetool not found - skipping AppImage build${NC}"
        echo "  Download from: https://appimage.github.io/appimagetool/"
        echo "  Or run: wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage"
        echo "         chmod +x appimagetool-x86_64.AppImage"
        echo "         sudo mv appimagetool-x86_64.AppImage /usr/local/bin/appimagetool"
        rm -rf "$APPDIR"
        return 1
    fi

    rm -rf "$APPDIR"
}

# =============================================================================
# Main
# =============================================================================

# Build all package types
build_deb

if command -v rpmbuild &> /dev/null; then
    build_rpm
else
    echo ""
    echo -e "${YELLOW}⚠ rpmbuild not found - skipping .rpm package${NC}"
fi

if command -v appimagetool &> /dev/null; then
    build_appimage
else
    echo ""
    echo -e "${YELLOW}⚠ appimagetool not found - skipping AppImage${NC}"
fi

echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Linux packages built successfully!${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "Output directory: $OUTPUT_DIR"
echo ""
ls -lh "$OUTPUT_DIR"/*.deb "$OUTPUT_DIR"/*.rpm "$OUTPUT_DIR"/*.AppImage 2>/dev/null || true
