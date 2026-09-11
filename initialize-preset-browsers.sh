#!/bin/bash
# Initialize presetBrowser_ in editor constructors

echo "Initializing presetBrowser_ members..."

cd plugins

# Plugins that just got presetBrowser_ added
PLUGINS=(
    "DrumSynth"
    "FreezeFX"
    "Gate"
    "Limiter"
    "MultibandCompressor"
    "OrganEmulator"
    "PadSynth"
    "SamplerPlugin"
    "WavetableSynth"
)

for plugin in "${PLUGINS[@]}"; do
    editor_file="$plugin/Source/PluginEditor.cpp"

    if [ ! -f "$editor_file" ]; then
        echo "  ⚠️  $plugin: editor not found"
        continue
    fi

    # Check what processor member name is used
    if grep -q 'processor_' "$editor_file"; then
        PROC_NAME="processor_"
    elif grep -q 'audioProcessor' "$editor_file"; then
        PROC_NAME="audioProcessor"
    elif grep -q ': processor(' "$editor_file"; then
        PROC_NAME="processor"
    else
        echo "  ⚠️  $plugin: cannot determine processor member name"
        continue
    fi

    # Check if presetBrowser_ is already initialized
    if grep -q 'presetBrowser_(' "$editor_file"; then
        echo "  ✓ $plugin: already initialized"
        continue
    fi

    echo "  Initializing presetBrowser_ in $plugin (using $PROC_NAME)..."

    # Find the constructor initialization list and add presetBrowser_
    # Look for the pattern ": AudioProcessorEditor
    # Then add presetBrowser_ initialization

    # Use awk to add the initialization after the last existing member initializer
    awk -v proc="$PROC_NAME" '
        /: AudioProcessorEditor/ {
            in_init = 1
            print
            next
        }
        in_init && /{/ {
            # Found opening brace of constructor body
            print "      presetBrowser_(" proc ".getPresetManager())"
            in_init = 0
            print
            next
        }
        { print }
    ' "$editor_file" > "${editor_file}.tmp" && mv "${editor_file}.tmp" "$editor_file"

    echo "  ✅ $plugin initialized"
done

echo ""
echo "PresetBrowser initialization complete!"
