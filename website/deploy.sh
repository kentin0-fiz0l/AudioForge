#!/bin/bash
# Deploy AudioForge website to DigitalOcean Spaces

set -e  # Exit on error

echo "🔨 Building AudioForge website..."
npm run build

echo ""
echo "📦 Build complete! Output directory: out/"
echo ""

# Check if doctl is installed
if ! command -v doctl &> /dev/null; then
    echo "⚠️  doctl CLI not found. Install it first:"
    echo "   brew install doctl"
    echo "   doctl auth init"
    exit 1
fi

# Configuration (update these values)
SPACE_NAME="audioforge-site"
REGION="sfo3"  # Change to your DigitalOcean region

echo "🚀 Deploying to DigitalOcean Spaces..."
echo "   Space: $SPACE_NAME"
echo "   Region: $REGION"
echo ""

# Upload to Spaces
doctl spaces upload out/* \
  --space "$SPACE_NAME" \
  --region "$REGION" \
  --recursive \
  --force

echo ""
echo "✅ Deployment complete!"
echo "🌐 Site live at: https://audioforge.fluxstudio.art"
echo ""
echo "Note: DNS propagation may take a few minutes if this is your first deployment."
