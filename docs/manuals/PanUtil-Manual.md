# PanUtil User Manual

**AudioForge PanUtil v1.0.0**

Professional stereo panning and width control with phase correlation metering

---

## Table of Contents

1. [Overview](#overview)
2. [Quick Start](#quick-start)
3. [User Interface](#user-interface)
4. [Parameters](#parameters)
5. [Use Cases](#use-cases)
6. [Tips & Techniques](#tips--techniques)
7. [Technical Specifications](#technical-specifications)
8. [Troubleshooting](#troubleshooting)

---

## Overview

**PanUtil** is a precision stereo imaging tool that provides:

- **Stereo panning**: Position sounds in the stereo field (-100% L to +100% R)
- **Width control**: Expand or narrow the stereo image (0% to 200%)
- **Phase correlation meter**: Visualize mono compatibility
- **Constant-power panning**: Maintains perceived loudness when panning
- **Balance mode**: Alternative panning algorithm for stereo sources

### When to Use PanUtil

- **Stereo placement**: Position sounds in the left-right stereo field
- **Stereo width adjustment**: Make tracks wider or narrower
- **Mono compatibility checking**: Ensure mix translates to mono playback
- **Mid/side processing**: Control center vs sides independently
- **Creative effects**: Extreme width or phase manipulation

---

## Quick Start

### Basic Panning

1. **Load PanUtil** on a mono or stereo track
2. **Adjust Pan knob**:
   - Turn left for left positioning
   - Turn right for right positioning
   - Center (0%) for center placement
3. **Watch the stereo meter** to visualize positioning

### Width Control

1. **Load PanUtil** on a stereo track
2. **Adjust Width knob**:
   - 100% = Original width (no change)
   - <100% = Narrower (more mono)
   - >100% = Wider (more stereo separation)
3. **Watch phase correlation meter** to avoid phase issues

---

## User Interface

```
┌──────────────────────────────────────────┐
│                                          │
│  ┌─────────────┐    ┌─────────────┐     │
│  │ STEREO      │    │ PHASE CORR  │     │
│  │ METER       │    │   Meter     │     │
│  │  L      R   │    │  +1 ■       │     │
│  │  ▓      ▓   │    │   0 ■       │     │
│  │  ▓      ▓   │    │  -1 ■       │     │
│  └─────────────┘    └─────────────┘     │
│                                          │
│     ╭───╮            ╭───╮              │
│     │ ◯ │            │ ◯ │              │
│     ╰───╯            ╰───╯              │
│      Pan             Width               │
│     0%               100%                │
│                                          │
│  [Balance Mode]  ← Toggle               │
│                                          │
│  PanUtil · AudioForge v1.0.0            │
└──────────────────────────────────────────┘
```

### UI Elements

**Pan Knob**:
- **Range**: -100% (hard left) to +100% (hard right)
- **Default**: 0% (center)
- **Double-click** to reset to center

**Width Knob**:
- **Range**: 0% (mono) to 200% (ultra-wide)
- **Default**: 100% (original)
- **Double-click** to reset to 100%

**Stereo Meter**:
- Shows L/R channel levels in real-time
- Visual feedback for pan position

**Phase Correlation Meter**:
- **+1** (green): Perfect mono compatibility, in-phase
- **0** (yellow): Some phase difference
- **-1** (red): Phase-reversed, will cancel in mono

**Balance Mode Toggle**:
- **Off** (default): Pan mode (constant-power)
- **On**: Balance mode (for stereo sources)

---

## Parameters

### Pan

**Range**: -100% to +100%
**Default**: 0% (center)

**Description**:
Controls the left-right stereo position of the audio signal.

**Pan Mode** (default):
- Uses constant-power panning law (-3 dB in center)
- Maintains perceived loudness when panning
- Best for mono sources

**Balance Mode**:
- Adjusts relative level of L/R channels
- Preserves stereo image of stereo sources
- Best for stereo sources (loops, samples, groups)

**Technical Details**:
- **-100%**: Full left (L = 1.0, R = 0.0)
- **0%**: Center (L = 0.707, R = 0.707) [-3 dB each]
- **+100%**: Full right (L = 0.0, R = 1.0)

---

### Width

**Range**: 0% to 200%
**Default**: 100% (original)

**Description**:
Controls the stereo width/separation of the signal.

**Width Values**:
- **0%**: Mono (L and R summed to center)
- **100%**: Original stereo width (no change)
- **150%**: 50% wider than original
- **200%**: Double the original width (maximum)

**Technical Implementation**:
Uses mid/side (M/S) processing:
- **Mid**: Center/mono content (L+R)
- **Side**: Stereo content (L-R)
- Width control adjusts the Side level

**Warning**: Values >150% can cause phase issues. Always check phase correlation meter.

---

## Use Cases

### 1. Stereo Field Arrangement

**Goal**: Position multiple elements across the stereo field for clarity and separation.

**Technique**:
- **Center** (0%): Lead vocal, kick, snare, bass
- **Slight left/right** (±20-30%): Rhythm guitars, keys, secondary vocals
- **Wide** (±60-80%): Hi-hats, cymbals, pads, effects
- **Hard pan** (±100%): Doubled guitars, stereo effects, ear candy

**Why it works**: Creates space, prevents masking, makes mix feel wide and spacious.

---

### 2. Stereo Width Enhancement

**Goal**: Make a track sound wider/bigger without losing mono compatibility.

**Technique**:
1. Start with Width at 100%
2. Gradually increase to 120-140%
3. **Watch phase correlation meter**:
   - Stay above 0 for mono compatibility
   - If it goes below 0, reduce width

**When to use**:
- Synth pads (makes them lush and spacious)
- Stereo reverbs (adds depth)
- Background vocals (makes them feel "around" the lead)

**When NOT to use**:
- Bass (keep it mono/centered)
- Kick/snare (lose power when widened)
- Lead vocals (can lose focus)

---

### 3. Mono Compatibility Checking

**Goal**: Ensure your mix sounds good on mono playback (phones, club systems, Bluetooth speakers).

**Technique**:
1. Adjust width on various tracks
2. **Watch phase correlation meter**:
   - **+0.5 to +1.0**: Safe for mono (good)
   - **0 to +0.5**: Some phase cancellation (check in mono)
   - **<0**: Significant cancellation (fix by reducing width or adjusting pan)

**Fix phase issues**:
- Reduce width to 80-90%
- Adjust panning of clashing elements
- Flip phase on one track if needed

---

### 4. Haas Effect (Stereo Widening)

**Goal**: Create psychoacoustic width using timing differences.

**Technique** (requires 2 instances of PanUtil):
1. **Track 1**: Pan 100% left, no width change
2. **Track 2** (duplicate): Pan 100% right, delay by 10-30ms
3. Result: Wide stereo image from mono source

**Note**: PanUtil alone doesn't add delay - combine with a delay plugin for Haas effect.

---

### 5. Mid/Side Processing

**Goal**: Control center vs sides independently.

**Width as M/S control**:
- **Increase width**: Boosts sides, makes track spacious
- **Decrease width**: Boosts mid, makes track focused

**Creative uses**:
- **Vocals**: Reduce width to 80-90% for focused, centered sound
- **Pads**: Increase width to 130-150% for lush, enveloping feel
- **Guitars**: Pan left/right, then increase width for "wall of sound"

---

## Tips & Techniques

### 1. LCR Panning (Left-Center-Right)

**Technique**: Only pan to hard left, center, or hard right (no in-between).

**Benefits**:
- Maximum clarity and separation
- Works well for rock, metal, and dense mixes
- Easy to visualize and manage

**How to use PanUtil**:
- Center: 0%
- Hard left: -100%
- Hard right: +100%

---

### 2. Stereo Widening Without Phase Issues

**Rule of thumb**: Stay below 150% width for most sources.

**Safe widening**:
1. Set width to 120-140%
2. Check phase correlation meter (should stay >0.3)
3. A/B test in mono (solo track, toggle DAW mono button)
4. If it sounds thin in mono, reduce width

---

### 3. Use Balance Mode for Stereo Sources

**When you have**:
- Stereo loops
- Stereo samples
- Stereo recorded instruments (acoustic guitars, pianos)

**Why**: Balance mode preserves the internal stereo image while adjusting L/R position.

**Toggle**: Click "Balance Mode" button in UI.

---

### 4. Narrow Bass for Focus and Power

**Technique**:
1. Insert PanUtil on bass track
2. Reduce width to 0-50%
3. Result: Focused, centered bass with more punch

**Why**: Bass frequencies below ~200 Hz should be mostly mono for:
- Maximum power and impact
- Mono compatibility
- Better translation to small speakers

---

### 5. Wide Pad/Synth Trick

**Technique**:
1. Duplicate synth/pad track
2. **Track 1**: Pan -30%, Width 120%
3. **Track 2**: Pan +30%, Width 120%
4. Result: Massive, surrounding stereo image

**Bonus**: Detune Track 2 by +5-10 cents for chorus-like effect.

---

### 6. Check Phase Correlation Constantly

**Best practice**: Always keep an eye on the phase correlation meter.

**Target values**:
- **Master bus**: +0.7 to +1.0 (excellent mono compatibility)
- **Individual tracks**: +0.3 to +1.0 (acceptable)
- **Warning zone**: 0 to +0.3 (check in mono)
- **Problem**: <0 (phase issues, will sound thin in mono)

---

## Technical Specifications

### Audio Processing

- **Bit Depth**: 32-bit floating point
- **Sample Rate**: 44.1 kHz - 192 kHz
- **Latency**: 0 samples
- **Pan Law**: -3 dB constant-power (pan mode)
- **Width Algorithm**: Mid/side processing

### Performance

- **CPU Usage**: < 0.2% (negligible)
- **RAM Usage**: < 1 MB

### Metering

- **Stereo Meter**: Peak level, L/R independent
- **Phase Correlation**: -1.0 (phase-reversed) to +1.0 (perfect mono)
  - Update rate: 50ms
  - Smoothing: 200ms

### Compatibility

- **Formats**: VST3, AU (macOS), Standalone
- **Platforms**: macOS 10.13+, Windows 10/11, Linux
- **DAWs Tested**: Ableton, Logic, Reaper, FL Studio, Cubase, Bitwig

---

## Troubleshooting

### Mix sounds thin in mono

**Cause**: Excessive width or phase issues.

**Fix**:
1. Check phase correlation meter (should be >0.3)
2. Reduce width on tracks with values >140%
3. A/B test in mono to identify problematic tracks
4. Consider using less stereo widening plugins overall

---

### Pan doesn't seem to work

**Cause**: Balance mode enabled on mono source, or subsequent plugin is re-centering.

**Fix**:
1. Disable Balance Mode (use Pan mode for mono sources)
2. Check if other plugins after PanUtil are affecting pan
3. Verify track is actually receiving audio (check meters)

---

### Phase correlation meter always shows +1.0

**Cause**: Track is mono (both L/R identical).

**Fix**:
- This is normal for mono tracks
- Width control will have no effect on pure mono sources
- If you want stereo from mono, add stereo delay, chorus, or reverb first

---

## Support

For installation, bug reports, and feature requests, see [SimpleGain Manual - Support section](SimpleGain-Manual.md#support).

---

**AudioForge PanUtil v1.0.0**
© 2026 AudioForge
https://audioforge.fluxstudio.art

*Last updated: 2026-08-23*
