#!/bin/bash
# Validate all AudioForge plugins with PluginVal
# Strictness level 5 (default) - suitable for DAW compatibility

set -e

PLUGINVAL="/private/tmp/pluginval.app/Contents/MacOS/pluginval"
VST3_DIR="$HOME/Library/Audio/Plug-Ins/VST3"
RESULTS_DIR="/Users/kentino/Projects/Active/AudioForge/validation-results"

# Create results directory
mkdir -p "$RESULTS_DIR"

# Color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo "========================================="
echo "AudioForge Plugin Validation (PluginVal)"
echo "========================================="
echo ""

# AudioForge plugins
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
CURRENT=0

for PLUGIN in "${PLUGINS[@]}"; do
    CURRENT=$((CURRENT + 1))
    PLUGIN_PATH="$VST3_DIR/$PLUGIN"

    if [ ! -d "$PLUGIN_PATH" ]; then
        echo -e "${YELLOW}[${CURRENT}/${TOTAL}] Skipping ${PLUGIN} (not found)${NC}"
        continue
    fi

    echo -e "${YELLOW}[${CURRENT}/${TOTAL}] Validating ${PLUGIN}...${NC}"

    # Run PluginVal with timeout (5 minutes per plugin)
    LOG_FILE="$RESULTS_DIR/${PLUGIN%.vst3}.log"

    if gtimeout 300 "$PLUGINVAL" --strictness-level 5 --validate "$PLUGIN_PATH" > "$LOG_FILE" 2>&1; then
        # Check if validation actually passed
        if grep -q "^SUCCESS$" "$LOG_FILE"; then
            echo -e "${GREEN}  ✓ PASSED${NC}"
            PASSED=$((PASSED + 1))
        else
            echo -e "${RED}  ✗ FAILED (see $LOG_FILE)${NC}"
            FAILED=$((FAILED + 1))
        fi
    else
        echo -e "${RED}  ✗ FAILED (timeout or crash)${NC}"
        FAILED=$((FAILED + 1))
    fi
done

echo ""
echo "========================================="
echo "Validation Summary"
echo "========================================="
echo -e "${GREEN}Passed: ${PASSED}/${TOTAL}${NC}"
echo -e "${RED}Failed: ${FAILED}/${TOTAL}${NC}"
echo ""
echo "Detailed logs: $RESULTS_DIR"

if [ $FAILED -gt 0 ]; then
    echo ""
    echo "To view failed plugin logs:"
    for PLUGIN in "${PLUGINS[@]}"; do
        LOG_FILE="$RESULTS_DIR/${PLUGIN%.vst3}.log"
        if [ -f "$LOG_FILE" ] && ! grep -q "^SUCCESS$" "$LOG_FILE" 2>/dev/null; then
            echo "  cat $LOG_FILE | grep -A 5 FAIL"
        fi
    done
    exit 1
fi

echo -e "${GREEN}✓ All plugins passed PluginVal validation!${NC}"
