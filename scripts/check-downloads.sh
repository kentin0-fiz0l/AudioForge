#!/bin/bash
# Download Analytics Script for AudioForge
# Uses GitHub Releases API to track download statistics

set -e

echo "======================================"
echo "AudioForge Download Statistics"
echo "======================================"
echo ""

# Total downloads across all releases
echo "📊 Total Downloads:"
gh api repos/kentin0-fiz0l/AudioForge/releases \
  --jq '[.[] | .assets[].download_count] | add // 0' | \
  awk '{printf "   %d downloads\n", $1}'

echo ""
echo "📦 Downloads by Release:"
gh api repos/kentin0-fiz0l/AudioForge/releases \
  --jq '.[] | "\(.tag_name): \([.assets[].download_count] | add // 0) downloads"' | \
  awk '{printf "   %s\n", $0}'

echo ""
echo "💾 Downloads by Asset (v1.1.0):"
gh api repos/kentin0-fiz0l/AudioForge/releases/tags/v1.1.0 \
  --jq '.assets[] | "\(.name): \(.download_count) downloads"' 2>/dev/null | \
  awk '{printf "   %s\n", $0}' || echo "   No assets available yet"

echo ""
echo "======================================"
echo "📈 Platform Distribution (estimated):"
echo "   Run this script after installers are released"
echo "======================================"
