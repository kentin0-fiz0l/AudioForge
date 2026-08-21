#!/bin/bash
# Deploy AudioForge website to DigitalOcean Spaces

set -e  # Exit on error

echo "🔨 Building AudioForge website..."
npm run build

echo ""
echo "📦 Build complete! Output directory: out/"
echo ""

# Check if AWS CLI is installed
if ! command -v aws &> /dev/null; then
    echo "⚠️  AWS CLI not found. Install it first:"
    echo "   brew install awscli"
    echo "   aws configure --profile digitalocean"
    exit 1
fi

# Configuration (update these values)
BUCKET="audioforge-site"
REGION="sfo3"  # Change to your DigitalOcean region
ENDPOINT="https://${REGION}.digitaloceanspaces.com"

echo "🚀 Deploying to DigitalOcean Spaces..."
echo "   Bucket: $BUCKET"
echo "   Region: $REGION"
echo ""

# Upload to Spaces using AWS CLI
aws s3 sync out/ s3://$BUCKET/ \
  --endpoint-url=$ENDPOINT \
  --profile=digitalocean \
  --acl public-read \
  --delete

echo ""
echo "✅ Deployment complete!"
echo "🌐 Site live at: https://audioforge.fluxstudio.art"
echo ""
echo "Note: DNS propagation may take a few minutes if this is your first deployment."
