#!/bin/bash
# Rapid Screenshot Capture for AudioForge Plugins
# Semi-automated: launches plugins, you press Cmd+Shift+4+Space+Click

set -e

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
RED='\033[0;31m'
BOLD='\033[1m'
NC='\033[0m'

# Configuration
SCREENSHOT_DIR="website/public/screenshots/plugins"
DOCS_SCREENSHOT_DIR="docs/screenshots"
TARGET_WIDTH=1200

# Plugins that need screenshots
PLUGINS=(
    "PanUtil:PanUtil"
    "SimpleEQ:SimpleEQ"
    "SimpleComp:SimpleComp"
    "Reverb:Reverb"
    "CleanDelay:CleanDelay"
    "Saturation:Saturation"
    "WaveShaper:WaveShaper"
    "ChorusFlanger:ChorusFlanger"
    "SpectralFreeze:SpectralFreeze"
    "FreezeFX:FreezeFX"
    "BasicSynth:BasicSynth"
    "FMSynth:FMSynth"
    "WavetableSynth:WavetableSynth"
    "PadSynth:PadSynth"
    "DrumSynth:DrumSynth"
    "OrganEmulator:OrganEmulator"
    "GranularEngine:GranularEngine"
    "SamplerPlugin:Sampler"
)

print_header() {
    echo -e "${BOLD}${BLUE}═══════════════════════════════════════════════════${NC}"
    echo -e "${BOLD}${BLUE}  $1${NC}"
    echo -e "${BOLD}${BLUE}═══════════════════════════════════════════════════${NC}"
}

print_success() { echo -e "${GREEN}✓${NC} $1"; }
print_error() { echo -e "${RED}✗${NC} $1"; }
print_info() { echo -e "${YELLOW}→${NC} $1"; }
print_action() { echo -e "${BOLD}${YELLOW}⚡ $1${NC}"; }

# Check directory
if [ ! -d "plugins" ]; then
    print_error "Not in AudioForge root directory!"
    exit 1
fi

# Create directories
mkdir -p "$SCREENSHOT_DIR"
mkdir -p "$DOCS_SCREENSHOT_DIR"

# Function to find standalone app
find_app() {
    local plugin=$1
    find "plugins/$plugin/build" -name "${plugin}.app" -type d 2>/dev/null | head -1
}

# Function to check if has screenshot
has_screenshot() {
    local display_name=$1
    local plugin_lower=$(echo "$display_name" | tr '[:upper:]' '[:lower:]')
    [ -f "$SCREENSHOT_DIR/$display_name/${plugin_lower}-main.png" ]
}

# Function to process one plugin
process_plugin() {
    local plugin=$1
    local display_name=$2
    local plugin_lower=$(echo "$display_name" | tr '[:upper:]' '[:lower:]')
    local screenshot_path="$SCREENSHOT_DIR/$display_name/${plugin_lower}-main.png"

    clear
    print_header "$display_name ($current/$total)"
    echo ""

    # Check if already has screenshot
    if has_screenshot "$display_name"; then
        print_success "Already has screenshot!"
        echo ""
        echo "Press ENTER to continue..."
        read
        return 0
    fi

    # Find standalone app
    local app_path=$(find_app "$plugin")

    if [ -z "$app_path" ]; then
        print_info "Building standalone app for $display_name..."
        cd "plugins/$plugin"
        if [ ! -d "build" ]; then
            cmake -B build -DCMAKE_BUILD_TYPE=Release
        fi
        cmake --build build --config Release --target "${plugin}_Standalone"
        cd ../..
        app_path=$(find_app "$plugin")
    fi

    if [ -z "$app_path" ]; then
        print_error "Could not find/build standalone app"
        echo ""
        echo "Press ENTER to skip..."
        read
        return 1
    fi

    # Launch the plugin
    print_action "LAUNCHING: $display_name"
    open "$app_path"
    sleep 3

    echo ""
    print_action "NOW: Press Cmd+Shift+4, then Space, then click the $display_name window"
    echo ""
    echo "Waiting for screenshot on Desktop..."

    # Wait for new screenshot
    local start_count=$(ls ~/Desktop/Screenshot*.png 2>/dev/null | wc -l | tr -d ' ')

    while true; do
        local current_count=$(ls ~/Desktop/Screenshot*.png 2>/dev/null | wc -l | tr -d ' ')
        if [ "$current_count" -gt "$start_count" ]; then
            break
        fi
        sleep 0.5
    done

    # Get the latest screenshot
    local latest_screenshot=$(ls -t ~/Desktop/Screenshot*.png 2>/dev/null | head -1)

    # Close the plugin
    killall "$plugin" 2>/dev/null || osascript -e "tell application \"$plugin\" to quit" 2>/dev/null || true
    sleep 1

    if [ -z "$latest_screenshot" ]; then
        print_error "No screenshot found!"
        return 1
    fi

    # Process screenshot
    mkdir -p "$SCREENSHOT_DIR/$display_name"
    cp "$latest_screenshot" "$screenshot_path"

    # Optimize
    local orig_size=$(du -h "$screenshot_path" | cut -f1)
    sips -Z $TARGET_WIDTH "$screenshot_path" > /dev/null 2>&1
    local new_size=$(du -h "$screenshot_path" | cut -f1)

    # Backup
    cp "$screenshot_path" "$DOCS_SCREENSHOT_DIR/${display_name}-screenshot.png"

    # Clean up Desktop
    rm "$latest_screenshot"

    print_success "Screenshot saved! ($orig_size → $new_size)"
    echo ""
    sleep 1
}

# Main
clear
print_header "AudioForge Rapid Screenshot Capture"
echo ""
echo "This script will:"
echo "  1. Launch each plugin in standalone mode"
echo "  2. Wait for you to take a screenshot"
echo "  3. Automatically process and save it"
echo "  4. Move to the next plugin"
echo ""
echo -e "${BOLD}Screenshot Shortcut: Cmd+Shift+4, then Space, then Click window${NC}"
echo ""
echo "Plugins to capture: ${#PLUGINS[@]}"
echo ""
print_info "Press ENTER to start, or Ctrl+C to cancel..."
read

# Statistics
total=${#PLUGINS[@]}
current=0
success=0
skipped=0

# Process each plugin
for entry in "${PLUGINS[@]}"; do
    plugin="${entry%%:*}"
    display_name="${entry##*:}"
    current=$((current + 1))

    if process_plugin "$plugin" "$display_name"; then
        if has_screenshot "$display_name"; then
            success=$((success + 1))
        fi
    fi
done

# Summary
clear
print_header "Screenshot Capture Complete!"
echo ""
echo "Results:"
echo "  Processed: $total plugins"
echo "  Captured: $success new screenshots"
echo ""
total_screenshots=$(find "$SCREENSHOT_DIR" -type d -mindepth 1 -maxdepth 1 | wc -l | tr -d ' ')
print_success "Total plugins with screenshots: $total_screenshots/22"
echo ""
