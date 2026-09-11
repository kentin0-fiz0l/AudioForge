#!/bin/bash
# Mass build all 39 preset-integrated plugins

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "  AudioForge Mass Build - All Preset Plugins"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

# List of all APVTS-ready plugins with presets
PLUGINS=(
    "AcousticBass"
    "AnalogKick"
    "AutoPanner"
    "BrassSection"
    "ClassicMonosynth"
    "DrumSynth"
    "ElectricPiano"
    "FMSynth"
    "FreezeFX"
    "Gate"
    "HiHat"
    "Koto"
    "Limiter"
    "MacroController"
    "MIDIArpeggiator"
    "MIDICCMapper"
    "MIDIChordGenerator"
    "MIDIGrooveQuantizer"
    "MIDIHarmonizer"
    "MultibandCompressor"
    "OrganEmulator"
    "PadSynth"
    "PhaserFlanger"
    "PlateReverb"
    "Polysynth"
    "SamplerPlugin"
    "Shakuhachi"
    "ShimmerReverb"
    "Sitar"
    "SnareSynth"
    "StereoChorus"
    "StringEnsemble"
    "TapeDelay"
    "TomSynth"
    "TremoloVibrato"
    "VintageCompressor"
    "Vocoder"
    "WavetableSynth"
    "XYPadController"
)

TOTAL=${#PLUGINS[@]}
SUCCESS=0
FAILED=0
SKIPPED=0

START_TIME=$(date +%s)

cd plugins

for plugin in "${PLUGINS[@]}"; do
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "  Building: $plugin"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

    if [ ! -d "$plugin" ]; then
        echo "  ⚠️  Plugin directory not found, skipping"
        SKIPPED=$((SKIPPED + 1))
        continue
    fi

    cd "$plugin"

    # Clean previous build
    rm -rf build

    # Configure
    echo "  [1/3] Configuring..."
    if ! cmake -B build -DCMAKE_BUILD_TYPE=Release > build_config.log 2>&1; then
        echo "  ❌ Configuration failed - see plugins/$plugin/build_config.log"
        FAILED=$((FAILED + 1))
        cd ..
        continue
    fi

    # Build
    echo "  [2/3] Building..."
    if ! cmake --build build --config Release > build_compile.log 2>&1; then
        echo "  ❌ Build failed - see plugins/$plugin/build_compile.log"
        # Show last few error lines
        echo "  Last errors:"
        grep -E "error:" build_compile.log | tail -3 | sed 's/^/    /'
        FAILED=$((FAILED + 1))
        cd ..
        continue
    fi

    # Verify VST3 was created
    echo "  [3/3] Verifying..."
    VST3_FOUND=$(find build -name "*.vst3" -type d | head -1)

    if [ -z "$VST3_FOUND" ]; then
        echo "  ❌ VST3 not found after build"
        FAILED=$((FAILED + 1))
        cd ..
        continue
    fi

    echo "  ✅ SUCCESS: $plugin"
    SUCCESS=$((SUCCESS + 1))

    cd ..
done

END_TIME=$(date +%s)
DURATION=$((END_TIME - START_TIME))

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "  Build Summary"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
echo "  Total Plugins: $TOTAL"
echo "  ✅ Successful: $SUCCESS"
echo "  ❌ Failed: $FAILED"
echo "  ⊙ Skipped: $SKIPPED"
echo ""
echo "  Build Time: ${DURATION}s ($(($DURATION / 60))m $(($DURATION % 60))s)"
echo ""

if [ $FAILED -eq 0 ]; then
    echo "  🎉 ALL BUILDS SUCCESSFUL!"
    echo ""
    echo "  Ready for installation!"
else
    echo "  ⚠️  Some builds failed - check logs above"
fi

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
