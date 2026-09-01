#!/bin/bash
#
# Code Signing Script for AudioForge Plugins
# Signs all VST3 and AU bundles with Developer ID Application certificate
#

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
STAGING_DIR="${1:-$PROJECT_ROOT/releases/staging}"

# Code signing identity (from environment or default)
SIGNING_IDENTITY="${CODESIGN_IDENTITY:-Developer ID Application: Fizol LLC (J6Z4A7W2G9)}"

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}AudioForge Plugin Code Signing${NC}"
echo -e "${BLUE}======================================${NC}"
echo ""
echo -e "${BLUE}Staging directory: $STAGING_DIR${NC}"
echo -e "${BLUE}Signing identity: $SIGNING_IDENTITY${NC}"
echo ""

# Check if signing identity exists
if ! security find-identity -v -p codesigning | grep -q "$SIGNING_IDENTITY"; then
    echo -e "${RED}✗ Signing identity not found: $SIGNING_IDENTITY${NC}"
    echo -e "${YELLOW}Available identities:${NC}"
    security find-identity -v -p codesigning
    exit 1
fi

# Create entitlements file for hardened runtime
ENTITLEMENTS_FILE="$PROJECT_ROOT/scripts/AudioForge.entitlements"
cat > "$ENTITLEMENTS_FILE" << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <!-- Required for audio plugins -->
    <key>com.apple.security.cs.allow-jit</key>
    <true/>
    <key>com.apple.security.cs.allow-unsigned-executable-memory</key>
    <true/>
    <key>com.apple.security.cs.disable-library-validation</key>
    <true/>
    <!-- Audio device access -->
    <key>com.apple.security.device.audio-input</key>
    <true/>
    <!-- Network (for crash reporting, license validation) -->
    <key>com.apple.security.network.client</key>
    <true/>
</dict>
</plist>
EOF

echo -e "${GREEN}✓ Created entitlements file${NC}"
echo ""

# Function to sign a bundle
sign_bundle() {
    local bundle_path="$1"
    local bundle_name=$(basename "$bundle_path")

    echo -e "${YELLOW}Signing: $bundle_name${NC}"

    # Sign with hardened runtime and timestamp
    if codesign --force \
                --deep \
                --timestamp \
                --options runtime \
                --entitlements "$ENTITLEMENTS_FILE" \
                --sign "$SIGNING_IDENTITY" \
                "$bundle_path" > /dev/null 2>&1; then

        # Verify signature
        if codesign --verify --deep --strict "$bundle_path" > /dev/null 2>&1; then
            echo -e "${GREEN}  ✓ Signed and verified${NC}"
            return 0
        else
            echo -e "${RED}  ✗ Verification failed${NC}"
            return 1
        fi
    else
        echo -e "${RED}  ✗ Signing failed${NC}"
        return 1
    fi
}

# Sign all VST3 plugins
VST3_DIR="$STAGING_DIR/AudioForge Plugins/VST3"
if [ -d "$VST3_DIR" ]; then
    echo -e "${BLUE}→ Signing VST3 plugins...${NC}"
    VST3_SIGNED=0
    VST3_FAILED=0

    for vst3 in "$VST3_DIR"/*.vst3; do
        if [ -d "$vst3" ]; then
            if sign_bundle "$vst3"; then
                ((VST3_SIGNED++))
            else
                ((VST3_FAILED++))
            fi
        fi
    done

    echo -e "${GREEN}✓ VST3: $VST3_SIGNED signed, $VST3_FAILED failed${NC}"
    echo ""
fi

# Sign all AU plugins
AU_DIR="$STAGING_DIR/AudioForge Plugins/AU"
if [ -d "$AU_DIR" ]; then
    echo -e "${BLUE}→ Signing AU plugins...${NC}"
    AU_SIGNED=0
    AU_FAILED=0

    for au in "$AU_DIR"/*.component; do
        if [ -d "$au" ]; then
            if sign_bundle "$au"; then
                ((AU_SIGNED++))
            else
                ((AU_FAILED++))
            fi
        fi
    done

    echo -e "${GREEN}✓ AU: $AU_SIGNED signed, $AU_FAILED failed${NC}"
    echo ""
fi

# Cleanup
rm -f "$ENTITLEMENTS_FILE"

# Summary
TOTAL_SIGNED=$((VST3_SIGNED + AU_SIGNED))
TOTAL_FAILED=$((VST3_FAILED + AU_FAILED))

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}Code Signing Summary:${NC}"
echo -e "${GREEN}  Signed: $TOTAL_SIGNED${NC}"
if [ $TOTAL_FAILED -gt 0 ]; then
    echo -e "${RED}  Failed: $TOTAL_FAILED${NC}"
    echo -e "${BLUE}======================================${NC}"
    exit 1
else
    echo -e "${BLUE}======================================${NC}"
    echo ""
    echo -e "${GREEN}✓ All plugins signed successfully!${NC}"
    exit 0
fi
