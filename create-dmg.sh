#!/bin/bash

# AudioForge DMG Creator
# Creates professional macOS DMG installer from staging directory
# Version: 1.1.1

set -e  # Exit on error

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Version
VERSION="1.1.1"
PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"

# Paths
STAGING_DIR="$PROJECT_ROOT/releases/staging"
INSTALLER_DIR="$PROJECT_ROOT/releases/installers"
DMG_NAME="AudioForge-v${VERSION}-macOS"
DMG_PATH="$INSTALLER_DIR/${DMG_NAME}.dmg"
TEMP_DMG="$INSTALLER_DIR/temp.dmg"

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}AudioForge DMG Creator${NC}"
echo -e "${BLUE}Version: $VERSION${NC}"
echo -e "${BLUE}======================================${NC}"
echo ""

# Check if staging directory exists
if [ ! -d "$STAGING_DIR" ]; then
    echo -e "${RED}✗ Staging directory not found: $STAGING_DIR${NC}"
    echo -e "${YELLOW}Run ./build-installer.sh first${NC}"
    exit 1
fi

# Create installer directory if it doesn't exist
mkdir -p "$INSTALLER_DIR"

# Remove old DMG if exists
if [ -f "$DMG_PATH" ]; then
    echo -e "${YELLOW}→ Removing old DMG...${NC}"
    rm -f "$DMG_PATH"
fi

# Check if staging directory has content
if [ ! -d "$STAGING_DIR/AudioForge Plugins" ]; then
    echo -e "${RED}✗ AudioForge Plugins directory not found in staging${NC}"
    echo -e "${YELLOW}Run ./build-installer.sh first${NC}"
    exit 1
fi

# Count plugins
VST3_COUNT=$(find "$STAGING_DIR/AudioForge Plugins/VST3" -name "*.vst3" 2>/dev/null | wc -l | xargs)
AU_COUNT=$(find "$STAGING_DIR/AudioForge Plugins/AU" -name "*.component" 2>/dev/null | wc -l | xargs)

echo -e "${BLUE}Package Contents:${NC}"
echo "  VST3 plugins: $VST3_COUNT"
echo "  AU plugins: $AU_COUNT"
echo "  Install script: $([ -f "$STAGING_DIR/Install AudioForge.command" ] && echo "✓" || echo "✗")"
echo "  Uninstall script: $([ -f "$STAGING_DIR/Uninstall AudioForge.command" ] && echo "✓" || echo "✗")"
echo "  README: $([ -f "$STAGING_DIR/README.txt" ] && echo "✓" || echo "✗")"
echo "  LICENSE: $([ -f "$STAGING_DIR/LICENSE.txt" ] && echo "✓" || echo "✗")"
echo ""

# Calculate size
STAGING_SIZE=$(du -sh "$STAGING_DIR" | awk '{print $1}')
echo -e "${BLUE}Total size: $STAGING_SIZE${NC}"
echo ""

# Create DMG
echo -e "${YELLOW}→ Creating DMG...${NC}"

# Method 1: Using hdiutil (built-in macOS tool)
# This creates a simple, functional DMG without fancy customization

# Create temporary DMG with extra space
hdiutil create -srcfolder "$STAGING_DIR" \
    -volname "AudioForge v${VERSION}" \
    -fs HFS+ \
    -fsargs "-c c=64,a=16,e=16" \
    -format UDRW \
    -size 200m \
    "$TEMP_DMG"

echo -e "${GREEN}✓ Temporary DMG created${NC}"

# Mount the temporary DMG
echo -e "${YELLOW}→ Mounting DMG for customization...${NC}"

MOUNT_OUTPUT=$(hdiutil attach -readwrite -noverify -noautoopen "$TEMP_DMG")
MOUNT_DIR=$(echo "$MOUNT_OUTPUT" | egrep '^/dev/' | tail -1 | awk '{print $3}')

if [ -z "$MOUNT_DIR" ]; then
    echo -e "${RED}✗ Failed to mount DMG${NC}"
    rm -f "$TEMP_DMG"
    exit 1
fi

echo -e "${GREEN}✓ Mounted at: $MOUNT_DIR${NC}"

# Set DMG window appearance (using AppleScript)
echo -e "${YELLOW}→ Customizing DMG appearance...${NC}"

cat > /tmp/dmg_setup.applescript << EOF
tell application "Finder"
    tell disk "AudioForge v${VERSION}"
        open
        set current view of container window to list view
        set toolbar visible of container window to false
        set statusbar visible of container window to false
        set the bounds of container window to {100, 100, 700, 500}
        set arrangement of list view options of container window to not arranged
        set icon size of list view options of container window to 64
        close
        open
        update without registering applications
        delay 2
    end tell
end tell
EOF

osascript /tmp/dmg_setup.applescript 2>/dev/null || echo -e "${YELLOW}⚠ Could not customize window appearance${NC}"

# Eject the temporary DMG
echo -e "${YELLOW}→ Finalizing DMG...${NC}"
hdiutil detach "$MOUNT_DIR" > /dev/null 2>&1 || true
sleep 3

# Wait for disk to fully unmount
for i in {1..10}; do
    if ! mount | grep -q "AudioForge"; then
        break
    fi
    sleep 1
done

# Convert to compressed read-only DMG
echo -e "${YELLOW}→ Compressing DMG...${NC}"
hdiutil convert "$TEMP_DMG" \
    -format UDZO \
    -imagekey zlib-level=9 \
    -o "$DMG_PATH"

# Clean up temporary DMG
rm -f "$TEMP_DMG"

echo -e "${GREEN}✓ DMG created${NC}"

# Calculate final DMG size
DMG_SIZE=$(du -sh "$DMG_PATH" | awk '{print $1}')

# Generate SHA256 checksum
echo ""
echo -e "${YELLOW}→ Generating checksum...${NC}"
CHECKSUM=$(shasum -a 256 "$DMG_PATH" | awk '{print $1}')
echo "$CHECKSUM  ${DMG_NAME}.dmg" > "${DMG_PATH}.sha256"
echo -e "${GREEN}✓ Checksum: $CHECKSUM${NC}"

echo ""
echo -e "${GREEN}======================================${NC}"
echo -e "${GREEN}DMG Creation Complete!${NC}"
echo -e "${GREEN}======================================${NC}"
echo ""
echo "DMG File: $DMG_PATH"
echo "Size: $DMG_SIZE (compressed from $STAGING_SIZE)"
echo "Checksum: ${DMG_PATH}.sha256"
echo ""
echo "Next steps:"
echo "1. Test the DMG on a clean macOS system"
echo "2. Run the Install AudioForge.command script"
echo "3. Verify plugins load in your DAW"
echo "4. Test the Uninstall AudioForge.command script"
echo ""
echo "To test locally:"
echo "  open \"$DMG_PATH\""
echo ""
