#!/bin/bash
# AudioForge Automated Smoke Test
# Verifies all VST3 plugins are properly installed and valid

# Don't exit on error - we want to test all plugins
set +e

PLUGIN_DIR=~/Library/Audio/Plug-Ins/VST3
AUDIOFORGE_PLUGINS=(
    "AcousticBass.vst3"
    "BasicSynth.vst3"
    "BrassSection.vst3"
    "ChorusFlanger.vst3"
    "Classic Monosynth.vst3"
    "CleanDelay.vst3"
    "DrumSynth.vst3"
    "ElectricPiano.vst3"
    "FreezeFX.vst3"
    "Gate.vst3"
    "GranularEngine.vst3"
    "Koto.vst3"
    "Limiter.vst3"
    "Macro Controller.vst3"
    "MIDI Arpeggiator.vst3"
    "MIDI CC Mapper.vst3"
    "MIDI Chord Generator.vst3"
    "MIDI Groove Quantizer.vst3"
    "MIDI Harmonizer.vst3"
    "Multiband Compressor.vst3"
    "OrganEmulator.vst3"
    "PadSynth.vst3"
    "PanUtil.vst3"
    "Polysynth.vst3"
    "Reverb.vst3"
    "Sampler.vst3"
    "Saturation.vst3"
    "Shakuhachi.vst3"
    "SimpleComp.vst3"
    "SimpleEQ.vst3"
    "SimpleGain.vst3"
    "Sitar.vst3"
    "SpectralFreeze.vst3"
    "StringEnsemble.vst3"
    "WaveShaper.vst3"
    "WavetableSynth.vst3"
    "XY Pad Controller.vst3"
)

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "  AudioForge Smoke Test"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
echo "Testing ${#AUDIOFORGE_PLUGINS[@]} plugins..."
echo ""

PASSED=0
FAILED=0
MISSING=0

for plugin in "${AUDIOFORGE_PLUGINS[@]}"; do
    PLUGIN_PATH="$PLUGIN_DIR/$plugin"

    if [ -d "$PLUGIN_PATH" ]; then
        # Check if bundle structure is valid
        if [ -f "$PLUGIN_PATH/Contents/Info.plist" ]; then
            # Check if binary exists
            BINARY_PATH=$(find "$PLUGIN_PATH/Contents/MacOS" -type f -perm +111 2>/dev/null | head -n 1)

            if [ -n "$BINARY_PATH" ]; then
                # Check if binary is properly signed (ad-hoc signature is OK)
                codesign -v "$BINARY_PATH" 2>/dev/null
                SIGNATURE_STATUS=$?

                if [ $SIGNATURE_STATUS -eq 0 ]; then
                    echo "✅ PASS: $plugin"
                    PASSED=$((PASSED + 1))
                else
                    echo "⚠️  WARN: $plugin (signature invalid - may still work)"
                    PASSED=$((PASSED + 1))
                fi
            else
                echo "❌ FAIL: $plugin (no executable binary)"
                FAILED=$((FAILED + 1))
            fi
        else
            echo "❌ FAIL: $plugin (no Info.plist)"
            FAILED=$((FAILED + 1))
        fi
    else
        echo "❌ MISSING: $plugin (not installed)"
        MISSING=$((MISSING + 1))
    fi
done

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "  Test Results"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
echo "Total Plugins: ${#AUDIOFORGE_PLUGINS[@]}"
echo "✅ Passed:     $PASSED"
echo "❌ Failed:     $FAILED"
echo "❌ Missing:    $MISSING"
echo ""

if [ $FAILED -eq 0 ] && [ $MISSING -eq 0 ]; then
    echo "🎉 ALL TESTS PASSED! All plugins are properly installed."
    echo ""
    echo "Next steps:"
    echo "  1. Open your DAW (Logic, Ableton, Reaper)"
    echo "  2. Rescan plugins if needed"
    echo "  3. Follow TESTING_QUICKSTART.md for manual testing"
    echo ""
    exit 0
else
    echo "⚠️  ISSUES FOUND!"
    echo ""
    if [ $MISSING -gt 0 ]; then
        echo "Missing plugins should be rebuilt and reinstalled."
    fi
    if [ $FAILED -gt 0 ]; then
        echo "Failed plugins have structural issues - check build logs."
    fi
    echo ""
    exit 1
fi
