#!/bin/bash
# Build only the 21 previously failed plugins

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "  Building 21 Previously Failed Plugins"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

FAILED_PLUGINS=(
    "DrumSynth"
    "FMSynth"
    "FreezeFX"
    "Gate"
    "HiHat"
    "Limiter"
    "MultibandCompressor"
    "OrganEmulator"
    "PadSynth"
    "PhaserFlanger"
    "PlateReverb"
    "SamplerPlugin"
    "ShimmerReverb"
    "StereoChorus"
    "TapeDelay"
    "TomSynth"
    "TremoloVibrato"
    "VintageCompressor"
    "Vocoder"
    "WavetableSynth"
    "XYPadController"
)

SUCCESS=0
FAILED=0
START_TIME=$(date +%s)

cd plugins

for plugin in "${FAILED_PLUGINS[@]}"; do
    echo "Building: $plugin..."

    if [ ! -d "$plugin" ]; then
        echo "  ⚠️  Not found, skipping"
        continue
    fi

    cd "$plugin"
    rm -rf build

    # Configure
    if ! cmake -B build -DCMAKE_BUILD_TYPE=Release > build_config.log 2>&1; then
        echo "  ❌ Configuration failed"
        FAILED=$((FAILED + 1))
        cd ..
        continue
    fi

    # Build
    if ! cmake --build build --config Release > build_compile.log 2>&1; then
        echo "  ❌ Build failed - see plugins/$plugin/build_compile.log"
        # Show first error
        grep -E "error:" build_compile.log | head -1 | sed 's/^/    /'
        FAILED=$((FAILED + 1))
        cd ..
        continue
    fi

    # Verify
    VST3=$(find build -name "*.vst3" -type d | head -1)
    if [ -z "$VST3" ]; then
        echo "  ❌ No VST3 found"
        FAILED=$((FAILED + 1))
        cd ..
        continue
    fi

    echo "  ✅ SUCCESS"
    SUCCESS=$((SUCCESS + 1))
    cd ..
done

END_TIME=$(date +%s)
DURATION=$((END_TIME - START_TIME))

cd ..

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "  Build Summary"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
echo "  Total: 21 plugins"
echo "  ✅ Success: $SUCCESS"
echo "  ❌ Failed: $FAILED"
echo "  ⏱️  Time: ${DURATION}s"
echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
