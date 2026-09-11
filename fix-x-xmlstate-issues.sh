#!/bin/bash
# Fix plugins that use 'x' but access 'xmlState'

echo "Fixing x/xmlState mismatches..."

PLUGINS=(
    "PhaserFlanger"
    "PlateReverb"
    "ShimmerReverb"
    "StereoChorus"
    "TapeDelay"
    "TomSynth"
    "TremoloVibrato"
    "VintageCompressor"
    "Vocoder"
)

for plugin in "${PLUGINS[@]}"; do
    cpp="plugins/$plugin/Source/PluginProcessor.cpp"
    
    if [ -f "$cpp" ]; then
        echo "  Fixing $plugin..."
        # Change xmlState-> to x-> in setStateInformation
        sed -i '' 's/xmlState->getChildByName/x->getChildByName/g' "$cpp"
        # Also fix 'xml->' to 'x->' if it exists  
        sed -i '' 's/\([^a-zA-Z]\)xml->getChildByName/\1x->getChildByName/g' "$cpp"
        echo "  ✅ $plugin"
    fi
done

echo ""
echo "xml/xmlState fixes complete!"
