#!/usr/bin/env bash
# Install AudioForge for the current user: VST3 plugins to ~/.vst3 and the standalone
# apps to ~/.local/bin. Run from the unpacked AudioForge folder. ./install.sh --uninstall removes them.
set -euo pipefail
here="$(cd "$(dirname "$0")" && pwd)"
vst3_dir="${VST3_DIR:-$HOME/.vst3}"
bin_dir="${BIN_DIR:-$HOME/.local/bin}"

if [ "${1:-}" = "--uninstall" ]; then
  for p in "$here"/VST3/*.vst3; do rm -rf "$vst3_dir/$(basename "$p")"; done
  for p in "$here"/Standalone/*; do [ -f "$p" ] && rm -f "$bin_dir/$(basename "$p")"; done
  echo "AudioForge removed from $vst3_dir and $bin_dir"
  exit 0
fi

mkdir -p "$vst3_dir" "$bin_dir"
cp -R "$here"/VST3/*.vst3 "$vst3_dir/"
n_apps=0
for p in "$here"/Standalone/*; do
  [ -f "$p" ] || continue
  install -m 755 "$p" "$bin_dir/"
  n_apps=$((n_apps + 1))
done
echo "Installed $(ls -d "$here"/VST3/*.vst3 | wc -l) VST3 plugins to $vst3_dir and $n_apps apps to $bin_dir"
case ":$PATH:" in *":$bin_dir:"*) ;; *) echo "Add $bin_dir to your PATH to run the apps by name." ;; esac
echo "Restart your DAW and rescan plugins."
