#!/bin/bash
# Verify installed AudioForge plugins

VST3_DIR="$HOME/Library/Audio/Plug-Ins/VST3"

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "  AudioForge Installation Verification"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
echo "📁 VST3 Directory: $VST3_DIR"
echo ""

if [ ! -d "$VST3_DIR" ]; then
    echo "❌ VST3 directory not found!"
    exit 1
fi

# Expected plugins (39 with presets)
EXPECTED=(
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

INSTALLED=0
MISSING=0
TOTAL_SIZE=0

for plugin in "${EXPECTED[@]}"; do
    vst3_path="$VST3_DIR/${plugin}.vst3"

    if [ -d "$vst3_path" ]; then
        size=$(du -sk "$vst3_path" | cut -f1)
        size_mb=$((size / 1024))
        TOTAL_SIZE=$((TOTAL_SIZE + size))

        # Check for preset support
        if find "$vst3_path" -name "*.dylib" -o -name "*.so" -o -name "*.dll" 2>/dev/null | head -1 | xargs strings 2>/dev/null | grep -q "PresetManager"; then
            preset_status="✓ Presets"
        else
            preset_status="○ No presets"
        fi

        printf "  ✅ %-25s %4d MB   %s\n" "$plugin" "$size_mb" "$preset_status"
        INSTALLED=$((INSTALLED + 1))
    else
        echo "  ❌ $plugin - NOT INSTALLED"
        MISSING=$((MISSING + 1))
    fi
done

TOTAL_SIZE_MB=$((TOTAL_SIZE / 1024))

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "  Summary"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
echo "  ✅ Installed: $INSTALLED / ${#EXPECTED[@]} plugins"
echo "  ❌ Missing: $MISSING plugins"
echo "  💾 Total size: ${TOTAL_SIZE_MB} MB"
echo ""

if [ $INSTALLED -eq ${#EXPECTED[@]} ]; then
    echo "  🎉 ALL PLUGINS INSTALLED!"
elif [ $INSTALLED -gt 0 ]; then
    echo "  ⚠️  Some plugins missing - check build logs"
else
    echo "  ❌ No plugins installed - run install-all-plugins.sh"
fi

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
