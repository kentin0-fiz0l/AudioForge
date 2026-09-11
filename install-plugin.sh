#!/bin/bash
# Install a specific AudioForge plugin

if [ -z "$1" ]; then
    echo "Usage: ./install-plugin.sh <PluginName>"
    echo ""
    echo "Available plugins with builds:"
    cd plugins
    for dir in */; do
        plugin="${dir%/}"
        if find "$plugin/build" -name "*.vst3" -type d 2>/dev/null | grep -q .; then
            echo "  - $plugin"
        fi
    done
    exit 1
fi

PLUGIN_NAME="$1"
VST3_DIR="$HOME/Library/Audio/Plug-Ins/VST3"

cd plugins

if [ ! -d "$PLUGIN_NAME" ]; then
    echo "❌ Plugin not found: $PLUGIN_NAME"
    exit 1
fi

# Find VST3 bundle
VST3_BUNDLE=$(find "$PLUGIN_NAME/build" -name "*.vst3" -type d 2>/dev/null | head -1)

if [ -z "$VST3_BUNDLE" ]; then
    echo "❌ No build found for $PLUGIN_NAME"
    echo "   Run: cd $PLUGIN_NAME && cmake -B build && cmake --build build"
    exit 1
fi

VST3_NAME=$(basename "$VST3_BUNDLE")
TARGET="$VST3_DIR/$VST3_NAME"

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "  Installing: $PLUGIN_NAME"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

# Backup if exists
if [ -d "$TARGET" ]; then
    BACKUP="$TARGET.backup.$(date +%Y%m%d_%H%M%S)"
    echo "📦 Backing up: $BACKUP"
    mv "$TARGET" "$BACKUP"
fi

# Install
echo "📥 Installing: $TARGET"
cp -R "$VST3_BUNDLE" "$VST3_DIR/"

if [ -d "$TARGET" ]; then
    echo "✅ Installed successfully!"
    du -sh "$TARGET"
else
    echo "❌ Installation failed"
    exit 1
fi

echo ""
echo "🎹 Ready to use! Rescan plugins in your DAW."
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
