# BasicSynth User Manual

**AudioForge BasicSynth v1.0.0**

Subtractive synthesizer with 3 oscillators, ADSR envelope, and resonant filter

---

## Table of Contents

1. [Overview](#overview)
2. [Quick Start](#quick-start)
3. [User Interface](#user-interface)
4. [Parameters](#parameters)
5. [Sound Design Guide](#sound-design-guide)
6. [Presets & Techniques](#presets--techniques)
7. [MIDI Implementation](#midi-implementation)
8. [Technical Specifications](#technical-specifications)

---

## Overview

**BasicSynth** is a classic subtractive synthesizer featuring:

- **3 Oscillators**: Sine, sawtooth, and square waveforms
- **ADSR Envelope**: Attack, Decay, Sustain, Release
- **Low-pass Filter**: Resonant filter with cutoff and Q controls
- **Polyphony**: Up to 8 voices
- **MIDI**: Full note and velocity support
- **Simple & CPU-efficient**: Perfect for learning and production

### When to Use BasicSynth

- **Bass lines**: Deep, punchy bass with sawtooth + filter
- **Lead synth**: Bright leads with square wave + envelope
- **Pads**: Soft, evolving pads with sine wave + slow attack
- **Sound design learning**: Understand subtractive synthesis fundamentals
- **CPU-light production**: When you need synth sounds without heavy CPU

---

## Quick Start

### Play Your First Note

1. **Load BasicSynth** as an instrument in your DAW
2. **Connect a MIDI keyboard** or use your DAW's piano roll
3. **Play a note** (e.g., C3)
4. **Hear sound!** (default: sawtooth wave, bright and buzzy)

### Create a Bass Sound (30 seconds)

1. **Enable Sawtooth oscillator** (OSC 2 button)
2. **Disable other oscillators** (OSC 1, OSC 3)
3. **Lower filter cutoff** to 500-800 Hz
4. **Increase filter resonance** to 30-50%
5. **Shorten envelope release** to 0.1-0.2s
6. Result: **Punchy, filtered bass**

### Create a Pad Sound (30 seconds)

1. **Enable Sine oscillator** (OSC 1 button)
2. **Disable other oscillators**
3. **Increase attack** to 1.0-2.0s
4. **Increase release** to 2.0-3.0s
5. **Open filter cutoff** to 2000-5000 Hz
6. Result: **Soft, swelling pad**

---

## User Interface

```
┌────────────────────────────────────────────────┐
│  OSCILLATORS                                   │
│  ┌────┐  ┌────┐  ┌────┐                        │
│  │OSC1│  │OSC2│  │OSC3│  ← On/Off             │
│  │SINE│  │ SAW│  │ SQR│  ← Waveform           │
│  └────┘  └────┘  └────┘                        │
│   50%     50%     50%    ← Level (0-100%)      │
│                                                 │
│  FILTER                                         │
│    ╭───╮      ╭───╮                            │
│    │ ◯ │      │ ◯ │     ← Cutoff, Resonance   │
│    ╰───╯      ╰───╯                            │
│   Cutoff    Resonance                          │
│   1000 Hz      20%                             │
│                                                 │
│  ENVELOPE (ADSR)                               │
│    ╭───╮  ╭───╮  ╭───╮  ╭───╮                 │
│    │ ◯ │  │ ◯ │  │ ◯ │  │ ◯ │                 │
│    ╰───╯  ╰───╯  ╰───╯  ╰───╯                 │
│   Attack  Decay Sustain Release               │
│    0.01s   0.1s   70%    0.3s                 │
│                                                 │
│  BasicSynth · AudioForge v1.0.0               │
└────────────────────────────────────────────────┘
```

---

## Parameters

### Oscillators (OSC 1, 2, 3)

**OSC 1 - Sine Wave**:
- **Waveform**: Pure sine wave (fundamental only)
- **Character**: Smooth, soft, no harmonics
- **Use**: Sub-bass, pads, sine-bass

**OSC 2 - Sawtooth Wave**:
- **Waveform**: Sawtooth (all harmonics, decreasing amplitude)
- **Character**: Bright, buzzy, rich harmonics
- **Use**: Leads, bass, brass-like sounds

**OSC 3 - Square Wave**:
- **Waveform**: Square (odd harmonics only)
- **Character**: Hollow, clarinet-like, nasal
- **Use**: Leads, chiptune sounds, retro game music

**Level Control** (0-100% for each):
- Adjusts volume of each oscillator
- Mix multiple oscillators for layered sounds
- 50% default = balanced mix

---

### Filter

**Cutoff** (20 Hz - 20 kHz):
- **Low values** (200-800 Hz): Dark, muffled, bass-heavy
- **Mid values** (1-5 kHz): Warm, balanced
- **High values** (5-20 kHz): Bright, open, full harmonics

**Resonance** (0-100%):
- **0%**: No resonance (smooth rolloff)
- **20-40%**: Adds character, slight peak at cutoff
- **60-80%**: Strong resonance, whistling sound
- **90-100%**: Self-oscillation (filter resonates on its own)

**Filter Type**: 12 dB/octave low-pass (Moog-style)

---

### Envelope (ADSR)

**Attack** (0.001s - 5.0s):
- Time for sound to go from silence to full volume
- **Fast (0.01-0.1s)**: Plucky, percussive sounds (bass, leads)
- **Slow (1.0-5.0s)**: Swelling pads, ambient sounds

**Decay** (0.001s - 5.0s):
- Time to fall from attack peak to sustain level
- **Fast (0.01-0.1s)**: Snappy, defined (piano-like)
- **Slow (0.5-2.0s)**: Smooth transition (strings)

**Sustain** (0-100%):
- Level held while key is pressed
- **0%**: Sound decays to silence (percussive)
- **100%**: Held at full volume (organ-like)
- **50-80%**: Natural decay (most instruments)

**Release** (0.001s - 5.0s):
- Time to fade to silence after key release
- **Fast (0.01-0.2s)**: Abrupt stop (staccato)
- **Slow (1.0-5.0s)**: Long tail (reverb-like)

---

## Sound Design Guide

### Bass Sounds

**Sub Bass** (deep, low-end power):
```
OSC 1 (Sine): 100%
OSC 2 (Saw): 0%
OSC 3 (Square): 0%
Filter Cutoff: 200-400 Hz
Filter Res: 10-20%
Attack: 0.01s, Decay: 0.2s, Sustain: 50%, Release: 0.2s
```

**Reese Bass** (modulated, wobbling):
```
OSC 1 (Sine): 40%
OSC 2 (Saw): 60%
OSC 3 (Square): 0%
Filter Cutoff: 500-1000 Hz (automate for wobble)
Filter Res: 40-60%
Attack: 0.01s, Decay: 0.3s, Sustain: 70%, Release: 0.3s
```

**Pluck Bass** (short, percussive):
```
OSC 2 (Saw): 80%
OSC 3 (Square): 20%
Filter Cutoff: 1500 Hz
Filter Res: 30%
Attack: 0.005s, Decay: 0.15s, Sustain: 0%, Release: 0.1s
```

---

### Lead Sounds

**Bright Lead** (cutting through the mix):
```
OSC 2 (Saw): 60%
OSC 3 (Square): 40%
Filter Cutoff: 4000-8000 Hz
Filter Res: 40%
Attack: 0.01s, Decay: 0.2s, Sustain: 80%, Release: 0.5s
```

**Detuned Lead** (two instances, detune by +5 cents):
```
Instance 1: OSC 2 (Saw) 100%
Instance 2: OSC 2 (Saw) 100%, pitch +5 cents
Both: Filter Cutoff 5000 Hz, Res 30%
```

---

### Pad Sounds

**Soft Pad**:
```
OSC 1 (Sine): 100%
Filter Cutoff: 3000 Hz
Filter Res: 10%
Attack: 1.5s, Decay: 1.0s, Sustain: 60%, Release: 2.5s
```

**Lush Pad** (layered):
```
OSC 1 (Sine): 50%
OSC 2 (Saw): 30%
OSC 3 (Square): 20%
Filter Cutoff: 2500 Hz
Filter Res: 20%
Attack: 2.0s, Decay: 1.5s, Sustain: 70%, Release: 3.0s
```

---

### Other Sounds

**Pluck/Mallet**:
```
OSC 2 (Saw): 70%
OSC 3 (Square): 30%
Filter Cutoff: 3000 Hz
Filter Res: 50%
Attack: 0.005s, Decay: 0.3s, Sustain: 0%, Release: 0.2s
```

**Organ**:
```
OSC 3 (Square): 100%
Filter Cutoff: 8000 Hz (wide open)
Filter Res: 0%
Attack: 0.01s, Decay: 0.05s, Sustain: 100%, Release: 0.05s
```

---

## Presets & Techniques

### Technique 1: Layer Multiple Instances

**Goal**: Create rich, complex sounds

**How**:
1. Insert 2-3 instances of BasicSynth on the same track
2. **Instance 1**: Sub bass (sine, low cutoff)
3. **Instance 2**: Mid harmonics (saw, mid cutoff)
4. **Instance 3**: High air (square, high cutoff)

**Result**: Full-spectrum bass or lead

---

### Technique 2: Filter Automation

**Goal**: Add movement and expression

**How**:
1. Create a MIDI clip with held notes
2. **Automate filter cutoff** from low (200 Hz) to high (5000 Hz)
3. **Automate filter resonance** from 20% to 60%

**Result**: Classic filter sweep (dubstep wobble, disco filter)

---

### Technique 3: Velocity → Filter Cutoff

**Goal**: Harder notes = brighter sound (expressive)

**How**:
1. In your DAW, map MIDI velocity to filter cutoff
2. Soft notes (velocity 40-60) = dark, mellow
3. Hard notes (velocity 100-127) = bright, aggressive

**Note**: BasicSynth v1.0 doesn't have built-in velocity routing - use DAW's MIDI CC mapping.

---

### Technique 4: Combine with Effects

**BasicSynth alone is simple** - add effects for pro sounds:

**Bass**:
- BasicSynth (sub bass) → **Saturation** (harmonics) → **Reverb** (space)

**Lead**:
- BasicSynth (bright lead) → **Chorus** (width) → **CleanDelay** (depth)

**Pad**:
- BasicSynth (soft pad) → **Reverb** (long tail) → **SimpleComp** (glue)

---

## MIDI Implementation

### Note Range

- **C0 to C8** (full MIDI range)
- **Polyphony**: 8 voices (notes beyond 8 will steal oldest voice)

### Velocity Sensitivity

- **Velocity → Volume**: Harder notes = louder (0-127 MIDI velocity)
- **Velocity → Filter**: Not implemented in v1.0 (use DAW MIDI CC)

### MIDI CC Support

BasicSynth responds to these MIDI CC messages:

- **CC 1 (Mod Wheel)**: Not assigned (future: vibrato)
- **CC 7 (Volume)**: Overall synth volume
- **CC 10 (Pan)**: Stereo pan
- **CC 11 (Expression)**: Volume (similar to CC 7)

**Tip**: Map DAW MIDI CC to parameters for real-time control.

---

## Technical Specifications

### Synthesis Engine

- **Type**: Subtractive (oscillator → filter → envelope)
- **Polyphony**: 8 voices
- **Voice stealing**: Yes (oldest note)
- **Oscillators**: 3 (sine, saw, square)
- **Filter**: 12 dB/oct low-pass, resonant
- **Envelope**: ADSR (applies to both amplitude and filter)

### Performance

- **CPU Usage**: ~1-2% per voice (8 voices = ~8-16% total)
- **Latency**: 0 samples (real-time processing)

### Audio Quality

- **Bit Depth**: 32-bit float
- **Sample Rate**: 44.1 - 192 kHz
- **Anti-aliasing**: Yes (on sawtooth and square)

---

## Troubleshooting

### No sound when playing notes

**Fix**:
1. Check at least one oscillator is enabled
2. Verify MIDI is reaching the plugin (check DAW MIDI monitor)
3. Ensure sustain level >0% (or notes will decay to silence)

### Sound cuts off abruptly

**Cause**: Release time too short

**Fix**: Increase Release to 0.3s or higher

### Filter self-oscillates (whistles)

**Cause**: Resonance too high (>90%)

**Fix**: Reduce resonance to 60-80% for normal use

---

**AudioForge BasicSynth v1.0.0**
© 2026 AudioForge
https://audioforge.fluxstudio.art

*Last updated: 2026-08-23*
