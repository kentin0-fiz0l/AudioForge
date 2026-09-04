#!/bin/bash
# Sign all AudioForge plugins with Developer ID certificate
# Must be run AFTER build-installer.sh and BEFORE create-dmg.sh

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Configuration
IDENTITY="Developer ID Application: Fizol LLC (J6Z4A7W2G9)"
STAGING_DIR="${1:-../releases/staging}"

if [ ! -d "$STAGING_DIR" ]; then
    echo -e "${RED}Error: Staging directory not found: $STAGING_DIR${NC}"
    echo "Run build-installer.sh first to create the staging directory"
    exit 1
fi

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}AudioForge Plugin Signer${NC}"
echo -e "${BLUE}Identity: $IDENTITY${NC}"
echo -e "${BLUE}======================================${NC}"
echo ""

# Sign all VST3 plugins
echo -e "${YELLOW}→ Signing VST3 plugins...${NC}"
VST3_COUNT=0
for plugin in "$STAGING_DIR/AudioForge Plugins/VST3"/*.vst3; do
    if [ -d "$plugin" ]; then
        PLUGIN_NAME=$(basename "$plugin")
        echo -e "${BLUE}  Signing: $PLUGIN_NAME${NC}"

        # Sign the plugin bundle with hardened runtime and timestamp
        codesign --force --sign "$IDENTITY" \
            --timestamp \
            --options runtime \
            --deep \
            "$plugin"

        # Verify signature
        codesign --verify --deep --strict --verbose=2 "$plugin" 2>&1 | grep -q "valid on disk" && \
            echo -e "${GREEN}    ✓ Signed and verified${NC}" || \
            echo -e "${RED}    ✗ Verification failed${NC}"

        ((VST3_COUNT++))
    fi
done
echo -e "${GREEN}✓ Signed $VST3_COUNT VST3 plugins${NC}"
echo ""

# Sign all AU plugins
echo -e "${YELLOW}→ Signing AU plugins...${NC}"
AU_COUNT=0
for plugin in "$STAGING_DIR/AudioForge Plugins/AU"/*.component; do
    if [ -d "$plugin" ]; then
        PLUGIN_NAME=$(basename "$plugin")
        echo -e "${BLUE}  Signing: $PLUGIN_NAME${NC}"

        # Sign the plugin bundle with hardened runtime and timestamp
        codesign --force --sign "$IDENTITY" \
            --timestamp \
            --options runtime \
            --deep \
            "$plugin"

        # Verify signature
        codesign --verify --deep --strict --verbose=2 "$plugin" 2>&1 | grep -q "valid on disk" && \
            echo -e "${GREEN}    ✓ Signed and verified${NC}" || \
            echo -e "${RED}    ✗ Verification failed${NC}"

        ((AU_COUNT++))
    fi
done
echo -e "${GREEN}✓ Signed $AU_COUNT AU plugins${NC}"
echo ""

echo -e "${GREEN}=====================================${NC}"
echo -e "${GREEN}Signing Complete!${NC}"
echo -e "${GREEN}=====================================${NC}"
echo ""
echo "Signed:"
echo "  - $VST3_COUNT VST3 plugins"
echo "  - $AU_COUNT AU plugins"
echo ""
echo "Next step:"
echo "  ./installers/create-dmg.sh 1.9.1"
