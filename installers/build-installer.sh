#!/bin/bash
# AudioForge Installer Build Script for macOS
# Builds all plugins and prepares staging directory for DMG creation

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Version
VERSION="${1:-1.4.0}"

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
STAGING_DIR="$PROJECT_ROOT/releases/staging"
PLUGINS_DIR="$PROJECT_ROOT/plugins"

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}AudioForge Installer Builder${NC}"
echo -e "${BLUE}Version: $VERSION${NC}"
echo -e "${BLUE}======================================${NC}"
echo ""

# Plugin lists
EFFECTS=(
    "SimpleGain"
    "SimpleEQ"
    "SimpleComp"
    "Saturation"
    "Reverb"
    "CleanDelay"
    "ChorusFlanger"
    "PanUtil"
    "WaveShaper"
    "FreezeFX"
    "GranularEngine"
)

ADVANCED_EFFECTS=(
    "SpectralFreeze"
)

INSTRUMENTS=(
    "BasicSynth"
    "FMSynth"
    "WavetableSynth"
    "PadSynth"
    "DrumSynth"
    "OrganEmulator"
)

ALL_PLUGINS=("${EFFECTS[@]}" "${ADVANCED_EFFECTS[@]}" "${INSTRUMENTS[@]}")

echo -e "${YELLOW}Plugins to build: ${#ALL_PLUGINS[@]}${NC}"
echo -e "${YELLOW}  - Effects: ${#EFFECTS[@]}${NC}"
echo -e "${YELLOW}  - Advanced Effects: ${#ADVANCED_EFFECTS[@]}${NC}"
echo -e "${YELLOW}  - Instruments: ${#INSTRUMENTS[@]}${NC}"
echo ""

# Clean and create staging directory
echo -e "${BLUE}→ Preparing staging directory...${NC}"
rm -rf "$STAGING_DIR"
mkdir -p "$STAGING_DIR/AudioForge Plugins/VST3"
mkdir -p "$STAGING_DIR/AudioForge Plugins/AU"
mkdir -p "$STAGING_DIR/Documentation"

echo -e "${GREEN}✓ Staging directory created${NC}"
echo ""

# Build and copy plugins
BUILT_COUNT=0
FAILED_PLUGINS=()

for plugin in "${ALL_PLUGINS[@]}"; do
    echo -e "${BLUE}→ Building $plugin...${NC}"

    PLUGIN_DIR="$PLUGINS_DIR/$plugin"

    if [ ! -d "$PLUGIN_DIR" ]; then
        echo -e "${RED}✗ Plugin directory not found: $plugin${NC}"
        FAILED_PLUGINS+=("$plugin")
        continue
    fi

    cd "$PLUGIN_DIR"

    # Configure if build directory doesn't exist
    if [ ! -d "build" ]; then
        echo -e "${YELLOW}  Configuring...${NC}"
        cmake -B build -DCMAKE_BUILD_TYPE=Release > /dev/null 2>&1
    fi

    # Build
    echo -e "${YELLOW}  Compiling...${NC}"
    if cmake --build build --config Release > /dev/null 2>&1; then
        echo -e "${GREEN}  ✓ Build successful${NC}"

        # Copy VST3
        if [ -d "build/${plugin}_artefacts/Release/VST3/${plugin}.vst3" ]; then
            cp -R "build/${plugin}_artefacts/Release/VST3/${plugin}.vst3" \
                "$STAGING_DIR/AudioForge Plugins/VST3/"
            echo -e "${GREEN}  ✓ VST3 copied${NC}"
        else
            echo -e "${YELLOW}  ⚠ VST3 not found${NC}"
        fi

        # Copy AU
        if [ -d "build/${plugin}_artefacts/Release/AU/${plugin}.component" ]; then
            cp -R "build/${plugin}_artefacts/Release/AU/${plugin}.component" \
                "$STAGING_DIR/AudioForge Plugins/AU/"
            echo -e "${GREEN}  ✓ AU copied${NC}"
        else
            echo -e "${YELLOW}  ⚠ AU not found${NC}"
        fi

        ((BUILT_COUNT++))
    else
        echo -e "${RED}  ✗ Build failed${NC}"
        FAILED_PLUGINS+=("$plugin")
    fi

    echo ""
done

cd "$PROJECT_ROOT"

# Create README
echo -e "${BLUE}→ Creating documentation...${NC}"

cat > "$STAGING_DIR/README.txt" << 'EOF'
AudioForge v1.4.0
=================

Thank you for choosing AudioForge!

CONTENTS
--------
This package includes 18 professional audio plugins:

EFFECTS (11):
• SimpleGain - Clean gain/trim utility
• SimpleEQ - 3-band parametric equalizer
• SimpleComp - Transparent compressor
• Saturation - Harmonic saturation/distortion
• Reverb - Algorithmic reverb
• CleanDelay - Stereo delay with feedback
• ChorusFlanger - Modulation effects
• PanUtil - Stereo width and panning
• WaveShaper - Waveshaping distortion
• FreezeFX - Audio freeze effect
• GranularEngine - Granular synthesis

ADVANCED EFFECTS (2):
• SpectralFreeze - Spectral freezing and processing
• GranularEngine - Advanced granular processing

INSTRUMENTS (6):
• BasicSynth - 8-voice subtractive synthesizer
• FM Synth - 2-operator FM synthesis
• Wavetable Synth - Digital wavetable synthesis
• Pad Synth - Lush unison pad synthesizer
• Drum Synth - 3-drum percussion synthesizer
• Organ Emulator - Hammond-style organ with Leslie

INSTALLATION
------------
Run the "Install AudioForge Plugins" script to install all plugins.

Alternatively, manually copy:
- VST3 plugins to: ~/Library/Audio/Plug-Ins/VST3/
- AU plugins to: ~/Library/Audio/Plug-Ins/Components/

After installation, rescan plugins in your DAW.

SYSTEM REQUIREMENTS
-------------------
• macOS 10.13 or later
• Intel or Apple Silicon (universal binaries)
• Compatible DAW (Logic Pro, Ableton Live, Reaper, etc.)

SUPPORT
-------
Issues: https://github.com/your-username/audioforge/issues
Documentation: https://github.com/your-username/audioforge

LICENSE
-------
AudioForge is open-source software.
See LICENSE file for details.

Enjoy making music!
EOF

echo -e "${GREEN}✓ README created${NC}"

# Create installation script
echo -e "${BLUE}→ Creating installation script...${NC}"

cat > "$STAGING_DIR/Install AudioForge Plugins.command" << 'EOF'
#!/bin/bash
# AudioForge Plugin Installer
# Installs VST3 and AU plugins to system directories

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo ""
echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}AudioForge Plugin Installer${NC}"
echo -e "${BLUE}======================================${NC}"
echo ""

# Get script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Plugin directories
VST3_SOURCE="$SCRIPT_DIR/AudioForge Plugins/VST3"
AU_SOURCE="$SCRIPT_DIR/AudioForge Plugins/AU"

VST3_DEST="$HOME/Library/Audio/Plug-Ins/VST3"
AU_DEST="$HOME/Library/Audio/Plug-Ins/Components"

# Create destination directories if they don't exist
mkdir -p "$VST3_DEST"
mkdir -p "$AU_DEST"

# Install VST3 plugins
echo -e "${BLUE}→ Installing VST3 plugins...${NC}"
VST3_COUNT=0
if [ -d "$VST3_SOURCE" ]; then
    for plugin in "$VST3_SOURCE"/*.vst3; do
        if [ -e "$plugin" ]; then
            PLUGIN_NAME=$(basename "$plugin")
            echo -e "${YELLOW}  Installing: $PLUGIN_NAME${NC}"

            # Remove existing if present
            if [ -d "$VST3_DEST/$PLUGIN_NAME" ]; then
                rm -rf "$VST3_DEST/$PLUGIN_NAME"
            fi

            # Copy plugin
            cp -R "$plugin" "$VST3_DEST/"
            ((VST3_COUNT++))
            echo -e "${GREEN}  ✓ Installed${NC}"
        fi
    done
fi
echo -e "${GREEN}✓ Installed $VST3_COUNT VST3 plugins${NC}"
echo ""

# Install AU plugins
echo -e "${BLUE}→ Installing AU plugins...${NC}"
AU_COUNT=0
if [ -d "$AU_SOURCE" ]; then
    for plugin in "$AU_SOURCE"/*.component; do
        if [ -e "$plugin" ]; then
            PLUGIN_NAME=$(basename "$plugin")
            echo -e "${YELLOW}  Installing: $PLUGIN_NAME${NC}"

            # Remove existing if present
            if [ -d "$AU_DEST/$PLUGIN_NAME" ]; then
                rm -rf "$AU_DEST/$PLUGIN_NAME"
            fi

            # Copy plugin
            cp -R "$plugin" "$AU_DEST/"
            ((AU_COUNT++))
            echo -e "${GREEN}  ✓ Installed${NC}"
        fi
    done
fi
echo -e "${GREEN}✓ Installed $AU_COUNT AU plugins${NC}"
echo ""

echo -e "${GREEN}=====================================${NC}"
echo -e "${GREEN}Installation Complete!${NC}"
echo -e "${GREEN}=====================================${NC}"
echo ""
echo "Installed:"
echo "  - $VST3_COUNT VST3 plugins to: $VST3_DEST"
echo "  - $AU_COUNT AU plugins to: $AU_DEST"
echo ""
echo "Next steps:"
echo "1. Open your DAW"
echo "2. Rescan plugins"
echo "3. Look for 'AudioForge' in your plugin browser"
echo ""
echo "Press any key to close..."
read -n 1 -s
EOF

chmod +x "$STAGING_DIR/Install AudioForge Plugins.command"
echo -e "${GREEN}✓ Installation script created${NC}"

# Create uninstall script
echo -e "${BLUE}→ Creating uninstallation script...${NC}"

cat > "$STAGING_DIR/Uninstall AudioForge Plugins.command" << 'EOF'
#!/bin/bash
# AudioForge Plugin Uninstaller

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo ""
echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}AudioForge Plugin Uninstaller${NC}"
echo -e "${BLUE}======================================${NC}"
echo ""

VST3_DIR="$HOME/Library/Audio/Plug-Ins/VST3"
AU_DIR="$HOME/Library/Audio/Plug-Ins/Components"

# List of all AudioForge plugins
PLUGINS=(
    "SimpleGain" "SimpleEQ" "SimpleComp" "Saturation" "Reverb"
    "CleanDelay" "ChorusFlanger" "PanUtil" "WaveShaper" "FreezeFX"
    "GranularEngine" "SpectralFreeze" "BasicSynth" "FMSynth"
    "WavetableSynth" "PadSynth" "DrumSynth" "OrganEmulator"
)

echo -e "${YELLOW}This will remove all AudioForge plugins from your system.${NC}"
echo ""
read -p "Continue? (y/n) " -n 1 -r
echo ""

if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "Cancelled."
    exit 0
fi

echo ""
REMOVED_COUNT=0

for plugin in "${PLUGINS[@]}"; do
    # Remove VST3
    if [ -d "$VST3_DIR/${plugin}.vst3" ]; then
        echo -e "${YELLOW}Removing: ${plugin}.vst3${NC}"
        rm -rf "$VST3_DIR/${plugin}.vst3"
        ((REMOVED_COUNT++))
    fi

    # Remove AU
    if [ -d "$AU_DIR/${plugin}.component" ]; then
        echo -e "${YELLOW}Removing: ${plugin}.component${NC}"
        rm -rf "$AU_DIR/${plugin}.component"
        ((REMOVED_COUNT++))
    fi
done

echo ""
echo -e "${GREEN}=====================================${NC}"
echo -e "${GREEN}Uninstallation Complete!${NC}"
echo -e "${GREEN}=====================================${NC}"
echo ""
echo "Removed $REMOVED_COUNT plugin files"
echo ""
echo "Press any key to close..."
read -n 1 -s
EOF

chmod +x "$STAGING_DIR/Uninstall AudioForge Plugins.command"
echo -e "${GREEN}✓ Uninstallation script created${NC}"
echo ""

# Summary
echo -e "${GREEN}=====================================${NC}"
echo -e "${GREEN}Build Complete!${NC}"
echo -e "${GREEN}=====================================${NC}"
echo ""
echo "Built: $BUILT_COUNT/${#ALL_PLUGINS[@]} plugins"

if [ ${#FAILED_PLUGINS[@]} -gt 0 ]; then
    echo -e "${RED}Failed plugins:${NC}"
    for plugin in "${FAILED_PLUGINS[@]}"; do
        echo "  - $plugin"
    done
    echo ""
fi

echo "Staging directory: $STAGING_DIR"
echo ""
echo "Next steps:"
echo "1. Test installation script: \"$STAGING_DIR/Install AudioForge Plugins.command\""
echo "2. Create DMG: ./installers/create-dmg.sh $VERSION"
echo ""

# Count plugins in staging
VST3_COUNT=$(find "$STAGING_DIR/AudioForge Plugins/VST3" -name "*.vst3" 2>/dev/null | wc -l | tr -d ' ')
AU_COUNT=$(find "$STAGING_DIR/AudioForge Plugins/AU" -name "*.component" 2>/dev/null | wc -l | tr -d ' ')

echo "Plugins in staging:"
echo "  - VST3: $VST3_COUNT"
echo "  - AU: $AU_COUNT"
echo ""
