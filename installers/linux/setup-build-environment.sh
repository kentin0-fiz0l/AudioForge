#!/bin/bash

# AudioForge Linux Build Environment Setup Script
# Run this script on Ubuntu/Debian-based Linux VM

set -e

echo "======================================"
echo "AudioForge Linux Build Setup"
echo "======================================"

# Check if running as root (for apt)
if [ "$EUID" -ne 0 ]; then
    echo "Please run as root (sudo ./setup-build-environment.sh)"
    exit 1
fi

# Detect Linux distribution
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$ID
else
    echo "Cannot detect Linux distribution"
    exit 1
fi

echo ""
echo "Detected OS: $OS"
echo ""

# Update package lists
echo "[1/6] Updating package lists..."
if [ "$OS" = "ubuntu" ] || [ "$OS" = "debian" ]; then
    apt update
elif [ "$OS" = "fedora" ] || [ "$OS" = "rhel" ] || [ "$OS" = "centos" ]; then
    dnf check-update || true
fi

# Install build essentials
echo "[2/6] Installing build essentials..."
if [ "$OS" = "ubuntu" ] || [ "$OS" = "debian" ]; then
    apt install -y build-essential git cmake pkg-config
elif [ "$OS" = "fedora" ] || [ "$OS" = "rhel" ] || [ "$OS" = "centos" ]; then
    dnf install -y gcc gcc-c++ git cmake pkgconfig
fi

# Install JUCE dependencies
echo "[3/6] Installing JUCE dependencies..."
if [ "$OS" = "ubuntu" ] || [ "$OS" = "debian" ]; then
    apt install -y \
        libasound2-dev \
        libjack-jackd2-dev \
        ladspa-sdk \
        libcurl4-openssl-dev \
        libfreetype6-dev \
        libx11-dev \
        libxcomposite-dev \
        libxcursor-dev \
        libxcursor-dev \
        libxext-dev \
        libxinerama-dev \
        libxrandr-dev \
        libxrender-dev \
        libwebkit2gtk-4.0-dev \
        libglu1-mesa-dev \
        mesa-common-dev
elif [ "$OS" = "fedora" ] || [ "$OS" = "rhel" ] || [ "$OS" = "centos" ]; then
    dnf install -y \
        alsa-lib-devel \
        jack-audio-connection-kit-devel \
        ladspa-devel \
        libcurl-devel \
        freetype-devel \
        libX11-devel \
        libXcomposite-devel \
        libXcursor-devel \
        libXext-devel \
        libXinerama-devel \
        libXrandr-devel \
        libXrender-devel \
        webkit2gtk3-devel \
        mesa-libGLU-devel
fi

# Install packaging tools
echo "[4/6] Installing packaging tools..."
if [ "$OS" = "ubuntu" ] || [ "$OS" = "debian" ]; then
    apt install -y dpkg-dev fakeroot rpm
elif [ "$OS" = "fedora" ] || [ "$OS" = "rhel" ] || [ "$OS" = "centos" ]; then
    dnf install -y rpm-build rpmdevtools dpkg
fi

# Install AppImage tools
echo "[5/6] Installing AppImage tools..."
if [ "$OS" = "ubuntu" ] || [ "$OS" = "debian" ]; then
    apt install -y wget fuse libfuse2
elif [ "$OS" = "fedora" ] || [ "$OS" = "rhel" ] || [ "$OS" = "centos" ]; then
    dnf install -y wget fuse fuse-libs
fi

# Download appimagetool if not present
if [ ! -f /usr/local/bin/appimagetool ]; then
    echo "Downloading appimagetool..."
    wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage \
        -O /usr/local/bin/appimagetool
    chmod +x /usr/local/bin/appimagetool
fi

echo "[6/6] Verifying installation..."
echo ""

echo "======================================"
echo "Setup Complete!"
echo "======================================"

echo ""
echo "Installed tools:"
echo "- GCC: $(gcc --version | head -n1)"
echo "- G++: $(g++ --version | head -n1)"
echo "- CMake: $(cmake --version | head -n1)"
echo "- Git: $(git --version)"
echo ""

echo "Next steps:"
echo "1. Clone AudioForge repository:"
echo "   git clone https://github.com/kentin0-fiz0l/AudioForge.git"
echo "2. Build packages:"
echo "   cd AudioForge/installers/linux"
echo "   sudo ./build-packages.sh"
