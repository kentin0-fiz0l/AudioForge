# MelodyMaker 🎵

**AI-Powered Melody Generator VST3/AU Plugin**

MelodyMaker generates melodies that fit perfectly over chord progressions using music theory and AI algorithms.

---

## Features

### 🎼 8 Musical Styles
- **Pop** - Catchy, repetitive melodies with simple rhythms
- **Rock** - Energetic patterns with power and movement
- **Jazz** - Complex, chromatic lines with swing feel  
- **Blues** - Soulful phrases using blues scale
- **EDM** - Arpeggiated, high-energy patterns
- **Ballad** - Smooth, long notes for emotional impact
- **Funk** - Syncopated, rhythmically complex grooves
- **Classical** - Sophisticated contours and phrasing

### 🎹 Melody Generation
- **Smart Music Theory** - Stays within scale, targets chord tones
- **Customizable Parameters** - Note density, range, rhythm complexity
- **Real-time Visualization** - See melodies as piano roll
- **MIDI Output** - Send melodies to your DAW tracks
- **Humanization** - Subtle timing/velocity variations

---

## Architecture

### Core Components

#### **MelodyEngine** (Pure C++, JUCE-independent)
- `generateMelody()` - Main generation over chord progressions
- `generateMelodyInScale()` - Generate in a key/scale
- `humanize()` - Add natural variations
- Music theory utilities (scales, chord tones, etc.)

#### **PluginProcessor** (JUCE Audio Engine)
- Hosts MelodyEngine
- Manages MIDI I/O
- Handles parameter state

#### **PluginEditor** (JUCE UI)
- Style selector dropdown
- Generate button
- Piano roll visualization
- Stats display

---

## Usage

### In Your DAW:
1. Load **MelodyMaker** as a **MIDI effect** on a MIDI track
2. Select a musical **style** (Pop, Jazz, EDM, etc.)
3. Click **"Generate Melody"**
4. Watch the melody appear in the piano roll!

### Parameters:
- **Style** - Musical genre/character
- **Note Density** - How many notes (sparse to dense)
- **Pitch Range** - Lowest and highest notes
- **Rhythm Complexity** - Simple to complex patterns
- **Syncopation** - Add rhythmic syncopation

---

## Algorithm

### Melody Generation Strategy

1. **Scale Selection** - Choose scale based on key/chords
2. **Rhythm Pattern** - Generate timing from style templates
3. **Pitch Contour** - Create melodic shape (arc, ascending, etc.)
4. **Note Placement** - Place notes on rhythm points
5. **Theory Constraints** - Ensure notes fit scale/chords
6. **Humanization** - Add subtle variations

### Style-Specific Behaviors

**Pop**: 
- 4 notes per bar
- Stepwise motion (small intervals)
- Repetition for catchiness

**Jazz**:
- 6 notes per bar (triplet feel)
- More chromatic passing tones
- Complex rhythms

**EDM**:
- 8 notes per bar (16th notes)
- Arpeggiated patterns
- High register, constant velocity

**Ballad**:
- 2 notes per bar (long notes)
- Smooth, emotional contours
- Lower velocities

---

## Building

```bash
cd ~/Projects/Active/AudioForge/plugins/MelodyMaker
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The plugin will be automatically installed to:
- **macOS**: `~/Library/Audio/Plug-Ins/VST3/MelodyMaker.vst3`
- **Windows**: `C:\Program Files\Common Files\VST3\`

---

## Future Enhancements

- [ ] **Export MIDI** - Save melodies as MIDI files
- [ ] **Chord Awareness** - Input chord progression from ChordGenius
- [ ] **More Styles** - Trap, House, Country, Metal, etc.
- [ ] **Variation Generator** - Create variations of existing melodies
- [ ] **Motif Development** - Develop short musical ideas
- [ ] **Contour Editor** - Draw custom melodic shapes
- [ ] **Scale Library** - Exotic scales (Phrygian, Dorian, etc.)

---

## Technical Details

**Format**: VST3, AU  
**Type**: MIDI Effect  
**Requirements**: C++17, JUCE 7.0+  
**Sample Rate**: Any  
**Latency**: None (real-time generation)

---

## License

Part of the AudioForge plugin suite.

---

**Built with ❤️ using JUCE**
