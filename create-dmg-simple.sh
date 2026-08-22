#!/bin/bash

# AudioForge DMG Creator (Simplified)
# Creates macOS DMG installer without customization
# Version: 1.0.0

set -e

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

VERSION="1.0.0"
PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"

STAGING_DIR="$PROJECT_ROOT/releases/staging"
INSTALLER_DIR="$PROJECT_ROOT/releases/installers"
DMG_NAME="AudioForge-v${VERSION}-macOS"
DMG_PATH="$INSTALLER_DIR/${DMG_NAME}.dmg"

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}AudioForge DMG Creator (Simple)${NC}"
echo -e "${BLUE}======================================${NC}"
echo ""

mkdir -p "$INSTALLER_DIR"
rm -f "$DMG_PATH"

# Create DMG directly from staging directory
echo -e "${YELLOW}→ Creating compressed DMG...${NC}"

hdiutil create -volname "AudioForge v${VERSION}" \
    -srcfolder "$STAGING_DIR" \
    -ov \
    -format UDZO \
    -imagekey zlib-level=9 \
    "$DMG_PATH"

echo -e "${GREEN}✓ DMG created${NC}"

# Calculate size
DMG_SIZE=$(du -sh "$DMG_PATH" | awk '{print $1}')

# Generate checksum
echo -e "${YELLOW}→ Generating checksum...${NC}"
CHECKSUM=$(shasum -a 256 "$DMG_PATH" | awk '{print $1}')
echo "$CHECKSUM  ${DMG_NAME}.dmg" > "${DMG_PATH}.sha256"
echo -e "${GREEN}✓ Checksum saved${NC}"

echo ""
echo -e "${GREEN}=====================================${NC}"
echo -e "${GREEN}DMG Creation Complete!${NC}"
echo -e "${GREEN}=====================================${NC}"
echo ""
echo "DMG File: $DMG_PATH"
echo "Size: $DMG_SIZE"
echo "SHA256: $CHECKSUM"
echo ""
echo "To test: open \"$DMG_PATH\""
echo ""
