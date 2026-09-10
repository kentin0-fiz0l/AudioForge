#!/bin/bash
# AudioForge MIDI Learn Integration Script
# Automates adding MIDI learn to existing plugins

PLUGIN_NAME=$1

if [ -z "$PLUGIN_NAME" ]; then
    echo "Usage: ./integrate_midi_learn.sh <PluginName>"
    echo "Example: ./integrate_midi_learn.sh FMSynth"
    exit 1
fi

PLUGIN_DIR="plugins/$PLUGIN_NAME"

if [ ! -d "$PLUGIN_DIR" ]; then
    echo "Error: Plugin directory $PLUGIN_DIR not found"
    exit 1
fi

echo "🎹 Integrating MIDI Learn into $PLUGIN_NAME..."

# Step 1: Update PluginProcessor.h
PROCESSOR_H="$PLUGIN_DIR/Source/PluginProcessor.h"

if [ -f "$PROCESSOR_H" ]; then
    echo "  ✓ Updating PluginProcessor.h..."

    # Check if already integrated
    if grep -q "MIDILearnManager" "$PROCESSOR_H"; then
        echo "  ⚠ MIDI Learn already integrated in $PLUGIN_NAME"
        exit 0
    fi

    # Add include after last #include
    sed -i '' '/^#include.*$/a\
#include "../../../midi/MIDILearnManager.h"
' "$PROCESSOR_H"

    # Add getMidiLearnManager() accessor before private:
    sed -i '' '/^private:$/i\
    AudioForge::MIDILearnManager& getMidiLearnManager() { return midiLearnManager_; }\

' "$PROCESSOR_H"

    # Add member variable after last member
    sed -i '' '/JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR/i\
    AudioForge::MIDILearnManager midiLearnManager_;\

' "$PROCESSOR_H"
fi

# Step 2: Update CMakeLists.txt
CMAKE_FILE="$PLUGIN_DIR/CMakeLists.txt"

if [ -f "$CMAKE_FILE" ]; then
    echo "  ✓ Updating CMakeLists.txt..."

    # Add MIDI source files to target_sources
    sed -i '' '/Source\/VocoderEngine.cpp/d' "$CMAKE_FILE"
    sed -i '' '/target_sources.*PRIVATE/a\
    ../../midi/MIDILearnManager.cpp\

' "$CMAKE_FILE"
fi

echo "  ✓ Manual steps required:"
echo "    1. Update PluginProcessor.cpp constructor to initialize midiLearnManager_(apvts_)"
echo "    2. Update processBlock() to process MIDI through learn manager"
echo "    3. Update getStateInformation() to save MIDI mappings"
echo "    4. Update setStateInformation() to load MIDI mappings"
echo ""
echo "📝 See MIDI_LEARN_DEMO.md for integration template"
