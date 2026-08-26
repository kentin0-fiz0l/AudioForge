#!/bin/bash
set -e

# AudioForge Screenshot Verification Script
# Checks that all 50 plugin screenshots are present

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}AudioForge Screenshot Verification${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Base directory
BASE_DIR="website/public/screenshots"

if [ ! -d "$BASE_DIR" ]; then
    echo -e "${RED}Error: Screenshot directory not found: $BASE_DIR${NC}"
    exit 1
fi

# Plugin list
PLUGINS=(
    "SimpleGain"
    "PanUtil"
    "BasicSynth"
    "CleanDelay"
    "SimpleEQ"
    "SimpleComp"
    "WaveShaper"
    "Saturation"
    "ChorusFlanger"
    "Reverb"
)

# Expected screenshots per plugin
EXPECTED_COUNT=5

# Counters
TOTAL_EXPECTED=$((${#PLUGINS[@]} * EXPECTED_COUNT))
TOTAL_FOUND=0
MISSING_COUNT=0

echo -e "${BLUE}Checking screenshots for ${#PLUGINS[@]} plugins (${EXPECTED_COUNT} each)...${NC}"
echo ""

# Check each plugin
for plugin in "${PLUGINS[@]}"; do
    plugin_dir="$BASE_DIR/$plugin"

    if [ ! -d "$plugin_dir" ]; then
        echo -e "${YELLOW}→ ${plugin}${NC}"
        echo -e "  ${RED}✗ Directory not found: $plugin_dir${NC}"
        MISSING_COUNT=$((MISSING_COUNT + EXPECTED_COUNT))
        continue
    fi

    # Count PNG files
    screenshot_count=$(ls "$plugin_dir"/*.png 2>/dev/null | wc -l | tr -d ' ')

    if [ "$screenshot_count" -eq 0 ]; then
        echo -e "${YELLOW}→ ${plugin}${NC}"
        echo -e "  ${RED}✗ No screenshots found${NC}"
        MISSING_COUNT=$((MISSING_COUNT + EXPECTED_COUNT))
    elif [ "$screenshot_count" -lt "$EXPECTED_COUNT" ]; then
        echo -e "${YELLOW}→ ${plugin}${NC}"
        echo -e "  ${YELLOW}⚠ Found ${screenshot_count} / ${EXPECTED_COUNT} screenshots${NC}"
        MISSING_COUNT=$((MISSING_COUNT + EXPECTED_COUNT - screenshot_count))
        TOTAL_FOUND=$((TOTAL_FOUND + screenshot_count))

        # List existing screenshots
        for screenshot in "$plugin_dir"/*.png; do
            filename=$(basename "$screenshot")
            echo -e "    ${GREEN}✓${NC} $filename"
        done
    else
        echo -e "${GREEN}→ ${plugin}${NC}"
        echo -e "  ${GREEN}✓ Found ${screenshot_count} / ${EXPECTED_COUNT} screenshots${NC}"
        TOTAL_FOUND=$((TOTAL_FOUND + screenshot_count))
    fi
done

echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

if [ "$TOTAL_FOUND" -eq "$TOTAL_EXPECTED" ]; then
    echo -e "${GREEN}✓ All screenshots present!${NC}"
    echo -e "${GREEN}  Found: ${TOTAL_FOUND} / ${TOTAL_EXPECTED}${NC}"
else
    echo -e "${YELLOW}Status: ${TOTAL_FOUND} / ${TOTAL_EXPECTED} screenshots${NC}"
    echo -e "${RED}Missing: ${MISSING_COUNT} screenshots${NC}"
fi

echo ""

# Calculate total size
if [ "$TOTAL_FOUND" -gt 0 ]; then
    TOTAL_SIZE=$(du -sh "$BASE_DIR" 2>/dev/null | awk '{print $1}')
    echo -e "${BLUE}Total size: ${TOTAL_SIZE}${NC}"
fi

echo ""
echo -e "${BLUE}Done!${NC}"

# Exit code
if [ "$TOTAL_FOUND" -eq "$TOTAL_EXPECTED" ]; then
    exit 0
else
    exit 1
fi
