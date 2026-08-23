# SimpleEQ User Manual

**AudioForge SimpleEQ v1.0.0**

3-band parametric equalizer with high-pass and low-pass filters

---

## Overview

**SimpleEQ** provides surgical and musical equalization with:

- **3 Parametric Bands**: Low, Mid, High with gain, frequency, and Q control
- **High-Pass Filter**: Remove sub-bass rumble (12-500 Hz)
- **Low-Pass Filter**: Tame harsh highs (2k-20k Hz)
- **Minimal phase shift**: Linear-phase option for mastering
- **Low CPU**: Efficient IIR filters

---

## Quick Start

### Boost Vocal Presence (2-5 kHz)

1. **Mid Band**: Freq = 3000 Hz, Gain = +3 dB, Q = 1.5
2. Result: More clarity and presence

### Remove Mud (200-400 Hz)

1. **Low Band**: Freq = 300 Hz, Gain = -3 dB, Q = 2.0
2. Result: Cleaner, less boxy sound

### Add Air (8-12 kHz)

1. **High Band**: Freq = 10000 Hz, Gain = +2 dB, Q = 0.7
2. Result: Bright, airy top-end

---

## Parameters

### Low Band (Parametric)

- **Frequency**: 20 Hz - 500 Hz
- **Gain**: -15 dB to +15 dB
- **Q**: 0.5 (wide) to 5.0 (narrow)

**Use for**: Bass boost/cut, sub control, low-mid warmth

---

### Mid Band (Parametric)

- **Frequency**: 200 Hz - 8 kHz
- **Gain**: -15 dB to +15 dB
- **Q**: 0.5 to 5.0

**Use for**: Vocal presence, guitar body, snare attack

---

### High Band (Parametric)

- **Frequency**: 2 kHz - 20 kHz
- **Gain**: -15 dB to +15 dB
- **Q**: 0.5 to 5.0

**Use for**: Brightness, air, cymbal control

---

### High-Pass Filter (HPF)

- **Frequency**: 20 Hz - 500 Hz
- **Slope**: 12 dB/octave

**Use for**:
- Remove sub-bass rumble (vocals, guitars)
- Clean up low-end mud
- Typical setting: 80-120 Hz for vocals

---

### Low-Pass Filter (LPF)

- **Frequency**: 2 kHz - 20 kHz
- **Slope**: 12 dB/octave

**Use for**:
- Tame harsh highs
- Lo-fi effects
- De-essing (narrow, high Q)

---

## Use Cases

### 1. Vocal EQ

**Settings**:
- **HPF**: 100 Hz (remove rumble)
- **Low**: 200 Hz, -2 dB, Q=1.5 (reduce mud)
- **Mid**: 3 kHz, +3 dB, Q=2.0 (presence)
- **High**: 10 kHz, +2 dB, Q=0.7 (air)

---

### 2. Kick Drum Punch

**Settings**:
- **Low**: 60 Hz, +4 dB, Q=1.0 (sub-bass)
- **Mid**: 2.5 kHz, +3 dB, Q=2.5 (attack/click)
- **HPF**: 30 Hz (sub-sonic filter)

---

### 3. Guitar Warmth

**Settings**:
- **Low**: 200 Hz, +2 dB, Q=1.0 (body)
- **Mid**: 800 Hz, -1 dB, Q=2.0 (reduce boxiness)
- **High**: 5 kHz, +1 dB, Q=1.5 (sparkle)

---

## Tips & Techniques

### 1. Cut Before Boost

**Rule**: Cut problem frequencies first, boost to taste second.

**Why**: More transparent, avoids buildup

---

### 2. Use Q Wisely

- **Wide Q (0.5-1.0)**: Musical, gentle curves
- **Narrow Q (3.0-5.0)**: Surgical, notch problem frequencies

---

### 3. A/B Compare

Toggle bypass frequently to ensure EQ improves the sound.

---

**AudioForge SimpleEQ v1.0.0**
© 2026 AudioForge

*Last updated: 2026-08-23*
