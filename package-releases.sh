#!/bin/bash

# AudioForge Plugin Release Packager
# Builds all plugins in Release mode and packages them for distribution

set -e  # Exit on error

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RELEASES_DIR="$PROJECT_ROOT/releases"

# Plugin list with versions (name:version)
PLUGINS=(
  "SimpleGain:1.0.0"
  "PanUtil:1.0.0"
  "BasicSynth:1.0.0"
  "CleanDelay:1.0.0"
  "SimpleEQ:1.0.0"
  "SimpleComp:1.0.0"
)

echo "🔨 AudioForge Release Packager"
echo "=============================="
echo ""

# Clean and create releases directory
echo "📁 Setting up releases directory..."
rm -rf "$RELEASES_DIR"
mkdir -p "$RELEASES_DIR"

echo ""
echo "🏗️  Building and packaging plugins..."

for plugin_info in "${PLUGINS[@]}"; do
  IFS=':' read -r plugin version <<< "$plugin_info"

  echo ""
  echo "  → Building $plugin v$version..."

  PLUGIN_DIR="$PROJECT_ROOT/plugins/$plugin"
  BUILD_DIR="$PLUGIN_DIR/build"

  # Clean and build plugin
  cd "$PLUGIN_DIR"
  rm -rf "$BUILD_DIR"
  cmake -B build -DCMAKE_BUILD_TYPE=Release
  cmake --build build --config Release

  # Find the .vst3 bundle
  VST3_PATH="$BUILD_DIR/${plugin}_artefacts/Release/VST3/${plugin}.vst3"

  if [ ! -d "$VST3_PATH" ]; then
    echo "    ⚠️  Warning: $plugin.vst3 not found at $VST3_PATH"
    continue
  fi

  # Create zip package
  ZIP_NAME="${plugin}-v${version}-macOS.zip"
  cd "$BUILD_DIR/${plugin}_artefacts/Release/VST3"
  zip -r "$RELEASES_DIR/$ZIP_NAME" "${plugin}.vst3" > /dev/null

  # Get file size
  SIZE=$(du -h "$RELEASES_DIR/$ZIP_NAME" | cut -f1)
  echo "    ✅ Created $ZIP_NAME ($SIZE)"
done

cd "$PROJECT_ROOT"

echo ""
echo "✨ All plugins packaged successfully!"
echo ""
echo "📋 Release packages:"
ls -lh "$RELEASES_DIR"/*.zip | awk '{print "  " $9 " (" $5 ")"}'

echo ""
echo "📍 Packages location: $RELEASES_DIR"
