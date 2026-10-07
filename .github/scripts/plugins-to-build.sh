#!/usr/bin/env bash
# Which plugins a change needs built, for "Build All Plugins".
#
# Reads changed file paths on stdin, one per line, and prints the plugins to
# build, one per line, from those given as arguments (every plugin there is).
#
# A change inside plugins/<Name>/ needs only that plugin built. A change to
# something no plugin build reads (the Ableton scripts, the feedback log, the
# tests, docs, other workflows) needs nothing built. Anything else, such as
# shared/, JUCE, cmake/ or the stress host, could affect every plugin, so it
# needs them all: a path not listed here is assumed to matter.
set -euo pipefail

all_plugins=("$@")
everything=no
wanted=" "

while IFS= read -r path; do
    [ -n "$path" ] || continue
    case "$path" in
        plugins/*/*)
            name=${path#plugins/}
            wanted="$wanted${name%%/*} " ;;
        .github/workflows/build-all-plugins.yml | .github/scripts/plugins-to-build.sh)
            everything=yes ;;
        ableton/* | feedback/* | tests/* | docs/* | website/* | .github/* | *.md)
            ;;
        *)
            everything=yes ;;
    esac
done

for plugin in "${all_plugins[@]}"; do
    if [ "$everything" = yes ] || [[ "$wanted" == *" $plugin "* ]]; then
        echo "$plugin"
    fi
done
