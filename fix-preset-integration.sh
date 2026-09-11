#!/bin/bash
# Fix remaining preset integration issues across all plugins

PLUGIN_NAME="$1"

if [ -z "$PLUGIN_NAME" ]; then
    echo "Usage: $0 <PluginName>"
    exit 1
fi

PLUGIN_DIR="plugins/$PLUGIN_NAME"
echo "Fixing: $PLUGIN_NAME"

# 1. Add presetManager_ member variable to PluginProcessor.h
PROCESSOR_H="$PLUGIN_DIR/Source/PluginProcessor.h"
if [ -f "$PROCESSOR_H" ]; then
    # Check if member is missing
    if ! grep -q "AudioForge::PresetManager presetManager_;" "$PROCESSOR_H"; then
        # Add after midiLearnManager_
        sed -i '' '/AudioForge::MIDILearnManager midiLearnManager_;/a\
    AudioForge::PresetManager presetManager_;
' "$PROCESSOR_H"
        echo "  ✓ Added presetManager_ member variable"
    else
        echo "  ⊙ presetManager_ already present"
    fi
fi

# 2. Fix constructor in PluginProcessor.cpp - ensure midiLearnManager_ is initialized
PROCESSOR_CPP="$PLUGIN_DIR/Source/PluginProcessor.cpp"
if [ -f "$PROCESSOR_CPP" ]; then
    # Check if midiLearnManager_ is missing from init list but presetManager_ exists
    if grep -q "presetManager_(apvts_" "$PROCESSOR_CPP"; then
        if ! grep -B1 "presetManager_(apvts_" "$PROCESSOR_CPP" | grep -q "midiLearnManager_(apvts_)"; then
            # Add midiLearnManager before presetManager
            sed -i '' 's/presetManager_(apvts_/midiLearnManager_(apvts_),\
      presetManager_(apvts_/' "$PROCESSOR_CPP"
            echo "  ✓ Fixed constructor initialization order"
        else
            echo "  ⊙ Constructor already correct"
        fi
    fi
fi

# 3. Increase editor height in PluginEditor.cpp (if setSize exists)
EDITOR_CPP="$PLUGIN_DIR/Source/PluginEditor.cpp"
if [ -f "$EDITOR_CPP" ]; then
    # Find setSize and increase height by 50 if not already done
    if grep -q "setSize(" "$EDITOR_CPP"; then
        # Check if comment about preset browser exists
        if ! grep -q "Increased for preset browser" "$EDITOR_CPP"; then
            # Increase height values by 50
            sed -i '' 's/setSize(\([0-9]\+\), \([0-9]\+\))/setSize(\1, \2 + 50) \/\/ Increased for preset browser/' "$EDITOR_CPP"
            echo "  ✓ Increased editor height"
        else
            echo "  ⊙ Editor height already increased"
        fi
    fi
fi

echo "  ✅ $PLUGIN_NAME fixed"
