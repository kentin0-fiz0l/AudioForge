#!/usr/bin/env bash
#
# Generate SVG Mockup Screenshots for AudioForge
# Creates placeholder images for all 30 screenshots
#

set -e

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
RAW_DIR="$SCRIPT_DIR/raw"
WEB_DIR="$SCRIPT_DIR/web"

mkdir -p "$RAW_DIR" "$WEB_DIR"

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}AudioForge Mockup Generator${NC}"
echo -e "${BLUE}======================================${NC}"
echo ""

# Plugin list
PLUGINS=(
    "SimpleGain"
    "SimpleEQ"
    "SimpleComp"
    "CleanDelay"
    "Saturation"
    "Reverb"
)

# Screenshot types with descriptions
get_description() {
    case "$1" in
        "01-default") echo "Default State - Clean UI, default parameters" ;;
        "02-active") echo "Active Processing - Meters showing signal activity" ;;
        "03-extreme") echo "Extreme Settings - Creative/aggressive parameters" ;;
        "04-daw") echo "In-DAW Context - Real-world integration" ;;
        "05-detail") echo "Detail Shot - Unique visual feature" ;;
    esac
}

# Generate SVG mockup function
generate_svg_mockup() {
    local plugin="$1"
    local type_code="$2"
    local type_name="$3"
    local description="$4"
    local output_file="$5"
    local width="$6"
    local height="$7"

    # Color scheme (dark theme to match DAW screenshots)
    local bg_color="#1a1a1a"
    local panel_color="#2d2d2d"
    local accent_color="#4a9eff"
    local text_color="#e0e0e0"
    local subtext_color="#999999"

    cat > "$output_file" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<svg width="$width" height="$height" xmlns="http://www.w3.org/2000/svg">
  <!-- Background -->
  <rect width="$width" height="$height" fill="$bg_color"/>

  <!-- Plugin window mockup -->
  <rect x="100" y="80" width="1080" height="560" rx="8" fill="$panel_color" stroke="$accent_color" stroke-width="2"/>

  <!-- Header bar -->
  <rect x="100" y="80" width="1080" height="50" rx="8" fill="$accent_color"/>
  <text x="640" y="112" font-family="system-ui, -apple-system, sans-serif" font-size="24" font-weight="bold" fill="white" text-anchor="middle">
    AudioForge - $plugin
  </text>

  <!-- Type badge -->
  <rect x="140" y="160" width="280" height="40" rx="20" fill="$accent_color" opacity="0.3"/>
  <text x="280" y="187" font-family="system-ui, -apple-system, sans-serif" font-size="18" font-weight="600" fill="$accent_color" text-anchor="middle">
    Screenshot $type_code: $type_name
  </text>

  <!-- Description -->
  <text x="640" y="250" font-family="system-ui, -apple-system, sans-serif" font-size="20" fill="$text_color" text-anchor="middle">
    $description
  </text>

  <!-- Placeholder content area -->
  <rect x="180" y="300" width="920" height="260" rx="4" fill="$bg_color" stroke="$subtext_color" stroke-width="1" stroke-dasharray="5,5"/>

  <!-- Placeholder text -->
  <text x="640" y="410" font-family="system-ui, -apple-system, sans-serif" font-size="16" fill="$subtext_color" text-anchor="middle">
    Plugin UI Mockup
  </text>
  <text x="640" y="440" font-family="system-ui, -apple-system, sans-serif" font-size="14" fill="$subtext_color" text-anchor="middle">
    Replace with actual screenshot from DAW
  </text>
  <text x="640" y="470" font-family="system-ui, -apple-system, sans-serif" font-size="14" fill="$subtext_color" text-anchor="middle">
    (Cmd+Shift+4 → Space → Click plugin window)
  </text>

  <!-- Footer -->
  <text x="640" y="620" font-family="system-ui, -apple-system, sans-serif" font-size="12" fill="$subtext_color" text-anchor="middle">
    AudioForge Plugin Suite • Professional Audio Processing • MIT License
  </text>

  <!-- Watermark -->
  <text x="1170" y="700" font-family="system-ui, -apple-system, sans-serif" font-size="10" fill="$subtext_color" text-anchor="end" opacity="0.5">
    MOCKUP - Replace with real screenshot
  </text>
</svg>
EOF

    echo -e "${GREEN}✓ Generated: $output_file${NC}"
}

# Generate all mockups
TOTAL=0
for plugin in "${PLUGINS[@]}"; do
    echo ""
    echo -e "${BLUE}→ Generating mockups for $plugin...${NC}"

    for type_key in "01-default" "02-active" "03-extreme" "04-daw" "05-detail"; do
        type_num=$(echo "$type_key" | cut -d'-' -f1)
        type_name=$(echo "$type_key" | cut -d'-' -f2 | sed 's/^./\U&/')
        description=$(get_description "$type_key")

        # Generate 2K version (raw)
        raw_file="$RAW_DIR/${plugin}-${type_key}.svg"
        generate_svg_mockup "$plugin" "$type_num" "$type_name" "$description" "$raw_file" "2560" "1440"

        # Generate web version (720p)
        web_file="$WEB_DIR/${plugin}-${type_key}.svg"
        generate_svg_mockup "$plugin" "$type_num" "$type_name" "$description" "$web_file" "1280" "720"

        ((TOTAL++))
    done
done

echo ""
echo -e "${BLUE}======================================${NC}"
echo -e "${GREEN}Mockup Generation Complete!${NC}"
echo -e "${BLUE}======================================${NC}"
echo ""
echo -e "${BLUE}Generated: $TOTAL mockups (30 raw + 30 web)${NC}"
echo ""
echo -e "${BLUE}Output directories:${NC}"
echo "  Raw (2K): $RAW_DIR"
echo "  Web (720p): $WEB_DIR"
echo ""

# Create gallery index
GALLERY_INDEX="$SCRIPT_DIR/gallery-index.md"
echo "# AudioForge Screenshot Gallery (Mockups)" > "$GALLERY_INDEX"
echo "" >> "$GALLERY_INDEX"
echo "**Note**: These are SVG mockups. Replace with real screenshots from DAW." >> "$GALLERY_INDEX"
echo "" >> "$GALLERY_INDEX"
echo "Generated: $(date)" >> "$GALLERY_INDEX"
echo "" >> "$GALLERY_INDEX"
echo "## Plugins" >> "$GALLERY_INDEX"
echo "" >> "$GALLERY_INDEX"

for plugin in "${PLUGINS[@]}"; do
    echo "### $plugin" >> "$GALLERY_INDEX"
    echo "" >> "$GALLERY_INDEX"

    for type_key in "01-default" "02-active" "03-extreme" "04-daw" "05-detail"; do
        type_name=$(echo "$type_key" | cut -d'-' -f2 | sed 's/^./\U&/')
        description=$(get_description "$type_key")

        echo "- **$type_name**: \`web/${plugin}-${type_key}.svg\`" >> "$GALLERY_INDEX"
        echo "  - $description" >> "$GALLERY_INDEX"
    done

    echo "" >> "$GALLERY_INDEX"
done

echo -e "${GREEN}✓ Gallery index created: $GALLERY_INDEX${NC}"
echo ""

# File size statistics
RAW_SIZE=$(du -sh "$RAW_DIR" | cut -f1)
WEB_SIZE=$(du -sh "$WEB_DIR" | cut -f1)

echo -e "${BLUE}File sizes:${NC}"
echo "  Raw mockups: $RAW_SIZE"
echo "  Web mockups: $WEB_SIZE"
echo ""

echo -e "${YELLOW}Next steps:${NC}"
echo "  1. View mockups in $WEB_DIR"
echo "  2. Deploy to website/public/screenshots/"
echo "  3. Test gallery page with mockups"
echo "  4. Replace with real screenshots when ready"
echo ""

echo -e "${GREEN}✓ Done! You now have 30 placeholder screenshots ready to deploy.${NC}"
