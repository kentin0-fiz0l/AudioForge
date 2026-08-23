#!/bin/bash
set -e

DMG_FILE="$1"
KEYCHAIN_PROFILE="audioforge-notarize"

if [ -z "$DMG_FILE" ]; then
    echo "Usage: ./notarize-dmg.sh AudioForge-v1.1.0-macOS.dmg"
    exit 1
fi

if [ ! -f "$DMG_FILE" ]; then
    echo "Error: DMG file not found: $DMG_FILE"
    exit 1
fi

echo "→ Submitting $DMG_FILE for notarization..."
xcrun notarytool submit "$DMG_FILE" \
    --keychain-profile "$KEYCHAIN_PROFILE" \
    --wait

echo "→ Stapling notarization ticket..."
xcrun stapler staple "$DMG_FILE"

echo "→ Verifying notarization..."
xcrun stapler validate "$DMG_FILE"
spctl --assess --type install --verbose "$DMG_FILE"

echo "✓ $DMG_FILE is now notarized and ready for distribution!"
