#!/bin/bash
#
# Test Audio Generator for AudioForge Screenshot Capture
# Creates pink noise, loops, and MIDI patterns for plugin testing
#

set -e

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
OUTPUT_DIR="$SCRIPT_DIR/test-audio"

mkdir -p "$OUTPUT_DIR"

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}AudioForge Test Audio Generator${NC}"
echo -e "${BLUE}======================================${NC}"
echo ""

# Check for ffmpeg
if ! command -v ffmpeg &> /dev/null; then
    echo -e "${YELLOW}⚠ ffmpeg not found${NC}"
    echo "Install with: brew install ffmpeg"
    echo ""
    echo "Skipping audio file generation (you can create these manually in your DAW)"
    exit 0
fi

# 1. Generate pink noise (10 seconds, -12 dBFS for effects testing)
echo -e "${BLUE}→ Generating pink noise...${NC}"
ffmpeg -f lavfi -i "anoisesrc=d=10:c=pink:r=44100:a=0.25" \
       -y "$OUTPUT_DIR/pink-noise-10s.wav" \
       -loglevel error

if [ -f "$OUTPUT_DIR/pink-noise-10s.wav" ]; then
    SIZE=$(du -h "$OUTPUT_DIR/pink-noise-10s.wav" | cut -f1)
    echo -e "${GREEN}✓ Pink noise created: pink-noise-10s.wav ($SIZE)${NC}"
else
    echo -e "${YELLOW}⚠ Pink noise generation failed${NC}"
fi

echo ""

# 2. Generate simple sine wave test tone (440 Hz A, for basic testing)
echo -e "${BLUE}→ Generating test tone (440 Hz)...${NC}"
ffmpeg -f lavfi -i "sine=frequency=440:duration=5:sample_rate=44100" \
       -y "$OUTPUT_DIR/test-tone-440hz.wav" \
       -loglevel error

if [ -f "$OUTPUT_DIR/test-tone-440hz.wav" ]; then
    SIZE=$(du -h "$OUTPUT_DIR/test-tone-440hz.wav" | cut -f1)
    echo -e "${GREEN}✓ Test tone created: test-tone-440hz.wav ($SIZE)${NC}"
else
    echo -e "${YELLOW}⚠ Test tone generation failed${NC}"
fi

echo ""
echo -e "${BLUE}======================================${NC}"
echo -e "${GREEN}Test audio files generated!${NC}"
echo -e "${BLUE}======================================${NC}"
echo ""
echo -e "${BLUE}Files created in: $OUTPUT_DIR${NC}"
ls -lh "$OUTPUT_DIR"/*.wav 2>/dev/null || true
echo ""

echo -e "${YELLOW}Note: You should also create these manually in your DAW:${NC}"
echo ""
echo "  1. full-mix-loop.wav"
echo "     - 4-bar instrumental loop (drums, bass, melody)"
echo "     - Export at -14 LUFS for realistic mixing"
echo "     - Use for EQ, compressor, saturation screenshots"
echo ""
echo "  2. midi-test-pattern.mid"
echo "     - C-E-G triad, whole notes, 120 BPM"
echo "     - Use for instrument screenshots (synths, samplers)"
echo "     - Create in Logic/Reaper, export as MIDI file"
echo ""

echo -e "${GREEN}✓ Setup complete! See SCREENSHOT_GUIDE.md for capture instructions.${NC}"
