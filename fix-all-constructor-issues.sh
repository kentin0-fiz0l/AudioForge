#!/bin/bash
# Fix all constructor initialization issues

echo "Fixing constructor initialization syntax..."

cd plugins

# Fix pattern: midiLearnManager_(apvts_) {
#              presetManager_(apvts_, "Name") {
# Should be:   midiLearnManager_(apvts_),
#              presetManager_(apvts_, "Name") {

for plugin in */; do
    plugin_name="${plugin%/}"
    processor_file="$plugin_name/Source/PluginProcessor.cpp"

    if [ ! -f "$processor_file" ]; then
        continue
    fi

    echo "Checking $plugin_name..."

    # Check if the malformed pattern exists
    if grep -q 'midiLearnManager_(apvts_) {' "$processor_file"; then
        echo "  Fixing $plugin_name constructor..."

        # Fix the pattern - change the { to , on the midiLearnManager line
        sed -i '' 's/midiLearnManager_(apvts_) {/midiLearnManager_(apvts_),/' "$processor_file"

        # Now the presetManager line should already have {
        # Find the presetManager_.scanPresets(); call and remove the closing } before it
        # Pattern: we need to find:
        #     presetManager_.scanPresets();
        # }
        # And remove the } line

        # Use perl for multi-line replacement
        perl -i -0pe 's/presetManager_\.scanPresets\(\);\n\}/presetManager_.scanPresets();/g' "$processor_file"

        echo "  ✅ Fixed $plugin_name"
    fi
done

echo ""
echo "Constructor fixes complete!"
