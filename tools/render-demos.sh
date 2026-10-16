#!/usr/bin/env bash
# Render an audio demo of every AudioForge instrument and effect for the website:
# website/public/demos/<id>.mp3, loudness-matched (-18 LUFS, peaks under -1 dBTP) so that
# plugins can be compared by sound rather than level.
#
# Usage: tools/render-demos.sh <dir with the .vst3 bundles>
#   e.g. tools/render-demos.sh ~/Library/Audio/Plug-Ins/VST3
# Needs the stress host built (see tools/stress-host/README.md), ffmpeg, and python3.
# Instruments play a phrase that suits them (see tools/stress-host/Demo.h); effects
# process a built-in loop of drums and chords. MIDI tools make no sound, so get no demo.
# Then rebuild the catalog: node website/scripts/build-catalog.mjs

set -uo pipefail

vst3_dir=$1
root="$(cd "$(dirname "$0")/.." && pwd)"
host="$root/build/stress-host/StressHost_artefacts/Release/StressHost"
out="$root/website/public/demos"
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
mkdir -p "$out"

[ -x "$host" ] || { echo "Build the stress host first (tools/stress-host/README.md)"; exit 1; }
command -v ffmpeg > /dev/null || { echo "ffmpeg is needed"; exit 1; }

phrase_for() {  # <folder> <type>
  case "$1" in
    DrumSynth|AnalogKick|SnareSynth|HiHat|TomSynth|DrumMachine) echo drums ;;
    AcousticBass|BassLine) echo bass ;;
    ClassicMonosynth|Shakuhachi|BrassSection|Sitar|Koto) echo lead ;;
    *) [ "$2" = effect ] && echo effect || echo keys ;;
  esac
}

# id, folder, product name and type for every plugin, from the catalog
python3 - "$root/website/src/lib/catalog.json" > "$work/plugins.tsv" <<'EOF'
import json, sys
for p in json.load(open(sys.argv[1])):
    print('\t'.join([p['id'], p['folder'], p['name'], p['type']]))
EOF

rendered=0 skipped=()
while IFS=$'\t' read -r id folder name type; do
  [ "$type" = midi ] && continue
  bundle=""
  for candidate in "$vst3_dir/$name.vst3" "$vst3_dir/$folder.vst3" "$vst3_dir/AudioForge - $name.vst3"; do
    [ -d "$candidate" ] && { bundle=$candidate; break; }
  done
  if [ -z "$bundle" ]; then skipped+=("$folder (not found)"); continue; fi

  phrase=$(phrase_for "$folder" "$type")
  result=$(timeout 120 "$host" --render "$work/$id.wav" --phrase "$phrase" "$bundle" < /dev/null 2>&1 | tail -1)
  if [ ! -f "$work/$id.wav" ]; then
    skipped+=("$folder (${result##*$'\t'})")
    continue
  fi
  ffmpeg -nostdin -loglevel error -y -i "$work/$id.wav" -af loudnorm=I=-18:TP=-1:LRA=11 -ar 48000 \
    -c:a libmp3lame -b:a 128k "$out/$id.mp3"
  echo "$folder: $phrase -> demos/$id.mp3"
  rendered=$((rendered + 1))
done < "$work/plugins.tsv"

echo "Rendered $rendered demos to website/public/demos"
[ ${#skipped[@]} -gt 0 ] && printf 'No demo: %s\n' "${skipped[@]}"
exit 0
