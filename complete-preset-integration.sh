#!/bin/bash
# Complete PresetManager integration for plugins missing it

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "  Complete PresetManager Integration"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

PLUGINS=(
    "Gate"
    "HiHat"
    "Limiter"
    "MultibandCompressor"
    "OrganEmulator"
    "PadSynth"
    "SamplerPlugin"
    "WavetableSynth"
)

for plugin in "${PLUGINS[@]}"; do
    echo "Processing $plugin..."

    header="plugins/$plugin/Source/PluginProcessor.h"
    cpp="plugins/$plugin/Source/PluginProcessor.cpp"

    if [ ! -f "$header" ] || [ ! -f "$cpp" ]; then
        echo "  ⚠️  Files not found, skipping"
        continue
    fi

    # 1. Add includes to header if missing
    if ! grep -q "MIDILearnManager.h" "$header"; then
        echo "  [1/6] Adding includes..."
        # Find the last #include and add after it
        lastInclude=$(grep -n "^#include" "$header" | tail -1 | cut -d: -f1)
        if [ ! -z "$lastInclude" ]; then
            sed -i '' "${lastInclude}a\\
#include \"../../../midi/MIDILearnManager.h\"\\
#include \"../../../shared/preset/PresetManager.h\"
" "$header"
        fi
    else
        echo "  [1/6] Includes already present"
    fi

    # 2. Add accessor methods if missing
    if ! grep -q "getPresetManager" "$header"; then
        echo "  [2/6] Adding accessor methods..."
        # Find getAPVTS or getStateInformation and add before it
        if grep -q "getStateInformation" "$header"; then
            sed -i '' '/getStateInformation/i\
    AudioForge::MIDILearnManager& getMIDILearnManager() { return midiLearnManager_; }\
    AudioForge::PresetManager& getPresetManager() { return presetManager_; }\

' "$header"
        fi
    else
        echo "  [2/6] Accessors already present"
    fi

    # 3. Add member variables if missing
    if ! grep -q "presetManager_" "$header"; then
        echo "  [3/6] Adding member variables..."
        # Add before JUCE_DECLARE_NON_COPYABLE
        sed -i '' '/JUCE_DECLARE_NON_COPYABLE/i\
\
    AudioForge::MIDILearnManager midiLearnManager_;\
    AudioForge::PresetManager presetManager_;
' "$header"
    else
        echo "  [3/6] Members already present"
    fi

    # 4. Add constructor initialization if missing
    if ! grep -q "presetManager_.*apvts" "$cpp"; then
        echo "  [4/6] Adding constructor initialization..."
        echo "      (Manual verification required)"
    else
        echo "  [4/6] Constructor init already present"
    fi

    # 5. Update getStateInformation if missing
    if ! grep -q "presetManager_.saveToXml" "$cpp"; then
        echo "  [5/6] Needs getStateInformation update"
        echo "      (Manual verification required)"
    else
        echo "  [5/6] getStateInformation already updated"
    fi

    # 6. Update setStateInformation if missing
    if ! grep -q "presetManager_.loadFromXml" "$cpp"; then
        echo "  [6/6] Needs setStateInformation update"
        echo "      (Manual verification required)"
    else
        echo "  [6/6] setStateInformation already updated"
    fi

    echo "  ✅ $plugin processed"
    echo ""
done

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "Integration complete! Check plugins that need manual work."
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
