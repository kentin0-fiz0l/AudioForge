#!/bin/bash
# Fix all xml variable references to match actual variable names

echo "Fixing xml variable references..."

# Plugins that use xmlState
XMLSTATE_PLUGINS=(
    "AcousticBass"
    "BrassSection"
    "ClassicMonosynth"
    "DrumSynth"
    "ElectricPiano"
    "FreezeFX"
    "Gate"
    "Koto"
    "MultibandCompressor"
    "OrganEmulator"
    "PadSynth"
    "Polysynth"
    "SamplerPlugin"
    "Shakuhachi"
    "Sitar"
    "StringEnsemble"
    "WavetableSynth"
)

# Plugins that use xml (already correct)
XML_PLUGINS=(
    "AnalogKick"
    "FMSynth"
    "MacroController"
    "MIDIArpeggiator"
    "MIDICCMapper"
    "MIDIChordGenerator"
    "MIDIGrooveQuantizer"
    "MIDIHarmonizer"
    "SnareSynth"
    "XYPadController"
)

# Fix xmlState plugins - change xml-> back to xmlState->
for plugin in "${XMLSTATE_PLUGINS[@]}"; do
    file="plugins/$plugin/Source/PluginProcessor.cpp"
    if [ -f "$file" ]; then
        echo "  Fixing $plugin (xml-> → xmlState->)..."
        sed -i '' 's/xml->getChildByName/xmlState->getChildByName/g' "$file"
    fi
done

# Fix AutoPanner which uses 'x'
echo "  Fixing AutoPanner (xmlState-> → x->)..."
sed -i '' 's/xmlState->getChildByName/x->getChildByName/g' "plugins/AutoPanner/Source/PluginProcessor.cpp"

# Check for other special cases
for dir in plugins/*/; do
    plugin=$(basename "$dir")
    file="$dir/Source/PluginProcessor.cpp"

    [ -f "$file" ] || continue

    # Check if uses a different variable name
    if grep -q 'std::unique_ptr<juce::XmlElement> [a-z][a-z]*(' "$file"; then
        var_name=$(grep 'std::unique_ptr<juce::XmlElement> ' "$file" | grep 'getXmlFromBinary' | sed 's/.*<juce::XmlElement> \([a-z][a-zA-Z]*\)(.*/\1/')

        # Check if child access uses a different variable
        if grep -q '\(xml\|xmlState\|x\)->getChildByName' "$file"; then
            actual_var=$(grep 'getChildByName' "$file" | head -1 | sed 's/.*if (auto\* [a-z]*Xml = \([a-z][a-zA-Z]*\)->.*/\1/')

            if [ "$var_name" != "$actual_var" ] && [ ! -z "$var_name" ] && [ ! -z "$actual_var" ]; then
                echo "  ⚠️  $plugin: uses '$var_name' but accesses via '$actual_var'"
            fi
        fi
    fi
done

echo ""
echo "XML variable fixes complete!"
