#!/bin/bash
# Build all AudioForge plugins for v1.8.0 release
# Builds VST3, AU, and Standalone formats

set -e  # Exit on error

PLUGINS_DIR="/Users/kentino/Projects/Active/AudioForge/plugins"
BUILD_TYPE="Release"
FAILED_PLUGINS=()
BUILT_PLUGINS=()

# Color codes for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "========================================="
echo "AudioForge v1.8.0 - Building All Plugins"
echo "========================================="
echo ""

# List of all plugins
PLUGINS=(
    "BasicSynth"
    "ChorusFlanger"
    "CleanDelay"
    "DrumSynth"
    "FMSynth"
    "FreezeFX"
    "Gate"
    "GranularEngine"
    "Limiter"
    "MultibandCompressor"
    "OrganEmulator"
    "PadSynth"
    "PanUtil"
    "Reverb"
    "SamplerPlugin"
    "Saturation"
    "SimpleComp"
    "SimpleEQ"
    "SimpleGain"
    "SpectralFreeze"
    "WaveShaper"
    "WavetableSynth"
)

TOTAL=${#PLUGINS[@]}
CURRENT=0

for PLUGIN in "${PLUGINS[@]}"; do
    CURRENT=$((CURRENT + 1))
    echo -e "${YELLOW}[${CURRENT}/${TOTAL}] Building ${PLUGIN}...${NC}"

    PLUGIN_DIR="${PLUGINS_DIR}/${PLUGIN}"

    if [ ! -d "$PLUGIN_DIR" ]; then
        echo -e "${RED}  ✗ Plugin directory not found: ${PLUGIN_DIR}${NC}"
        FAILED_PLUGINS+=("$PLUGIN (not found)")
        continue
    fi

    cd "$PLUGIN_DIR"

    # Configure CMake
    if ! cmake -B build -DCMAKE_BUILD_TYPE=$BUILD_TYPE > /dev/null 2>&1; then
        echo -e "${RED}  ✗ CMake configuration failed${NC}"
        FAILED_PLUGINS+=("$PLUGIN (cmake config)")
        continue
    fi

    # Build
    if ! cmake --build build --config $BUILD_TYPE > /dev/null 2>&1; then
        echo -e "${RED}  ✗ Build failed${NC}"
        FAILED_PLUGINS+=("$PLUGIN (build)")
        continue
    fi

    echo -e "${GREEN}  ✓ Built successfully${NC}"
    BUILT_PLUGINS+=("$PLUGIN")
done

echo ""
echo "========================================="
echo "Build Summary"
echo "========================================="
echo -e "${GREEN}Successfully built: ${#BUILT_PLUGINS[@]}/${TOTAL}${NC}"
echo -e "${RED}Failed: ${#FAILED_PLUGINS[@]}/${TOTAL}${NC}"

if [ ${#FAILED_PLUGINS[@]} -gt 0 ]; then
    echo ""
    echo "Failed plugins:"
    for PLUGIN in "${FAILED_PLUGINS[@]}"; do
        echo "  - $PLUGIN"
    done
    exit 1
fi

echo ""
echo -e "${GREEN}✓ All plugins built successfully!${NC}"
echo ""
echo "Installed locations:"
echo "  VST3: ~/Library/Audio/Plug-Ins/VST3/"
echo "  AU:   ~/Library/Audio/Plug-Ins/Components/"
echo "  Standalone: Individual plugin build directories"
