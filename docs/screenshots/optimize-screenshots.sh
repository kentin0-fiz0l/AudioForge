#!/bin/bash
#
# Screenshot Optimization Script for AudioForge
# Compresses and resizes screenshots for web deployment
#

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
RAW_DIR="$SCRIPT_DIR/raw"
OPTIMIZED_DIR="$SCRIPT_DIR/optimized"
WEB_DIR="$SCRIPT_DIR/web"

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}AudioForge Screenshot Optimizer${NC}"
echo -e "${BLUE}======================================${NC}"
echo ""

# Check if raw directory exists
if [ ! -d "$RAW_DIR" ]; then
    echo -e "${RED}✗ Raw screenshots directory not found: $RAW_DIR${NC}"
    echo "Please capture screenshots first (see SCREENSHOT_GUIDE.md)"
    exit 1
fi

# Count raw screenshots
RAW_COUNT=$(find "$RAW_DIR" -name "*.png" | wc -l | tr -d ' ')
if [ "$RAW_COUNT" -eq 0 ]; then
    echo -e "${YELLOW}⚠ No PNG files found in $RAW_DIR${NC}"
    echo "Please capture screenshots first (see SCREENSHOT_GUIDE.md)"
    exit 0
fi

echo -e "${BLUE}Found $RAW_COUNT raw screenshots${NC}"
echo ""

# Create output directories
mkdir -p "$OPTIMIZED_DIR" "$WEB_DIR"

# Check for optimization tools
HAS_PNGCRUSH=false
HAS_OPTIPNG=false

if command -v pngcrush &> /dev/null; then
    HAS_PNGCRUSH=true
    echo -e "${GREEN}✓ pngcrush available${NC}"
elif command -v optipng &> /dev/null; then
    HAS_OPTIPNG=true
    echo -e "${GREEN}✓ optipng available${NC}"
else
    echo -e "${YELLOW}⚠ No PNG optimizer found (pngcrush or optipng)${NC}"
    echo "Install with: brew install pngcrush optipng"
    echo "Proceeding with resize only (no compression)..."
fi

echo ""
echo -e "${BLUE}→ Optimizing screenshots...${NC}"
echo ""

PROCESSED=0
FAILED=0

for img in "$RAW_DIR"/*.png; do
    if [ ! -f "$img" ]; then
        continue
    fi

    filename=$(basename "$img")
    echo -e "${YELLOW}Processing: $filename${NC}"

    # 1. Create web version (1280×720, optimized)
    echo "  → Creating web version (1280×720)..."
    if sips -Z 1280 "$img" --out "$WEB_DIR/$filename" > /dev/null 2>&1; then

        # Compress web version
        if [ "$HAS_PNGCRUSH" = true ]; then
            pngcrush -q -ow "$WEB_DIR/$filename" > /dev/null 2>&1 || true
        elif [ "$HAS_OPTIPNG" = true ]; then
            optipng -q -o7 "$WEB_DIR/$filename" > /dev/null 2>&1 || true
        fi

        WEB_SIZE=$(du -h "$WEB_DIR/$filename" | cut -f1)
        echo -e "${GREEN}  ✓ Web version created: $WEB_SIZE${NC}"
    else
        echo -e "${RED}  ✗ Failed to resize${NC}"
        ((FAILED++))
        continue
    fi

    # 2. Create optimized version (keep 2K resolution, compress only)
    echo "  → Creating optimized version (2K)..."
    cp "$img" "$OPTIMIZED_DIR/$filename"

    if [ "$HAS_OPTIPNG" = true ]; then
        optipng -q -o7 "$OPTIMIZED_DIR/$filename" > /dev/null 2>&1 || true
    elif [ "$HAS_PNGCRUSH" = true ]; then
        pngcrush -q -ow "$OPTIMIZED_DIR/$filename" > /dev/null 2>&1 || true
    fi

    OPT_SIZE=$(du -h "$OPTIMIZED_DIR/$filename" | cut -f1)
    echo -e "${GREEN}  ✓ Optimized version created: $OPT_SIZE${NC}"
    echo ""

    ((PROCESSED++))
done

# Generate gallery index
GALLERY_INDEX="$SCRIPT_DIR/gallery-index.md"
echo "# AudioForge Screenshot Gallery" > "$GALLERY_INDEX"
echo "" >> "$GALLERY_INDEX"
echo "Generated: $(date)" >> "$GALLERY_INDEX"
echo "" >> "$GALLERY_INDEX"
echo "## Screenshots by Plugin" >> "$GALLERY_INDEX"
echo "" >> "$GALLERY_INDEX"

# Group screenshots by plugin
for plugin_prefix in $(find "$WEB_DIR" -name "*.png" -exec basename {} \; | cut -d'-' -f1 | sort -u); do
    echo "### $plugin_prefix" >> "$GALLERY_INDEX"
    echo "" >> "$GALLERY_INDEX"

    # Find all screenshots for this plugin
    for screenshot in $(find "$WEB_DIR" -name "${plugin_prefix}-*.png" -exec basename {} \; | sort); do
        # Extract type from filename (e.g., "01-default" → "Default")
        type=$(echo "$screenshot" | sed "s/${plugin_prefix}-//" | sed 's/\.png$//' | sed 's/^[0-9]*-//' | sed 's/.*/\u&/')

        echo "- **$type**: \`web/$screenshot\`" >> "$GALLERY_INDEX"
    done

    echo "" >> "$GALLERY_INDEX"
done

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}Optimization Summary:${NC}"
echo -e "${GREEN}  Processed: $PROCESSED${NC}"
if [ $FAILED -gt 0 ]; then
    echo -e "${RED}  Failed: $FAILED${NC}"
fi
echo -e "${BLUE}======================================${NC}"
echo ""
echo -e "${BLUE}Output directories:${NC}"
echo -e "  Raw (2K): $RAW_DIR"
echo -e "  Optimized (2K): $OPTIMIZED_DIR"
echo -e "  Web (720p): $WEB_DIR"
echo ""
echo -e "${GREEN}✓ Gallery index created: $GALLERY_INDEX${NC}"
echo ""

# Show file size statistics
echo -e "${BLUE}File size statistics:${NC}"
RAW_TOTAL=$(du -sh "$RAW_DIR" | cut -f1)
OPT_TOTAL=$(du -sh "$OPTIMIZED_DIR" | cut -f1)
WEB_TOTAL=$(du -sh "$WEB_DIR" | cut -f1)

echo -e "  Raw total: $RAW_TOTAL"
echo -e "  Optimized total: $OPT_TOTAL"
echo -e "  Web total: $WEB_TOTAL"

# Calculate compression ratio
RAW_BYTES=$(du -s "$RAW_DIR" | cut -f1)
WEB_BYTES=$(du -s "$WEB_DIR" | cut -f1)
RATIO=$(echo "scale=1; ($RAW_BYTES - $WEB_BYTES) * 100 / $RAW_BYTES" | bc)

echo ""
echo -e "${GREEN}✓ Compression: ${RATIO}% size reduction (raw → web)${NC}"
echo ""

echo -e "${BLUE}Next steps:${NC}"
echo "  1. Review screenshots in $WEB_DIR"
echo "  2. Upload web versions to website/public/screenshots/"
echo "  3. Update plugin gallery page with image URLs"
echo "  4. Use detail shots for YouTube thumbnails"
echo ""
