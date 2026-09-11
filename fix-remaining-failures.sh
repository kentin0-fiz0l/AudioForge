#!/bin/bash
# Fix the remaining 4 failed plugins

echo "Fixing remaining plugin failures..."

cd plugins

# FreezeFX, Gate, Limiter, HiHat - all missing presetManager integration

for plugin in FreezeFX Gate HiHat Limiter; do
    echo "Fixing $plugin..."

    header="$plugin/Source/PluginProcessor.h"
    cpp="$plugin/Source/PluginProcessor.cpp"

    # Add PresetManager include if missing
    if ! grep -q "PresetManager.h" "$header"; then
        sed -i '' '/#include.*PluginProcessor/a\
#include "../../../shared/preset/PresetManager.h"
' "$header"
    fi

    # Add getPresetManager() method if missing
    if ! grep -q "getPresetManager" "$header"; then
        sed -i '' '/getStateInformation/i\
\
    AudioForge::PresetManager& getPresetManager() { return presetManager_; }
' "$header"
    fi

    # Add presetManager_ member if missing
    if ! grep -q "presetManager_" "$header"; then
        sed -i '' '/JUCE_DECLARE_NON_COPYABLE/i\
\
    AudioForge::PresetManager presetManager_;
' "$header"
    fi

    # Add to constructor initialization if missing
    if ! grep -q "presetManager_" "$cpp"; then
        # Find midiLearnManager line and add after it
        sed -i '' '/midiLearnManager_(apvts_)/a\
,\
      presetManager_(apvts_, "'$plugin'")
' "$cpp"

        # Add scanPresets call if missing
        if ! grep -q "presetManager_.scanPresets" "$cpp"; then
            # Find first line after constructor opening brace
            awk '/'"${plugin}"'Processor::'"${plugin}"'Processor\(\)/{flag=1} flag && /{/ && !done{print; print "    // Scan for presets on startup"; print "    presetManager_.scanPresets();"; print ""; done=1; next} 1' "$cpp" > "${cpp}.tmp" && mv "${cpp}.tmp" "$cpp"
        fi
    fi

    echo "  ✅ $plugin"
done

echo ""
echo "Fixes complete! Rebuild these plugins."
