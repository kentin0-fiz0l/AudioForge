#!/bin/bash
# Real-time build status display

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "  AudioForge Build Status Monitor"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

# Count successes and failures
SUCCESS=$(grep -c "✅ SUCCESS" build-final.log 2>/dev/null || echo "0")
FAILED=$(grep -c "❌ Build failed" build-final.log 2>/dev/null || echo "0")

# Current plugin
CURRENT=$(tail -20 build-final.log | grep "Building:" | tail -1 | sed 's/.*Building: //')
PHASE=$(tail -10 build-final.log | grep -E "\[(1|2|3)/3\]" | tail -1)

echo "Progress:"
echo "  ✅ Successful: $SUCCESS"
echo "  ❌ Failed: $FAILED"
echo "  📊 Total: $((SUCCESS + FAILED)) / 39"
echo ""

if [ ! -z "$CURRENT" ]; then
    echo "Currently building: $CURRENT"
    if [ ! -z "$PHASE" ]; then
        echo "  $PHASE"
    fi
    echo ""
fi

# Show recent successes
echo "Recent builds:"
grep -E "(SUCCESS|failed)" build-final.log | tail -5 | sed 's/^/  /'

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
