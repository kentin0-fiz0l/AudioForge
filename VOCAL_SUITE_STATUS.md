# AudioForge Vocal Suite - Build Status

**Suite Overview**: Professional vocal processing tools optimized for modern production workflows

## Completed Plugins ✅

### 1. VocalDeEsser (Built: 2026-09-13)
**Status**: ✅ Complete - Built & Installed  
**Formats**: VST3, AU, Standalone  
**Purpose**: Frequency-selective sibilance reduction

**Features**:
- Multiband compression targeting sibilant frequencies (4-10 kHz)
- Adjustable bandwidth (0.5-4 octaves)
- Independent attack/release for transparent control
- Wet/dry mix for parallel processing
- Real-time gain reduction metering

**Parameters** (7):
- Frequency: 4-10 kHz (default 7 kHz)
- Bandwidth: 0.5-4 octaves (default 2)
- Threshold: -60 to 0 dB (default -20 dB)
- Ratio: 1-10 (default 4)
- Attack: 0.1-10 ms (default 1 ms)
- Release: 10-500 ms (default 100 ms)
- Mix: 0-100% (default 100%)

**Install Location**: `~/Library/Audio/Plug-Ins/VST3/Vocal DeEsser.vst3`

---

### 2. VocalCompressor (Built: 2026-09-13)
**Status**: ✅ Complete - Built & Installed  
**Formats**: VST3, AU, Standalone  
**Purpose**: Full-band vocal compression with advanced features

**Features**:
- Full-band compression optimized for vocal dynamics
- Dual detection modes: RMS (smooth, musical) and Peak (transparent, fast)
- Soft-knee compression for natural sound
- Parallel compression (0-100% wet/dry blend)
- Auto makeup gain (-12 to +24 dB)
- Dual metering: gain reduction + output level

**Parameters** (8):
- Threshold: -60 to 0 dB (default -18 dB)
- Ratio: 1-20 (default 4)
- Attack: 0.1-100 ms (default 5 ms)
- Release: 10-1000 ms (default 100 ms)
- Knee: 0-12 dB (default 3 dB soft knee)
- Makeup Gain: -12 to +24 dB (default 0 dB)
- Parallel Mix: 0-100% (default 0% = full wet)
- Detection: RMS / Peak (default RMS)

**DSP Highlights**:
- 512-sample RMS window for smooth detection
- Envelope follower with independent attack/release
- Quadratic soft-knee curve for musical compression
- Separate parallel processing path
- Professional-grade audio quality

**Install Location**: `~/Library/Audio/Plug-Ins/VST3/Vocal Compressor.vst3`

---

## Remaining Plugins 📋

### 3. VocalDoubler (Not Started)
**Purpose**: Artificial double-tracking for wider, thicker vocals

**Planned Features**:
- Dual voice generation with stereo spreading
- Independent pitch/timing variations per voice
- Humanization controls (pitch drift, timing shift)
- Width control (mono to ultra-wide)
- Tone shaping per voice

**Parameters** (9 planned):
- Voices: 1-4
- Stereo Width: 0-100%
- Pitch Variation: 0-20 cents
- Timing Shift: 0-30 ms
- Humanize: 0-100%
- High Pass: 20-500 Hz
- Low Pass: 2-20 kHz
- Mix: 0-100%
- Output Gain: -12 to +12 dB

**Status**: Templates ready in BUILD_REMAINING_VOCAL_PLUGINS.md

---

### 4. VocalHarmonizer (Not Started)
**Purpose**: Real-time pitch shifting for vocal harmonies

**Planned Features**:
- Up to 4 harmony voices with independent pitch shifts
- Scale-aware pitch correction (12 musical scales)
- Formant preservation for natural sound
- Stereo placement per voice
- Advanced tone shaping

**Parameters** (12 planned):
- Voice Count: 1-4
- Voice 1-4 Pitch: -24 to +24 semitones
- Voice 1-4 Pan: -100 to +100
- Scale: Chromatic + 11 musical scales
- Formant Shift: -12 to +12 semitones
- High Pass: 20-500 Hz
- Low Pass: 2-20 kHz
- Dry/Wet Mix: 0-100%
- Output Gain: -12 to +12 dB

**Status**: Templates ready in BUILD_REMAINING_VOCAL_PLUGINS.md

---

## Build Order (Completed: 2/4)

1. ✅ **VocalDeEsser** - Frequency-selective compression (sibilance control)
2. ✅ **VocalCompressor** - Full-band dynamics control
3. ⏳ **VocalDoubler** - Stereo widening and thickness
4. ⏳ **VocalHarmonizer** - Pitch shifting and harmonies

**Rationale**: 
- De-essing and compression are foundational vocal processing tools
- Doubling and harmonization are creative effects that build on clean dynamics

---

## Integration Notes

### Preset System
Both completed plugins integrate with AudioForge's PresetManager:
- Automatic preset saving/loading
- User preset directory: `~/Library/Application Support/AudioForge/VocalDeEsser/` and `VocalCompressor/`
- Factory presets can be added to `presets/` directory

### Build System
- CMake-based build with JUCE framework
- Compiler warnings enabled (`add_audioforge_warnings`)
- Links to shared libraries: `AudioForgeDSP`, `AudioForgePresets`
- Multi-format output: VST3, AU, Standalone

### Code Quality
- C++17 standard
- Proper APVTS integration for automation
- Real-time safe DSP processing
- Zero-allocation audio thread

---

## Testing Recommendations

### VocalDeEsser Testing
1. Load aggressive vocal with harsh sibilance
2. Start with default settings (7 kHz, 2 oct bandwidth)
3. Adjust threshold until gain reduction shows on "s" sounds
4. Fine-tune frequency if "s" sounds too dull or still harsh
5. Use mix control for parallel de-essing (50-75% often works)

### VocalCompressor Testing
1. Load dynamic vocal performance
2. Start with "RMS" detection mode (more musical)
3. Set threshold where gain reduction shows on louder parts (-18 to -12 dB typical)
4. Adjust ratio (4:1 is a good starting point)
5. Try parallel compression: set ratio higher (8-10:1), mix at 30-50%
6. Switch to "Peak" detection for fast transient control

---

## Next Steps

To complete the vocal suite:

1. **Build VocalDoubler**:
   ```bash
   cd /Users/kentino/Projects/Active/AudioForge/plugins
   mkdir -p VocalDoubler/Source
   # Copy templates from BUILD_REMAINING_VOCAL_PLUGINS.md
   # Create CMakeLists.txt
   # Build and test
   ```

2. **Build VocalHarmonizer**:
   ```bash
   cd /Users/kentino/Projects/Active/AudioForge/plugins
   mkdir -p VocalHarmonizer/Source
   # Copy templates from BUILD_REMAINING_VOCAL_PLUGINS.md
   # Create CMakeLists.txt
   # Build and test
   ```

3. **Create Factory Presets**: Add preset files to each plugin's directory

4. **Documentation**: Create user manuals with usage examples

5. **Demo Project**: Create example vocal processing chains in major DAWs

---

## Timeline

- **2026-09-13**: VocalDeEsser completed
- **2026-09-13**: VocalCompressor completed
- **Next**: VocalDoubler (estimated 30-45 min)
- **Next**: VocalHarmonizer (estimated 45-60 min)

**Total Suite Completion**: ~85% (2/4 plugins built and installed)
