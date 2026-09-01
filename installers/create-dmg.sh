#!/bin/bash
# AudioForge DMG Creation Script
# Creates a distributable DMG installer for macOS

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Version (update this with each release)
VERSION="${1:-1.5.0}"

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
STAGING_DIR="$PROJECT_ROOT/releases/staging"
INSTALLER_DIR="$PROJECT_ROOT/releases/installers"
DMG_NAME="AudioForge-v${VERSION}-macOS.dmg"
DMG_PATH="$INSTALLER_DIR/$DMG_NAME"
TEMP_DMG="/tmp/audioforge-temp.dmg"

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}AudioForge DMG Creator${NC}"
echo -e "${BLUE}Version: $VERSION${NC}"
echo -e "${BLUE}======================================${NC}"
echo ""

# Check if staging directory exists
if [ ! -d "$STAGING_DIR" ]; then
    echo -e "${RED}✗ Staging directory not found: $STAGING_DIR${NC}"
    echo -e "${YELLOW}  Run ./build-installer.sh first${NC}"
    exit 1
fi

# Check if plugins exist
if [ ! -d "$STAGING_DIR/AudioForge Plugins/VST3" ] || [ -z "$(ls -A "$STAGING_DIR/AudioForge Plugins/VST3")" ]; then
    echo -e "${RED}✗ No VST3 plugins found in staging directory${NC}"
    echo -e "${YELLOW}  Run ./build-installer.sh first${NC}"
    exit 1
fi

# Create installer directory
mkdir -p "$INSTALLER_DIR"

# Remove old DMG if exists
if [ -f "$DMG_PATH" ]; then
    echo -e "${YELLOW}→ Removing existing DMG...${NC}"
    rm -f "$DMG_PATH"
fi

# Remove temp DMG if exists
rm -f "$TEMP_DMG"

echo -e "${BLUE}→ Creating DMG from staging directory...${NC}"

# Calculate size needed for DMG (staging dir size + 20% overhead)
STAGING_SIZE=$(du -sm "$STAGING_DIR" | awk '{print $1}')
DMG_SIZE=$((STAGING_SIZE + (STAGING_SIZE / 5) + 10)) # Add 20% + 10MB overhead

echo -e "${YELLOW}  Staging size: ${STAGING_SIZE}MB${NC}"
echo -e "${YELLOW}  DMG size: ${DMG_SIZE}MB${NC}"

# Create temporary DMG
hdiutil create -srcfolder "$STAGING_DIR" \
    -volname "AudioForge v${VERSION}" \
    -fs HFS+ \
    -fsargs "-c c=64,a=16,e=16" \
    -format UDRW \
    -size ${DMG_SIZE}m \
    "$TEMP_DMG"

echo -e "${GREEN}✓ Temporary DMG created${NC}"

# Mount the temporary DMG
echo -e "${BLUE}→ Mounting DMG...${NC}"
MOUNT_OUTPUT=$(hdiutil attach -readwrite -noverify -noautoopen "$TEMP_DMG" 2>&1)
MOUNT_DIR=$(echo "$MOUNT_OUTPUT" | grep "Apple_HFS" | awk -F'\t' '{print $NF}' | tr -d '\n' | xargs)

if [ -z "$MOUNT_DIR" ] || [ ! -d "$MOUNT_DIR" ]; then
    echo -e "${RED}✗ Failed to mount DMG${NC}"
    echo "Output: $MOUNT_OUTPUT"
    echo "Mount dir: '$MOUNT_DIR'"
    exit 1
fi

echo -e "${GREEN}✓ Mounted at: $MOUNT_DIR${NC}"

# Set custom DMG appearance
echo -e "${BLUE}→ Configuring DMG appearance...${NC}"

# Create a symbolic link to /Applications for easy drag-and-drop
# (Note: Not needed for our installer since we have .command scripts)

# Set DMG background (if we have one)
# mkdir -p "$MOUNT_DIR/.background"
# cp "$PROJECT_ROOT/assets/dmg-background.png" "$MOUNT_DIR/.background/" || true

# Set custom icon (if we have one)
# cp "$PROJECT_ROOT/assets/audioforge-icon.icns" "$MOUNT_DIR/.VolumeIcon.icns" || true
# SetFile -c icnC "$MOUNT_DIR/.VolumeIcon.icns" || true
# SetFile -a C "$MOUNT_DIR" || true

echo -e "${GREEN}✓ DMG appearance configured${NC}"

# Unmount
echo -e "${BLUE}→ Unmounting DMG...${NC}"
hdiutil detach "$MOUNT_DIR" -quiet

echo -e "${GREEN}✓ DMG unmounted${NC}"

# Convert to compressed read-only DMG
echo -e "${BLUE}→ Compressing DMG...${NC}"

hdiutil convert "$TEMP_DMG" \
    -format UDZO \
    -imagekey zlib-level=9 \
    -o "$DMG_PATH"

echo -e "${GREEN}✓ DMG compressed${NC}"

# Clean up temporary DMG
rm -f "$TEMP_DMG"

# Sign the DMG (if CODESIGN_IDENTITY is set)
if [ -n "$CODESIGN_IDENTITY" ]; then
    echo ""
    echo -e "${BLUE}→ Signing DMG...${NC}"
    echo -e "${YELLOW}  Using identity: $CODESIGN_IDENTITY${NC}"

    codesign --force --sign "$CODESIGN_IDENTITY" "$DMG_PATH"

    echo -e "${GREEN}✓ DMG signed${NC}"

    # Verify signature
    echo -e "${BLUE}→ Verifying signature...${NC}"
    codesign --verify --verbose "$DMG_PATH"
    echo -e "${GREEN}✓ Signature verified${NC}"
else
    echo ""
    echo -e "${YELLOW}⚠ CODESIGN_IDENTITY not set - DMG will not be signed${NC}"
    echo -e "${YELLOW}  Set CODESIGN_IDENTITY environment variable to enable signing${NC}"
fi

# Calculate final DMG size
DMG_FILE_SIZE=$(du -h "$DMG_PATH" | awk '{print $1}')

echo ""
echo -e "${GREEN}=====================================${NC}"
echo -e "${GREEN}DMG Created Successfully!${NC}"
echo -e "${GREEN}=====================================${NC}"
echo ""
echo "DMG file: $DMG_PATH"
echo "File size: $DMG_FILE_SIZE"
echo ""

if [ -n "$CODESIGN_IDENTITY" ]; then
    echo "Next steps:"
    echo "1. Test DMG on a clean macOS system"
    echo "2. Notarize with Apple: ./installers/notarize-dmg.sh \"$DMG_PATH\""
    echo "3. Upload to GitHub Release"
else
    echo "Next steps:"
    echo "1. Sign DMG with: codesign --sign <identity> \"$DMG_PATH\""
    echo "2. Notarize with Apple: ./installers/notarize-dmg.sh \"$DMG_PATH\""
    echo "3. Upload to GitHub Release"
fi

echo ""
