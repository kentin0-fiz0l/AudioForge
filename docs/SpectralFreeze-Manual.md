# SpectralFreeze User Manual

**Version 1.0.0**
**AudioForge Plugin Suite**

---

## Table of Contents

1. [Overview](#overview)
2. [Installation](#installation)
3. [Quick Start](#quick-start)
4. [User Interface](#user-interface)
5. [Parameters](#parameters)
6. [Presets](#presets)
7. [Creative Techniques](#creative-techniques)
8. [Technical Specifications](#technical-specifications)
9. [Tips & Tricks](#tips--tricks)
10. [Troubleshooting](#troubleshooting)

---

## Overview

**SpectralFreeze** is a real-time spectral manipulation plugin that captures and freezes the frequency content of audio, allowing you to manipulate it in creative ways. Built on a 2048-sample FFT engine with 75% overlap, it provides high-quality spectral processing with minimal artifacts.

### Key Features

- **Spectral Freezing**: Capture and hold any spectrum indefinitely
- **Real-Time Visualization**: 60 FPS spectrogram display with frozen spectrum overlay
- **Advanced Effects**: Stretch, shift, morph, gate, and randomize spectral content
- **6 Factory Presets**: Ready-to-use settings showcasing different techniques
- **Parameter Smoothing**: Click-free parameter changes for live performance
- **Low Latency**: Optimized FFT processing for real-time use

---

## Installation

### macOS

**VST3**: `~/Library/Audio/Plug-Ins/VST3/SpectralFreeze.vst3`
**AU**: `~/Library/Audio/Plug-Ins/Components/SpectralFreeze.component`

### Supported DAWs

- Logic Pro X / Logic Pro 11
- Ableton Live 11+
- FL Studio 20+
- Reaper 6+
- Pro Tools (VST3)
- Any VST3 or AU-compatible DAW

---

## Quick Start

1. **Load the plugin** on an audio track or insert
2. **Play audio** through the plugin to see the spectrogram visualization
3. **Click FREEZE** to capture the current spectrum
4. **Adjust parameters** (Blur, Stretch, etc.) to manipulate the frozen spectrum
5. **Try factory presets** from the dropdown menu for instant results

### Basic Freeze Workflow

```
Audio Input → FFT Analysis → Freeze (Capture) → Spectral Effects → IFFT Synthesis → Audio Output
```

---

## User Interface

### Layout

```
┌─────────────────────────────────────────────────────┐
│  SpectralFreeze                                     │
│  Production-Ready Spectral Manipulation             │
├─────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────┐  │
│  │  Spectrogram Visualization (60 FPS)         ��  │
│  │  Color: Blue (low) → Green → Yellow → Red   │  │
│  └──────────────────────────────���──────────────┘  │
├─────────────────────────────────────────────────────┤
│          ┌─────────┐                                │
│          │ FREEZE  │  [FROZEN indicator]            │
│          └─────────┘                                │
│  Presets: [Factory Presets ▼] [Save] [Load]        │
├─────────────────────────────────────────────────────┤
│  Row 1: [Blur] [Low Cut] [High Cut] [Dry/Wet] [Stretch] │
│  Row 2: [Shift] [Morph] [Gate] [Randomize]        │
└─────────────────��───────────────────────────────────┘
```

### Spectrogram Display

- **Time axis**: Right = newest, Left = oldest (~3.3 seconds of history)
- **Frequency axis**: Top = 20 kHz, Bottom = 20 Hz (logarithmic)
- **Color coding**: Intensity represents magnitude in dB (-60 dB to 0 dB)
- **Red overlay**: Shows frozen spectrum when FREEZE is active

---

## Parameters

### Phase 1: Core Controls

#### FREEZE (Toggle Button)
- **Function**: Captures and holds the current spectrum
- **States**: OFF (live audio) / ON (frozen spectrum)
- **Use Case**: Freeze pads, drones, or transients for manipulation
- **Tip**: Freeze during interesting moments (chords, impacts, noise bursts)

#### Blur (0-100%)
- **Function**: Gaussian smoothing across frequency bins
- **Range**: 0% (no blur) to 100% (maximum smoothing)
- **Effect**: Softens spectral peaks, creates ethereal textures
- **CPU**: Minimal impact
- **Tip**: Use 50-75% for smooth pad sounds, 100% for extreme diffusion

#### Low Cut (20-20000 Hz)
- **Function**: High-pass filter (removes low frequencies)
- **Range**: 20 Hz to 20 kHz (logarithmic scale)
- **Default**: 20 Hz (no filtering)
- **Use Case**: Remove rumble, isolate highs
- **Tip**: Combine with High Cut for bandpass effects

#### High Cut (20-20000 Hz)
- **Function**: Low-pass filter (removes high frequencies)
- **Range**: 20 Hz to 20 kHz (logarithmic scale)
- **Default**: 20 kHz (no filtering)
- **Use Case**: Remove harshness, create darkness
- **Tip**: Sweep in real-time for filter sweep effects

#### Dry/Wet (0-100%)
- **Function**: Blend between original (dry) and processed (wet) signal
- **Range**: 0% (100% dry) to 100% (100% wet)
- **Default**: 100% (fully wet)
- **Use Case**: Parallel processing, subtle effects
- **Tip**: 30-50% for layered textures, 100% for full effect

### Phase 2: Advanced Spectral Effects

#### Stretch (0-100%)
- **Function**: Time-stretches the frozen spectrum by interpolating bins
- **Range**: 0% (1x speed) to 100% (5x slower)
- **Effect**: Slows down spectral evolution without pitch change
- **Use Case**: Frozen pads that evolve slowly, time-stretching harmonics
- **Tip**: Combine with Blur for smooth, stretched textures

#### Shift (-100 to +100)
- **Function**: Shifts frequencies up or down by moving FFT bins
- **Range**: -100 (down ~2 octaves) to +100 (up ~2 octaves)
- **Effect**: Formant shifting, pitch manipulation
- **Use Case**: Create chipmunk/monster voices, pitch frozen spectrum
- **Tip**: Negative values = darker, positive = brighter

#### Morph (0-100%)
- **Function**: Crossfades between live and frozen spectrum
- **Range**: 0% (live audio) to 100% (frozen spectrum)
- **Effect**: Gradual blend even when not frozen
- **Use Case**: Smooth transitions, ghosting effects
- **Tip**: Freeze something interesting, then morph between new audio and frozen

#### Gate (0-100%)
- **Function**: Silences frequency bins below threshold (relative to max)
- **Range**: 0% (no gating) to 100% (extreme gating)
- **Effect**: Isolates prominent frequencies, removes noise
- **Use Case**: Clean up muddy mixes, isolate peaks
- **Tip**: 30-50% for gentle cleanup, 70%+ for aggressive isolation

#### Randomize (0-100%)
- **Function**: Randomizes phase of frozen spectrum
- **Range**: 0% (no randomization) to 100% (full chaos)
- **Effect**: Creates granular, textured, diffuse sounds
- **Use Case**: Turn frozen tones into textures, pseudo-reverb
- **Tip**: 50%+ for grainy textures, 100% for complete phase chaos

---

## Presets

### Factory Presets

#### 1. Init
- **Description**: Clean slate, all effects off
- **Use Case**: Starting point for custom settings

#### 2. Frozen Blur
- **Settings**: Freeze ON, Blur 75%
- **Effect**: Smooth, ethereal frozen spectrum
- **Use Case**: Ambient pads, drone layers

#### 3. Spectral Stretch
- **Settings**: Freeze ON, Stretch 80%
- **Effect**: Time-stretched frozen spectrum (5x slower)
- **Use Case**: Slow-motion spectral evolution

#### 4. Frequency Shifter
- **Settings**: Shift +50 (up ~1 octave)
- **Effect**: Real-time pitch shifting up
- **Use Case**: Formant shifting, harmonizer effects

#### 5. Spectral Gate
- **Settings**: Gate 50%
- **Effect**: Isolates prominent frequencies
- **Use Case**: Cleaning up mixes, isolating peaks

#### 6. Granular Texture
- **Settings**: Freeze ON, Randomize 75%
- **Effect**: Frozen spectrum with granular texture
- **Use Case**: Textured pads, pseudo-granular synthesis

### Saving Custom Presets

1. **Adjust parameters** to your liking
2. **Click "Save"** button (saves as "quick-save.xml")
3. **Location**: `~/Documents/AudioForge/SpectralFreeze/Presets/`
4. **Recall**: Click "Load" button to restore

### Sharing Presets

- Presets are stored as human-readable XML files
- Location: `~/Documents/AudioForge/SpectralFreeze/Presets/`
- Share `.xml` files with other users
- Rename files to organize your library

---

## Creative Techniques

### 1. Frozen Pad Layer

**Goal**: Create an evolving ambient pad layer

1. Play a rich chord or harmonic sound
2. FREEZE at a musically interesting moment
3. Set Blur to 60-80% for smoothness
4. Add Stretch (40-60%) for slow evolution
5. Set Dry/Wet to 40% to blend with live audio
6. Automate Morph (0-100%) for gradual transitions

### 2. Spectral Time-Freeze

**Goal**: Pause time and manipulate the frozen moment

1. FREEZE on a transient (drum hit, impact)
2. Set Gate to 40% to isolate strong harmonics
3. Sweep Shift (-50 to +50) for pitch bends
4. Add Randomize (30-50%) for texture
5. Automate Dry/Wet for dramatic reveal

### 3. Formant Vocoder Effect

**Goal**: Create robotic vocal textures

1. Input: Vocal or melodic line
2. Enable Shift (20-40) for upward formant shift
3. Add slight Blur (20-30%) for smoothness
4. Optional: Freeze interesting syllables for stuttering

### 4. Granular Texture from Tones

**Goal**: Transform simple tones into complex textures

1. Input: Sine wave or simple oscillator
2. FREEZE
3. Set Randomize to 80-100%
4. Add Stretch (50-70%) for slower grain evolution
5. Adjust Gate (20-40%) to sculpt density

### 5. Spectral Gating for Mixing

**Goal**: Clean up muddy recordings

1. Input: Full mix or instrument track
2. Set Gate to 30-50%
3. Adjust Low Cut / High Cut to target frequency range
4. Use Dry/Wet (60-80%) for subtle cleanup
5. Automate Gate threshold for dynamic control

---

## Technical Specifications

### DSP Engine

- **FFT Size**: 2048 samples (11th order)
- **Window Function**: Hann (pre-generated for efficiency)
- **Overlap**: 75% (512-sample hop size)
- **Latency**: ~512 samples (11.6 ms @ 44.1 kHz)
- **Processing**: Overlap-add STFT with magnitude/phase manipulation

### Performance

- **Sample Rates**: 44.1 kHz, 48 kHz, 88.2 kHz, 96 kHz
- **CPU Usage**: ~3-5% single core (2.5 GHz Intel)
- **RAM**: ~15 MB resident
- **Parameter Smoothing**: Exponential smoothing (0.99 coefficient)
- **Visualization**: 60 FPS refresh, 200-frame history buffer

### Audio Quality

- **Bit Depth**: 32-bit float internal processing
- **Frequency Response**: 20 Hz - 20 kHz (±0.1 dB)
- **THD+N**: < 0.001% @ 1 kHz (effects off)
- **Dynamic Range**: > 110 dB

---

## Tips & Tricks

### Performance Optimization

1. **Disable visualization**: Close plugin window when not in use (saves CPU)
2. **Freeze-then-render**: Capture interesting moments, then bounce to audio
3. **Use Dry/Wet**: Parallel processing reduces artifacts

### Creative Workflow

1. **Automate FREEZE**: Map to MIDI CC for live performance control
2. **Chain effects**: Use after reverb/delay for frozen ambience
3. **Layer instances**: Stack 2-3 SpectralFreeze on different tracks with varying settings
4. **Sidechain control**: Use with sidechain gate for rhythmic freezing

### Avoiding Artifacts

1. **Gentle parameter changes**: Smoothing prevents clicks, but fast tweaks can glitch intentionally
2. **Match input level**: Loud signals = better spectrogram visibility
3. **Freeze on harmonics**: Freezing white noise sounds less interesting than pitched content
4. **Use filters**: Low Cut / High Cut prevent extreme buildup

---

## Troubleshooting

### No Audio Output

- **Check Dry/Wet**: If set to 0%, no processed signal passes
- **Check FREEZE state**: Frozen spectrum may be silent if captured during silence
- **Check host routing**: Verify plugin is on correct track/bus

### Clicking or Pops

- **Parameter automation**: Smooth transitions via DAW automation curves
- **CPU overload**: Increase buffer size in DAW settings
- **Plugin instance limit**: Reduce number of active SpectralFreeze instances

### Visualization Not Updating

- **Window not visible**: Plugin must be open for 60 FPS updates
- **Low input level**: Increase gain before SpectralFreeze for better visualization
- **DAW playback**: Visualization updates only during audio processing

### Presets Not Saving

- **Directory permissions**: Ensure `~/Documents/AudioForge/SpectralFreeze/Presets/` is writable
- **Disk space**: Check available storage
- **File conflicts**: Rename existing "quick-save.xml" if corrupted

### High CPU Usage

- **Multiple instances**: Each instance uses ~3-5% CPU
- **High sample rate**: 96 kHz uses more CPU than 44.1 kHz
- **Visualization**: Close plugin window to save ~1-2% CPU

---

## Support & Resources

### Documentation

- **Manual**: This document
- **Video Tutorials**: Coming soon
- **GitHub**: [AudioForge Repository](https://github.com/kentin0-fiz0l/AudioForge)

### Community

- **Discussions**: GitHub Discussions
- **Issues**: GitHub Issues
- **Contact**: support@audioforge.dev

### License

SpectralFreeze is part of the AudioForge plugin suite.
Copyright © 2025 AudioForge. All rights reserved.

---

**Thank you for using SpectralFreeze!**
We hope this plugin inspires creative spectral manipulation in your productions.

---

*Manual Version 1.0.0 — Last Updated: 2025-08-30*
