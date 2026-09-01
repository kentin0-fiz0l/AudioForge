# AudioForge Sampler Plugin - User Guide

## Overview

The AudioForge Sampler is a professional multi-sample instrument plugin that allows you to load your own audio samples and map them across MIDI notes with velocity layers. Perfect for creating custom drum kits, sampled instruments, and unique sound libraries.

**Key Features**:
- Multi-sample support (up to 512 MB memory)
- Key zone mapping with visual piano keyboard
- Velocity layers (up to 4 layers per zone)
- Loop playback (Forward, Ping-Pong modes)
- ADSR envelope shaping
- Multi-mode filter with envelope modulation
- 20 factory presets
- Drag-and-drop sample loading
- 16-voice polyphony

---

## Getting Started

### Loading Your First Sample

1. **Click "Load Sample..."** button or drag-and-drop a WAV/AIFF/FLAC file onto the waveform display
2. **Sample appears** in the waveform viewer and sample browser
3. **Auto-creates a zone** at C4-C5 if no zones exist
4. **Play MIDI notes** C4-C5 to hear your sample

### Supported File Formats

- ✅ **WAV** (16/24/32-bit, float)
- ✅ **AIFF** (Apple format)
- ✅ **FLAC** (lossless compression)
- ❌ MP3 (not supported - convert to WAV/FLAC first)

**Recommended**: 44.1kHz or 48kHz, stereo or mono

---

## Interface Overview

```
┌──────────────────────────────────────────────────┐
│ WAVEFORM DISPLAY (Top)                          │
│ • Shows loaded sample with loop markers         │
│ • Drag yellow handles to set loop points        │
│ • Scroll wheel + Cmd: Horizontal zoom           │
│ • Scroll wheel + Shift: Vertical zoom           │
├──────────────────────────────────────────────────┤
│ ZONE EDITOR (Middle)                            │
│ • 88-key piano keyboard (A0-C8)                 │
│ • Colored rectangles = key zones                │
│ • Drag edges to resize zones                    │
│ • Right-click for menu (Add/Delete/Set Root)    │
├──────────────────────────────────────────────────┤
│ CONTROLS (Bottom)                               │
│ ┌─────────────────┬──────────────────────────┐ │
│ │ LEFT SIDE       │ RIGHT SIDE               │ │
│ │ • Preset select │ • Velocity Layer Editor  │ │
│ │ • Loop mode     │ • Add/Delete layers      │ │
│ │ • ADSR envelope │ • Sample Browser         │ │
│ │ • Filter        │ • Loaded samples list    │ │
│ └─────────────────┴──────────────────────────┘ │
└──────────────────────────────────────────────────┘
```

---

## Working with Key Zones

### What are Key Zones?

Key zones map MIDI note ranges to samples. For example:
- Zone 1: C1 (kick drum)
- Zone 2: D1 (snare)
- Zone 3: F#1 (hi-hat)

Each zone can have multiple velocity layers for dynamic response.

### Creating Zones

**Method 1: Add Zone Button**
1. Click **"Add Zone"**
2. New zone appears at C4 (middle C)
3. Drag zone edges to resize

**Method 2: Right-Click Menu**
1. Right-click on piano keyboard
2. Select **"Add Zone Here"**
3. Zone created at clicked note

**Method 3: Auto-Create**
- First sample loaded auto-creates C4-C5 zone

### Editing Zones

**Resize Zone**:
- Hover near zone edge (cursor changes to ↔)
- Click and drag to resize
- Snap-to-note grid for precision

**Set Root Note**:
- Right-click zone → "Set Root Note"
- Yellow marker shows root note
- Sample plays at original pitch on root note

**Delete Zone**:
1. Click zone to select
2. Click **"Delete Zone"** button
3. Or right-click → "Delete Zone"

### Zone Colors

Zones use 8 distinct colors for easy identification:
- 🟦 Blue, 🟩 Green, 🟥 Red, 🟨 Yellow
- 🟧 Orange, 🟪 Purple, 🟫 Cyan, ⬜ White

---

## Velocity Layers

### What are Velocity Layers?

Velocity layers allow different samples to play based on how hard you hit a key (MIDI velocity 0-127).

**Example**:
- Layer 1 (0-63): Soft snare sample
- Layer 2 (64-127): Hard snare sample

### Adding Velocity Layers

1. **Select a zone** on the piano keyboard
2. **Select a sample** in the sample browser
3. Click **"Add Layer"** button
4. Layer appears in velocity layer editor

### Editing Layer Boundaries

1. **Hover** over boundary between layers (horizontal line)
2. **Drag up/down** to adjust velocity split point
3. **Release** to finalize

**Example**: Move boundary from 64 to 80 for softer response threshold

### Layer Display

```
┌─────────────────┐
│ 127 ┌─────────┐ │ Layer 2: Hard sample
│     │ Sample 1│ │
│  80 ├─────────┤ │ ← Drag this line
│     │ Sample 0│ │ Layer 1: Soft sample
│   0 └─────────┘ │
└─────────────────┘
```

---

## Loop Playback

### Loop Modes

**None** (default):
- Sample plays once from start to end
- Use for drums, percussion, one-shots

**Forward**:
- Loops continuously between loop points
- Use for sustained notes (pads, strings, organ)

**Ping-Pong**:
- Loops forward then backward
- Smoother loop transitions, less repetitive

### Setting Loop Points

**Method 1: Waveform Handles**
1. Load a sample
2. Drag **yellow handles** on waveform to desired loop points
3. Start handle = loop start, End handle = loop end

**Method 2: Sample Properties** (future)
- Right-click sample → "Edit Loop Points"
- Enter exact values in seconds or samples

### Loop Best Practices

**Find Zero Crossings**:
- Zoom in on waveform (Cmd + scroll wheel)
- Place loop points where waveform crosses zero
- Prevents clicks and pops

**Test with Sustain**:
- Play and hold a MIDI note
- Listen for smooth loop transition
- Adjust loop points if you hear clicks

**Crossfade Length**:
- Built-in 3ms crossfade automatically applied
- No additional setup needed

---

## Envelope & Filter

### ADSR Envelope

Controls amplitude (volume) over time:

- **Attack**: How quickly sound reaches full volume (0-2s)
- **Decay**: How quickly sound drops to sustain level (0-2s)
- **Sustain**: Held volume level while key pressed (0-100%)
- **Release**: How quickly sound fades after key release (0-5s)

**Presets Examples**:
- Drums: A=0.001, D=0.2, S=0, R=0.2
- Pad: A=0.8, D=0.5, S=0.9, R=2.0
- Piano: A=0.001, D=0.5, S=0.5, R=0.8

### Filter

Multi-mode filter with cutoff and resonance:

- **Cutoff**: Frequency where filter starts cutting (20 Hz - 20 kHz)
- **Resonance**: Emphasis at cutoff frequency (0.1 - 10.0)

**Filter Types** (future):
- Low-pass (default): Cuts high frequencies
- High-pass: Cuts low frequencies
- Band-pass: Keeps only middle frequencies
- Notch: Removes middle frequencies

---

## Factory Presets

### Preset Categories

**Drums (5 presets)**:
- Acoustic Kit, Electronic Kit, 808 Kit, Hip-Hop Kit, Jazz Kit

**Instruments (10 presets)**:
- Grand Piano, Electric Piano, Acoustic Bass, Strings Ensemble, Brass Section, Synth Lead, Organ, Plucked Strings

**Textures (5 presets)**:
- Ambient Pad, Cinematic Impact, Vinyl Texture, Rain Ambience, Sci-Fi FX, Vocal Chops, Granular Clouds

### Loading Presets

1. Click **Preset** dropdown (top-left)
2. Select preset from list
3. Parameters update automatically

**Note**: Presets only set parameters (ADSR, filter, loop mode). You must load samples separately.

### Saving Custom Presets (Future)

Currently, presets are read-only factory presets. User preset saving coming in v1.6.0.

---

## Memory Management

### Memory Budget: 512 MB

The sampler limits total sample memory to **512 MB** to prevent excessive RAM usage.

**Capacity**:
- ~60 minutes of 44.1kHz stereo audio
- ~100 typical instrument samples (1-30 seconds each)

### Memory Display

Top-right corner shows: `Memory: 45.2 / 512 MB`

### What Happens When Memory is Full?

- New samples are **rejected** with error message
- Delete unused samples to free memory
- Use shorter samples or lower sample rates

### Sample Size Guidelines

| Sample Type | Typical Size |
|-------------|--------------|
| Drum hit (1s, 44.1kHz, stereo) | ~350 KB |
| Sustained note (5s, 48kHz, stereo) | ~1.8 MB |
| Long pad (30s, 44.1kHz, stereo) | ~10.5 MB |

**Tip**: Use mono samples instead of stereo where possible (50% memory savings).

---

## Performance Tips

### CPU Optimization

**16-Voice Polyphony**:
- Default: 16 simultaneous voices
- Typical CPU usage: <10% per instance
- Voice stealing: oldest or quietest voice replaced when limit reached

**Sample Rate Matching**:
- Load samples at your project sample rate (44.1/48 kHz)
- Mismatched rates require resampling (higher CPU)

### Workflow Tips

**Organize Samples Before Loading**:
- Name samples clearly (kick_soft.wav, snare_hard.wav)
- Group by instrument/kit
- Use consistent sample rates

**Use Velocity Layers Sparingly**:
- 2 layers = soft/hard (most common)
- 3 layers = soft/medium/hard (orchestral)
- 4+ layers = diminishing returns, more CPU

**Test in Context**:
- Solo instrument to verify zone mapping
- Play with other tracks to check CPU usage
- Use standalone app for testing outside DAW

---

## Keyboard Shortcuts

| Action | Shortcut |
|--------|----------|
| Horizontal zoom | Cmd + Mouse wheel |
| Vertical zoom | Shift + Mouse wheel |
| Reset zoom | Double-click waveform |
| Delete zone | Delete key (zone selected) |
| Add zone | Cmd + N (future) |

---

## Troubleshooting

### Sample Won't Load

**Error: "Unsupported format"**
- Convert MP3 to WAV/FLAC
- Try exporting as 16-bit WAV from DAW

**Error: "Memory limit exceeded"**
- Delete unused samples
- Use shorter samples
- Reduce sample rate to 44.1 kHz

### No Sound When Playing Notes

1. **Check zone mapping**: Does zone cover the MIDI note you're playing?
2. **Check velocity**: Is MIDI velocity within layer range?
3. **Check sample assignment**: Does zone have a sample assigned?
4. **Check envelope**: Is attack/release too slow?
5. **Check DAW monitoring**: Is track armed/monitored?

### Loop Clicks/Pops

1. **Find zero crossings**: Zoom in on waveform, place loop points where waveform crosses zero
2. **Adjust loop end**: Move loop end slightly earlier/later
3. **Use Ping-Pong mode**: Less noticeable loop transitions

### High CPU Usage

1. **Reduce polyphony**: Fewer simultaneous notes
2. **Match sample rates**: Use samples at project rate
3. **Simplify velocity layers**: Use 2 layers instead of 4
4. **Disable unused zones**: Delete zones you're not using

---

## Sample Library Recommendations

### Drum Kits
- **One-shot samples**: Kick, snare, hi-hat, toms, cymbals
- **Multiple velocities**: Soft, medium, hard (2-3 layers)
- **Sample rate**: 44.1 kHz mono
- **Length**: <1 second (tight, punchy)

### Instruments
- **Sampled at root notes**: C2, C3, C4, C5 (easier mapping)
- **Sustained notes**: Piano, strings, brass (use loop mode)
- **Sample rate**: 48 kHz stereo
- **Length**: 5-30 seconds with loop

### Textures & Pads
- **Long evolving samples**: 30-60 seconds
- **Loop points**: Set carefully for seamless loops
- **Sample rate**: 44.1 kHz stereo
- **Effects**: Reverb, delay pre-baked into sample

---

## Advanced Techniques

### Round-Robin Playback (Future)

Load 3 variations of the same sample (e.g., 3 snare hits). Each trigger cycles through variations for natural, less repetitive sound.

### Granular Sampling

Load short grains (50-200ms) and use high attack/release to create granular clouds.

### Reverse Samples

Reverse audio in your DAW, export as sample, then trigger reversed sounds.

### Layering

Create multiple zones for the same note range with different samples (e.g., kick + sub bass).

---

## Resources

- **Sample Libraries**: Freesound.org, Splice, Native Instruments
- **AudioForge Discord**: (link TBD)
- **GitHub Issues**: Report bugs at github.com/kentin0-fiz0l/AudioForge

---

## Version History

**v1.5.0** (2026-09-01):
- Initial release
- Multi-sample support, key zones, velocity layers
- 20 factory presets
- Loop playback (Forward, Ping-Pong)

**Future (v1.6.0)**:
- User preset saving
- Round-robin playback
- Sample editing (reverse, normalize, trim)
- Modulation matrix
