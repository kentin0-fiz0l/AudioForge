# DrumMachine 🥁

**AI-Powered Drum Pattern Generator VST3/AU Plugin**

DrumMachine generates rhythmic drum patterns using synthesized drum sounds and genre-specific algorithms.

---

## Features

### 🎵 7 Musical Styles
- **Rock** - 4-on-floor kick, backbeat snare, 8th/16th hihats
- **Hip-Hop** - Boom-bap patterns, syncopated kicks, ghost snares
- **Jazz** - Swing feel, ride patterns, feathered snares
- **EDM** - Four-on-floor, quantized, high-energy
- **Funk** - Syncopated grooves, heavy ghost notes, 16th hihats
- **Latin** - Clave patterns, tresillo rhythms, percussive
- **Blues** - Shuffle feel, triplet-based grooves

### 🥁 5 Drum Voices (Synthesized)
- **Kick** - Sine wave with pitch envelope (150Hz → 50Hz)
- **Snare** - Noise + tone (200Hz), crisp attack
- **Hihat** - Filtered high-frequency noise, very short
- **Tom** - Pitched tone with decay (120Hz → 100Hz)
- **Crash** - Filtered noise with long decay

### 🎛️ Pattern Controls
- **Style Selector** - 7 genres with distinct grooves
- **Complexity** - 1-10 (affects note density and variation)
- **Swing** - 0.0-1.0 (triplet feel vs straight)
- **Fills** - Toggle drum fills at end of pattern
- **16-Step Grid** - Visual pattern display

---

## Architecture

### Core Components

#### **DrumEngine** (Pure C++)
- `generatePattern()` - Genre-specific pattern generation
- `getHitsAtStep()` - Retrieve hits for sequencer
- Style-specific generators for each genre
- 16-step grid (4 bars of 16th notes)

#### **DrumSynth** (Audio Synthesis)
- Synthesized drum sounds (no samples)
- Kick: Pitch-swept sine wave
- Snare: Noise + tone combination
- Hihat: Filtered noise burst
- Tom: Pitched tone with envelope
- Crash: Long-decay filtered noise

#### **PluginProcessor** (JUCE Sequencer)
- Transport-synced pattern playback
- Step triggering based on DAW position
- Pattern looping (4 bars)

#### **PluginEditor** (JUCE UI)
- Pattern grid visualization (5 voices × 16 steps)
- Velocity-based hit brightness
- Color-coded voices
- Real-time pattern display

---

## Usage

### In Your DAW:
1. Load **DrumMachine** as an **Instrument** on a new track
2. Select a **style** (Rock, Hip-Hop, EDM, etc.)
3. Adjust **complexity** (1=simple, 10=busy)
4. Add **swing** for groove feel
5. Toggle **fills** for pattern endings
6. Click **"Generate Pattern"** or let it auto-generate!

### Pattern Grid:
- **Orange** = Kick
- **Teal** = Snare
- **Yellow** = Hihat
- **Purple** = Tom
- **Red** = Crash
- Brightness = Velocity

---

## Pattern Generation

### Style Characteristics

**Rock**:
- Kick: 4-on-floor (0, 4, 8, 12)
- Snare: Backbeat (4, 12)
- Hihat: 8th or 16th notes

**Hip-Hop**:
- Kick: Boom-bap (syncopated)
- Snare: Backbeat + ghost notes
- Hihat: Varied pattern with offbeats

**Jazz**:
- Kick: Sparse (1, 3)
- Snare: Light backbeat + feathering
- Hihat: Swing feel (triplet-based)

**EDM**:
- Kick: Every beat (quantized)
- Snare: 2 and 4 (backbeat)
- Hihat: Straight 16ths
- No humanization

**Funk**:
- Kick: Syncopated funk pattern
- Snare: Backbeat + heavy ghost notes
- Hihat: 16ths with accents

**Latin**:
- Kick: 3-2 clave pattern
- Snare: Clave accents
- Hihat: Tresillo pattern

**Blues**:
- Kick: Shuffle pattern
- Snare: Backbeat with shuffle
- Hihat: Shuffle feel (triplets)

---

## Synthesized Drums

### Kick Synthesis
```
Frequency: 150Hz → 50Hz (pitch envelope)
Envelope: 0.001s attack, 0.3s decay
Waveform: Sine wave
```

### Snare Synthesis
```
Tone: 200Hz sine (30%)
Noise: White noise (70%)
Envelope: 0.001s attack, 0.15s decay
```

### Hihat Synthesis
```
Source: Filtered white noise
Envelope: 0.0005s attack, 0.05s decay
```

### Tom Synthesis
```
Frequency: 120Hz → 100Hz (pitch envelope)
Envelope: 0.001s attack, 0.2s decay
Waveform: Sine wave
```

### Crash Synthesis
```
Source: Filtered white noise
Envelope: 0.001s attack, 0.8s decay (long tail)
```

---

## Building

```bash
cd ~/Projects/Active/AudioForge/plugins/DrumMachine
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The plugin will be automatically installed to:
- **macOS**: `~/Library/Audio/Plug-Ins/VST3/DrumMachine.vst3`
- **Windows**: `C:\Program Files\Common Files\VST3\`

---

## Integration with AudioForge Suite

**Complete song foundation:**
- **ChordGenius** - Harmonic progression
- **MelodyMaker** - Top-line melody
- **BassLine** - Bass foundation
- **DrumMachine** - Rhythmic foundation

Layer all four for instant song arrangements!

---

## Technical Details

**Format**: VST3, AU  
**Type**: Instrument (Synth)  
**Requirements**: C++17, JUCE 7.0+  
**Sample Rate**: Any  
**Latency**: None (real-time generation)  
**Pattern Length**: 16 steps (4 bars of 16th notes)  
**Voices**: 5 (Kick, Snare, Hihat, Tom, Crash)  
**Synthesis**: Procedural (no samples)

---

## Future Enhancements

- [ ] **Pattern Editor** - Click to edit hits in grid
- [ ] **Per-Voice Controls** - Volume, pan, tune for each drum
- [ ] **More Voices** - Toms (low/mid/high), open hihat, ride cymbal
- [ ] **Sample Loading** - Optional sample-based mode
- [ ] **MIDI Export** - Save patterns as MIDI files
- [ ] **Humanization Controls** - Timing/velocity variation amount
- [ ] **Pattern Variations** - Generate variations of current pattern
- [ ] **Effects Per Voice** - Reverb, compression, distortion

---

## License

Part of the AudioForge plugin suite.

---

**Built with ❤️ using JUCE**
