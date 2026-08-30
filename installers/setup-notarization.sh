#!/bin/bash
# AudioForge Notarization Setup Script
# Stores Apple notarization credentials in Keychain

set -e

KEYCHAIN_PROFILE="audioforge-notarize"

echo "========================================"
echo "AudioForge Notarization Setup"
echo "========================================"
echo ""

# Check if already configured
if xcrun notarytool store-credentials --list 2>/dev/null | grep -q "$KEYCHAIN_PROFILE"; then
    echo "⚠ Notarization profile '$KEYCHAIN_PROFILE' already exists"
    echo ""
    read -p "Reconfigure credentials? (y/n) " -n 1 -r
    echo ""

    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Aborted. No changes made."
        exit 0
    fi
fi

echo "This will store your Apple notarization credentials in Keychain."
echo ""
echo "You will need:"
echo "1. Apple ID email (your developer account email)"
echo "2. Team ID (found at developer.apple.com/account)"
echo "3. App-specific password (create at appleid.apple.com)"
echo ""
echo "To create an app-specific password:"
echo "1. Go to https://appleid.apple.com"
echo "2. Sign In → Security → App-Specific Passwords"
echo "3. Click '+' → Name it 'AudioForge Notarization'"
echo "4. Copy the generated password"
echo ""

read -p "Press Enter when ready to continue..."
echo ""

# Prompt for credentials
read -p "Apple ID Email: " APPLE_ID
read -p "Team ID: " TEAM_ID

echo ""
echo "→ Storing credentials in Keychain..."
echo ""
echo "You will be prompted for the app-specific password."
echo ""

# Store credentials
xcrun notarytool store-credentials "$KEYCHAIN_PROFILE" \
    --apple-id "$APPLE_ID" \
    --team-id "$TEAM_ID"

echo ""
echo "→ Verifying credentials..."
if xcrun notarytool store-credentials --list 2>/dev/null | grep -q "$KEYCHAIN_PROFILE"; then
    echo "✓ Credentials stored successfully"
    echo ""
    echo "========================================"
    echo "Setup Complete!"
    echo "========================================"
    echo ""
    echo "Notarization profile '$KEYCHAIN_PROFILE' is ready."
    echo ""
    echo "Next steps:"
    echo "1. Build and sign plugins: ./build-installer.sh"
    echo "2. Create DMG installer"
    echo "3. Notarize DMG: ./installers/notarize-dmg.sh AudioForge-v1.1.0-macOS.dmg"
    echo ""
else
    echo "✗ Failed to verify credentials"
    exit 1
fi
