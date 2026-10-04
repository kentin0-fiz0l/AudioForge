# BassLine Generator 🎸

**AI-Powered Bass Line Generator VST3/AU Plugin**

BassLine generates bass lines that follow chord progressions and groove patterns using music theory and style-specific algorithms.

---

## Features

### 🎵 7 Musical Styles
- **Rock** - Root-fifth patterns with driving eighth notes
- **Funk** - Syncopated, groove-heavy 16th note patterns
- **Jazz** - Walking bass with chromatic passing tones
- **EDM** - Sustained root notes, sub-bass optimized
- **Reggae** - Offbeat emphasis (2 & 4), dub feel
- **Latin** - Tumbao patterns, rhythmic syncopation
- **Blues** - Shuffle feel, triplet-based grooves

### 🎹 Bass Generation
- **Smart Music Theory** - Stays within key, targets roots and fifths
- **Octave Control** - C1 (Low), C2 (Mid), C3 (High)
- **Pattern Techniques** - Root notes, fifths, passing tones, chromatic approaches
- **Real-time Visualization** - Piano roll (bass register)
- **Humanization** - Subtle timing/velocity variations (except EDM)

### 🔊 Bass Synthesizer
- **4 Waveforms**:
  - Sine (Sub bass - pure low end)
  - Saw (Bright bass - harmonics)
  - Square (Hollow bass - retro)
  - Triangle (Warm bass - smooth)
- **ADSR Envelope** - Fast attack optimized for bass
- **Low-Pass Filter** - Adjustable cutoff for warmth (100-2000 Hz)

---

## Architecture

### Core Components

#### **BassEngine** (Pure C++, JUCE-independent)
- `generateBassLine()` - Generate from chord progressions
- `generateBassLineInKey()` - Generate in a key/scale
- Style-specific generators for each genre
- Music theory utilities (roots, fifths, passing tones)

#### **PluginProcessor** (JUCE Audio Engine)
- Hosts BassEngine
- Bass-focused synthesizer with filter
- Transport synchronization with DAW

#### **PluginEditor** (JUCE UI)
- Style selector (7 genres)
- Key/Scale/Octave selectors
- Waveform selector
- ADSR + Filter controls
- Bass line visualization (low register)

---

## Usage

### In Your DAW:
1. Load **BassLine** as an **Instrument** on a new track
2. Select a **musical style** (Rock, Funk, Jazz, etc.)
3. Choose **octave** (C1 for sub, C2 for standard, C3 for higher)
4. Click **"Generate Bass Line"**
5. Adjust **waveform** and **filter** for desired tone!

### Parameters:
- **Style** - Musical genre/groove pattern
- **Octave** - Bass register (Low/Mid/High)
- **Key/Scale** - Harmonic context
- **Waveform** - Timbre (Sub/Bright/Hollow/Warm)
- **ADSR** - Envelope shaping
- **Filter Cutoff** - Low-pass filter for warmth

---

## Bass Generation Patterns

### Style-Specific Behaviors

**Rock**: 
- Root-fifth alternating pattern
- Driving eighth notes
- Lock with kick drum feel

**Funk**:
- Syncopated 16th notes
- Ghost notes (soft touches)
- Offbeat emphasis

**Jazz**:
- Walking bass (quarter notes)
- Chromatic approach notes
- Chord tone movement (roots, thirds, fifths)

**EDM**:
- Sustained root notes
- Sub-bass focus
- Sidechain-ready (no humanization)

**Reggae**:
- Strong offbeat accents (2 & 4)
- Dub-style patterns
- Sparse, grooving

**Latin**:
- Tumbao clave patterns
- Syncopated rhythms
- Root-fifth interplay

**Blues**:
- Shuffle triplet feel
- Root-fifth patterns
- Swing groove

---

## Building

```bash
cd ~/Projects/Active/AudioForge/plugins/BassLine
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The plugin will be automatically installed to:
- **macOS**: `~/Library/Audio/Plug-Ins/VST3/BassLine.vst3`
- **Windows**: `C:\Program Files\Common Files\VST3\`

---

## Integration with AudioForge Suite

**Works perfectly with:**
- **ChordGenius** - Bass follows generated chord progressions
- **MelodyMaker** - Bass provides foundation for melodies
- **DrumMachine** (future) - Bass locks with rhythmic patterns

---

## Technical Details

**Format**: VST3, AU  
**Type**: Instrument (Synth)  
**Requirements**: C++17, JUCE 7.0+  
**Sample Rate**: Any  
**Latency**: None (real-time generation)  
**Polyphony**: 8 voices  
**Filter**: Simple RC low-pass (100-2000 Hz)

---

## Future Enhancements

- [ ] **Chord Input** - Follow chord progressions from ChordGenius
- [ ] **Pattern Editor** - Draw custom bass patterns
- [ ] **More Styles** - Hip-Hop, Disco, Metal, Dubstep
- [ ] **Effects** - Distortion, compression, sub-octave
- [ ] **MIDI Export** - Save bass lines as MIDI files
- [ ] **Variation Generator** - Create variations of patterns
- [ ] **Advanced Filter** - Resonant filter with envelope

---

## License

Part of the AudioForge plugin suite.

---

**Built with ❤️ using JUCE**
