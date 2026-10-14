#!/usr/bin/env bash
# Make the macOS installer and zip from a release stage (see .github/scripts/build-release-shard.sh).
#
#   AudioForge-<version>-macOS.pkg   installs VST3 and AU plugins to /Library/Audio/Plug-Ins
#                                    and the standalone apps to /Applications/AudioForge;
#                                    each of the three can be left out in the installer
#   AudioForge-<version>-macOS.zip   the same files, to copy by hand
#
# Usage: make-pkg.sh <stage-dir> <version> <out-dir>
# The package is unsigned (there is no Developer ID certificate), so macOS asks for
# "Open Anyway" the first time; see the release notes.

set -euo pipefail

stage=$1
version=$2
out=$3
here="$(cd "$(dirname "$0")" && pwd)"
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
mkdir -p "$out"

component() {  # <id> <payload dir> <install location>
  local id=$1 payload=$2 location=$3
  [ -n "$(ls -A "$payload" 2>/dev/null)" ] || return 0
  # Bundles must install where they are put, not wherever macOS last saw a bundle with that id
  pkgbuild --analyze --root "$payload" "$work/$id.plist" > /dev/null
  python3 - "$work/$id.plist" <<'EOF'
import plistlib, sys
path = sys.argv[1]
with open(path, 'rb') as f:
    items = plistlib.load(f)
for item in items:
    item['BundleIsRelocatable'] = False
    item['BundleOverwriteAction'] = 'upgrade'
with open(path, 'wb') as f:
    plistlib.dump(items, f)
EOF
  pkgbuild --root "$payload" --component-plist "$work/$id.plist" \
    --identifier "art.fluxstudio.audioforge.$id" --version "$version" \
    --install-location "$location" "$work/$id.pkg" > /dev/null
  echo "$id"
}

parts=()
parts+=($(component vst3 "$stage/VST3" /Library/Audio/Plug-Ins/VST3))
parts+=($(component au "$stage/AU" /Library/Audio/Plug-Ins/Components))
parts+=($(component apps "$stage/Standalone" /Applications/AudioForge))

count() { ls -1 "$1" 2>/dev/null | wc -l | tr -d ' '; }
vst3=$(count "$stage/VST3"); au=$(count "$stage/AU"); apps=$(count "$stage/Standalone")

# The installer's choices, one per component, all selected
{
  cat <<EOF
<?xml version="1.0" encoding="utf-8"?>
<installer-gui-script minSpecVersion="2">
  <title>AudioForge $version</title>
  <welcome file="welcome.html" mime-type="text/html"/>
  <options customize="allow" require-scripts="false" hostArchitectures="arm64,x86_64"/>
  <domains enable_localSystem="true"/>
  <os-version min="11.0"/>
  <choices-outline>
EOF
  for p in "${parts[@]}"; do echo "    <line choice=\"$p\"/>"; done
  echo "  </choices-outline>"
  for p in "${parts[@]}"; do
    case $p in
      vst3) title="VST3 plugins ($vst3)"; text="Installed to /Library/Audio/Plug-Ins/VST3" ;;
      au) title="Audio Units ($au)"; text="Installed to /Library/Audio/Plug-Ins/Components (Logic Pro, GarageBand and other AU hosts)" ;;
      apps) title="Standalone apps ($apps)"; text="Installed to /Applications/AudioForge" ;;
    esac
    echo "  <choice id=\"$p\" title=\"$title\" description=\"$text\"><pkg-ref id=\"art.fluxstudio.audioforge.$p\"/></choice>"
    echo "  <pkg-ref id=\"art.fluxstudio.audioforge.$p\" version=\"$version\">$p.pkg</pkg-ref>"
  done
  echo "</installer-gui-script>"
} > "$work/distribution.xml"

mkdir -p "$work/resources"
sed -e "s/@VERSION@/$version/g" -e "s/@VST3@/$vst3/g" -e "s/@AU@/$au/g" -e "s/@APPS@/$apps/g" \
  "$here/welcome.html" > "$work/resources/welcome.html"

productbuild --distribution "$work/distribution.xml" --resources "$work/resources" \
  --package-path "$work" "$out/AudioForge-$version-macOS.pkg"

# The zip unpacks to one folder and keeps the bundles' structure and symlinks
mkdir -p "$work/zip"
ditto --norsrc --noextattr "$stage" "$work/zip/AudioForge-$version-macOS"
cp "$here/../README-install.txt" "$work/zip/AudioForge-$version-macOS/README.txt"
ditto -c -k --norsrc --noextattr --keepParent "$work/zip/AudioForge-$version-macOS" "$out/AudioForge-$version-macOS.zip"

echo "macOS: $vst3 VST3, $au AU, $apps apps"
ls -lh "$out"/AudioForge-"$version"-macOS.*
