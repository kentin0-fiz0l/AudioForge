#!/bin/bash
# AudioForge Screenshot Capture Helper
# Helps you systematically capture and optimize plugin screenshots

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
SCREENSHOT_DIR="website/public/screenshots/plugins"
DOCS_SCREENSHOT_DIR="docs/screenshots"
TARGET_WIDTH=1200

# Plugins that need screenshots (18 remaining)
PLUGINS_NEEDED=(
    "PanUtil"
    "SimpleEQ"
    "SimpleComp"
    "Reverb"
    "CleanDelay"
    "Saturation"
    "WaveShaper"
    "ChorusFlanger"
    "SpectralFreeze"
    "FreezeFX"
    "BasicSynth"
    "FMSynth"
    "WavetableSynth"
    "PadSynth"
    "DrumSynth"
    "OrganEmulator"
    "GranularEngine"
    "SamplerPlugin"
)

# Function to print colored output
print_header() {
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
}

print_success() {
    echo -e "${GREEN}✓${NC} $1"
}

print_error() {
    echo -e "${RED}✗${NC} $1"
}

print_info() {
    echo -e "${YELLOW}→${NC} $1"
}

# Check if we're in the right directory
if [ ! -d "plugins" ]; then
    print_error "Not in AudioForge root directory!"
    echo "Please run this script from /Users/kentino/Projects/Active/AudioForge"
    exit 1
fi

# Create directories if they don't exist
mkdir -p "$SCREENSHOT_DIR"
mkdir -p "$DOCS_SCREENSHOT_DIR"

# Function to check if plugin has screenshot
has_screenshot() {
    local plugin=$1
    [ -f "$SCREENSHOT_DIR/$plugin/${plugin,,}-main.png" ]
}

# Function to optimize screenshot
optimize_screenshot() {
    local file=$1
    local original_size=$(du -h "$file" | cut -f1)

    print_info "Original size: $original_size"
    sips -Z $TARGET_WIDTH "$file" > /dev/null 2>&1

    local new_size=$(du -h "$file" | cut -f1)
    print_success "Optimized to: $new_size (max width: ${TARGET_WIDTH}px)"
}

# Function to show instructions for one plugin
capture_plugin() {
    local plugin=$1
    local plugin_lower=$(echo "$plugin" | tr '[:upper:]' '[:lower:]')
    local screenshot_path="$SCREENSHOT_DIR/$plugin/${plugin_lower}-main.png"

    print_header "Plugin: $plugin"

    # Check if already has screenshot
    if has_screenshot "$plugin"; then
        print_success "Already has screenshot!"
        echo ""
        return
    fi

    echo ""
    print_info "Steps to capture:"
    echo "  1. Open your DAW (Logic Pro, Ableton, etc.)"
    echo "  2. Load the $plugin plugin (VST3 or AU)"
    echo "  3. Adjust the window to show the full UI clearly"
    echo "  4. Take a screenshot (Cmd+Shift+4, then Space, click window)"
    echo "  5. Screenshot will be saved to Desktop as 'Screenshot YYYY-MM-DD...png'"
    echo ""

    print_info "Waiting for screenshot on Desktop..."
    echo "Press ENTER when you've taken the screenshot, or 's' to skip this plugin:"
    read -r response

    if [ "$response" = "s" ] || [ "$response" = "S" ]; then
        print_info "Skipped $plugin"
        echo ""
        return
    fi

    # Find the most recent screenshot on Desktop
    DESKTOP=~/Desktop
    latest_screenshot=$(ls -t "$DESKTOP"/Screenshot*.png 2>/dev/null | head -1)

    if [ -z "$latest_screenshot" ]; then
        print_error "No screenshot found on Desktop!"
        echo ""
        return
    fi

    print_info "Found: $(basename "$latest_screenshot")"

    # Create plugin directory
    mkdir -p "$SCREENSHOT_DIR/$plugin"

    # Move and rename screenshot
    cp "$latest_screenshot" "$screenshot_path"
    print_success "Moved to: $screenshot_path"

    # Optimize screenshot
    optimize_screenshot "$screenshot_path"

    # Copy to docs/screenshots as backup
    cp "$screenshot_path" "$DOCS_SCREENSHOT_DIR/${plugin}-screenshot.png"
    print_success "Backup saved to: $DOCS_SCREENSHOT_DIR/${plugin}-screenshot.png"

    # Clean up desktop
    print_info "Remove screenshot from Desktop? (y/n):"
    read -r cleanup
    if [ "$cleanup" = "y" ] || [ "$cleanup" = "Y" ]; then
        rm "$latest_screenshot"
        print_success "Desktop cleaned up"
    fi

    echo ""
}

# Main script
clear
print_header "AudioForge Screenshot Capture Tool"
echo ""
echo "This tool will guide you through capturing screenshots for all 18 remaining plugins."
echo ""
echo "📸 Capture Tips:"
echo "  • Use Cmd+Shift+4, then Space, then click the plugin window"
echo "  • Make sure the plugin UI is fully visible"
echo "  • Adjust window size if needed before capturing"
echo "  • Light mode or dark mode - your choice (be consistent!)"
echo ""
echo "Plugins remaining: ${#PLUGINS_NEEDED[@]}"
echo ""
print_info "Press ENTER to start, or Ctrl+C to cancel..."
read -r

# Capture each plugin
for plugin in "${PLUGINS_NEEDED[@]}"; do
    capture_plugin "$plugin"
done

# Summary
print_header "Screenshot Capture Complete!"
echo ""
echo "Screenshots captured and optimized!"
echo ""
echo "Next steps:"
echo "  1. Review screenshots in: $SCREENSHOT_DIR"
echo "  2. Update website/src/lib/plugins.ts to add screenshot paths"
echo "  3. Deploy website with new screenshots"
echo ""
print_success "Total plugins with screenshots: $(ls -1 "$SCREENSHOT_DIR" | wc -l | tr -d ' ')/22"
echo ""
