# SimpleGain User Manual

**AudioForge SimpleGain v1.0.0**

![AudioForge Logo](../assets/audioforge-logo.png)

Professional gain/volume control plugin with real-time level metering

---

## Table of Contents

1. [Overview](#overview)
2. [Installation](#installation)
3. [Quick Start](#quick-start)
4. [User Interface](#user-interface)
5. [Parameters](#parameters)
6. [Use Cases](#use-cases)
7. [Tips & Techniques](#tips--techniques)
8. [Technical Specifications](#technical-specifications)
9. [Troubleshooting](#troubleshooting)
10. [Support](#support)

---

## Overview

**SimpleGain** is a clean, transparent gain/volume control plugin designed for precise level adjustments in your mix. Unlike some DAWs' built-in gain controls, SimpleGain provides:

- **Wide range**: -60 dB to +12 dB gain adjustment
- **Real-time metering**: Peak and RMS level display
- **Smooth parameter changes**: No clicks or pops when adjusting gain
- **Zero-latency processing**: No delay introduced
- **Minimal CPU usage**: Optimized for efficiency

### When to Use SimpleGain

- **Gain staging**: Set optimal levels before processing chains
- **Volume automation**: Ride faders or automate gain changes
- **A/B comparison**: Quickly match levels when comparing different mixes
- **Headroom management**: Reduce levels to prevent clipping in later stages
- **Utility mixing**: Simple volume adjustments without color or character

---

## Installation

### macOS

**Automatic Installation** (recommended):
1. Download `AudioForge-v1.1.0-macOS.dmg`
2. Double-click DMG → Run "Install AudioForge.command"
3. Rescan plugins in your DAW

**Manual Installation**:
- **VST3**: Copy `SimpleGain.vst3` to `~/Library/Audio/Plug-Ins/VST3/`
- **AU**: Copy `SimpleGain.component` to `~/Library/Audio/Plug-Ins/Components/`

### Windows

**Automatic Installation** (recommended):
1. Download `AudioForge-Setup-v1.1.0.exe`
2. Run installer → Click "Install"
3. Rescan plugins in your DAW

**Manual Installation**:
- **VST3**: Copy `SimpleGain.vst3` to `C:\Program Files\Common Files\VST3\`

### Linux

**Ubuntu/Debian** (.deb):
```bash
sudo dpkg -i audioforge-1.1.0-amd64.deb
```

**Fedora/RHEL** (.rpm):
```bash
sudo rpm -ivh audioforge-1.1.0-1.fc39.x86_64.rpm
```

**AppImage** (universal):
```bash
./AudioForge-1.1.0-x86_64.AppImage
```

See [install-linux.md](../install-linux.md) or [install-windows.md](../install-windows.md) for detailed instructions.

---

## Quick Start

### 5-Minute Tutorial

1. **Load SimpleGain** on a track in your DAW
2. **Play audio** through the track
3. **Watch the meter**: Green = good, yellow = getting loud, red = clipping
4. **Adjust the gain knob**:
   - Turn left to reduce volume
   - Turn right to increase volume
5. **Fine-tune**: Use the dB readout for precise adjustments

**That's it!** SimpleGain is designed to be intuitive and immediate.

### Common Workflows

**Gain Staging Before Compression**:
1. Insert SimpleGain before compressor
2. Adjust gain so peaks hit around -12 to -6 dB
3. Compressor receives optimal input level

**Matching Levels for A/B Comparison**:
1. Insert SimpleGain on both tracks
2. Solo track A, note the meter reading
3. Solo track B, adjust SimpleGain to match track A's level
4. A/B compare at equal volumes

**Creating Headroom**:
1. Insert SimpleGain on master bus
2. Reduce gain by 3-6 dB
3. Prevents clipping during final mixing stages

---

## User Interface

SimpleGain features a clean, minimalist interface focused on functionality:

```
┌─────────────────────────────────────┐
│                                     │
│        ┌─────────────────┐          │
│        │  PEAK METER     │          │
│        │  ▓▓▓▓▓▓▓▓▓░░░░  │          │
│        │  RMS METER      │          │
│        │  ▓▓▓▓▓▓░░░░░░░  │          │
│        └─────────────────┘          │
│                                     │
│            ╭───╮                    │
│            │ ◯ │  ← Gain Knob       │
│            ╰───╯                    │
│           -6.0 dB  ← dB Display     │
│                                     │
│          SimpleGain                 │
│          AudioForge v1.0.0          │
└─────────────────────────────────────┘
```

### UI Elements

**Gain Knob** (center):
- **Click and drag** vertically to adjust
- **Double-click** to reset to 0 dB (unity gain)
- **Shift+drag** for fine adjustment (0.1 dB steps)
- **Ctrl/Cmd+click** to type exact value

**dB Display** (below knob):
- Shows current gain value in decibels
- Updates in real-time as you adjust the knob

**Peak Meter** (top):
- Shows instantaneous peak level
- **Green**: -∞ to -18 dB (safe)
- **Yellow**: -18 to -6 dB (moderate)
- **Red**: -6 to 0 dB (hot, risk of clipping)

**RMS Meter** (below peak):
- Shows average (RMS) level over short time window
- Better representation of perceived loudness
- Responds more slowly than peak meter

---

## Parameters

### Gain

**Range**: -60 dB to +12 dB
**Default**: 0 dB (unity gain)
**Resolution**: 0.01 dB steps

**Description**:
Controls the overall volume/level of the audio signal. Applies a simple multiplication to the audio samples.

**Technical Details**:
- **0 dB** = Unity gain (1.0x, no change)
- **-6 dB** = Half amplitude (0.5x)
- **-12 dB** = Quarter amplitude (0.25x)
- **+6 dB** = Double amplitude (2.0x)
- **-∞ dB** = Silence (mute)

**Parameter Smoothing**:
Gain changes are smoothed over ~50ms to prevent clicks and pops when adjusting in real-time or automating.

**Automation**:
This parameter can be automated in your DAW for dynamic gain changes (fade-ins, fade-outs, ducking, etc.).

---

## Use Cases

### 1. Gain Staging

**Problem**: Plugins sound different at different input levels (especially compressors, saturators, and distortion).

**Solution**:
1. Insert SimpleGain **before** your processing chain
2. Adjust so the input level hits the "sweet spot" for each processor
3. Common targets:
   - Compressor: -12 to -6 dB peaks
   - Saturation/distortion: -18 to -12 dB for subtle, -6 to 0 dB for heavy
   - EQ: Any level (usually level-independent, but watch for nonlinear EQs)

**Why it matters**: Consistent input levels = consistent processing = better sound.

---

### 2. Volume Automation

**Problem**: Need to automate volume changes without touching the fader (preserve your fader for final mix balance).

**Solution**:
1. Insert SimpleGain on the track
2. Automate the Gain parameter instead of the track fader
3. Use for:
   - Fade-ins/fade-outs
   - Ducking (lowering volume when another element plays)
   - Dynamic level adjustments within a performance

**Benefit**: Keep your fader clean for final mix adjustments, automate earlier in the chain.

---

### 3. Headroom Management

**Problem**: Mix is clipping on the master bus, but you don't want to lower every fader individually.

**Solution**:
1. Insert SimpleGain on **master bus** (or group buses)
2. Reduce gain by 3-6 dB
3. Creates headroom for mastering or final processing

**Alternative**: Use on submix buses (drums, vocals, instruments) to balance groups.

---

### 4. A/B Level Matching

**Problem**: When comparing two mixes/versions, the louder one always sounds "better" (psychoacoustic bias).

**Solution**:
1. Insert SimpleGain on both versions
2. Solo version A, note the peak meter reading (e.g., -8 dB)
3. Solo version B, adjust SimpleGain to match version A's level
4. Now A/B compare at equal loudness for accurate judgment

**Scientific basis**: Louder = perceived as better quality, even if it's not. Matching levels removes this bias.

---

### 5. Mixing Utility

**Problem**: Need simple, transparent volume control without coloration.

**Solution**:
SimpleGain adds **zero coloration** - no EQ curve, no harmonic distortion, no phase shift. Pure gain change.

**When to use**:
- Balancing layers in a vocal stack
- Adjusting individual drum mic levels
- Trimming sample volume before processing
- Any time you need "invisible" gain adjustment

---

## Tips & Techniques

### 1. Double-Click to Reset

Quickly return to 0 dB (unity gain) by double-clicking the gain knob. Useful when experimenting with levels and wanting to compare to the original.

### 2. Use Meters for Reference, Not Targets

The meters show you what's happening, but don't obsess over hitting specific numbers. Use your ears first, meters second.

**Good practice**:
- Peaks around -12 to -6 dB for most tracks (leaves headroom)
- Master bus peaks around -6 to -3 dB (before mastering)

### 3. Shift+Drag for Fine Adjustments

When you need precision (e.g., matching levels to 0.1 dB), hold **Shift** while dragging the knob. This slows down the adjustment for fine control.

### 4. Combine with Fader for Two-Stage Control

**Workflow**:
1. Use **SimpleGain** for rough level setting (e.g., "this vocal is 6 dB too hot")
2. Use **track fader** for fine mix balance

**Benefit**: SimpleGain does the heavy lifting, fader stays in a usable range for subtle adjustments.

### 5. Use Before Dynamic Processors

**Rule of thumb**: Insert SimpleGain **before** compressors, limiters, and gates.

**Why**: These processors react to input level. If your signal is too hot or too quiet, they won't work optimally. SimpleGain fixes the level before it hits the processor.

### 6. Automate for Dynamic Ducking

**Example - Podcast Ducking**:
1. Insert SimpleGain on music track
2. Automate Gain parameter to reduce by -6 dB when host speaks
3. Automate back to 0 dB during music-only sections

**Smoother than**: Automating track fader (which can cause clicks) or using a compressor (which can sound unnatural).

### 7. Use on Reference Tracks

When comparing your mix to a reference track:
1. Load reference track in your DAW
2. Insert SimpleGain on reference track
3. Adjust to match your mix's level
4. A/B compare without loudness bias

---

## Technical Specifications

### Audio Processing

- **Bit Depth**: 32-bit floating point internal processing
- **Sample Rate**: Supports all standard rates (44.1 kHz - 192 kHz)
- **Latency**: 0 samples (zero latency)
- **THD+N**: < -120 dB (inaudible distortion)
- **Frequency Response**: Perfectly flat (0 dB ± 0.01 dB, 20 Hz - 20 kHz)
- **Channel Processing**: Independent per-channel or linked stereo

### Performance

- **CPU Usage**: < 0.1% on modern CPUs (negligible)
- **RAM Usage**: < 1 MB
- **Buffer Size**: Independent of buffer size (works with all settings)

### Compatibility

- **Formats**: VST3, AU (macOS), Standalone
- **Platforms**: macOS 10.13+, Windows 10/11, Linux (Ubuntu 20.04+, Fedora 39+)
- **DAWs Tested**: Ableton Live, Logic Pro, Reaper, FL Studio, Cubase, Bitwig, Studio One, Pro Tools (VST3)

### Metering

- **Peak Meter**:
  - Attack time: Instantaneous (0 ms)
  - Release time: 300 ms
  - Range: -60 dB to 0 dB

- **RMS Meter**:
  - Window size: 300 ms (ITU-R BS.1770-4 compliant)
  - Range: -60 dB to 0 dB

---

## Troubleshooting

### Plugin doesn't appear in DAW

**Cause**: Plugin not installed or DAW not rescanned.

**Fix**:
1. Verify plugin location:
   - macOS VST3: `~/Library/Audio/Plug-Ins/VST3/SimpleGain.vst3`
   - macOS AU: `~/Library/Audio/Plug-Ins/Components/SimpleGain.component`
   - Windows: `C:\Program Files\Common Files\VST3\SimpleGain.vst3`
   - Linux: `/usr/lib/vst3/SimpleGain.vst3` or `~/.vst3/SimpleGain.vst3`

2. Rescan plugins in your DAW:
   - **Ableton**: Preferences → Plug-ins → Rescan
   - **Logic**: Quit Logic → Delete AU cache → Reopen Logic
   - **Reaper**: Options → Preferences → Plug-ins → VST → Re-scan
   - **FL Studio**: Options → Manage plugins → Find more plugins

### Audio is distorted or clipping

**Cause**: Gain set too high, causing digital clipping.

**Fix**:
1. Check the peak meter - is it hitting 0 dB (red)?
2. Reduce gain until peaks stay below -1 dB
3. If input signal is already clipping, reduce earlier in the chain

**Note**: SimpleGain doesn't add distortion - if you hear distortion, it's either clipping or coming from elsewhere.

### Knob doesn't respond smoothly

**Cause**: Graphics driver issue or high CPU load.

**Fix**:
1. Update graphics drivers
2. Increase DAW buffer size (reduces CPU load)
3. Close other plugins/applications

### Automation sounds "steppy" or "zipper-like"

**Cause**: Very fast automation movements or low sample rate.

**Fix**:
1. Use slower automation curves (avoid abrupt jumps)
2. Increase sample rate (if below 48 kHz)
3. SimpleGain already uses 50ms parameter smoothing, so this should be rare

### Meters don't move

**Cause**: No audio signal, or DAW playback stopped.

**Fix**:
1. Verify audio is playing through the track
2. Check track isn't muted
3. Try inserting a test tone generator before SimpleGain

---

## Support

### Documentation

- **Website**: https://audioforge.fluxstudio.art
- **Installation Guides**:
  - [macOS DMG Installer](../README.md#macos)
  - [Windows Installation](../install-windows.md)
  - [Linux Installation](../install-linux.md)
- **Community**: https://github.com/username/AudioForge/discussions

### Report Bugs

Found a bug? Please report it:
1. Go to https://github.com/username/AudioForge/issues
2. Click "New Issue" → "Bug Report"
3. Include:
   - OS and DAW version
   - Plugin version
   - Steps to reproduce
   - Expected vs actual behavior

### Feature Requests

Have an idea for SimpleGain? We'd love to hear it:
1. Go to https://github.com/username/AudioForge/discussions
2. Start a discussion in "Ideas" category
3. Or submit a feature request: https://github.com/username/AudioForge/issues

### Contact

- **Email**: support@audioforge.art
- **GitHub**: https://github.com/username/AudioForge

---

## License

SimpleGain is part of AudioForge, released under the **GNU General Public License v3.0**.

- **Free**: No cost, ever
- **Open Source**: Source code available on GitHub
- **Commercial Use**: Allowed (GPL3)
- **Modify**: Create your own versions (must also be GPL3)

See [LICENSE](../../LICENSE) for full terms.

---

**AudioForge SimpleGain v1.0.0**
© 2026 AudioForge
https://audioforge.fluxstudio.art

---

*This manual was last updated: 2026-08-23*
