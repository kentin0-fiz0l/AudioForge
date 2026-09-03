#!/bin/bash
#
# PluginVal Validation Script for AudioForge
# Runs Steinberg's pluginval in strict mode against all installed VST3 plugins
#

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

PLUGINVAL="/private/tmp/pluginval.app/Contents/MacOS/pluginval"
VST3_DIR="$HOME/Library/Audio/Plug-Ins/VST3"
RESULTS_DIR="$(cd "$(dirname "$0")/.." && pwd)/docs/validation"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

# Create results directory
mkdir -p "$RESULTS_DIR"

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}AudioForge PluginVal Validation${NC}"
echo -e "${BLUE}Timestamp: $(date)${NC}"
echo -e "${BLUE}======================================${NC}"
echo

# AudioForge plugins to validate
PLUGINS=(
    "BasicSynth.vst3"
    "ChorusFlanger.vst3"
    "CleanDelay.vst3"
    "DrumSynth.vst3"
    "FMSynth.vst3"
    "FreezeFX.vst3"
    "Gate.vst3"
    "GranularEngine.vst3"
    "Limiter.vst3"
    "Multiband Compressor.vst3"
    "OrganEmulator.vst3"
    "PadSynth.vst3"
    "PanUtil.vst3"
    "Reverb.vst3"
    "Sampler.vst3"
    "Saturation.vst3"
    "SimpleComp.vst3"
    "SimpleEQ.vst3"
    "SimpleGain.vst3"
    "SpectralFreeze.vst3"
    "WaveShaper.vst3"
    "WavetableSynth.vst3"
)

TOTAL=${#PLUGINS[@]}
PASSED=0
FAILED=0
WARNINGS=0

# Summary file
SUMMARY_FILE="$RESULTS_DIR/validation_summary_$TIMESTAMP.txt"
echo "AudioForge PluginVal Validation Summary" > "$SUMMARY_FILE"
echo "Timestamp: $(date)" >> "$SUMMARY_FILE"
echo "PluginVal version: $(\"$PLUGINVAL\" --version 2>&1)" >> "$SUMMARY_FILE"
echo "========================================" >> "$SUMMARY_FILE"
echo >> "$SUMMARY_FILE"

for plugin in "${PLUGINS[@]}"; do
    PLUGIN_PATH="$VST3_DIR/$plugin"

    if [ ! -d "$PLUGIN_PATH" ]; then
        echo -e "${YELLOW}⚠  Plugin not found: $plugin${NC}"
        echo "SKIP: $plugin (not found)" >> "$SUMMARY_FILE"
        continue
    fi

    echo -e "${BLUE}→ Validating: $plugin${NC}"

    # Output file for this plugin
    PLUGIN_LOG="$RESULTS_DIR/${plugin%.vst3}_$TIMESTAMP.log"

    # Run PluginVal in strict mode
    # --strictness-level 10 = highest validation level
    # --validate-in-process = run in same process (faster, but less isolation)
    # --timeout-ms 30000 = 30 second timeout per test

    if "$PLUGINVAL" --strictness-level 10 \
                     --validate-in-process \
                     --timeout-ms 30000 \
                     --output-dir "$RESULTS_DIR" \
                     "$PLUGIN_PATH" > "$PLUGIN_LOG" 2>&1; then
        echo -e "${GREEN}  ✓ PASSED${NC}"
        PASSED=$((PASSED + 1))
        echo "PASS: $plugin" >> "$SUMMARY_FILE"
    else
        EXIT_CODE=$?
        if grep -q "warning" "$PLUGIN_LOG"; then
            echo -e "${YELLOW}  ⚠ WARNINGS${NC}"
            WARNINGS=$((WARNINGS + 1))
            echo "WARN: $plugin (check $PLUGIN_LOG)" >> "$SUMMARY_FILE"
        else
            echo -e "${RED}  ✗ FAILED${NC}"
            FAILED=$((FAILED + 1))
            echo "FAIL: $plugin (exit code $EXIT_CODE, see $PLUGIN_LOG)" >> "$SUMMARY_FILE"

            # Show first few lines of error
            echo -e "${RED}  Error excerpt:${NC}"
            head -20 "$PLUGIN_LOG" | sed 's/^/    /'
        fi
    fi
    echo
done

# Final summary
echo >> "$SUMMARY_FILE"
echo "========================================" >> "$SUMMARY_FILE"
echo "SUMMARY:" >> "$SUMMARY_FILE"
echo "  Total plugins: $TOTAL" >> "$SUMMARY_FILE"
echo "  Passed: $PASSED" >> "$SUMMARY_FILE"
echo "  Warnings: $WARNINGS" >> "$SUMMARY_FILE"
echo "  Failed: $FAILED" >> "$SUMMARY_FILE"
echo >> "$SUMMARY_FILE"

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}Validation Summary:${NC}"
echo -e "${BLUE}  Total:    $TOTAL${NC}"
echo -e "${GREEN}  Passed:   $PASSED${NC}"
echo -e "${YELLOW}  Warnings: $WARNINGS${NC}"
echo -e "${RED}  Failed:   $FAILED${NC}"
echo -e "${BLUE}======================================${NC}"
echo
echo -e "${BLUE}Results saved to: $RESULTS_DIR/${NC}"
echo -e "${BLUE}Summary: $SUMMARY_FILE${NC}"

if [ $FAILED -gt 0 ]; then
    echo
    echo -e "${RED}⚠ Some plugins failed validation. Check logs in $RESULTS_DIR/${NC}"
    exit 1
elif [ $WARNINGS -gt 0 ]; then
    echo
    echo -e "${YELLOW}⚠ Some plugins have warnings. Review logs in $RESULTS_DIR/${NC}"
    exit 0
else
    echo
    echo -e "${GREEN}✓ All plugins passed strict validation!${NC}"
    exit 0
fi
