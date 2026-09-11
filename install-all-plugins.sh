#!/bin/bash
# Install all successfully built AudioForge plugins

set -e

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "  AudioForge Plugin Installation"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

# Installation target
VST3_DIR="$HOME/Library/Audio/Plug-Ins/VST3"
BACKUP_DIR="$HOME/Library/Audio/Plug-Ins/VST3_Backup_$(date +%Y%m%d_%H%M%S)"

# Ensure VST3 directory exists
mkdir -p "$VST3_DIR"

echo "📁 Installation Directory: $VST3_DIR"
echo ""

# Scan for built plugins
INSTALLED=0
SKIPPED=0
BACKED_UP=0

cd plugins

for plugin_dir in */; do
    plugin_name="${plugin_dir%/}"

    # Find VST3 bundle in build directory
    vst3_bundle=$(find "$plugin_name/build" -name "*.vst3" -type d 2>/dev/null | head -1)

    if [ -z "$vst3_bundle" ]; then
        # No VST3 found - plugin didn't build
        continue
    fi

    vst3_name=$(basename "$vst3_bundle")
    target_path="$VST3_DIR/$vst3_name"

    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "  Installing: $plugin_name"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

    # Backup existing version if it exists
    if [ -d "$target_path" ]; then
        echo "  📦 Backing up existing version..."
        mkdir -p "$BACKUP_DIR"
        cp -R "$target_path" "$BACKUP_DIR/"
        BACKED_UP=$((BACKED_UP + 1))
        echo "     → $BACKUP_DIR/$vst3_name"
    fi

    # Install new version
    echo "  📥 Installing new version..."
    cp -R "$vst3_bundle" "$VST3_DIR/"

    # Verify installation
    if [ -d "$target_path" ]; then
        echo "  ✅ Installed successfully"
        INSTALLED=$((INSTALLED + 1))

        # Show file size
        SIZE=$(du -sh "$target_path" | cut -f1)
        echo "     Size: $SIZE"
    else
        echo "  ❌ Installation failed"
        SKIPPED=$((SKIPPED + 1))
    fi

    echo ""
done

cd ..

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "  Installation Summary"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
echo "  ✅ Installed: $INSTALLED plugins"
echo "  📦 Backed up: $BACKED_UP plugins"
echo "  ⊙ Skipped: $SKIPPED plugins"
echo ""

if [ $BACKED_UP -gt 0 ]; then
    echo "  💾 Backup location: $BACKUP_DIR"
    echo ""
fi

if [ $INSTALLED -gt 0 ]; then
    echo "  🎹 Ready to use in your DAW!"
    echo ""
    echo "  Next steps:"
    echo "  1. Launch your DAW (Logic Pro, Ableton, etc.)"
    echo "  2. Rescan plugins (if needed)"
    echo "  3. Look for 'AudioForge' manufacturer"
    echo ""
fi

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
