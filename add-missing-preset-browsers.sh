#!/bin/bash
# Add missing PresetBrowser members to editor headers

echo "Adding missing PresetBrowser members..."

cd plugins

# List of APVTS plugins
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

for plugin in "${PLUGINS[@]}"; do
    header_file="$plugin/Source/PluginEditor.h"

    if [ ! -f "$header_file" ]; then
        echo "  ⚠️  $plugin: header not found"
        continue
    fi

    # Check if PresetBrowser include is missing
    if ! grep -q '#include.*PresetBrowser.h' "$header_file"; then
        echo "  Adding PresetBrowser include to $plugin..."
        # Add include after PluginProcessor.h
        sed -i '' '/#include "PluginProcessor.h"/a\
#include "../../../shared/preset/PresetBrowser.h"
' "$header_file"
    fi

    # Check if presetBrowser_ member is missing
    if ! grep -q 'presetBrowser_' "$header_file"; then
        echo "  Adding presetBrowser_ member to $plugin..."
        # Add before JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR
        sed -i '' '/JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR/i\
\
    AudioForge::PresetBrowser presetBrowser_;
' "$header_file"
        echo "  ✅ Added to $plugin"
    fi
done

echo ""
echo "PresetBrowser member addition complete!"
