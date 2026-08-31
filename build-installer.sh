#!/bin/bash

# AudioForge Installer Build Script
# Builds all plugins and creates distribution packages
# Version: 1.1.1

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Version
VERSION="1.3.0"
PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"

# Plugin list
PLUGINS=(
    "SimpleGain"
    "PanUtil"
    "BasicSynth"
    "CleanDelay"
    "SimpleEQ"
    "SimpleComp"
    "WaveShaper"
    "Saturation"
    "ChorusFlanger"
    "Reverb"
    "FreezeFX"
    "GranularEngine"
    "SpectralFreeze"
)

# Paths
STAGING_DIR="$PROJECT_ROOT/releases/staging"
INSTALLER_DIR="$PROJECT_ROOT/releases/installers"
PLUGINS_DIR="$STAGING_DIR/AudioForge Plugins"
VST3_DIR="$PLUGINS_DIR/VST3"
AU_DIR="$PLUGINS_DIR/AU"

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}AudioForge Installer Build Script${NC}"
echo -e "${BLUE}Version: $VERSION${NC}"
echo -e "${BLUE}======================================${NC}"
echo ""

# Clean previous builds
echo -e "${YELLOW}→ Cleaning previous staging directory...${NC}"
rm -rf "$STAGING_DIR"
mkdir -p "$STAGING_DIR"
mkdir -p "$INSTALLER_DIR"
mkdir -p "$VST3_DIR"
mkdir -p "$AU_DIR"

# Build all plugins
echo ""
echo -e "${BLUE}Building all plugins in Release mode...${NC}"
echo ""

BUILD_FAILED=0

for plugin in "${PLUGINS[@]}"; do
    echo -e "${YELLOW}→ Building $plugin...${NC}"

    PLUGIN_DIR="$PROJECT_ROOT/plugins/$plugin"

    if [ ! -d "$PLUGIN_DIR" ]; then
        echo -e "${RED}✗ Plugin directory not found: $PLUGIN_DIR${NC}"
        BUILD_FAILED=1
        continue
    fi

    cd "$PLUGIN_DIR"

    # Configure with CMake
    if ! cmake -B build -DCMAKE_BUILD_TYPE=Release > /dev/null 2>&1; then
        echo -e "${RED}✗ CMake configuration failed for $plugin${NC}"
        BUILD_FAILED=1
        continue
    fi

    # Build
    if ! cmake --build build --config Release > /dev/null 2>&1; then
        echo -e "${RED}✗ Build failed for $plugin${NC}"
        BUILD_FAILED=1
        continue
    fi

    echo -e "${GREEN}✓ $plugin built successfully${NC}"
done

cd "$PROJECT_ROOT"

if [ $BUILD_FAILED -eq 1 ]; then
    echo ""
    echo -e "${RED}Some plugins failed to build. Please check the errors above.${NC}"
    exit 1
fi

# Collect VST3 plugins
echo ""
echo -e "${BLUE}Collecting VST3 plugins...${NC}"

for plugin in "${PLUGINS[@]}"; do
    VST3_SOURCE="$PROJECT_ROOT/plugins/$plugin/build/${plugin}_artefacts/Release/VST3/${plugin}.vst3"

    if [ -d "$VST3_SOURCE" ]; then
        cp -R "$VST3_SOURCE" "$VST3_DIR/"
        echo -e "${GREEN}✓ Collected ${plugin}.vst3${NC}"
    else
        echo -e "${RED}✗ VST3 not found for $plugin${NC}"
        BUILD_FAILED=1
    fi
done

# Collect AU plugins (macOS only)
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo ""
    echo -e "${BLUE}Collecting AU plugins...${NC}"

    for plugin in "${PLUGINS[@]}"; do
        AU_SOURCE="$PROJECT_ROOT/plugins/$plugin/build/${plugin}_artefacts/Release/AU/${plugin}.component"

        if [ -d "$AU_SOURCE" ]; then
            cp -R "$AU_SOURCE" "$AU_DIR/"
            echo -e "${GREEN}✓ Collected ${plugin}.component${NC}"
        else
            echo -e "${RED}✗ AU not found for $plugin${NC}"
            BUILD_FAILED=1
        fi
    done
fi

if [ $BUILD_FAILED -eq 1 ]; then
    echo ""
    echo -e "${RED}Failed to collect some plugin artifacts. Check build outputs.${NC}"
    exit 1
fi

# Code signing (macOS only)
if [[ "$OSTYPE" == "darwin"* ]] && [ -n "$CODESIGN_IDENTITY" ]; then
    echo ""
    echo -e "${BLUE}Signing macOS plugins...${NC}"
    echo -e "${YELLOW}Using identity: $CODESIGN_IDENTITY${NC}"

    # Sign VST3 plugins
    for plugin in "$VST3_DIR"/*.vst3; do
        if [ -d "$plugin" ]; then
            codesign --force --deep --sign "$CODESIGN_IDENTITY" \
                --options runtime "$plugin"
            echo -e "${GREEN}✓ Signed $(basename "$plugin")${NC}"
        fi
    done

    # Sign AU plugins
    for plugin in "$AU_DIR"/*.component; do
        if [ -d "$plugin" ]; then
            codesign --force --deep --sign "$CODESIGN_IDENTITY" \
                --options runtime "$plugin"
            echo -e "${GREEN}✓ Signed $(basename "$plugin")${NC}"
        fi
    done

    echo -e "${GREEN}✓ All plugins signed successfully${NC}"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    echo ""
    echo -e "${YELLOW}⚠ CODESIGN_IDENTITY not set - plugins will not be signed${NC}"
    echo -e "${YELLOW}  Set CODESIGN_IDENTITY environment variable to enable signing${NC}"
    echo -e "${YELLOW}  Example: export CODESIGN_IDENTITY=\"Developer ID Application: Your Name (TEAM_ID)\"${NC}"
fi

# Create README
echo ""
echo -e "${BLUE}Creating README.txt...${NC}"

cat > "$STAGING_DIR/README.txt" << 'EOF'
AudioForge v1.3.0
=================

Professional-quality DAW plugins, free and open source.

INSTALLATION (macOS)
--------------------

Automatic Installation:
1. Double-click "Install AudioForge.command"
2. Follow the prompts
3. Rescan plugins in your DAW

Manual Installation:
1. Copy VST3 plugins to: ~/Library/Audio/Plug-Ins/VST3/
2. Copy AU plugins to: ~/Library/Audio/Plug-Ins/Components/
3. Rescan plugins in your DAW

PLUGINS INCLUDED (12 Total)
----------------------------

Core Processing Suite (v1.0.0):
1. SimpleGain - Clean gain/volume control with metering
2. PanUtil - Stereo panning and width control
3. BasicSynth - Simple subtractive synthesizer
4. CleanDelay - Stereo delay with ping-pong mode
5. SimpleEQ - 3-band parametric equalizer
6. SimpleComp - Compressor with auto makeup gain
7. WaveShaper - Waveshaping distortion
8. Saturation - Multi-mode analog saturation
9. ChorusFlanger - LFO-based modulation effects
10. Reverb - Freeverb-inspired algorithmic reverb
11. FreezeFX - Spectral freezing plugin

Advanced Effects Suite (v1.2.0):
12. GranularEngine - Real-time granular synthesis
13. SpectralFreeze - Professional spectral manipulation

COMPATIBILITY
-------------

- macOS 10.15 (Catalina) or later
- VST3 compatible DAWs: Reaper, Ableton Live, FL Studio, Studio One, etc.
- AU compatible DAWs: Logic Pro, GarageBand, MainStage

UNINSTALLATION
--------------

Double-click "Uninstall AudioForge.command" to remove all plugins.

SUPPORT
-------

Website: https://audioforge.fluxstudio.art
GitHub: https://github.com/kentin0-fiz0l/AudioForge
Issues: https://github.com/kentin0-fiz0l/AudioForge/issues

LICENSE
-------

MIT License - Free for personal and commercial use.
See LICENSE.txt for full license text.
EOF

echo -e "${GREEN}✓ README.txt created${NC}"

# Copy LICENSE
echo -e "${BLUE}Copying LICENSE.txt...${NC}"

if [ -f "$PROJECT_ROOT/LICENSE" ]; then
    cp "$PROJECT_ROOT/LICENSE" "$STAGING_DIR/LICENSE.txt"
    echo -e "${GREEN}✓ LICENSE.txt copied${NC}"
else
    echo -e "${YELLOW}⚠ LICENSE file not found${NC}"
fi

# Create Install script
echo ""
echo -e "${BLUE}Creating installation scripts...${NC}"

cat > "$STAGING_DIR/Install AudioForge.command" << 'EOF'
#!/bin/bash

# AudioForge v1.3.0 Installation Script

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

clear

echo "======================================"
echo "AudioForge v1.3.0 Installer"
echo "======================================"
echo ""
echo "This script will install all AudioForge plugins to:"
echo "  • VST3: ~/Library/Audio/Plug-Ins/VST3/"
echo "  • AU: ~/Library/Audio/Plug-Ins/Components/"
echo ""
read -p "Press Enter to continue or Ctrl+C to cancel..."

# Get script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Target directories
VST3_TARGET="$HOME/Library/Audio/Plug-Ins/VST3"
AU_TARGET="$HOME/Library/Audio/Plug-Ins/Components"

# Create directories if they don't exist
mkdir -p "$VST3_TARGET"
mkdir -p "$AU_TARGET"

echo ""
echo -e "${BLUE}Installing VST3 plugins...${NC}"

# Install VST3 plugins
if [ -d "$SCRIPT_DIR/AudioForge Plugins/VST3" ]; then
    for plugin in "$SCRIPT_DIR/AudioForge Plugins/VST3"/*.vst3; do
        if [ -e "$plugin" ]; then
            plugin_name=$(basename "$plugin")
            cp -R "$plugin" "$VST3_TARGET/"
            echo -e "${GREEN}✓ Installed $plugin_name${NC}"
        fi
    done
else
    echo -e "${RED}✗ VST3 folder not found${NC}"
fi

echo ""
echo -e "${BLUE}Installing AU plugins...${NC}"

# Install AU plugins
if [ -d "$SCRIPT_DIR/AudioForge Plugins/AU" ]; then
    for plugin in "$SCRIPT_DIR/AudioForge Plugins/AU"/*.component; do
        if [ -e "$plugin" ]; then
            plugin_name=$(basename "$plugin")
            cp -R "$plugin" "$AU_TARGET/"
            echo -e "${GREEN}✓ Installed $plugin_name${NC}"
        fi
    done
else
    echo -e "${RED}✗ AU folder not found${NC}"
fi

echo ""
echo "======================================"
echo -e "${GREEN}Installation Complete!${NC}"
echo "======================================"
echo ""
echo "Next steps:"
echo "1. Rescan plugins in your DAW"
echo "2. AudioForge plugins should now appear"
echo ""
echo "Plugin locations:"
echo "  • VST3: $VST3_TARGET"
echo "  • AU: $AU_TARGET"
echo ""
read -p "Press Enter to exit..."
EOF

chmod +x "$STAGING_DIR/Install AudioForge.command"
echo -e "${GREEN}✓ Install AudioForge.command created${NC}"

# Create Uninstall script
cat > "$STAGING_DIR/Uninstall AudioForge.command" << 'EOF'
#!/bin/bash

# AudioForge v1.3.0 Uninstallation Script

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

clear

echo "======================================"
echo "AudioForge v1.3.0 Uninstaller"
echo "======================================"
echo ""
echo -e "${YELLOW}WARNING: This will remove all AudioForge plugins${NC}"
echo ""
read -p "Are you sure you want to continue? (yes/no): " confirm

if [ "$confirm" != "yes" ]; then
    echo "Uninstallation cancelled."
    exit 0
fi

# Target directories
VST3_TARGET="$HOME/Library/Audio/Plug-Ins/VST3"
AU_TARGET="$HOME/Library/Audio/Plug-Ins/Components"

# List of plugins
PLUGINS=(
    "SimpleGain"
    "PanUtil"
    "BasicSynth"
    "CleanDelay"
    "SimpleEQ"
    "SimpleComp"
    "WaveShaper"
    "Saturation"
    "ChorusFlanger"
    "Reverb"
    "FreezeFX"
    "GranularEngine"
    "SpectralFreeze"
)

echo ""
echo -e "${YELLOW}Removing VST3 plugins...${NC}"

for plugin in "${PLUGINS[@]}"; do
    if [ -d "$VST3_TARGET/${plugin}.vst3" ]; then
        rm -rf "$VST3_TARGET/${plugin}.vst3"
        echo -e "${GREEN}✓ Removed ${plugin}.vst3${NC}"
    fi
done

echo ""
echo -e "${YELLOW}Removing AU plugins...${NC}"

for plugin in "${PLUGINS[@]}"; do
    if [ -d "$AU_TARGET/${plugin}.component" ]; then
        rm -rf "$AU_TARGET/${plugin}.component"
        echo -e "${GREEN}✓ Removed ${plugin}.component${NC}"
    fi
done

# Clear AU cache
echo ""
echo -e "${YELLOW}Clearing AU cache...${NC}"
killall -9 AudioComponentRegistrar 2>/dev/null || true
echo -e "${GREEN}✓ AU cache cleared${NC}"

echo ""
echo "======================================"
echo -e "${GREEN}Uninstallation Complete!${NC}"
echo "======================================"
echo ""
echo "All AudioForge plugins have been removed."
echo "You may need to rescan plugins in your DAW."
echo ""
read -p "Press Enter to exit..."
EOF

chmod +x "$STAGING_DIR/Uninstall AudioForge.command"
echo -e "${GREEN}✓ Uninstall AudioForge.command created${NC}"

# Calculate sizes
echo ""
echo -e "${BLUE}Package Statistics:${NC}"
VST3_SIZE=$(du -sh "$VST3_DIR" | awk '{print $1}')
AU_SIZE=$(du -sh "$AU_DIR" | awk '{print $1}')
TOTAL_SIZE=$(du -sh "$STAGING_DIR" | awk '{print $1}')

echo "  VST3 plugins: $VST3_SIZE"
echo "  AU plugins: $AU_SIZE"
echo "  Total size: $TOTAL_SIZE"

echo ""
echo -e "${GREEN}=====================================${NC}"
echo -e "${GREEN}Build Complete!${NC}"
echo -e "${GREEN}=====================================${NC}"
echo ""
echo "Staging directory: $STAGING_DIR"
echo ""
echo "Next steps:"
echo "1. Review contents in: $STAGING_DIR"
echo "2. Test Install AudioForge.command"
echo "3. Create DMG with: ./create-dmg.sh"
echo ""
