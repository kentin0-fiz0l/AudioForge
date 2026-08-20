#!/bin/bash

##############################################################################
# AudioForge Plugin Upload Script
# Packages and uploads VST3 plugins to DigitalOcean Spaces
##############################################################################

set -e

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

# Configuration
SPACES_REGION="${SPACES_REGION:-sfo3}"
SPACES_BUCKET="${SPACES_BUCKET:-fluxstudio}"
SPACES_FOLDER="audioforge"
CDN_URL="https://${SPACES_BUCKET}.${SPACES_REGION}.cdn.digitaloceanspaces.com"

# Plugin directories
PLUGINS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/plugins"
UPLOAD_DIR="/tmp/audioforge-upload"

echo "=========================================="
echo "AudioForge Plugin Upload"
echo "=========================================="
echo ""

# Check dependencies
if ! command -v aws &> /dev/null; then
    echo -e "${RED}✗${NC} AWS CLI not found. Install with: brew install awscli"
    exit 1
fi

# Check for Spaces credentials
if [ -z "$SPACES_KEY" ] || [ -z "$SPACES_SECRET" ]; then
    echo -e "${YELLOW}⚠${NC}  DigitalOcean Spaces credentials not set"
    echo "  Set them in your environment:"
    echo "    export SPACES_KEY='your-key'"
    echo "    export SPACES_SECRET='your-secret'"
    echo ""
    read -p "Enter Spaces Access Key: " SPACES_KEY
    read -sp "Enter Spaces Secret Key: " SPACES_SECRET
    echo ""
fi

# Create temporary upload directory
rm -rf "$UPLOAD_DIR"
mkdir -p "$UPLOAD_DIR"

echo -e "${BLUE}Packaging plugins...${NC}"

##############################################################################
# Package SimpleGain
##############################################################################
echo ""
echo "1. SimpleGain v1.0.0"
echo "-------------------"

SIMPLEGAIN_DIR="$PLUGINS_DIR/SimpleGain/build/SimpleGain_artefacts"
SIMPLEGAIN_VERSION="1.0.0"

if [ ! -d "$SIMPLEGAIN_DIR" ]; then
    echo -e "${RED}✗${NC} SimpleGain not built. Run: cd plugins/SimpleGain && cmake -B build && cmake --build build"
    exit 1
fi

# Package macOS VST3
if [ -d "$SIMPLEGAIN_DIR/VST3/SimpleGain.vst3" ]; then
    echo -e "${GREEN}✓${NC} Packaging macOS VST3..."
    cd "$SIMPLEGAIN_DIR/VST3"
    zip -r "$UPLOAD_DIR/SimpleGain-${SIMPLEGAIN_VERSION}-macOS.vst3.zip" SimpleGain.vst3 -q
    MAC_SIZE=$(wc -c < "$UPLOAD_DIR/SimpleGain-${SIMPLEGAIN_VERSION}-macOS.vst3.zip" | tr -d ' ')
    echo "  Size: $(numfmt --to=iec $MAC_SIZE 2>/dev/null || echo "${MAC_SIZE} bytes")"
fi

# Package Standalone macOS app
if [ -d "$SIMPLEGAIN_DIR/Standalone/SimpleGain.app" ]; then
    echo -e "${GREEN}✓${NC} Packaging macOS Standalone..."
    cd "$SIMPLEGAIN_DIR/Standalone"
    zip -r "$UPLOAD_DIR/SimpleGain-${SIMPLEGAIN_VERSION}-macOS-Standalone.zip" SimpleGain.app -q
    STANDALONE_SIZE=$(wc -c < "$UPLOAD_DIR/SimpleGain-${SIMPLEGAIN_VERSION}-macOS-Standalone.zip" | tr -d ' ')
    echo "  Size: $(numfmt --to=iec $STANDALONE_SIZE 2>/dev/null || echo "${STANDALONE_SIZE} bytes")"
fi

##############################################################################
# Package PanUtil
##############################################################################
echo ""
echo "2. PanUtil v1.0.0"
echo "----------------"

PANUTIL_DIR="$PLUGINS_DIR/PanUtil/build/PanUtil_artefacts"
PANUTIL_VERSION="1.0.0"

if [ ! -d "$PANUTIL_DIR" ]; then
    echo -e "${RED}✗${NC} PanUtil not built. Run: cd plugins/PanUtil && cmake -B build && cmake --build build"
    exit 1
fi

# Package macOS VST3
if [ -d "$PANUTIL_DIR/VST3/PanUtil.vst3" ]; then
    echo -e "${GREEN}✓${NC} Packaging macOS VST3..."
    cd "$PANUTIL_DIR/VST3"
    zip -r "$UPLOAD_DIR/PanUtil-${PANUTIL_VERSION}-macOS.vst3.zip" PanUtil.vst3 -q
    PANUTIL_MAC_SIZE=$(wc -c < "$UPLOAD_DIR/PanUtil-${PANUTIL_VERSION}-macOS.vst3.zip" | tr -d ' ')
    echo "  Size: $(numfmt --to=iec $PANUTIL_MAC_SIZE 2>/dev/null || echo "${PANUTIL_MAC_SIZE} bytes")"
fi

# Package Standalone macOS app
if [ -d "$PANUTIL_DIR/Standalone/PanUtil.app" ]; then
    echo -e "${GREEN}✓${NC} Packaging macOS Standalone..."
    cd "$PANUTIL_DIR/Standalone"
    zip -r "$UPLOAD_DIR/PanUtil-${PANUTIL_VERSION}-macOS-Standalone.zip" PanUtil.app -q
    PANUTIL_STANDALONE_SIZE=$(wc -c < "$UPLOAD_DIR/PanUtil-${PANUTIL_VERSION}-macOS-Standalone.zip" | tr -d ' ')
    echo "  Size: $(numfmt --to=iec $PANUTIL_STANDALONE_SIZE 2>/dev/null || echo "${PANUTIL_STANDALONE_SIZE} bytes")"
fi

##############################################################################
# Upload to DigitalOcean Spaces
##############################################################################
echo ""
echo -e "${BLUE}Uploading to DigitalOcean Spaces...${NC}"
echo ""

# Configure AWS CLI for Spaces
export AWS_ACCESS_KEY_ID="$SPACES_KEY"
export AWS_SECRET_ACCESS_KEY="$SPACES_SECRET"

ENDPOINT_URL="https://${SPACES_REGION}.digitaloceanspaces.com"

# Upload function
upload_file() {
    local file=$1
    local plugin=$2
    local version=$3
    local filename=$(basename "$file")
    local s3_path="s3://${SPACES_BUCKET}/${SPACES_FOLDER}/${plugin}/${version}/${filename}"

    echo -e "${BLUE}↑${NC} Uploading ${filename}..."

    if aws s3 cp "$file" "$s3_path" \
        --endpoint-url="$ENDPOINT_URL" \
        --acl public-read \
        --content-type "application/zip" \
        --region "$SPACES_REGION" 2>&1; then

        local cdn_url="${CDN_URL}/${SPACES_FOLDER}/${plugin}/${version}/${filename}"
        echo -e "${GREEN}✓${NC} ${cdn_url}"
        echo "$cdn_url"
    else
        echo -e "${RED}✗${NC} Upload failed"
        return 1
    fi
}

# Upload SimpleGain files
SIMPLEGAIN_MAC_URL=$(upload_file "$UPLOAD_DIR/SimpleGain-${SIMPLEGAIN_VERSION}-macOS.vst3.zip" "simplegain" "$SIMPLEGAIN_VERSION")

# Upload PanUtil files
PANUTIL_MAC_URL=$(upload_file "$UPLOAD_DIR/PanUtil-${PANUTIL_VERSION}-macOS.vst3.zip" "panutil" "$PANUTIL_VERSION")

##############################################################################
# Generate SQL to update database
##############################################################################
echo ""
echo -e "${BLUE}Generating database update SQL...${NC}"
echo ""

SQL_FILE="$UPLOAD_DIR/update_plugin_urls.sql"

cat > "$SQL_FILE" <<EOF
-- Update SimpleGain download URLs
INSERT INTO plugin_versions (plugin_id, version, download_url_mac, file_size_mac, is_latest, released_at)
SELECT
    id,
    '${SIMPLEGAIN_VERSION}',
    '${SIMPLEGAIN_MAC_URL}',
    ${MAC_SIZE},
    true,
    NOW()
FROM plugins WHERE slug = 'simplegain'
ON CONFLICT (plugin_id, version)
DO UPDATE SET
    download_url_mac = EXCLUDED.download_url_mac,
    file_size_mac = EXCLUDED.file_size_mac,
    is_latest = EXCLUDED.is_latest;

-- Update PanUtil download URLs
INSERT INTO plugin_versions (plugin_id, version, download_url_mac, file_size_mac, is_latest, released_at)
SELECT
    id,
    '${PANUTIL_VERSION}',
    '${PANUTIL_MAC_URL}',
    ${PANUTIL_MAC_SIZE},
    true,
    NOW()
FROM plugins WHERE slug = 'panutil'
ON CONFLICT (plugin_id, version)
DO UPDATE SET
    download_url_mac = EXCLUDED.download_url_mac,
    file_size_mac = EXCLUDED.file_size_mac,
    is_latest = EXCLUDED.is_latest;
EOF

echo -e "${GREEN}✓${NC} SQL file created: $SQL_FILE"
echo ""
cat "$SQL_FILE"

##############################################################################
# Summary
##############################################################################
echo ""
echo "=========================================="
echo -e "${GREEN}Upload Complete!${NC}"
echo "=========================================="
echo ""
echo "Files uploaded:"
echo "  • SimpleGain macOS VST3: $(numfmt --to=iec $MAC_SIZE 2>/dev/null || echo "${MAC_SIZE} bytes")"
echo "  • PanUtil macOS VST3: $(numfmt --to=iec $PANUTIL_MAC_SIZE 2>/dev/null || echo "${PANUTIL_MAC_SIZE} bytes")"
echo ""
echo "CDN URLs:"
echo "  • ${SIMPLEGAIN_MAC_URL}"
echo "  • ${PANUTIL_MAC_URL}"
echo ""
echo "Next steps:"
echo "  1. Run the SQL update on production database:"
echo "     ${SQL_FILE}"
echo ""
echo "  2. Or run directly:"
echo "     psql \$DATABASE_URL -f ${SQL_FILE}"
echo ""
echo "  3. Test downloads at:"
echo "     https://fluxstudio.art/audioforge"
echo ""

# Clean up
# Uncomment to auto-remove temp files
# rm -rf "$UPLOAD_DIR"

echo -e "${GREEN}✓${NC} Done!"
