#!/usr/bin/env bash
# Build a group of plugins in Release and stage what each one produces, for the
# release workflow:
#
#   stage/VST3/<Product>.vst3          every platform
#   stage/AU/<Product>.component       macOS
#   stage/Standalone/<Product>.app     macOS
#   stage/Standalone/<Product>.exe     Windows
#   stage/Standalone/<Product>         Linux
#
# Usage: build-release-shard.sh <stage-dir> <plugin>...
# On macOS the build is universal (arm64 + x86_64) for macOS 11 and later.
# Fails if any plugin fails to build or produces no VST3.

set -uo pipefail

stage=$1
shift
mkdir -p "$stage/VST3" "$stage/AU" "$stage/Standalone" logs

extra=() retry=(--parallel 1)
case "$(uname -s)" in
  Darwin) extra=(-DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0) ;;
  # Also limits CL.exe to one compile thread (no /MP). MSBuild still passes all of a target's
  # sources to a single CL.exe, so this is not one file per process
  MINGW*|MSYS*|CYGWIN*) retry=(--parallel 1 -- -p:CL_MPCount=1 -p:UseMultiToolTask=false) ;;
esac

# Build one plugin and copy its outputs into the stage; returns non-zero on failure
build_and_stage() {
  local plugin=$1 start=$SECONDS
  # A failed build is retried once on one core, for failures that come from building in
  # parallel. It did not get past the MSVC 19.44 crash (0xE06D7363) building WavetableSynth;
  # moving the Windows build to the windows-2025-vs2026 runner (MSVC 19.51) did
  if ! cmake -S "plugins/$plugin" -B "build/$plugin" -DCMAKE_BUILD_TYPE=Release "${extra[@]}" \
         > "logs/$plugin-configure.log" 2>&1 \
     || ! { cmake --build "build/$plugin" --config Release --parallel 4 > "logs/$plugin-build.log" 2>&1 \
            || cmake --build "build/$plugin" --config Release "${retry[@]}" >> "logs/$plugin-build.log" 2>&1; }; then
    echo "::error::$plugin failed to build"
    tail -n 40 "logs/$plugin-configure.log" "logs/$plugin-build.log"
    return 1
  fi

  local artefacts="build/$plugin/${plugin}_artefacts" vst3 au app
  # Each format's output sits in <artefacts>/Release/<Format>/ (or <artefacts>/<Format>/)
  vst3=$(find "$artefacts" -path '*/VST3/*.vst3' -prune -print | head -1)
  au=$(find "$artefacts" -path '*/AU/*.component' -prune -print | head -1)
  app=$(find "$artefacts" -path '*/Standalone/*' \( -name '*.app' -prune -o -name '*.exe' -o -type f -perm -u+x \) -print \
        | grep -v '\.app/' | head -1)

  if [ -z "$vst3" ]; then
    echo "::error::$plugin built but produced no VST3"
    return 1
  fi
  cp -R "$vst3" "$stage/VST3/"
  [ -n "$au" ] && cp -R "$au" "$stage/AU/"
  [ -n "$app" ] && cp -R "$app" "$stage/Standalone/"
  echo "$plugin: $(basename "$vst3")${au:+, $(basename "$au")}${app:+, $(basename "$app")} in $((SECONDS - start))s"
}

failed=()
for plugin in "$@"; do
  echo "::group::$plugin"
  build_and_stage "$plugin" || failed+=("$plugin")
  echo "::endgroup::"
done

if [ ${#failed[@]} -gt 0 ]; then
  echo "::error::Failed: ${failed[*]}"
  exit 1
fi
