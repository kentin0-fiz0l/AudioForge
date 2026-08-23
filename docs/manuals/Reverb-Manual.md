# Reverb User Manual

**AudioForge Reverb v1.0.0**

Freeverb-inspired algorithmic reverb with room size and damping

---

## Overview

**Reverb** provides spacious reverberation with:

- **Room Size**: 0-100% (small room to cathedral)
- **Damping**: 0-100% (bright to dark)
- **Width**: 0-100% (mono to stereo)
- **Wet/Dry Mix**: Blend reverb with dry signal
- **Pre-Delay**: 0-100ms (early reflections)

---

## Quick Start

### Small Room Ambience

1. **Room Size**: 30%
2. **Damping**: 40%
3. **Mix**: 15%
4. Result: Tight, natural room sound

### Large Hall

1. **Room Size**: 80%
2. **Damping**: 60%
3. **Width**: 100%
4. **Mix**: 30%
5. Result: Spacious hall reverb

### Plate Reverb (Short + Bright)

1. **Room Size**: 40%
2. **Damping**: 20%
3. **Mix**: 25%
4. Result: Classic plate-like reverb

---

## Parameters

### Room Size

**Range**: 0-100%
**Description**: Simulated room dimensions

- **0-30%**: Small room, tight ambience
- **40-60%**: Medium room/studio
- **70-100%**: Large hall, cathedral

---

### Damping

**Range**: 0-100%
**Description**: High-frequency absorption

- **0-30%**: Bright, reflective (tile, glass)
- **40-60%**: Neutral, balanced
- **70-100%**: Dark, absorptive (carpet, curtains)

---

### Width

**Range**: 0-100%
**Description**: Stereo width of reverb

- **0%**: Mono reverb (center)
- **50%**: Moderate width
- **100%**: Full stereo width

---

### Mix

**Range**: 0-100%
**Description**: Wet/dry balance

- **10-20%**: Subtle ambience (vocals, leads)
- **30-40%**: Moderate reverb (pads, guitars)
- **50-100%**: Wet FX (send/return use)

---

### Pre-Delay

**Range**: 0-100ms
**Description**: Delay before reverb starts

- **0ms**: Immediate reverb (thick, lush)
- **20-40ms**: Slight separation (clarity)
- **60-100ms**: Distinct echo + reverb (depth)

---

## Use Cases

### 1. Vocal Reverb (Natural)

**Settings**: Room 45%, Damping 50%, Width 70%, Mix 18%, Pre-Delay 30ms
**Result**: Clear, present vocals with space

### 2. Snare Reverb

**Settings**: Room 35%, Damping 40%, Width 80%, Mix 25%, Pre-Delay 0ms
**Result**: Punchy snare with natural ambience

### 3. Ambient Pad

**Settings**: Room 90%, Damping 70%, Width 100%, Mix 50%, Pre-Delay 10ms
**Result**: Lush, evolving ambient texture

### 4. Slapback + Reverb (Rockabilly)

**Settings**: Room 25%, Damping 30%, Mix 20%, Pre-Delay 80ms
**Result**: Short slap + subtle reverb tail

---

## Tips & Techniques

### 1. Pre-Delay for Clarity

Use 20-40ms pre-delay to separate direct sound from reverb (keeps vocals upfront).

### 2. Damping for Tone

- **Bright reverb** (low damping): Adds air and sparkle
- **Dark reverb** (high damping): Warm, vintage tone

### 3. Use as Send Effect

Set Mix to 100%, use on send/return for parallel reverb (better control).

### 4. EQ Before Reverb

Insert **SimpleEQ** before Reverb:
- HPF at 200 Hz (remove low-end mud in reverb)
- Slight boost at 5-8 kHz (airy reverb)

### 5. Automate Room Size

Automate Room Size from 30% → 90% during breakdown sections for dramatic builds.

---

## Technical Specifications

### Algorithm

- **Type**: Freeverb-inspired (Schroeder reverberator)
- **Comb Filters**: 8 parallel combs
- **Allpass Filters**: 4 in series
- **Stereo**: True stereo processing

### Performance

- **CPU**: 2-4% (moderate load)
- **Latency**: ~5-10ms (algorithmic processing)

---

## Troubleshooting

### Reverb sounds muddy

**Fix**:
1. Increase Damping to 60-80%
2. Use HPF before Reverb (200-300 Hz)
3. Reduce Room Size slightly

### Too metallic/ringy

**Cause**: Excessive resonance in algorithm

**Fix**:
1. Reduce Room Size
2. Increase Damping
3. Lower Mix percentage

### Reverb kills punch

**Cause**: Too much wet signal, or no pre-delay

**Fix**:
1. Reduce Mix to 15-25%
2. Add Pre-Delay (20-30ms)
3. Use as send effect instead of insert

---

**AudioForge Reverb v1.0.0**
© 2026 AudioForge

*Last updated: 2026-08-23*
