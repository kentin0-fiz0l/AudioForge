#!/bin/bash
# Calculate coverage for AudioForge plugins only

echo "Calculating code coverage for AudioForge plugins..."
echo ""

# Extract coverage for each plugin
for plugin in SimpleGain PanUtil BasicSynth CleanDelay SimpleEQ SimpleComp; do
    echo "=== $plugin Plugin ==="
    lcov --list tests/build/coverage.info --ignore-errors all 2>/dev/null | \
        grep -A 50 "plugins/$plugin/Source" | \
        grep -E "Processor\.cpp|Voice\.cpp|Oscillator\.cpp" | \
        awk -v plugin="$plugin" '{
            gsub(/%/, "", $2);
            gsub(/%/, "", $4);
            print "  " $1 ": " $2 "% lines, " $4 "% functions"
        }'
    echo ""
done

# Overall plugin coverage (excluding editors)
echo "=== Overall Plugin Coverage (excluding UI) ==="
lcov --list tests/build/coverage.info --ignore-errors all 2>/dev/null | \
    grep -E "plugins/.*/Source/(PluginProcessor|Voice|Oscillator)\.cpp" | \
    awk '
        BEGIN {total_lines=0; hit_lines=0; total_funcs=0; hit_funcs=0}
        {
            gsub(/%/, "", $2);
            gsub(/%/, "", $4);
            lines = $3; funcs = $5;
            hit_lines += (lines * $2 / 100);
            total_lines += lines;
            hit_funcs += (funcs * $4 / 100);
            total_funcs += funcs;
        }
        END {
            line_cov = (total_lines > 0) ? (hit_lines / total_lines * 100) : 0;
            func_cov = (total_funcs > 0) ? (hit_funcs / total_funcs * 100) : 0;
            printf "  Line Coverage: %.1f%%\n", line_cov;
            printf "  Function Coverage: %.1f%%\n", func_cov;
        }
    '
