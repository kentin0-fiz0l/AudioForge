#!/bin/bash
# AudioForge Code Signing Setup Script
# Configures environment variables for automatic code signing

set -e

echo "========================================"
echo "AudioForge Code Signing Setup"
echo "========================================"
echo ""

# Check for Developer ID certificate
echo "→ Checking for Developer ID Application certificate..."
CERT_INFO=$(security find-identity -v -p codesigning | grep "Developer ID Application" || true)

if [ -z "$CERT_INFO" ]; then
    echo "✗ No Developer ID Application certificate found!"
    echo ""
    echo "Please create a certificate in Xcode:"
    echo "1. Open Xcode → Settings → Accounts"
    echo "2. Select your Apple ID → Manage Certificates"
    echo "3. Click + → Developer ID Application"
    echo ""
    exit 1
fi

echo "✓ Found certificate:"
echo "$CERT_INFO"
echo ""

# Extract certificate name (between quotes)
CERT_NAME=$(echo "$CERT_INFO" | sed 's/.*"\(.*\)".*/\1/')

if [ -z "$CERT_NAME" ]; then
    echo "✗ Could not extract certificate name"
    exit 1
fi

echo "→ Certificate identity: $CERT_NAME"
echo ""

# Check if already configured
if grep -q "CODESIGN_IDENTITY" ~/.zshrc 2>/dev/null; then
    echo "⚠ CODESIGN_IDENTITY already exists in ~/.zshrc"
    echo ""
    read -p "Replace existing configuration? (y/n) " -n 1 -r
    echo ""

    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Aborted. No changes made."
        exit 0
    fi

    # Remove old configuration
    sed -i '' '/CODESIGN_IDENTITY/d' ~/.zshrc
fi

# Add to ~/.zshrc
echo "→ Adding CODESIGN_IDENTITY to ~/.zshrc..."
echo "" >> ~/.zshrc
echo "# AudioForge code signing" >> ~/.zshrc
echo "export CODESIGN_IDENTITY=\"$CERT_NAME\"" >> ~/.zshrc

echo "✓ Configuration added to ~/.zshrc"
echo ""

# Apply to current shell
export CODESIGN_IDENTITY="$CERT_NAME"

echo "→ Testing configuration..."
if [ -n "$CODESIGN_IDENTITY" ]; then
    echo "✓ CODESIGN_IDENTITY is set correctly"
    echo ""
    echo "========================================"
    echo "Setup Complete!"
    echo "========================================"
    echo ""
    echo "The following has been configured:"
    echo "  CODESIGN_IDENTITY=\"$CERT_NAME\""
    echo ""
    echo "Next steps:"
    echo "1. Close this terminal and open a new one (to load new environment)"
    echo "2. Run: ./build-installer.sh"
    echo "3. Plugins will be automatically signed during build"
    echo ""
else
    echo "✗ Failed to set environment variable"
    exit 1
fi
