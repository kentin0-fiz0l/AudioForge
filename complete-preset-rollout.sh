#!/bin/bash
# Complete the preset rollout - add presetBrowser_ initialization to all editors

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "  Completing Preset Rollout - Final Step"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

FIXED=0
SKIPPED=0

cd plugins

for dir in */; do
    plugin="${dir%/}"
    EDITOR_CPP="$plugin/Source/PluginEditor.cpp"

    if [ ! -f "$EDITOR_CPP" ]; then
        continue
    fi

    # Check if presetBrowser_ initialization is missing
    if grep -q "presetBrowser_" "$EDITOR_CPP" 2>/dev/null; then
        if ! grep -q "presetBrowser_(processor_.getPresetManager())" "$EDITOR_CPP"; then
            # Pattern 1: processor_(p) without trailing comma
            if grep -q "processor_(p)$" "$EDITOR_CPP"; then
                sed -i '' 's/processor_(p)$/processor_(p),\
      presetBrowser_(processor_.getPresetManager())/' "$EDITOR_CPP"
                echo "✓ $plugin (pattern 1)"
                FIXED=$((FIXED + 1))
            # Pattern 2: processor_(p), with trailing comma
            elif grep -q "processor_(p),$" "$EDITOR_CPP"; then
                sed -i '' '/processor_(p),$/a\
      presetBrowser_(processor_.getPresetManager()),
' "$EDITOR_CPP"
                echo "✓ $plugin (pattern 2)"
                FIXED=$((FIXED + 1))
            # Pattern 3: After AudioProcessorEditor line
            elif grep -q ": AudioProcessorEditor" "$EDITOR_CPP"; then
                # Check if there's already an init list
                if grep -A1 ": AudioProcessorEditor" "$EDITOR_CPP" | grep -q "processor_"; then
                    # Add after processor_ line
                    sed -i '' '/processor_(p)/a\
      presetBrowser_(processor_.getPresetManager()),
' "$EDITOR_CPP"
                    echo "✓ $plugin (pattern 3)"
                    FIXED=$((FIXED + 1))
                fi
            else
                echo "⚠ $plugin - manual review needed"
            fi
        else
            SKIPPED=$((SKIPPED + 1))
        fi
    fi
done

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "  Results"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "  Fixed: $FIXED"
echo "  Already done: $SKIPPED"
echo ""
echo "🎉 Preset rollout complete!"
