#!/bin/bash
#
# Semi-Automated Screenshot Capture for AudioForge
# This script helps automate the screenshot capture process
#

set -e

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
RAW_DIR="$SCRIPT_DIR/raw"

mkdir -p "$RAW_DIR"

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}AudioForge Screenshot Auto-Capture${NC}"
echo -e "${BLUE}======================================${NC}"
echo ""

# Plugin list for Session 1
PLUGINS=(
    "SimpleGain"
    "SimpleEQ"
    "SimpleComp"
    "CleanDelay"
    "Saturation"
    "Reverb"
)

SCREENSHOT_TYPES=(
    "01-default"
    "02-active"
    "03-extreme"
    "04-daw"
    "05-detail"
)

echo -e "${BLUE}This script will guide you through capturing screenshots.${NC}"
echo -e "${BLUE}It will prompt you at each step.${NC}"
echo ""
echo -e "${YELLOW}Prerequisites:${NC}"
echo "  1. DAW is open with pink-noise-10s.wav loaded"
echo "  2. Audio track is ready for plugin insertion"
echo "  3. Buffer size set to 512 samples"
echo ""

read -p "Press ENTER when ready to start..."

for plugin in "${PLUGINS[@]}"; do
    echo ""
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}Plugin: $plugin${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo ""

    echo -e "${YELLOW}→ Load $plugin on your audio track${NC}"
    read -p "Press ENTER when plugin is loaded..."

    for screenshot_type in "${SCREENSHOT_TYPES[@]}"; do
        screenshot_num=$(echo "$screenshot_type" | cut -d'-' -f1)
        type_name=$(echo "$screenshot_type" | cut -d'-' -f2)

        echo ""
        echo -e "${YELLOW}Screenshot $screenshot_num: $type_name${NC}"

        case "$type_name" in
            "default")
                echo "  1. Reset plugin to defaults (Initialize Preset)"
                echo "  2. STOP playback (silent meters)"
                echo "  3. Ready for screenshot"
                ;;
            "active")
                echo "  1. START playback (enable loop)"
                case "$plugin" in
                    "SimpleGain")
                        echo "  2. Set Gain: -6.0 dB"
                        echo "  3. Wait for meters to stabilize (~-18 dBFS)"
                        ;;
                    "SimpleEQ")
                        echo "  2. Band 1: +3 dB at 100 Hz"
                        echo "  3. Band 2: +2 dB at 1000 Hz"
                        echo "  4. HP Filter: 80 Hz"
                        ;;
                    "SimpleComp")
                        echo "  2. Threshold: -10 dB"
                        echo "  3. Ratio: 4:1"
                        echo "  4. Watch for 4-6 dB gain reduction"
                        ;;
                    "CleanDelay")
                        echo "  2. Switch to test-tone-440hz.wav if not already"
                        echo "  3. Delay: 250 ms"
                        echo "  4. Feedback: 50%"
                        echo "  5. Mix: 40%"
                        ;;
                    "Saturation")
                        echo "  2. Drive: 12 dB"
                        echo "  3. Type: Tape"
                        echo "  4. Mix: 60%"
                        ;;
                    "Reverb")
                        echo "  2. Switch to test-tone-440hz.wav if not already"
                        echo "  3. Room Size: 0.7"
                        echo "  4. Decay: 3.5s"
                        echo "  5. Mix: 50%"
                        ;;
                esac
                echo "  → Wait 3 seconds for meters to stabilize"
                ;;
            "extreme")
                echo "  1. Playback still running"
                case "$plugin" in
                    "SimpleGain")
                        echo "  2. Set Gain: +12 dB (clipping)"
                        ;;
                    "SimpleEQ")
                        echo "  2. All bands: +10 dB (smiley curve)"
                        ;;
                    "SimpleComp")
                        echo "  2. Threshold: -20 dB"
                        echo "  3. Ratio: 20:1 (limiter mode)"
                        ;;
                    "CleanDelay")
                        echo "  2. Delay: 2000 ms"
                        echo "  3. Feedback: 90%"
                        echo "  4. Mix: 100%"
                        ;;
                    "Saturation")
                        echo "  2. Drive: 30 dB (max)"
                        echo "  3. Type: Hard Clip"
                        echo "  4. Mix: 100%"
                        ;;
                    "Reverb")
                        echo "  2. Room Size: 1.0 (Cathedral)"
                        echo "  3. Decay: 10.0s"
                        echo "  4. Mix: 100%"
                        ;;
                esac
                ;;
            "daw")
                echo "  1. Zoom DAW out (show mixer + plugin)"
                echo "  2. Frame: Plugin window + 2-3 tracks visible"
                echo "  3. Use Cmd+Shift+4 → DRAG SELECTION (not window capture)"
                ;;
            "detail")
                echo "  1. Focus on unique visual element:"
                case "$plugin" in
                    "SimpleGain")
                        echo "     → Gain meter/VU display"
                        ;;
                    "SimpleEQ")
                        echo "     → EQ curve graph"
                        ;;
                    "SimpleComp")
                        echo "     → Gain reduction meter"
                        ;;
                    "CleanDelay")
                        echo "     → Delay time display or waveform"
                        ;;
                    "Saturation")
                        echo "     → Saturation type selector"
                        ;;
                    "Reverb")
                        echo "     → Room size visualization"
                        ;;
                esac
                echo "  2. Use Cmd+Shift+4 → DRAG around element only"
                ;;
        esac

        echo ""
        echo -e "${GREEN}Ready to capture${NC}"

        if [ "$type_name" = "daw" ] || [ "$type_name" = "detail" ]; then
            echo -e "${YELLOW}Use: Cmd+Shift+4 → DRAG selection${NC}"
        else
            echo -e "${YELLOW}Use: Cmd+Shift+4 → SPACE → Click plugin window${NC}"
        fi

        echo ""
        read -p "Press ENTER after you've taken the screenshot..."

        # Wait for user to manually save the screenshot
        filename="${plugin}-${screenshot_type}.png"
        expected_path="$RAW_DIR/$filename"

        echo ""
        echo -e "${BLUE}Expected filename: $filename${NC}"
        echo -e "${BLUE}Expected location: $RAW_DIR${NC}"
        echo ""

        # Check if screenshot was saved
        sleep 1
        if [ -f "$expected_path" ]; then
            echo -e "${GREEN}✓ Found: $filename${NC}"
        else
            echo -e "${YELLOW}⚠ Screenshot not found yet at expected location${NC}"
            echo -e "${YELLOW}Make sure you saved it as: $RAW_DIR/$filename${NC}"
            read -p "Press ENTER when file is saved..."

            if [ -f "$expected_path" ]; then
                echo -e "${GREEN}✓ Found: $filename${NC}"
            else
                echo -e "${RED}⚠ Still not found - continuing anyway${NC}"
            fi
        fi
    done

    echo ""
    echo -e "${GREEN}✓ $plugin complete (5 screenshots)${NC}"
    echo ""

    if [ "$plugin" != "Reverb" ]; then
        echo -e "${YELLOW}→ Unload $plugin and prepare for next plugin${NC}"
        read -p "Press ENTER to continue..."
    fi
done

echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}Session 1 Complete!${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Count captured screenshots
CAPTURED=$(find "$RAW_DIR" -name "*.png" | wc -l | tr -d ' ')
echo -e "${BLUE}Screenshots captured: $CAPTURED / 30${NC}"
echo ""

if [ "$CAPTURED" -eq 30 ]; then
    echo -e "${GREEN}✓ All 30 screenshots captured!${NC}"
    echo ""
    echo -e "${BLUE}Next step: Optimize screenshots${NC}"
    echo -e "Run: ${YELLOW}./optimize-screenshots.sh${NC}"
else
    echo -e "${YELLOW}⚠ Expected 30 screenshots, found $CAPTURED${NC}"
    echo ""
    echo "Missing screenshots:"

    for plugin in "${PLUGINS[@]}"; do
        for screenshot_type in "${SCREENSHOT_TYPES[@]}"; do
            filename="${plugin}-${screenshot_type}.png"
            if [ ! -f "$RAW_DIR/$filename" ]; then
                echo "  - $filename"
            fi
        done
    done
fi

echo ""
