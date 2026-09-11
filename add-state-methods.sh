#!/bin/bash
# Add missing state save/load methods for PresetManager

echo "Adding state methods to plugins..."

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
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "Processing: $plugin"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

    cpp="plugins/$plugin/Source/PluginProcessor.cpp"

    if [ ! -f "$cpp" ]; then
        echo "  ⚠️  File not found, skipping"
        continue
    fi

    # Check and add to getStateInformation
    if ! grep -q "presetManager_.saveToXml" "$cpp"; then
        echo "  [1/2] Adding to getStateInformation..."

        # Find the line with copyXmlToBinary and add preset save before it
        sed -i '' '/copyXmlToBinary.*xml.*destData/i\
    xml->addChildElement(presetManager_.saveToXml().release());
' "$cpp"
        echo "      ✅ Added preset save"
    else
        echo "  [1/2] getStateInformation already has preset save"
    fi

    # Check and add to setStateInformation
    if ! grep -q "presetManager_.loadFromXml" "$cpp"; then
        echo "  [2/2] Adding to setStateInformation..."

        # Find the last loadFromXml and add preset load after it
        # This is trickier - we need to add after MIDI learn load
        if grep -q "midiLearnManager_.loadFromXml" "$cpp"; then
            sed -i '' '/midiLearnManager_.loadFromXml/a\
        if (auto* presetXml = xml->getChildByName("PresetManagerState"))\
            presetManager_.loadFromXml(*presetXml);
' "$cpp"
        else
            # No MIDI learn, add after the apvts replace
            sed -i '' '/apvts_.replaceState/a\
        if (auto* presetXml = xml->getChildByName("PresetManagerState"))\
            presetManager_.loadFromXml(*presetXml);
' "$cpp"
        fi
        echo "      ✅ Added preset load"
    else
        echo "  [2/2] setStateInformation already has preset load"
    fi

    echo "  ✅ $plugin complete"
    echo ""
done

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "State methods added! Now need to add constructor initialization."
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
