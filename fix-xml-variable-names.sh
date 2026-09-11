#!/bin/bash
# Fix xml/xmlState variable name mismatches

echo "Fixing xml/xmlState variable naming..."

cd plugins

PLUGINS_TO_FIX=(
    "AcousticBass"
    "AnalogKick"
    "BrassSection"
    "ClassicMonosynth"
    "ElectricPiano"
    "FMSynth"
    "Koto"
    "MIDICCMapper"
    "MIDIChordGenerator"
    "MIDIGrooveQuantizer"
    "MIDIHarmonizer"
    "Polysynth"
    "SamplerPlugin"
    "Shakuhachi"
    "Sitar"
    "SnareSynth"
    "StringEnsemble"
    "XYPadController"
)

for plugin in "${PLUGINS_TO_FIX[@]}"; do
    file="$plugin/Source/PluginProcessor.cpp"
    
    if [ ! -f "$file" ]; then
        echo "  ⚠️  $plugin: file not found"
        continue
    fi
    
    echo "  Fixing $plugin..."
    # Change xmlState-> to xml-> in setStateInformation
    sed -i '' 's/xmlState->getChildByName/xml->getChildByName/g' "$file"
    echo "  ✅ $plugin"
done

echo ""
echo "xml variable naming fixes complete!"
