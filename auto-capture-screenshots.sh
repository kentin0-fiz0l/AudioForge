#!/bin/bash
# Automated Screenshot Capture for AudioForge Plugins
# Launches standalone apps and captures screenshots automatically

set -e

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

# Configuration
SCREENSHOT_DIR="website/public/screenshots/plugins"
DOCS_SCREENSHOT_DIR="docs/screenshots"
TARGET_WIDTH=1200
TEMP_SCREENSHOT="/tmp/audioforge_temp_screenshot.png"

# Plugins that need screenshots (format: "PluginFolderName:DisplayName")
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
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
}

print_success() { echo -e "${GREEN}✓${NC} $1"; }
print_error() { echo -e "${RED}✗${NC} $1"; }
print_info() { echo -e "${YELLOW}→${NC} $1"; }

# Check we're in the right directory
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

# Function to check if plugin already has screenshot
has_screenshot() {
    local display_name=$1
    local plugin_lower=$(echo "$display_name" | tr '[:upper:]' '[:lower:]')
    [ -f "$SCREENSHOT_DIR/$display_name/${plugin_lower}-main.png" ]
}

# Function to capture screenshot of a plugin
capture_plugin() {
    local plugin=$1
    local display_name=$2
    local plugin_lower=$(echo "$display_name" | tr '[:upper:]' '[:lower:]')
    local screenshot_path="$SCREENSHOT_DIR/$display_name/${plugin_lower}-main.png"

    print_header "Capturing: $display_name"

    # Check if already has screenshot
    if has_screenshot "$display_name"; then
        print_success "Already has screenshot - skipping"
        echo ""
        return 0
    fi

    # Find standalone app
    local app_path=$(find_app "$plugin")

    if [ -z "$app_path" ]; then
        print_error "Standalone app not found for $plugin"
        print_info "Building standalone app..."

        # Try to build it
        cd "plugins/$plugin"
        if [ ! -d "build" ]; then
            cmake -B build -DCMAKE_BUILD_TYPE=Release > /dev/null 2>&1
        fi
        cmake --build build --config Release --target "${plugin}_Standalone" > /dev/null 2>&1
        cd ../..

        # Try to find it again
        app_path=$(find_app "$plugin")
        if [ -z "$app_path" ]; then
            print_error "Could not build standalone app for $plugin"
            echo ""
            return 1
        fi
    fi

    print_info "Found: $(basename "$app_path")"

    # Launch the app
    print_info "Launching $display_name..."
    open "$app_path"

    # Wait for app to launch and window to appear
    sleep 4

    # Capture the window
    print_info "Capturing screenshot..."

    # Use AppleScript to get the window and capture it
    screencapture -w -o "$TEMP_SCREENSHOT" 2>/dev/null &
    local capture_pid=$!

    # Wait for screencapture (it waits for window click)
    # We'll use a timeout
    local timeout=10
    local elapsed=0
    while kill -0 $capture_pid 2>/dev/null && [ $elapsed -lt $timeout ]; do
        sleep 1
        elapsed=$((elapsed + 1))

        # Try to automate the click using AppleScript
        if [ $elapsed -eq 2 ]; then
            osascript << EOF > /dev/null 2>&1 || true
tell application "System Events"
    set appName to name of first process whose name contains "$plugin"
    tell process appName
        set frontmost to true
        click window 1
    end tell
end tell
EOF
        fi
    done

    # If capture is still running, kill it
    kill $capture_pid 2>/dev/null || true

    # Close the app
    print_info "Closing $display_name..."
    osascript -e "tell application \"$plugin\" to quit" 2>/dev/null || killall "$plugin" 2>/dev/null || true
    sleep 1

    # Check if screenshot was captured
    if [ ! -f "$TEMP_SCREENSHOT" ]; then
        print_error "Failed to capture screenshot"
        echo ""
        return 1
    fi

    # Create plugin directory
    mkdir -p "$SCREENSHOT_DIR/$display_name"

    # Move screenshot to final location
    cp "$TEMP_SCREENSHOT" "$screenshot_path"
    print_success "Saved to: $screenshot_path"

    # Optimize screenshot
    local original_size=$(du -h "$screenshot_path" | cut -f1)
    print_info "Original size: $original_size"

    sips -Z $TARGET_WIDTH "$screenshot_path" > /dev/null 2>&1

    local new_size=$(du -h "$screenshot_path" | cut -f1)
    print_success "Optimized to: $new_size"

    # Create backup
    cp "$screenshot_path" "$DOCS_SCREENSHOT_DIR/${display_name}-screenshot.png"
    print_success "Backup created"

    # Clean up temp file
    rm -f "$TEMP_SCREENSHOT"

    echo ""
    return 0
}

# Main script
clear
print_header "AudioForge Automated Screenshot Capture"
echo ""
echo "This script will automatically:"
echo "  1. Launch each plugin standalone app"
echo "  2. Capture a screenshot of the window"
echo "  3. Optimize and save it"
echo "  4. Close the plugin"
echo ""
echo "Plugins to capture: ${#PLUGINS[@]}"
echo ""
print_info "Starting in 3 seconds... (Press Ctrl+C to cancel)"
sleep 3

# Track statistics
total=0
success=0
skipped=0
failed=0

# Capture each plugin
for entry in "${PLUGINS[@]}"; do
    plugin="${entry%%:*}"
    display_name="${entry##*:}"
    total=$((total + 1))

    # Check if already has screenshot before attempting
    if has_screenshot "$display_name"; then
        print_info "Skipping $display_name (already has screenshot)"
        skipped=$((skipped + 1))
        continue
    fi

    if capture_plugin "$plugin" "$display_name"; then
        success=$((success + 1))
    else
        failed=$((failed + 1))
    fi

    # Small delay between plugins
    sleep 1
done

# Final summary
print_header "Screenshot Capture Complete!"
echo ""
echo "Results:"
echo "  Total plugins processed: $total"
echo "  Successfully captured: $success"
echo "  Already had screenshots: $skipped"
echo "  Failed: $failed"
echo ""

# Count total screenshots now
total_screenshots=$(find "$SCREENSHOT_DIR" -type d -mindepth 1 -maxdepth 1 | wc -l | tr -d ' ')
print_success "Total plugins with screenshots: $total_screenshots/22"
echo ""

if [ $failed -gt 0 ]; then
    print_error "Some plugins failed to capture. You may need to capture them manually."
    echo ""
fi

echo "Next steps:"
echo "  1. Review screenshots in: $SCREENSHOT_DIR"
echo "  2. Update website/src/lib/plugins.ts with screenshot paths"
echo "  3. Deploy website"
echo ""
