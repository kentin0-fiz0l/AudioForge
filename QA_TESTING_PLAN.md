# AudioForge QA Testing Plan
**Date:** 2026-09-10  
**Total Plugins:** 37  
**Test Status:** Ready for QA

## Testing Methodology

### Test Levels
1. **Smoke Test** - Plugin loads without crashing
2. **Functional Test** - Core features work as expected
3. **MIDI Learn Test** - MIDI CC learning works correctly
4. **Edge Case Test** - Parameter limits, extreme values
5. **Integration Test** - Works in real-world DAW scenarios

### Test Environment
- **OS:** macOS (Darwin 24.6.0)
- **DAW Options:** Logic Pro, Ableton Live, Reaper, etc.
- **MIDI Controller:** (Specify your controller)
- **Audio Interface:** (Specify your interface)

---

## Plugin Testing Checklist

### 🎹 MIDI GENERATORS (7 plugins)
Plugins that create MIDI/audio from user input

#### ✓ BasicSynth
- [ ] Smoke: Loads in DAW
- [ ] Functional: Plays notes, oscillator types work
- [ ] MIDI Learn: Can learn MIDI CC for filter cutoff
- [ ] Edge: Test extreme frequency ranges (20Hz - 20kHz)
- [ ] Integration: Works in a track with MIDI input

#### ✓ Classic Monosynth
- [ ] Smoke: Loads in DAW
- [ ] Functional: Monophonic mode, portamento works
- [ ] MIDI Learn: Can learn CC for portamento time
- [ ] Edge: Test glide at extreme speeds
- [ ] Integration: Responds to pitch bend correctly

#### ✓ Polysynth
- [ ] Smoke: Loads in DAW
- [ ] Functional: Polyphonic playback (8+ voices)
- [ ] MIDI Learn: Learn CC for voice count
- [ ] Edge: Test with 32+ simultaneous notes
- [ ] Integration: CPU usage acceptable

#### ✓ DrumSynth
- [ ] Smoke: Loads in DAW
- [ ] Functional: Kick, snare, hi-hat sounds trigger
- [ ] MIDI Learn: Learn CC for decay times
- [ ] Edge: Test rapid triggering (hi-hat rolls)
- [ ] Integration: Works with drum MIDI patterns

#### ✓ WavetableSynth
- [ ] Smoke: Loads in DAW
- [ ] Functional: Wavetable scanning, morphing works
- [ ] MIDI Learn: Learn CC for wavetable position
- [ ] Edge: Test smooth scanning across full range
- [ ] Integration: CPU usage with complex wavetables

#### ✓ PadSynth
- [ ] Smoke: Loads in DAW
- [ ] Functional: Smooth pad sounds, filtering works
- [ ] MIDI Learn: Learn CC for pad harmonics
- [ ] Edge: Test with full chord (6+ notes)
- [ ] Integration: Suitable for ambient/pad sounds

#### ✓ GranularEngine
- [ ] Smoke: Loads in DAW
- [ ] Functional: Granular synthesis, grain size control
- [ ] MIDI Learn: Learn CC for grain density
- [ ] Edge: Test extreme grain sizes (1ms - 1000ms)
- [ ] Integration: Works with audio input

---

### 🎻 ACOUSTIC EMULATORS (7 plugins)
Plugins emulating real instruments

#### ✓ ElectricPiano
- [ ] Smoke: Loads in DAW
- [ ] Functional: Rhodes-style sound, velocity response
- [ ] MIDI Learn: Learn CC for tone control
- [ ] Edge: Test velocity range (1-127)
- [ ] Integration: Sounds realistic in mix

#### ✓ OrganEmulator
- [ ] Smoke: Loads in DAW
- [ ] Functional: Drawbar control, percussion works
- [ ] MIDI Learn: Learn CC for drawbar levels
- [ ] Edge: Test all drawbar combinations
- [ ] Integration: Leslie/rotary effect works

#### ✓ StringEnsemble
- [ ] Smoke: Loads in DAW
- [ ] Functional: String ensemble sound, vibrato
- [ ] MIDI Learn: Learn CC for vibrato depth
- [ ] Edge: Test with full orchestral chords
- [ ] Integration: Blends well with other instruments

#### ✓ BrassSection
- [ ] Smoke: Loads in DAW
- [ ] Functional: Brass ensemble, articulation
- [ ] MIDI Learn: Learn CC for brightness
- [ ] Edge: Test staccato vs legato
- [ ] Integration: Velocity affects dynamics

#### ✓ Koto
- [ ] Smoke: Loads in DAW
- [ ] Functional: Koto pluck sound, decay
- [ ] MIDI Learn: Learn CC for string tension
- [ ] Edge: Test rapid arpeggios
- [ ] Integration: Authentic Japanese sound

#### ✓ Shakuhachi
- [ ] Smoke: Loads in DAW
- [ ] Functional: Flute breath sound, vibrato
- [ ] MIDI Learn: Learn CC for breath pressure
- [ ] Edge: Test smooth pitch bends
- [ ] Integration: Expressive performance

#### ✓ Sitar
- [ ] Smoke: Loads in DAW
- [ ] Functional: Sitar pluck, sympathetic strings
- [ ] MIDI Learn: Learn CC for drone level
- [ ] Edge: Test rapid picking patterns
- [ ] Integration: Microtonal bending works

#### ✓ AcousticBass
- [ ] Smoke: Loads in DAW
- [ ] Functional: Upright bass sound, fingered/picked
- [ ] MIDI Learn: Learn CC for tone
- [ ] Edge: Test low frequencies (41Hz - E1)
- [ ] Integration: Sits well in bass mix

---

### 🔊 AUDIO EFFECTS (13 plugins)
Plugins that process audio

#### ✓ SimpleGain
- [ ] Smoke: Loads in DAW
- [ ] Functional: Volume control works
- [ ] MIDI Learn: Learn CC for gain level
- [ ] Edge: Test at +12dB and -60dB
- [ ] Integration: No distortion at unity gain

#### ✓ SimpleEQ
- [ ] Smoke: Loads in DAW
- [ ] Functional: 3-band EQ works correctly
- [ ] MIDI Learn: Learn CC for frequency/gain
- [ ] Edge: Test extreme boost/cut (±12dB)
- [ ] Integration: Sounds musical on instruments

#### ✓ SimpleComp
- [ ] Smoke: Loads in DAW
- [ ] Functional: Compression, threshold, ratio work
- [ ] MIDI Learn: Learn CC for threshold
- [ ] Edge: Test with extreme ratios (20:1)
- [ ] Integration: No pumping artifacts

#### ✓ Multiband Compressor
- [ ] Smoke: Loads in DAW
- [ ] Functional: Independent band compression
- [ ] MIDI Learn: Learn CC for crossover frequencies
- [ ] Edge: Test extreme band splits
- [ ] Integration: Works on full mix

#### ✓ Limiter
- [ ] Smoke: Loads in DAW
- [ ] Functional: Prevents clipping, ceiling control
- [ ] MIDI Learn: Learn CC for ceiling level
- [ ] Edge: Test with heavily dynamic material
- [ ] Integration: No distortion at 0dB ceiling

#### ✓ Gate
- [ ] Smoke: Loads in DAW
- [ ] Functional: Noise gate, threshold control
- [ ] MIDI Learn: Learn CC for threshold
- [ ] Edge: Test fast attack/release times
- [ ] Integration: Cleanly gates drums/vocals

#### ✓ Reverb
- [ ] Smoke: Loads in DAW
- [ ] Functional: Room, hall, plate algorithms
- [ ] MIDI Learn: Learn CC for room size
- [ ] Edge: Test 100% wet signal
- [ ] Integration: Natural-sounding reverb

#### ✓ CleanDelay
- [ ] Smoke: Loads in DAW
- [ ] Functional: Delay time, feedback, mix
- [ ] MIDI Learn: Learn CC for delay time
- [ ] Edge: Test feedback at 99%
- [ ] Integration: Tempo-synced delays work

#### ✓ ChorusFlanger
- [ ] Smoke: Loads in DAW
- [ ] Functional: Chorus and flanger modes
- [ ] MIDI Learn: Learn CC for rate/depth
- [ ] Edge: Test extreme modulation rates
- [ ] Integration: Stereo width enhancement

#### ✓ Saturation
- [ ] Smoke: Loads in DAW
- [ ] Functional: Warmth/distortion, drive control
- [ ] MIDI Learn: Learn CC for drive amount
- [ ] Edge: Test clean vs heavy saturation
- [ ] Integration: Adds harmonic richness

#### ✓ WaveShaper
- [ ] Smoke: Loads in DAW
- [ ] Functional: Waveshaping curves, distortion
- [ ] MIDI Learn: Learn CC for curve shape
- [ ] Edge: Test extreme asymmetry
- [ ] Integration: Creative distortion effects

#### ✓ FreezeFX
- [ ] Smoke: Loads in DAW
- [ ] Functional: Freeze buffer, spectral hold
- [ ] MIDI Learn: Learn CC for freeze trigger
- [ ] Edge: Test long freeze durations
- [ ] Integration: Creative ambient textures

#### ✓ SpectralFreeze
- [ ] Smoke: Loads in DAW
- [ ] Functional: FFT freeze, spectral smearing
- [ ] MIDI Learn: Learn CC for freeze amount
- [ ] Edge: Test with complex material
- [ ] Integration: Unique spectral effects

#### ✓ PanUtil
- [ ] Smoke: Loads in DAW
- [ ] Functional: Auto-pan, LFO control
- [ ] MIDI Learn: Learn CC for pan rate
- [ ] Edge: Test extreme pan speeds
- [ ] Integration: Stereo movement effects

---

### 🎵 MIDI EFFECTS (4 plugins)
Plugins that process MIDI data

#### ✓ MIDI Arpeggiator
- [ ] Smoke: Loads in DAW
- [ ] Functional: Up, down, up-down patterns work
- [ ] MIDI Learn: Learn CC for pattern type
- [ ] Edge: Test with 32nd note triplets
- [ ] Integration: Syncs to DAW tempo correctly
- [ ] Special: Test all 6 patterns, 7 rates, 4 octave ranges

#### ✓ MIDI Chord Generator
- [ ] Smoke: Loads in DAW
- [ ] Functional: Major, minor, 7th chords work
- [ ] MIDI Learn: Learn CC for chord type
- [ ] Edge: Test all 18 chord types
- [ ] Integration: Voicings sound musical
- [ ] Special: Test drop2/drop3 jazz voicings

#### ✓ MIDI Harmonizer
- [ ] Smoke: Loads in DAW
- [ ] Functional: Scale-aware harmony works
- [ ] MIDI Learn: Learn CC for harmony interval
- [ ] Edge: Test all 7 scales
- [ ] Integration: Stays in key correctly
- [ ] Special: Test with chromatic vs diatonic scales

#### ✓ MIDI Groove Quantizer
- [ ] Smoke: Loads in DAW
- [ ] Functional: Quantizes timing, swing works
- [ ] MIDI Learn: Learn CC for quantize strength
- [ ] Edge: Test 100% vs 0% quantization
- [ ] Integration: Preserves groove feel
- [ ] Special: Test swing at 50%, 60%, 75%

---

### 🎛️ MIDI GENERATORS (3 plugins)
Plugins that generate MIDI CC

#### ✓ XY Pad Controller
- [ ] Smoke: Loads in DAW
- [ ] Functional: X/Y position generates CC
- [ ] MIDI Learn: Learn which CCs to output
- [ ] Edge: Test all 4 corners of pad
- [ ] Integration: Controls other plugins
- [ ] Special: Test multi-target mode (4 CCs per axis)

#### ✓ Macro Controller
- [ ] Smoke: Loads in DAW
- [ ] Functional: One knob controls multiple CCs
- [ ] MIDI Learn: Learn CC for macro knob
- [ ] Edge: Test with 8 targets active
- [ ] Integration: Complex sound morphing
- [ ] Special: Test power curves (0.1x - 10x)

#### ✓ MIDI CC Mapper
- [ ] Smoke: Loads in DAW
- [ ] Functional: Remaps CC numbers correctly
- [ ] MIDI Learn: Input CCs trigger learning
- [ ] Edge: Test with 8 mappings active
- [ ] Integration: No MIDI latency
- [ ] Special: Test curve shaping, inversion

---

### 🎹 SAMPLER (1 plugin)
Sample playback engine

#### ✓ Sampler
- [ ] Smoke: Loads in DAW
- [ ] Functional: Loads WAV/AIFF samples
- [ ] MIDI Learn: Learn CC for sample start
- [ ] Edge: Test with large samples (>10MB)
- [ ] Integration: Pitch shifting sounds clean
- [ ] Special: Test looping, one-shot modes

---

## MIDI Learn Universal Tests

Test MIDI Learn on a representative sample (5-10 plugins minimum):

1. **Learning Flow**
   - [ ] Click "Learn" button activates learning mode
   - [ ] Send MIDI CC from controller
   - [ ] Parameter correctly binds to CC number
   - [ ] Visual feedback shows binding active

2. **Parameter Control**
   - [ ] Full CC range (0-127) maps to parameter range
   - [ ] Smooth parameter changes (no stepping)
   - [ ] Multiple parameters can learn different CCs
   - [ ] No CC conflicts between parameters

3. **State Persistence**
   - [ ] Save plugin state with MIDI bindings
   - [ ] Close and reopen DAW
   - [ ] MIDI bindings persist correctly
   - [ ] Bindings work after DAW restart

4. **Edge Cases**
   - [ ] Binding same CC to multiple parameters works
   - [ ] Unbinding/rebinding works correctly
   - [ ] CC 0-127 all work (test CC 0, 64, 127)
   - [ ] No crashes when spamming MIDI input

---

## Bug Tracker

Use this section to document any issues found during testing:

### High Priority
*(Crashes, data loss, major functional failures)*

### Medium Priority
*(Incorrect behavior, UI issues, performance problems)*

### Low Priority
*(Minor UI glitches, documentation errors)*

---

## Test Summary

**Start Date:** 2026-09-10  
**End Date:** ___________  
**Tester:** ___________  

**Results:**
- Total Plugins Tested: 0 / 37
- Passing: 0
- Failing: 0
- Blocked: 0

**Overall Status:** NOT STARTED

---

## Next Steps After QA

1. **Fix Critical Bugs** - Address any high-priority issues
2. **Document Known Issues** - Create issues list for users
3. **Performance Optimization** - Profile and optimize slow plugins
4. **Create User Guides** - Write documentation for complex features
5. **Prepare Release** - Version tagging, changelog, distribution
