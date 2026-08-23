# CleanDelay User Manual

**AudioForge CleanDelay v1.0.0**

Stereo delay with ping-pong mode and tempo sync

---

## Overview

**CleanDelay** provides crystal-clear delay effects with:

- **Stereo delay**: Independent L/R delay times
- **Ping-pong mode**: Bouncing echoes between L/R channels
- **Tempo sync**: Lock to DAW tempo (1/16 to 2 bars)
- **Feedback control**: 0-95% (infinite repeats possible)
- **High-pass filter**: Clean up low-end in feedback path
- **Wet/dry mix**: Blend delayed signal with original

### When to Use CleanDelay

- **Rhythmic delays**: Synced to tempo for musical timing
- **Stereo widening**: Ping-pong creates wide, spacious delays
- **Slapback echo**: Short delays (40-120ms) for vintage vibe
- **Dub effects**: High feedback, filtered delays
- **Vocal depth**: Subtle delays add dimension without mud

---

## Quick Start

### Basic Tempo-Synced Delay

1. **Load CleanDelay** on a vocal or instrument track
2. **Set Delay Time** to 1/4 note (quarter note)
3. **Set Feedback** to 30-50%
4. **Set Mix** to 20-30% (subtle)
5. Result: **Musical delay in time with song**

### Ping-Pong Effect

1. **Enable Ping-Pong mode** (button)
2. **Set Delay Time** to 1/8 note (eighth note)
3. **Set Feedback** to 40-60%
4. **Set Mix** to 30-40%
5. Result: **Bouncing stereo delay**

---

## Parameters

### Delay Time

**Sync Mode** (default):
- **1/32** to **2 bars** (musical divisions)
- Automatically adjusts to DAW tempo changes
- Examples: 1/16, 1/8, 1/4, 1/2, 1 bar

**Free Mode** (ms):
- **1 ms to 2000 ms** (2 seconds)
- Independent of tempo
- Use for slapback (40-120ms) or special effects

**Tip**: Click "Sync" button to toggle between Sync and Free modes.

---

### Feedback

**Range**: 0-95%
**Default**: 30%

**Description**:
- **0%**: Single echo (no repeats)
- **30-50%**: Natural decay (3-5 repeats)
- **60-80%**: Long tail (8-12 repeats)
- **90-95%**: Near-infinite repeats (dub-style)

**Warning**: Values >90% can build up quickly - watch your levels!

---

### Mix (Wet/Dry)

**Range**: 0-100%
**Default**: 25%

- **0%**: Dry only (bypass)
- **25-40%**: Subtle delay (vocals, leads)
- **50%**: Equal wet/dry (special effects)
- **100%**: Wet only (send/return use)

---

### High-Pass Filter

**Range**: 20 Hz - 1000 Hz
**Default**: 200 Hz

Filters low frequencies from feedback path to prevent muddiness.

**Settings**:
- **20 Hz**: No filtering (full bass in repeats)
- **200 Hz**: Clean up sub-bass (recommended for most uses)
- **500 Hz**: Thin repeats (telephone-like effect)

---

### Ping-Pong Mode

**Off** (default): Stereo delay with independent L/R
**On**: Ping-pong (delays bounce L → R → L → R)

**Use ping-pong for**:
- Wide, spacious delays
- Rhythmic effects
- Creative stereo movement

---

## Use Cases

### 1. Vocal Slapback Delay

**Settings**:
- Delay Time: 80-120ms (Free mode)
- Feedback: 0-20%
- Mix: 15-25%
- HPF: 300 Hz

**Result**: Classic rockabilly/50s vocal sound

---

### 2. Synth Lead Rhythmic Delay

**Settings**:
- Delay Time: 1/8 (Sync)
- Feedback: 40%
- Mix: 30%
- Ping-Pong: On

**Result**: Bouncing, rhythmic delay in time with track

---

### 3. Dub/Reggae Delay

**Settings**:
- Delay Time: 1/4 or 3/16 (Sync)
- Feedback: 70-85%
- Mix: 40-50%
- HPF: 400 Hz

**Result**: Classic dub echo with filtered repeats

---

### 4. Ambient Pad Delay

**Settings**:
- Delay Time: 1/2 or 1 bar (Sync)
- Feedback: 60%
- Mix: 35%
- Ping-Pong: On
- HPF: 250 Hz

**Result**: Lush, evolving delay tail

---

## Tips & Techniques

### 1. Automate Feedback for Swells

Automate Feedback from 30% → 90% for dramatic delay builds (drop sections).

### 2. Combine with Reverb

**Chain**: CleanDelay → Reverb
**Why**: Delays feed into reverb for depth and space

### 3. Use in Send/Return

Set Mix to 100%, use as send effect for parallel processing.

---

**AudioForge CleanDelay v1.0.0**
© 2026 AudioForge

*Last updated: 2026-08-23*
