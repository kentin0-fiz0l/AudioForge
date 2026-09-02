# AudioForge Screenshot Capture Guide

## Overview

This guide documents the process for capturing professional-quality screenshots of all AudioForge plugins for marketing, documentation, and content creation purposes.

**Goal**: 50 high-quality screenshots (5 per plugin × 10 priority plugins)  
**Timeline**: 6 hours (7 minutes per screenshot)  
**Output**: Web-optimized images for website, videos, social media, documentation

---

## Screenshot Strategy

### Priority Plugins (Capture First)

Focus on these 10 plugins for maximum marketing impact:

1. **SimpleGain** - Flagship utility, simplest to demonstrate
2. **SimpleEQ** - Essential mixing tool
3. **SimpleComp** - Core dynamics processor
4. **Limiter** - (Future v1.6.0, when available)
5. **CleanDelay** - Creative effect
6. **Reverb** - Spatial processor
7. **Saturation** - Character processor
8. **BasicSynth** - Flagship instrument
9. **WavetableSynth** - Advanced instrument
10. **SpectralFreeze** - Unique creative effect

### Screenshot Types (5 per plugin)

Each plugin needs these 5 screenshot types:

1. **Default State** (`[PluginName]-01-default.png`)
   - Clean UI, default parameters
   - Shows plugin at first load
   - Purpose: Documentation, first impressions

2. **Active Processing** (`[PluginName]-02-active.png`)
   - Parameters adjusted for typical use
   - Meters showing signal activity
   - Purpose: Demonstration, marketing

3. **Extreme Settings** (`[PluginName]-03-extreme.png`)
   - Creative/aggressive parameter values
   - Shows plugin's range and character
   - Purpose: Sound design demos, creative use cases

4. **In-DAW Context** (`[PluginName]-04-daw.png`)
   - Plugin loaded in Logic Pro or Reaper
   - Shows integration, real-world usage
   - Purpose: Installation verification, compatibility proof

5. **Detail Shot** (`[PluginName]-05-detail.png`)
   - Close-up of unique feature (waveform, spectrum, modulation)
   - Highlights plugin's distinctive visual element
   - Purpose: Thumbnails, social media, teasers

---

## Technical Specifications

### Resolution & Format

- **Capture Resolution**: 2560×1440 (2K, Retina quality)
- **Plugin Window**: Centered, full UI visible
- **File Format**: PNG (lossless, transparency support)
- **Color Space**: sRGB (web standard)
- **Bit Depth**: 24-bit RGB

### DAW Settings for Consistency

**Logic Pro X:**
```
Preferences → Display:
- Appearance: Dark (for contrast)
- Track Height: Medium
- Zoom Level: 100%
```

**Reaper:**
```
Options → Themes: Default_6.0 (clean, professional)
View → Screen Sets: Mixer (for processing screenshots)
```

### macOS Screenshot Settings

```bash
# Set screenshot format to PNG
defaults write com.apple.screencapture type png

# Disable screenshot shadow (cleaner images)
defaults write com.apple.screencapture disable-shadow -bool true

# Restart SystemUIServer to apply
killall SystemUIServer

# Screenshot shortcuts:
# Cmd+Shift+4, then Space = Capture window (no shadow)
# Cmd+Shift+4, then drag = Capture selection
```

---

## Capture Process (7 minutes per plugin)

### Step 1: Prepare Plugin (2 min)

1. Load plugin in Logic Pro or Reaper
2. Set appropriate buffer size (512 samples for stable meters)
3. Load test audio:
   - **Effects**: Pink noise or full mix loop
   - **Instruments**: MIDI test pattern (C-E-G triad)
4. Resize plugin window to fit 2K frame (if resizable)

### Step 2: Capture Screenshots (5 min)

**Screenshot 1: Default State**
1. Reset plugin to default parameters (`Initialize Preset` or reload)
2. Stop audio playback
3. Cmd+Shift+4 → Space → Click plugin window
4. Save to `docs/screenshots/raw/[PluginName]-01-default.png`

**Screenshot 2: Active Processing**
1. Start audio playback
2. Adjust parameters to typical/musical settings:
   - **SimpleGain**: -6 dB gain, meters at -12 dBFS
   - **SimpleEQ**: Mid boost +3 dB at 1 kHz
   - **SimpleComp**: 4:1 ratio, -10 dB threshold, 5 dB GR
3. Wait for meters to stabilize (3-5 seconds)
4. Capture window
5. Save to `docs/screenshots/raw/[PluginName]-02-active.png`

**Screenshot 3: Extreme Settings**
1. Set creative/aggressive parameters:
   - **Saturation**: Drive maxed, hard clip mode
   - **Reverb**: 10s decay, 100% wet
   - **Delay**: Short feedback loop, high feedback
2. Capture window
3. Save to `docs/screenshots/raw/[PluginName]-03-extreme.png`

**Screenshot 4: In-DAW Context**
1. Zoom out DAW to show:
   - Plugin in mixer channel strip
   - Surrounding tracks (2-3 visible)
   - Transport controls
2. Use Cmd+Shift+4 (selection capture) to frame:
   - Plugin window (center)
   - DAW interface (context)
3. Save to `docs/screenshots/raw/[PluginName]-04-daw.png`

**Screenshot 5: Detail Shot**
1. Identify unique visual element:
   - **WavetableSynth**: Wavetable display
   - **SpectralFreeze**: FFT spectrum
   - **SimpleEQ**: Filter curve graph
2. Use Cmd+Shift+4 (selection) to capture JUST that element
3. Save to `docs/screenshots/raw/[PluginName]-05-detail.png`

---

## Composition Guidelines

### Lighting & Contrast

- **Dark theme** for DAW (professional, cinematic)
- **High contrast** UI elements (readable at thumbnail size)
- **Active meters** showing signal flow (proves it works)

### Parameter Sweet Spots

**SimpleGain:**
- Default: 0 dB, meters silent
- Active: -6 dB, meters at -12 dBFS (headroom visible)
- Extreme: +12 dB, meters clipping (shows range)

**SimpleEQ:**
- Default: Flat response, all bands at 0 dB
- Active: Bell +3 dB at 1 kHz, HP at 80 Hz
- Extreme: All bands boosted +10 dB (obvious EQ curve)

**SimpleComp:**
- Default: 1:1 ratio (bypassed), 0 dB threshold
- Active: 4:1 ratio, -10 dB threshold, 5 dB GR showing
- Extreme: 20:1 ratio (limiter mode), 10+ dB GR

**Reverb:**
- Default: Small room, 1.5s decay, 30% wet
- Active: Hall, 3.5s decay, 50% wet, visible reflections
- Extreme: Cathedral, 10s decay, 100% wet

### Framing Rules

1. **No dead space**: Plugin UI fills 60-80% of frame
2. **Centered composition**: Plugin window centered horizontally
3. **Breathing room**: 10-20px margins around plugin
4. **No desktop clutter**: Hide dock, menu bar (if possible)

---

## Batch Optimization Script

After capturing raw screenshots, optimize for web:

```bash
#!/bin/bash
# optimize-screenshots.sh - Compress and resize screenshots for web

cd docs/screenshots

# Optimize PNGs (lossless compression)
for img in raw/*.png; do
    filename=$(basename "$img")
    
    # Create web version (1280×720, optimized)
    sips -Z 1280 "$img" --out "web/$filename"
    
    # Lossless compression (requires pngcrush or optipng)
    if command -v pngcrush &> /dev/null; then
        pngcrush -ow "web/$filename"
    elif command -v optipng &> /dev/null; then
        optipng -o7 "web/$filename"
    fi
    
    echo "✓ Optimized: $filename"
done

# Generate optimized originals (keep 2K resolution, compress only)
for img in raw/*.png; do
    filename=$(basename "$img")
    cp "$img" "optimized/$filename"
    
    if command -v optipng &> /dev/null; then
        optipng -o7 "optimized/$filename"
    fi
done

echo ""
echo "Screenshot optimization complete!"
echo "  Raw (2K): docs/screenshots/raw/"
echo "  Optimized (2K): docs/screenshots/optimized/"
echo "  Web (720p): docs/screenshots/web/"
```

---

## Naming Convention

**Format**: `[PluginName]-[Number]-[Type].png`

**Examples:**
- `SimpleGain-01-default.png`
- `SimpleEQ-02-active.png`
- `Reverb-03-extreme.png`
- `BasicSynth-04-daw.png`
- `SpectralFreeze-05-detail.png`

**Benefits:**
- Alphabetical sorting groups by plugin
- Number indicates screenshot sequence
- Type describes content (self-documenting)
- SEO-friendly (keywords in filename)

---

## Quality Checklist

Before marking a screenshot as "done", verify:

- [ ] **Resolution**: 2560×1440 (2K) for raw, 1280×720 for web
- [ ] **Focus**: UI elements sharp, no blur
- [ ] **Lighting**: Adequate contrast, readable text
- [ ] **Meters**: Active (not silent) for processing screenshots
- [ ] **Clean frame**: No desktop clutter, dock, or menu bar
- [ ] **Proper naming**: Follows `[Plugin]-[Num]-[Type].png` convention
- [ ] **File size**: Raw <2 MB, web <500 KB (after optimization)

---

## Usage Across Channels

### Website (`fluxstudio.art/audioforge`)

- **Gallery page**: 2×5 grid (10 plugins × 2 screenshots each)
- **Plugin detail pages**: All 5 screenshots per plugin
- **Homepage hero**: 1 stunning detail shot (rotating carousel)

### YouTube Videos

- **Thumbnails**: Detail shots + text overlay
- **B-roll**: Active processing, DAW context shots
- **Tutorials**: Screenshot sequences as visual guides

### Social Media

- **Twitter/X**: Detail shots (1:1 crop for profile)
- **Reddit**: DAW context (shows real-world use)
- **Instagram**: Active processing (colorful meters)

### Documentation

- **README**: Default state (shows clean UI)
- **User manual**: Active processing (shows features)
- **GitHub Release**: DAW context (proves compatibility)

---

## Timeline & Milestones

**Session 1 (2 hours): Core Processing Suite**
- SimpleGain (7 min)
- SimpleEQ (7 min)
- SimpleComp (7 min)
- CleanDelay (7 min)
- Saturation (7 min)
- Reverb (7 min)
- **Milestone**: 30 screenshots, core mixing workflow documented

**Session 2 (2 hours): Instruments**
- BasicSynth (7 min)
- WavetableSynth (7 min)
- FMSynth (7 min)
- **Milestone**: 45 screenshots, flagship instruments captured

**Session 3 (2 hours): Creative Effects**
- SpectralFreeze (7 min)
- FreezeFX (7 min)
- GranularEngine (7 min)
- **Milestone**: 50 screenshots complete, creative suite documented

**Post-Processing (30 min)**
- Run optimization script
- Verify file sizes and quality
- Upload web versions to website
- Update README with gallery links

**Total Time**: 6.5 hours → **50 professional marketing assets**

---

## Tools Required

### Software

- **Logic Pro X** or **Reaper** (DAW for plugin hosting)
- **macOS Screenshot** (Cmd+Shift+4, built-in)
- **pngcrush** or **optipng** (lossless compression)
  ```bash
  brew install pngcrush optipng
  ```
- **sips** (macOS image processing, built-in)

### Test Audio

Create test audio files in `docs/screenshots/test-audio/`:

1. **pink-noise-10s.wav** (for effects testing)
   ```bash
   # Generate 10s of pink noise at -12 dBFS
   ffmpeg -f lavfi -i "anoisesrc=d=10:c=pink:r=44100:a=0.25" \
          pink-noise-10s.wav
   ```

2. **full-mix-loop.wav** (for mixing effects)
   - Use a 4-bar instrumental loop (drums, bass, melody)
   - Export from existing project at -14 LUFS

3. **midi-test-pattern.mid** (for instruments)
   - C-E-G triad, whole notes, 120 BPM
   - Create in DAW, export as MIDI file

---

## Deliverables

After completing all 3 sessions:

- ✅ **50 raw screenshots** (2K resolution, `docs/screenshots/raw/`)
- ✅ **50 optimized screenshots** (2K, compressed, `docs/screenshots/optimized/`)
- ✅ **50 web screenshots** (720p, compressed, `docs/screenshots/web/`)
- ✅ **Gallery index** (Markdown file listing all screenshots)
- ✅ **Website integration** (screenshots uploaded to website)

**File Structure:**
```
docs/screenshots/
├── raw/               # 2560×1440, uncompressed (2-5 MB each)
│   ├── SimpleGain-01-default.png
│   ├── SimpleGain-02-active.png
│   └── ... (50 total)
├── optimized/         # 2560×1440, compressed (500 KB-1 MB)
│   └── ... (50 total)
├── web/               # 1280×720, optimized (200-400 KB)
│   └── ... (50 total)
├── test-audio/        # Test files for audio playback
│   ├── pink-noise-10s.wav
│   ├── full-mix-loop.wav
│   └── midi-test-pattern.mid
├── SCREENSHOT_GUIDE.md (this file)
├── optimize-screenshots.sh
└── gallery-index.md
```

---

## Next Steps After Screenshots

Once all 50 screenshots are captured and optimized:

1. **Website Integration** (1 hour)
   - Upload web versions to `website/public/screenshots/`
   - Update plugin gallery page with image URLs
   - Create hover effects and lightbox viewers

2. **README Update** (30 min)
   - Add "Plugin Gallery" section with 10 hero screenshots
   - Link to full gallery on website

3. **YouTube Preparation** (1 hour)
   - Select 3 detail shots for thumbnail templates
   - Create B-roll sequences for Video #1 (Installation Guide)

4. **Social Media Assets** (30 min)
   - Create 1:1 crops for Twitter/Instagram
   - Add text overlays for promotional posts

**Total ROI**: 6.5 hours → Marketing assets that work 24/7 across all channels

---

**Last Updated**: September 1, 2026  
**Status**: ✅ READY FOR EXECUTION  
**Next Action**: Capture Session 1 (Core Processing Suite, 2 hours)
