#!/bin/bash
# Automated Preset System Integration Script
# Integrates PresetManager and PresetBrowser into any AudioForge plugin

set -e

PLUGIN_NAME="$1"

if [ -z "$PLUGIN_NAME" ]; then
    echo "Usage: $0 <PluginName>"
    echo "Example: $0 BasicSynth"
    exit 1
fi

PLUGIN_DIR="plugins/$PLUGIN_NAME"

if [ ! -d "$PLUGIN_DIR" ]; then
    echo "Error: Plugin directory not found: $PLUGIN_DIR"
    exit 1
fi

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "  Integrating Preset System into: $PLUGIN_NAME"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

# 1. Update PluginProcessor.h
echo "[1/6] Updating PluginProcessor.h..."
PROCESSOR_H="$PLUGIN_DIR/Source/PluginProcessor.h"

if [ -f "$PROCESSOR_H" ]; then
    # Add PresetManager include if not already present
    if ! grep -q "PresetManager.h" "$PROCESSOR_H"; then
        # Find the last #include line and add after it
        sed -i '' '/^#include.*MIDILearnManager\.h/a\
#include "../../../shared/preset/PresetManager.h"
' "$PROCESSOR_H"
        echo "  ✓ Added PresetManager include"
    else
        echo "  ⊙ PresetManager include already present"
    fi

    # Add getPresetManager accessor if not present
    if ! grep -q "getPresetManager" "$PROCESSOR_H"; then
        # Add after getMidiLearnManager line
        sed -i '' '/getMidiLearnManager.*{/a\
    AudioForge::PresetManager& getPresetManager() { return presetManager_; }
' "$PROCESSOR_H"
        echo "  ✓ Added getPresetManager() accessor"
    else
        echo "  ⊙ getPresetManager() already present"
    fi

    # Add presetManager_ member variable if not present
    if ! grep -q "presetManager_" "$PROCESSOR_H"; then
        # Add after midiLearnManager_ line
        sed -i '' '/MIDILearnManager midiLearnManager_;/a\
    AudioForge::PresetManager presetManager_;
' "$PROCESSOR_H"
        echo "  ✓ Added presetManager_ member variable"
    else
        echo "  ⊙ presetManager_ already present"
    fi
else
    echo "  ✗ PluginProcessor.h not found"
    exit 1
fi

# 2. Update PluginProcessor.cpp
echo "[2/6] Updating PluginProcessor.cpp..."
PROCESSOR_CPP="$PLUGIN_DIR/Source/PluginProcessor.cpp"

if [ -f "$PROCESSOR_CPP" ]; then
    # Add presetManager initialization in constructor
    if ! grep -q "presetManager_" "$PROCESSOR_CPP"; then
        # Add after midiLearnManager initialization
        sed -i '' "/midiLearnManager_(apvts_)/a\\
      presetManager_(apvts_, \"$PLUGIN_NAME\") {\\
\\
    // Scan for presets on startup\\
    presetManager_.scanPresets();\\
}
" "$PROCESSOR_CPP"

        # Remove old closing brace
        sed -i '' '/midiLearnManager_(apvts_) {}/d' "$PROCESSOR_CPP"

        echo "  ✓ Added presetManager initialization"
    else
        echo "  ⊙ presetManager initialization already present"
    fi

    # Update getStateInformation if needed
    if ! grep -q "presetManager_.saveToXml" "$PROCESSOR_CPP"; then
        sed -i '' '/midiLearnManager_\.saveToXml/a\
    xml->addChildElement(presetManager_.saveToXml().release());
' "$PROCESSOR_CPP"
        echo "  ✓ Updated getStateInformation()"
    else
        echo "  ⊙ getStateInformation() already updated"
    fi

    # Update setStateInformation if needed
    if ! grep -q "presetManager_.loadFromXml" "$PROCESSOR_CPP"; then
        sed -i '' '/midiLearnManager_\.loadFromXml/a\
        if (auto* presetXml = xml->getChildByName("PresetManagerState"))\
            presetManager_.loadFromXml(*presetXml);
' "$PROCESSOR_CPP"
        echo "  ✓ Updated setStateInformation()"
    else
        echo "  ⊙ setStateInformation() already updated"
    fi
else
    echo "  ✗ PluginProcessor.cpp not found"
    exit 1
fi

# 3. Update PluginEditor.h
echo "[3/6] Updating PluginEditor.h..."
EDITOR_H="$PLUGIN_DIR/Source/PluginEditor.h"

if [ -f "$EDITOR_H" ]; then
    # Add PresetBrowser include if not present
    if ! grep -q "PresetBrowser.h" "$EDITOR_H"; then
        sed -i '' '/MIDILearnSlider\.h/a\
#include "../../../shared/preset/PresetBrowser.h"
' "$EDITOR_H"
        echo "  ✓ Added PresetBrowser include"
    else
        echo "  ⊙ PresetBrowser include already present"
    fi

    # Add presetBrowser_ member if not present
    if ! grep -q "presetBrowser_" "$EDITOR_H"; then
        # Add after processor_ reference
        sed -i '' '/processor_;$/a\
\
    // Preset browser\
    AudioForge::PresetBrowser presetBrowser_;
' "$EDITOR_H"
        echo "  ✓ Added presetBrowser_ member variable"
    else
        echo "  ⊙ presetBrowser_ already present"
    fi
else
    echo "  ✗ PluginEditor.h not found"
    exit 1
fi

# 4. Update PluginEditor.cpp
echo "[4/6] Updating PluginEditor.cpp..."
EDITOR_CPP="$PLUGIN_DIR/Source/PluginEditor.cpp"

if [ -f "$EDITOR_CPP" ]; then
    # Add presetBrowser initialization in constructor
    if ! grep -q "presetBrowser_" "$EDITOR_CPP"; then
        # Find constructor and add initialization
        sed -i '' '/processor_(p) {/a\
      presetBrowser_(processor_.getPresetManager()) {
' "$EDITOR_CPP"

        # Remove old opening brace
        sed -i '' 's/processor_(p) {$/processor_(p),/' "$EDITOR_CPP"

        echo "  ✓ Added presetBrowser initialization"
    else
        echo "  ⊙ presetBrowser initialization already present"
    fi

    # Add presetBrowser to visible components if not present
    if ! grep -q "addAndMakeVisible(presetBrowser_)" "$EDITOR_CPP"; then
        # Add after setSize line
        sed -i '' '/setSize(/a\
\
    // Preset browser\
    addAndMakeVisible(presetBrowser_);
' "$EDITOR_CPP"
        echo "  ✓ Added presetBrowser to visible components"
    else
        echo "  ⊙ presetBrowser already visible"
    fi

    # Note: User will need to manually add presetBrowser positioning in resized()
    echo "  ⚠ Manual step: Add presetBrowser_.setBounds() in resized()"
else
    echo "  ✗ PluginEditor.cpp not found"
    exit 1
fi

# 5. Update CMakeLists.txt
echo "[5/6] Updating CMakeLists.txt..."
CMAKE="$PLUGIN_DIR/CMakeLists.txt"

if [ -f "$CMAKE" ]; then
    # Add preset source files if not present
    if ! grep -q "PresetManager.cpp" "$CMAKE"; then
        sed -i '' '/MIDILearnManager\.cpp/a\
    ../../shared/preset/PresetManager.cpp\
    ../../shared/preset/PresetBrowser.cpp
' "$CMAKE"
        echo "  ✓ Added preset source files"
    else
        echo "  ⊙ Preset source files already present"
    fi
else
    echo "  ✗ CMakeLists.txt not found"
    exit 1
fi

# 6. Summary
echo "[6/6] Integration complete!"
echo ""
echo "✓ Preset system integrated into $PLUGIN_NAME"
echo ""
echo "Manual steps required:"
echo "  1. Add presetBrowser_.setBounds() in resized() method"
echo "  2. Increase editor height to accommodate preset browser (+50px)"
echo "  3. Rebuild plugin: cd $PLUGIN_DIR && cmake -B build && cmake --build build"
echo ""
