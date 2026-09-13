#!/bin/bash

echo "📦 Installing AudioForge v2.0.1 (Critical Bug Fix)"
echo "=================================================="
echo ""

VST3_DIR="$HOME/Library/Audio/Plug-Ins/VST3"
AU_DIR="$HOME/Library/Audio/Plug-Ins/Components"

# Ensure directories exist
mkdir -p "$VST3_DIR"
mkdir -p "$AU_DIR"

INSTALLED_VST3=0
INSTALLED_AU=0

# Install all built plugins
for plugin_dir in plugins/*/build; do
    plugin_name=$(basename $(dirname $plugin_dir))
    
    # Check both Release and Debug paths
    for config in Release Debug; do
        # Install VST3
        vst3_path="$plugin_dir/${plugin_name}_artefacts/$config/VST3/${plugin_name}.vst3"
        if [ -d "$vst3_path" ]; then
            cp -r "$vst3_path" "$VST3_DIR/"
            ((INSTALLED_VST3++))
            break
        fi
    done
    
    for config in Release Debug; do
        # Install AU (macOS only)
        au_path="$plugin_dir/${plugin_name}_artefacts/$config/AU/${plugin_name}.component"
        if [ -d "$au_path" ]; then
            cp -r "$au_path" "$AU_DIR/"
            ((INSTALLED_AU++))
            break
        fi
    done
done

echo ""
echo "=================================================="
echo "✅ Installation Complete!"
echo "   VST3 Plugins: $INSTALLED_VST3 → $VST3_DIR"
echo "   AU Plugins: $INSTALLED_AU → $AU_DIR"
echo ""
echo "🧪 Testing Instructions:"
echo "   1. Open Ableton Live (or your preferred DAW)"
echo "   2. Rescan plugins if needed"
echo "   3. Load multiple AudioForge plugins"
echo "   4. Load and switch between presets"
echo "   5. Play audio - crashes should be eliminated"
echo ""
