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

# Upload HTML files with no-cache to force CDN refresh
echo "📤 Uploading HTML files (no-cache)..."
aws s3 sync out/ s3://$BUCKET/ \
  --endpoint-url=$ENDPOINT \
  --profile=digitalocean \
  --acl public-read \
  --exclude "*" \
  --include "*.html" \
  --cache-control "max-age=0, must-revalidate" \
  --delete

# Upload static assets with longer cache
echo "📤 Uploading static assets (1 hour cache)..."
aws s3 sync out/ s3://$BUCKET/ \
  --endpoint-url=$ENDPOINT \
  --profile=digitalocean \
  --acl public-read \
  --exclude "*.html" \
  --cache-control "max-age=3600, public"

echo ""
echo "✅ Deployment complete!"
echo "🌐 Site live at: https://audioforge-site.sfo3.cdn.digitaloceanspaces.com/index.html"
echo ""
