# AudioForge Vocal Suite - Build Plan

**Date**: 2026-09-13  
**Goal**: Create 4 essential vocal processing plugins in parallel

---

## Plugins Overview

### 1. VocalDeEsser
**Purpose**: Reduce harsh sibilance in vocals

**DSP Features**:
- Multiband compression (focus on 4-10 kHz)
- Frequency-selective sidechain
- Threshold, ratio, attack, release
- Dry/wet mix
- Spectral visualization (optional)

**Parameters**:
- Frequency (4-10 kHz, default 7 kHz)
- Bandwidth (0.5-4 octaves, default 1.5)
- Threshold (-60 to 0 dB, default -20 dB)
- Ratio (1:1 to 10:1, default 4:1)
- Attack (0.1-10 ms, default 0.5 ms)
- Release (10-500 ms, default 100 ms)
- Mix (0-100%, default 100%)

**Presets**:
- Gentle (soft reduction)
- Medium (standard de-essing)
- Aggressive (heavy reduction)
- Podcast (optimized for spoken word)

---

### 2. VocalCompressor
**Purpose**: Transparent vocal compression

**DSP Features**:
- RMS/Peak detection
- Soft/hard knee
- Parallel compression option
- Auto makeup gain
- Vocal-optimized attack/release curves

**Parameters**:
- Threshold (-60 to 0 dB, default -18 dB)
- Ratio (1:1 to 20:1, default 4:1)
- Attack (0.1-100 ms, default 5 ms)
- Release (10-1000 ms, default 100 ms)
- Knee (0-12 dB, default 3 dB)
- Makeup Gain (-12 to +24 dB, auto)
- Parallel Mix (0-100%, default 0%)
- Detection (RMS/Peak)

**Presets**:
- Broadcast (heavy, consistent)
- Podcast (moderate, natural)
- Rap Vocals (fast attack, aggressive)
- Sung Vocals (slower, musical)
- Parallel (50% wet for punch)

---

### 3. VocalDoubler
**Purpose**: Create natural vocal doubling/widening

**DSP Features**:
- Dual delay lines (10-50 ms)
- Pitch variation (±10 cents)
- Timing humanization
- Stereo width control
- High-pass filter on doubled signal

**Parameters**:
- Delay Time (10-50 ms, default 25 ms)
- Pitch Variation (0-20 cents, default 8 cents)
- Timing Variation (0-10 ms, default 3 ms)
- Stereo Width (0-200%, default 100%)
- Mix (0-100%, default 50%)
- High Pass (off, 100-500 Hz, default 200 Hz)
- Voices (2 or 4, default 2)

**Presets**:
- Subtle Thickening
- Classic Doubling
- Wide Stereo
- Quad Doubling (4 voices)
- Tight (minimal variation)

---

### 4. VocalHarmonizer
**Purpose**: Generate 2-4 voice harmonies

**DSP Features**:
- Real-time pitch shifting (formant-preserved)
- Key/scale-aware harmony generation
- 2-4 independent voices
- Per-voice pan, level, detune
- Chord type selection

**Parameters**:
- Key (C-B, default C)
- Scale (Major, Minor, Dorian, etc.)
- Voice 1 Interval (-12 to +12 semitones, default +3)
- Voice 2 Interval (-12 to +12 semitones, default +7)
- Voice 3 Interval (-12 to +12 semitones, default +12)
- Voice 4 Interval (-12 to +12 semitones, default -12)
- Voice 1-4 Level (0-100%, default 70%)
- Voice 1-4 Pan (-100 to +100%, default ±50)
- Formant Preserve (on/off, default on)
- Mix (0-100%, default 50%)

**Presets**:
- Major Third Harmony
- Perfect Fifth
- Octave Up/Down
- Full Chord (4 voices)
- Gospel Harmony

---

## DSP Implementation Details

### DeEsser Algorithm
```cpp
// 1. Split signal into bands (crossover at target freq)
// 2. Analyze high band energy
// 3. Apply compression only when energy exceeds threshold
// 4. Smooth gain reduction for natural sound
// 5. Mix dry/wet
```

### Compressor Algorithm
```cpp
// 1. Detect signal level (RMS or Peak)
// 2. Calculate gain reduction based on threshold/ratio
// 3. Apply knee smoothing
// 4. Envelope follower for attack/release
// 5. Apply gain reduction
// 6. Parallel mix option
// 7. Auto makeup gain
```

### Doubler Algorithm
```cpp
// 1. Create 2-4 delay lines
// 2. Modulate delay time (humanization)
// 3. Modulate pitch (slight detuning)
// 4. Pan each voice
// 5. Apply high-pass filter
// 6. Mix with dry signal
```

### Harmonizer Algorithm
```cpp
// 1. Pitch detection (autocorrelation or YIN)
// 2. Determine target pitches based on key/scale
// 3. Pitch shift with formant preservation
// 4. Per-voice level and pan
// 5. Mix harmonies with dry signal
```

---

## Build Order

### Phase 1: Create Plugin Skeletons
1. Create directory structure for all 4 plugins
2. Create CMakeLists.txt for each
3. Create basic PluginProcessor.cpp/h
4. Create basic PluginEditor.cpp/h

### Phase 2: Implement DSP Engines
1. VocalDeEsser → DeEsserEngine.cpp/h
2. VocalCompressor → CompressorEngine.cpp/h
3. VocalDoubler → DoublerEngine.cpp/h
4. VocalHarmonizer → HarmonizerEngine.cpp/h

### Phase 3: Wire Everything Up
1. Connect processors to engines
2. Create parameter layouts
3. Build all 4 plugins
4. Test in DAW

### Phase 4: Create Presets
1. Create factory presets for each plugin
2. Test presets
3. Document

---

## Technical Specifications

### Sample Rate
- Support: 44.1 kHz, 48 kHz, 88.2 kHz, 96 kHz
- Internal processing: Match host SR

### Latency
- VocalDeEsser: ~5 ms (lookahead)
- VocalCompressor: ~5 ms (lookahead)
- VocalDoubler: 10-50 ms (by design)
- VocalHarmonizer: ~15 ms (pitch detection + processing)

### CPU Usage Target
- Each plugin: <5% CPU on modern system
- Optimize for real-time processing

### Quality
- 32-bit float internal processing
- Anti-aliasing on pitch shifting
- Smooth parameter changes (no clicks)

---

## Testing Checklist

### Per Plugin
- [ ] Builds successfully (VST3, AU, Standalone)
- [ ] All parameters respond correctly
- [ ] No audio glitches or clicks
- [ ] Presets load correctly
- [ ] MIDI Learn works
- [ ] Preset system works
- [ ] CPU usage is reasonable

### Integration
- [ ] All 4 plugins work together in DAW
- [ ] Can be inserted on same track
- [ ] Stable under stress test
- [ ] No conflicts

---

## File Structure

```
AudioForge/
├── plugins/
│   ├── VocalDeEsser/
│   │   ├── Source/
│   │   │   ├── PluginProcessor.cpp
│   │   │   ├── PluginProcessor.h
│   │   │   ├── PluginEditor.cpp
│   │   │   ├── PluginEditor.h
│   │   │   ├── DeEsserEngine.cpp
│   │   │   └── DeEsserEngine.h
│   │   ├── Presets/
│   │   │   ├── Gentle.afpreset
│   │   │   ├── Medium.afpreset
│   │   │   └── Aggressive.afpreset
│   │   └── CMakeLists.txt
│   ├── VocalCompressor/
│   ├── VocalDoubler/
│   └── VocalHarmonizer/
└── shared/
    └── dsp/
        ├── PitchShifter.cpp/h (for harmonizer)
        └── FormantPreserver.cpp/h (for harmonizer)
```

---

## Dependencies

### Existing AudioForge Components
- ✅ AudioForgeDSP (filters, envelopes)
- ✅ AudioForgePresets (preset management)
- ✅ AudioForgeUI (theme)

### New Components Needed
- PitchShifter (for VocalHarmonizer)
- FormantPreserver (for VocalHarmonizer)
- CrossoverFilter (for VocalDeEsser)

---

## Timeline Estimate

**Total**: ~4-6 hours for all 4 plugins

- Plugin skeleton creation: 30 min
- DSP engine implementation: 2-3 hours
- UI and parameters: 1 hour
- Preset creation: 30 min
- Testing and debugging: 1-2 hours

---

## Success Criteria

✅ All 4 plugins build successfully  
✅ All plugins process audio correctly  
✅ Presets work and sound good  
✅ No crashes or audio glitches  
✅ CPU usage is reasonable  
✅ Integrated with AudioForge ecosystem  

---

**Status**: Ready to build  
**Next**: Create plugin skeletons
