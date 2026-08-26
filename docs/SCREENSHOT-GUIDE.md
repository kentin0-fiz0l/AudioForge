# AudioForge Screenshot Capture Guide

Complete guide for capturing professional plugin screenshots for the website.

**Goal**: 5 screenshots per plugin (50 total) showing UI in different states.

---

## Setup

### Requirements

- **DAW**: Logic Pro, Ableton Live, or Reaper (clean UI)
- **Screen Resolution**: 2560x1440 or higher (Retina display preferred)
- **Audio Interface**: For signal flow screenshots
- **Test Audio**: Download test files from `tests/fixtures/` or use simple signals

### DAW Preparation

1. **Close unnecessary windows** - Only plugin UI visible
2. **Clean background** - Solid dark gray or black background
3. **Disable tooltips** - Turn off hover tooltips temporarily
4. **Full screen** - Maximize DAW window
5. **Reset plugin state** - Start with default settings unless specified

### Screenshot Settings

**Format**: PNG (lossless)
**Resolution**: Native (Retina/HiDPI)
**Naming**: `<plugin>-<number>-<description>.png`

**Examples**:
```
SimpleGain-1-default.png
SimpleGain-2-reducing.png
SimpleGain-3-boosting.png
```

---

## Screenshot Specifications

### For Each Plugin: 5 Screenshots

1. **Default State** - Plugin just loaded, all controls at default
2. **In Use (Typical)** - Common real-world settings
3. **In Use (Extreme)** - Dramatic settings showing range
4. **Metering Active** - Shows meters responding to signal
5. **Annotated/Highlighted** - For website feature showcase

---

## Plugin-by-Plugin Guide

### 1. SimpleGain (5 screenshots)

**Screenshot 1: Default State**
- File: `SimpleGain-1-default.png`
- Settings: Gain at 0.0 dB, all defaults
- Meters: No signal (empty)

**Screenshot 2: Gain Reduction**
- File: `SimpleGain-2-reducing.png`
- Settings: Gain at -6.0 dB
- Meters: Active, showing reduced signal
- Use case: Gain staging before compressor

**Screenshot 3: Gain Boost**
- File: `SimpleGain-3-boosting.png`
- Settings: Gain at +12.0 dB
- Meters: Active, showing boosted signal
- Use case: Makeup gain after processing

**Screenshot 4: Fine Adjustment**
- File: `SimpleGain-4-fine-control.png`
- Settings: Gain at -3.2 dB (precise value)
- Meters: Active with moderate level
- Purpose: Show precision control

**Screenshot 5: A/B Comparison**
- File: `SimpleGain-5-bypass-compare.png`
- Settings: Gain at +6.0 dB, show bypass button
- Meters: Active
- Purpose: Demonstrate bypass functionality

---

### 2. PanUtil (5 screenshots)

**Screenshot 1: Default State**
- File: `PanUtil-1-default.png`
- Settings: Pan 0 (center), Width 100%, all defaults
- Meters: Phase correlation at 1.0 (perfect correlation)

**Screenshot 2: Panned Left**
- File: `PanUtil-2-panned-left.png`
- Settings: Pan -50% (left), Width 100%
- Meters: Showing L/R balance shift

**Screenshot 3: Wide Stereo**
- File: `PanUtil-3-wide-stereo.png`
- Settings: Pan 0 (center), Width 150%
- Meters: Phase correlation slightly reduced

**Screenshot 4: Narrow Mono**
- File: `PanUtil-4-narrow-mono.png`
- Settings: Pan 0 (center), Width 0% (mono)
- Meters: Phase correlation at 1.0

**Screenshot 5: Creative Panning**
- File: `PanUtil-5-creative-panning.png`
- Settings: Pan +30% (right), Width 120%
- Meters: Active with asymmetric stereo

---

### 3. BasicSynth (5 screenshots)

**Screenshot 1: Default State**
- File: `BasicSynth-1-default.png`
- Settings: All oscillators off, envelope at defaults
- MIDI: No note active

**Screenshot 2: Bass Sound**
- File: `BasicSynth-2-bass-sound.png`
- Settings:
  - Sawtooth: 0.8, Sine: 0.6
  - Filter: Cutoff 500 Hz, Resonance 40%
  - Attack: 5ms, Decay: 100ms, Sustain: 0.5, Release: 80ms
- MIDI: Low note active (C2)

**Screenshot 3: Lead Sound**
- File: `BasicSynth-3-lead-sound.png`
- Settings:
  - Sawtooth: 1.0, Square: 0.4
  - Filter: Cutoff 2000 Hz, Resonance 60%
  - Attack: 10ms, Decay: 200ms, Sustain: 0.7, Release: 150ms
- MIDI: Mid note active (C4)

**Screenshot 4: Pad Sound**
- File: `BasicSynth-4-pad-sound.png`
- Settings:
  - Sine: 0.9
  - Filter: Cutoff 800 Hz, Resonance 20%
  - Attack: 300ms, Decay: 500ms, Sustain: 0.8, Release: 800ms
- MIDI: Chord active (C4, E4, G4)

**Screenshot 5: Filter Sweep**
- File: `BasicSynth-5-filter-sweep.png`
- Settings:
  - All oscillators active
  - Filter: Cutoff 1200 Hz, Resonance 80%
  - Envelope: Fast attack, long release
- Purpose: Showcase filter resonance

---

### 4. CleanDelay (5 screenshots)

**Screenshot 1: Default State**
- File: `CleanDelay-1-default.png`
- Settings: Time 1/4 note, Feedback 30%, Mix 25%
- Signal: No delay tail

**Screenshot 2: Vocal Delay**
- File: `CleanDelay-2-vocal-delay.png`
- Settings:
  - Time: 1/4 note
  - Feedback: 40%
  - Mix: 25%
  - High-pass: 200 Hz
- Signal: Delay tail visible

**Screenshot 3: Ping-Pong**
- File: `CleanDelay-3-ping-pong.png`
- Settings:
  - Time: 1/8 note
  - Feedback: 50%
  - Mix: 30%
  - Ping-Pong: ON
- Signal: Alternating L/R delays

**Screenshot 4: Rhythmic Delay**
- File: `CleanDelay-4-rhythmic.png`
- Settings:
  - Time: 1/16 note (dotted)
  - Feedback: 60%
  - Mix: 40%
- Signal: Rapid delay repeats

**Screenshot 5: Dub Delay**
- File: `CleanDelay-5-dub-delay.png`
- Settings:
  - Time: 1/2 note
  - Feedback: 70%
  - Mix: 50%
  - High-pass: 150 Hz
- Signal: Long delay tail

---

### 5. SimpleEQ (5 screenshots)

**Screenshot 1: Default State**
- File: `SimpleEQ-1-default.png`
- Settings: All bands flat (0 dB), filters off
- Signal: Flat frequency response

**Screenshot 2: Vocal EQ**
- File: `SimpleEQ-2-vocal-eq.png`
- Settings:
  - High-pass: 100 Hz
  - Low: -3 dB @ 300 Hz (Q=2.0)
  - Mid: +3 dB @ 3000 Hz (Q=1.5)
  - High: +2 dB @ 10000 Hz (Q=0.7)
- Signal: Vocal spectrum

**Screenshot 3: Bass EQ**
- File: `SimpleEQ-3-bass-eq.png`
- Settings:
  - Low: +4 dB @ 60 Hz (Q=1.0)
  - Mid: -2 dB @ 250 Hz (Q=2.0)
  - High: -3 dB @ 5000 Hz (Q=0.5)
  - Low-pass: 8000 Hz
- Signal: Bass spectrum

**Screenshot 4: Surgical Cut**
- File: `SimpleEQ-4-surgical-cut.png`
- Settings:
  - Mid: -8 dB @ 1200 Hz (Q=5.0)
  - (Other bands flat)
- Signal: Narrow notch visible
- Purpose: Show surgical EQ capability

**Screenshot 5: Presence Boost**
- File: `SimpleEQ-5-presence-boost.png`
- Settings:
  - Mid: +5 dB @ 4000 Hz (Q=1.0)
  - High: +3 dB @ 12000 Hz (Q=0.5)
- Signal: High-frequency emphasis

---

### 6. SimpleComp (5 screenshots)

**Screenshot 1: Default State**
- File: `SimpleComp-1-default.png`
- Settings: Threshold -20 dB, Ratio 2:1, Attack/Release moderate
- Meters: No gain reduction

**Screenshot 2: Vocal Compression**
- File: `SimpleComp-2-vocal-compression.png`
- Settings:
  - Threshold: -18 dB
  - Ratio: 3:1
  - Attack: 5 ms
  - Release: 80 ms
  - Auto Makeup: ON
- Meters: 3-5 dB gain reduction

**Screenshot 3: Aggressive Compression**
- File: `SimpleComp-3-aggressive.png`
- Settings:
  - Threshold: -12 dB
  - Ratio: 8:1
  - Attack: 1 ms
  - Release: 50 ms
- Meters: 8-10 dB gain reduction

**Screenshot 4: Subtle Glue**
- File: `SimpleComp-4-subtle-glue.png`
- Settings:
  - Threshold: -24 dB
  - Ratio: 1.5:1
  - Attack: 10 ms
  - Release: 100 ms
- Meters: 2-3 dB gain reduction

**Screenshot 5: Limiting**
- File: `SimpleComp-5-limiting.png`
- Settings:
  - Threshold: -6 dB
  - Ratio: 20:1
  - Attack: 0.1 ms
  - Release: 20 ms
- Meters: 6-8 dB gain reduction (limiting)

---

### 7. WaveShaper (5 screenshots)

**Screenshot 1: Default State**
- File: `WaveShaper-1-default.png`
- Settings: Soft curve, Drive 0%, Mix 100%
- Signal: Clean waveform

**Screenshot 2: Gentle Warmth**
- File: `WaveShaper-2-gentle-warmth.png`
- Settings:
  - Curve: Soft
  - Drive: 30%
  - Mix: 100%
- Signal: Subtle harmonic distortion

**Screenshot 3: Hard Clipping**
- File: `WaveShaper-3-hard-clipping.png`
- Settings:
  - Curve: Hard
  - Drive: 80%
  - Mix: 100%
- Signal: Clipped waveform

**Screenshot 4: Fold Distortion**
- File: `WaveShaper-4-fold-distortion.png`
- Settings:
  - Curve: Fold
  - Drive: 90%
  - Mix: 100%
- Signal: Wave-folded distortion

**Screenshot 5: Parallel Processing**
- File: `WaveShaper-5-parallel.png`
- Settings:
  - Curve: Hard
  - Drive: 70%
  - Mix: 50%
- Signal: Blend of clean + distorted

---

### 8. Saturation (5 screenshots)

**Screenshot 1: Default State**
- File: `Saturation-1-default.png`
- Settings: Tape mode, Drive 0%, Tone 0%
- Signal: Clean

**Screenshot 2: Tape Warmth**
- File: `Saturation-2-tape-warmth.png`
- Settings:
  - Mode: Tape
  - Drive: 25%
  - Tone: -5%
- Signal: Warm, subtle saturation

**Screenshot 3: Tube Overdrive**
- File: `Saturation-3-tube-overdrive.png`
- Settings:
  - Mode: Tube
  - Drive: 60%
  - Tone: 0%
- Signal: Creamy overdrive

**Screenshot 4: Transistor Punch**
- File: `Saturation-4-transistor-punch.png`
- Settings:
  - Mode: Transistor
  - Drive: 40%
  - Tone: +10%
- Signal: Punchy, bright saturation

**Screenshot 5: Master Bus Glue**
- File: `Saturation-5-master-bus.png`
- Settings:
  - Mode: Tape
  - Drive: 15%
  - Tone: -3%
- Signal: Subtle cohesion

---

### 9. ChorusFlanger (5 screenshots)

**Screenshot 1: Default State**
- File: `ChorusFlanger-1-default.png`
- Settings: Chorus mode, Rate 0.5 Hz, Depth 30%, Mix 25%
- Signal: Subtle modulation

**Screenshot 2: Classic Chorus**
- File: `ChorusFlanger-2-classic-chorus.png`
- Settings:
  - Mode: Chorus
  - Rate: 0.8 Hz
  - Depth: 40%
  - Feedback: 20%
  - Mix: 25%
- Signal: Gentle stereo widening

**Screenshot 3: Fast Chorus**
- File: `ChorusFlanger-3-fast-chorus.png`
- Settings:
  - Mode: Chorus
  - Rate: 2.0 Hz
  - Depth: 50%
  - Mix: 30%
- Signal: Shimmering modulation

**Screenshot 4: Jet Flanger**
- File: `ChorusFlanger-4-jet-flanger.png`
- Settings:
  - Mode: Flanger
  - Rate: 0.3 Hz
  - Depth: 80%
  - Feedback: 70%
  - Mix: 40%
- Signal: Dramatic sweep

**Screenshot 5: Subtle Flange**
- File: `ChorusFlanger-5-subtle-flange.png`
- Settings:
  - Mode: Flanger
  - Rate: 0.5 Hz
  - Depth: 40%
  - Feedback: 30%
  - Mix: 20%
- Signal: Gentle comb filtering

---

### 10. Reverb (5 screenshots)

**Screenshot 1: Default State**
- File: `Reverb-1-default.png`
- Settings: Room Size 50%, Damping 50%, Mix 20%
- Signal: Moderate reverb tail

**Screenshot 2: Small Room**
- File: `Reverb-2-small-room.png`
- Settings:
  - Room Size: 25%
  - Damping: 40%
  - Pre-Delay: 0 ms
  - Width: 100%
  - Mix: 15%
- Signal: Tight, intimate reverb

**Screenshot 3: Large Hall**
- File: `Reverb-3-large-hall.png`
- Settings:
  - Room Size: 90%
  - Damping: 60%
  - Pre-Delay: 40 ms
  - Width: 100%
  - Mix: 35%
- Signal: Long, spacious tail

**Screenshot 4: Bright Plate**
- File: `Reverb-4-bright-plate.png`
- Settings:
  - Room Size: 60%
  - Damping: 20%
  - Pre-Delay: 30 ms
  - Width: 100%
  - Mix: 25%
- Signal: Bright, reflective reverb

**Screenshot 5: Dark Chamber**
- File: `Reverb-5-dark-chamber.png`
- Settings:
  - Room Size: 70%
  - Damping: 80%
  - Pre-Delay: 20 ms
  - Width: 80%
  - Mix: 30%
- Signal: Warm, dark reverb

---

## Capture Process

### Step-by-Step

1. **Load plugin in DAW**
   - Insert on audio track or MIDI track (for BasicSynth)
   - Open plugin UI in separate window

2. **Set up signal** (for effects):
   - Use test audio file or signal generator
   - Adjust levels for visible metering
   - Loop playback

3. **Configure plugin**
   - Follow settings for each screenshot
   - Wait for meters to stabilize

4. **Capture screenshot**:
   - **macOS**: `Cmd+Shift+4` → `Spacebar` → Click plugin window
   - **Windows**: `Win+Shift+S` → Select window
   - **Alternative**: Use DAW's built-in screenshot feature

5. **Save with correct filename**:
   - Use naming convention: `<Plugin>-<#>-<description>.png`
   - Save to: `website/public/screenshots/<Plugin>/`

6. **Verify screenshot**:
   - Check resolution (should be Retina/HiDPI)
   - Verify settings are visible
   - Ensure no tooltips or overlays

---

## Post-Processing (Optional)

### Image Optimization

**Resize for web** (if too large):
```bash
# Install ImageMagick
brew install imagemagick

# Resize to 1280px width (maintain aspect ratio)
cd website/public/screenshots/SimpleGain
mogrify -resize 1280x -quality 90 *.png
```

**Compress PNGs** (reduce file size):
```bash
# Install pngquant
brew install pngquant

# Compress with quality 80-100
pngquant --quality=80-100 *.png --ext .png --force
```

### Add Drop Shadow (Optional)

For marketing/showcase images only:

```bash
# Add subtle drop shadow
convert SimpleGain-1-default.png \
  \( +clone -background black -shadow 60x5+10+10 \) \
  +swap -background white -layers merge +repage \
  SimpleGain-1-default-shadow.png
```

---

## Directory Structure

```
website/public/screenshots/
├── SimpleGain/
│   ├── SimpleGain-1-default.png
│   ├── SimpleGain-2-reducing.png
│   ├── SimpleGain-3-boosting.png
│   ├── SimpleGain-4-fine-control.png
│   └── SimpleGain-5-bypass-compare.png
├── PanUtil/
│   ├── PanUtil-1-default.png
│   └── ... (5 total)
├── BasicSynth/
│   ├── BasicSynth-1-default.png
│   └── ... (5 total)
... (all 10 plugins)
```

---

## Website Integration

### Update Website Gallery

After capturing all screenshots, update the website to display them.

**Example (React)**:

```tsx
// website/src/components/PluginGallery.tsx

const screenshots = {
  SimpleGain: [
    { src: '/screenshots/SimpleGain/SimpleGain-1-default.png', caption: 'Default state' },
    { src: '/screenshots/SimpleGain/SimpleGain-2-reducing.png', caption: 'Gain reduction' },
    // ... more screenshots
  ],
  // ... other plugins
};

export function PluginGallery({ plugin }: { plugin: string }) {
  const images = screenshots[plugin];

  return (
    <div className="gallery">
      {images.map((img, i) => (
        <figure key={i}>
          <img src={img.src} alt={`${plugin} - ${img.caption}`} />
          <figcaption>{img.caption}</figcaption>
        </figure>
      ))}
    </div>
  );
}
```

---

## Checklist

### Before Starting
- [ ] DAW prepared (clean UI, no tooltips)
- [ ] Plugins installed and verified
- [ ] Test audio files ready
- [ ] Screenshot directories created

### During Capture (per plugin)
- [ ] Screenshot 1: Default state
- [ ] Screenshot 2: Typical use
- [ ] Screenshot 3: Extreme settings
- [ ] Screenshot 4: Metering active
- [ ] Screenshot 5: Feature showcase

### After Capture
- [ ] All 50 screenshots captured (10 plugins × 5)
- [ ] Filenames follow convention
- [ ] Screenshots saved to correct directories
- [ ] Images optimized (optional)
- [ ] Website gallery updated

---

## Time Estimate

**Per Plugin**: ~15-20 minutes (setup + 5 screenshots)
**Total**: 2.5-3 hours for all 10 plugins

**Breakdown**:
- Setup DAW and test audio: 15 min
- Capture screenshots: 2 hours
- Post-processing (optional): 30 min
- Website integration: 15 min

---

## Tips

- **Batch capture**: Do all screenshots for one plugin before moving to next
- **Consistency**: Use same DAW theme and background for all screenshots
- **Signal flow**: For effect plugins, ensure meters are active and visible
- **Lighting**: If photographing hardware or taking photos, use consistent lighting
- **Backup**: Keep original full-resolution screenshots before optimization

---

**Last Updated**: 2026-08-25
