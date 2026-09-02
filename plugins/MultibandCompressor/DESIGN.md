# Multiband Compressor Plugin - Design Document

**Version**: 1.0.0  
**Target Release**: v1.8.0  
**Timeline**: 3-4 weeks, 20-25 hours  
**Status**: Planning  

---

## Executive Summary

**AudioForge Multiband Compressor** is a professional-grade mastering tool that splits audio into 4 frequency bands and compresses each independently. This is the **crown jewel** of the AudioForge Mastering Suite, competing directly with:

- **FabFilter Pro-MB**: $149 (industry standard)
- **iZotope Ozone Dynamics**: $249 (part of Ozone suite)
- **Waves C6**: $149 (professional mastering)

### Why Multiband Compression Matters

**Problem**: Traditional compressors affect the entire frequency spectrum equally. Compressing a bass-heavy track reduces high frequencies unnecessarily, making the mix dull.

**Solution**: Multiband compression splits audio into frequency bands (low, low-mid, high-mid, high) and compresses each independently. This allows:
- **Bass control** without affecting vocals
- **Vocal presence** without pumping bass
- **Air and sparkle** without harshness
- **Mastering-grade dynamic control**

### Market Positioning

**AudioForge Multiband Compressor** (FREE):
- 4-band processing with visual crossover editor
- Linkwitz-Riley crossovers (phase-coherent)
- Per-band: threshold, ratio, attack, release, makeup gain
- Solo/bypass per band
- Visual FFT analyzer showing band splits
- Mid/Side processing per band (advanced)

**Target Users**:
- Mastering engineers (free alternative to Pro-MB/Ozone)
- Mix engineers (surgical frequency control)
- Podcast producers (voice clarity + bass control)
- Electronic music producers (EDM mastering)

---

## Technical Architecture

### DSP Components

#### 1. **Crossover Network** (Linkwitz-Riley 4th-order)

**Why Linkwitz-Riley?**
- Phase-coherent (bands sum to unity gain when bypassed)
- No phase distortion at crossover frequencies
- Steep rolloff (24 dB/octave)
- Industry standard for multiband processing

**Implementation**:
```cpp
class LinkwitzRileyCrossover {
    // 4 bands require 3 crossover frequencies
    std::array<float, 3> crossoverFreqs = {250.0f, 2000.0f, 8000.0f};
    
    // Each crossover = 2x cascaded Butterworth filters (2nd order)
    juce::dsp::IIR::Filter<float> lowPassFilters[3][2];   // 2 stages per crossover
    juce::dsp::IIR::Filter<float> highPassFilters[3][2];  // 2 stages per crossover
    
    void setCrossoverFreq(int index, float hz);
    void splitBands(const float* input, float** bandOutputs, int numSamples);
    void sumBands(const float** bandInputs, float* output, int numSamples);
};
```

**Shared Code**: Create `shared/dsp/LinkwitzRiley.h` for reuse

**Band Split Algorithm**:
```
Input Signal
    ↓
[Crossover 1: 250 Hz]
    ↓                    ↓
  Band 1 (Low)      [Crossover 2: 2 kHz]
  20-250 Hz              ↓                    ↓
                    Band 2 (Low-Mid)    [Crossover 3: 8 kHz]
                    250 Hz - 2 kHz           ↓            ↓
                                        Band 3 (High-Mid)  Band 4 (High)
                                        2 kHz - 8 kHz      8 kHz - 20 kHz
```

---

#### 2. **Per-Band Compressor**

Each of the 4 bands has its own compressor with independent controls.

**Parameters per band** (5 total):
- **Threshold** (-60 to 0 dB)
- **Ratio** (1:1 to 20:1)
- **Attack** (0.1 to 100 ms)
- **Release** (10 to 1000 ms)
- **Makeup Gain** (-12 to +12 dB)

**Implementation** (reuse from SimpleComp + Gate):
```cpp
class BandCompressor {
    audioforge::EnvelopeFollower envelope;
    
    float threshold = 0.5f;      // Linear gain
    float ratio = 4.0f;          // Compression ratio
    float attackMs = 10.0f;
    float releaseMs = 100.0f;
    float makeupGain = 1.0f;
    
    float attackCoeff;
    float releaseCoeff;
    float currentGainReduction = 1.0f;
    
    void setThreshold(float thresholdDb);
    void setRatio(float ratio);
    void setAttack(float attackMs);
    void setRelease(float releaseMs);
    void setMakeupGain(float gainDb);
    
    void process(float* samples, int numSamples);
    float getGainReductionDb() const;
};
```

**Shared Code**: Reuse `EnvelopeFollower` from Gate/Limiter

---

#### 3. **Mid/Side Processing** (Advanced Feature)

**What is Mid/Side?**
- **Mid**: Center content (mono sum of L+R)
- **Side**: Stereo content (difference of L-R)

**Why it matters**:
- Compress vocals (mid) without affecting stereo width
- Control bass (mid) without losing stereo image
- Enhance stereo field (side) without pumping center

**Implementation**:
```cpp
class MidSideProcessor {
    // Encode: L/R → Mid/Side
    void encode(float* left, float* right, int numSamples);
    
    // Decode: Mid/Side → L/R
    void decode(float* mid, float* side, int numSamples);
};

// Per-band processing
if (midSideEnabled[band]) {
    encoder.encode(leftBand, rightBand, numSamples);
    compressor.process(midBand, numSamples);   // Compress mid
    compressor.process(sideBand, numSamples);  // Compress side
    decoder.decode(midBand, sideBand, numSamples);
}
```

**Shared Code**: Create `shared/dsp/MidSide.h` for reuse

---

#### 4. **FFT Analyzer** (Visual Feedback)

**Purpose**: Show frequency spectrum with band boundaries overlaid

**Implementation**:
```cpp
class FFTAnalyzer {
    juce::dsp::FFT fft{10};  // 1024-point FFT
    juce::AudioBuffer<float> fftBuffer;
    std::array<float, 512> magnitudes;  // Frequency bins
    
    void processFrame(const float* samples, int numSamples);
    void getMagnitudes(float* output, int numBins);
};
```

**UI Integration**: Draw spectrum + vertical lines at crossover frequencies

---

### Signal Flow Diagram

```
Stereo Input (L/R)
    ↓
[Optional: Mid/Side Encode]
    ↓
[Crossover Network] ─→ Band 1 ─→ [Compressor 1] ─→ Solo/Bypass ─┐
    ↓                                                              │
    ├──────────────────→ Band 2 ─→ [Compressor 2] ─→ Solo/Bypass ─┤
    ↓                                                              │
    ├──────────────────→ Band 3 ─→ [Compressor 3] ─→ Solo/Bypass ─┤
    ↓                                                              │
    └──────────────────→ Band 4 ─→ [Compressor 4] ─→ Solo/Bypass ─┘
                                                                   ↓
[Sum Bands]
    ↓
[Optional: Mid/Side Decode]
    ↓
Output (L/R)
```

---

## Parameters

### Crossover Parameters (3 total)

| Parameter | Range | Default | Unit | Description |
|-----------|-------|---------|------|-------------|
| **Crossover 1** | 20 to 500 | 250 | Hz | Low / Low-Mid split |
| **Crossover 2** | 200 to 5k | 2000 | Hz | Low-Mid / High-Mid split |
| **Crossover 3** | 2k to 16k | 8000 | Hz | High-Mid / High split |

### Per-Band Parameters (5 × 4 bands = 20 total)

| Parameter | Range | Default | Unit | Description |
|-----------|-------|---------|------|-------------|
| **Threshold** | -60 to 0 | -20 | dB | Compression threshold |
| **Ratio** | 1:1 to 20:1 | 4:1 | ratio | Compression ratio |
| **Attack** | 0.1 to 100 | 10 | ms | Attack time |
| **Release** | 10 to 1000 | 100 | ms | Release time |
| **Makeup Gain** | -12 to +12 | 0 | dB | Output gain |

### Global Parameters (8 total)

| Parameter | Range | Default | Unit | Description |
|-----------|-------|---------|------|-------------|
| **Band 1 Solo** | On/Off | Off | - | Solo low band |
| **Band 2 Solo** | On/Off | Off | - | Solo low-mid band |
| **Band 3 Solo** | On/Off | Off | - | Solo high-mid band |
| **Band 4 Solo** | On/Off | Off | - | Solo high band |
| **Band 1 Bypass** | On/Off | Off | - | Bypass low band compression |
| **Band 2 Bypass** | On/Off | Off | - | Bypass low-mid band compression |
| **Band 3 Bypass** | On/Off | Off | - | Bypass high-mid band compression |
| **Band 4 Bypass** | On/Off | Off | - | Bypass high band compression |

### Advanced Parameters (4 total)

| Parameter | Range | Default | Unit | Description |
|-----------|-------|---------|------|-------------|
| **Mid/Side Band 1** | On/Off | Off | - | M/S processing for low band |
| **Mid/Side Band 2** | On/Off | Off | - | M/S processing for low-mid band |
| **Mid/Side Band 3** | On/Off | Off | - | M/S processing for high-mid band |
| **Mid/Side Band 4** | On/Off | Off | - | M/S processing for high band |

**Total Parameters**: 3 + 20 + 8 + 4 = **35 parameters**

---

## User Interface

### Layout (900x600 pixels)

```
┌─────────────────────────────────────────────────────────────────────────────┐
│  AudioForge Multiband Compressor                            [Analyzer: On]  │
│  Frequency-Selective Dynamics Control                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                               │
│  [FFT Analyzer with Band Boundaries]                                         │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │ dB                                                                   │    │
│  │  0 ─────────────────────────────────────────────────────────────── │    │
│  │ -20                   │         │         │                          │    │
│  │ -40                   250Hz    2kHz     8kHz                        │    │
│  │ -60 ───────────────────────────────────────────────────────────────│    │
│  │    20Hz      100Hz     1kHz      10kHz         20kHz               │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                                                               │
├─────────────────────────────────────────────────────────────────────────────┤
│  Band 1 (Low)    │  Band 2 (Low-Mid) │  Band 3 (High-Mid) │  Band 4 (High)│
│  20-250 Hz       │  250Hz-2kHz       │  2kHz-8kHz         │  8kHz-20kHz   │
├──────────────────┼───────────────────┼────────────────────┼───────────────┤
│  Thresh  Ratio   │  Thresh  Ratio    │  Thresh  Ratio     │  Thresh  Ratio│
│    ◯      ◯      │    ◯      ◯       │    ◯      ◯        │    ◯      ◯   │
│  -20dB   4:1     │  -20dB   4:1      │  -20dB   4:1       │  -20dB   4:1  │
│                  │                   │                    │               │
│  Attack Release  │  Attack Release   │  Attack Release    │  Attack Release│
│    ◯      ◯      │    ◯      ◯       │    ◯      ◯        │    ◯      ◯   │
│   10ms   100ms   │   10ms   100ms    │   10ms   100ms     │   10ms   100ms│
│                  │                   │                    │               │
│  Makeup          │  Makeup           │  Makeup            │  Makeup       │
│    ◯             │    ◯              │    ◯               │    ◯          │
│   0dB            │   0dB             │   0dB              │   0dB         │
│                  │                   │                    │               │
│  [Solo] [Bypass] │ [Solo] [Bypass]   │ [Solo] [Bypass]    │ [Solo] [Bypass]│
│  [M/S]           │ [M/S]             │ [M/S]              │ [M/S]         │
│                  │                   │                    │               │
│  GR: -6dB ▓▓▓░   │ GR: -3dB ▓▓░      │ GR: -2dB ▓░        │ GR: 0dB       │
└──────────────────┴───────────────────┴────────────────────┴───────────────┘
```

### Visual Feedback

**FFT Analyzer**:
- Real-time frequency spectrum (green line)
- Vertical lines at crossover frequencies (white/yellow)
- Band boundaries shaded (different colors per band)
- Logarithmic frequency scale (20 Hz - 20 kHz)
- Decibel scale (-60 to 0 dB)

**Per-Band Meters**:
- Gain reduction meter (horizontal bar, yellow)
- Updates at 30 Hz
- Shows current compression amount

**Solo/Bypass Buttons**:
- Solo: Light up when active, mute other bands
- Bypass: Light up when active, pass uncompressed
- Only one band can be soloed at a time

---

## Implementation Plan

### Phase 1: Crossover Network (Week 1, 8-10 hours)

**Files to create**:
1. `shared/dsp/LinkwitzRiley.h/.cpp` (200 lines)
   - 4th-order Linkwitz-Riley crossover
   - Band splitting (1 → 4 bands)
   - Band summing (4 → 1)
   - Phase-coherent design

2. `shared/dsp/MidSide.h/.cpp` (80 lines)
   - Mid/Side encoding (L/R → M/S)
   - Mid/Side decoding (M/S → L/R)

**Milestones**:
- ✅ Crossover splits audio into 4 bands correctly
- ✅ Summed bands = original signal (phase coherent)
- ✅ Mid/Side encoding/decoding works

---

### Phase 2: Band Compression (Week 2, 6-8 hours)

**Files to create**:
3. `plugins/MultibandCompressor/Source/BandCompressor.h/.cpp` (180 lines)
   - Reuse EnvelopeFollower from Gate
   - Threshold/ratio/attack/release/makeup gain
   - Gain reduction calculation
   - Smooth gain changes

4. `plugins/MultibandCompressor/Source/MultibandEngine.h/.cpp` (300 lines)
   - Integrate LinkwitzRiley crossover
   - 4 independent BandCompressor instances
   - Solo/bypass logic
   - Mid/Side processing per band

**Milestones**:
- ✅ Each band compresses independently
- ✅ Solo/bypass works correctly
- ✅ Mid/Side processing functional

---

### Phase 3: Plugin Integration (Week 2-3, 4-5 hours)

**Files to create**:
5. `plugins/MultibandCompressor/Source/PluginProcessor.h/.cpp` (250 lines)
   - 35 AudioProcessorValueTreeState parameters
   - Integration with MultibandEngine

6. `plugins/MultibandCompressor/Source/FFTAnalyzer.h/.cpp` (150 lines)
   - 1024-point FFT
   - Magnitude spectrum calculation
   - Smoothing for visual stability

**Milestones**:
- ✅ All 35 parameters mapped correctly
- ✅ FFT analyzer updates in real-time

---

### Phase 4: UI & Visualization (Week 3, 6-8 hours)

**Files to create**:
7. `plugins/MultibandCompressor/Source/PluginEditor.h/.cpp` (400 lines)
   - FFT analyzer display (top half)
   - 4 columns of controls (20 sliders)
   - 12 buttons (solo/bypass/M-S per band)
   - 4 gain reduction meters
   - 30Hz timer refresh

**Milestones**:
- ✅ FFT analyzer renders correctly
- ✅ Crossover lines overlay spectrum
- ✅ All 35 controls responsive

---

### Phase 5: Testing & Validation (Week 3-4, 2-3 hours)

**Test Cases**:
1. **Bass control**: Compress 20-250 Hz without affecting mids/highs
2. **Vocal clarity**: Compress 250 Hz-2 kHz without affecting bass
3. **De-essing**: Compress 2 kHz-8 kHz to reduce sibilance
4. **Air enhancement**: Compress 8 kHz+ to add sparkle
5. **Solo test**: Solo each band, verify frequency isolation
6. **Bypass test**: Bypass all bands, verify unity gain
7. **Mid/Side test**: Compress mid without affecting side

**Validation**:
- Apple AU validation (`auval -v aufx MbCo Afge`)
- PluginVal (Steinberg strict mode)
- CPU usage benchmark (<10% target, 4 compressors running)

---

## Technical Challenges & Solutions

### Challenge 1: Phase coherence
**Problem**: Naive crossovers cause phase distortion at band boundaries

**Solution**: 
- Linkwitz-Riley 4th-order (2x cascaded Butterworth)
- Guarantees flat magnitude response when summed
- Used by FabFilter, iZotope, Waves

---

### Challenge 2: CPU usage
**Problem**: 4 independent compressors + FFT = high CPU

**Solution**:
- Shared EnvelopeFollower (efficient RMS calculation)
- FFT runs at 30 Hz (not per sample)
- Optimize crossover filters (JUCE IIR)

**Target**: <10% CPU (1 instance, 44.1 kHz, 512 buffer)

---

### Challenge 3: UI complexity
**Problem**: 35 parameters = crowded interface

**Solution**:
- 4-column layout (one per band)
- Visual grouping (crossover at top, bands below)
- Color-coded bands (blue, green, yellow, red)
- Collapsible sections (hide advanced features)

---

## Code Quality Standards

### Shared DSP Libraries
**Goal**: Build maximum reusability

**Create**:
1. `shared/dsp/LinkwitzRiley.h` → Used in future multiband plugins
2. `shared/dsp/MidSide.h` → Used in Stereo Imaging, future tools
3. Reuse `shared/dsp/EnvelopeFollower.h` from Gate

**Benefits**:
- Consistent behavior
- Easier testing
- Faster future development

---

### Unit Tests (Optional, +3 hours)
**Test scenarios**:
1. Crossover frequency response (verify flat magnitude)
2. Band summing (verify phase coherence)
3. Mid/Side encoding/decoding (verify reversibility)
4. Compressor gain reduction accuracy

**Files**: `plugins/MultibandCompressor/Tests/MultibandEngineTest.cpp`

---

## Performance Targets

| Metric | Target | Measurement |
|--------|--------|-------------|
| **CPU Usage** | <10% | 1 instance, 4 compressors active |
| **Latency** | <5ms | Crossover filter delay |
| **Memory** | <50 MB | FFT buffers + band buffers |
| **UI Refresh** | 30 Hz | FFT analyzer + meters |

---

## Future Enhancements (v2.0)

### Advanced Features (Defer to later)
1. **Adaptive Crossovers**: Auto-adjust based on input content
2. **Linear Phase Mode**: Zero phase distortion (higher latency)
3. **External Sidechain**: Per-band external triggering
4. **Parallel Compression**: Blend dry/wet per band
5. **Visual Envelope**: Show attack/release curves
6. **Spectral Matching**: Match target frequency balance

---

## Success Criteria

**Functional**:
- ✅ Cleanly splits audio into 4 frequency bands
- ✅ Each band compresses independently
- ✅ Summed bands = phase-coherent output
- ✅ Solo/bypass works correctly
- ✅ Mid/Side processing functional
- ✅ FFT analyzer visualizes band splits

**Technical**:
- ✅ Passes Apple AU validation
- ✅ Passes PluginVal strict mode
- ✅ CPU usage <10% per instance
- ✅ Works in Ableton, Logic, Reaper

**User Experience**:
- ✅ Crossover frequencies easy to adjust
- ✅ Per-band compression intuitive
- ✅ Visual feedback helps understanding
- ✅ Presets cover common mastering tasks

---

## Presets (10 included)

**Mastering**:
1. Gentle Mastering (2:1 ratios, slow attack/release)
2. Aggressive Mastering (4:1 ratios, fast attack/release)
3. Vintage Warmth (compress lows + highs, boost mids)

**Mixing**:
4. Vocal Clarity (compress 250 Hz-2 kHz, boost presence)
5. Bass Control (compress 20-250 Hz, tighten low end)
6. De-Esser (compress 2 kHz-8 kHz, reduce sibilance)

**Creative**:
7. Parallel Compression (heavy compression + dry blend)
8. Upward Expansion (boost quiet frequencies)
9. Mid/Side Mastering (compress mid, preserve side)
10. EDM Loudness (aggressive ratios, fast times)

---

## Documentation Requirements

### User Manual (10-12 pages)
1. What is multiband compression?
2. When to use it (mastering, mixing, creative)
3. Crossover frequency selection guide
4. Per-band parameter explanations
5. Mid/Side processing tutorial
6. Solo/bypass workflow
7. FFT analyzer interpretation
8. Common use cases with examples
9. Troubleshooting (phase issues, over-compression)

### Developer Notes
1. Linkwitz-Riley theory and implementation
2. Mid/Side encoding mathematics
3. FFT analyzer optimization
4. Parameter ranges and defaults
5. Performance profiling results

---

## Timeline Summary

| Week | Phase | Hours | Deliverables |
|------|-------|-------|--------------|
| 1 | Crossover Network | 8-10 | LinkwitzRiley, MidSide libraries |
| 2 | Band Compression | 6-8 | BandCompressor, MultibandEngine |
| 2-3 | Plugin Integration | 4-5 | PluginProcessor, FFTAnalyzer |
| 3 | UI & Visualization | 6-8 | PluginEditor with FFT display |
| 3-4 | Testing & Validation | 2-3 | AU/PluginVal pass, DAW testing |

**Total**: 20-28 hours over 3-4 weeks

---

## Risk Assessment

### Low Risk
- ✅ Crossover filters (well-established algorithm)
- ✅ Compression (reuse from Gate/SimpleComp)
- ✅ Parameter mapping (same as Gate/Limiter)

### Medium Risk
- ⚠️ Phase coherence (requires careful testing)
- ⚠️ CPU usage (4 compressors + FFT)
- ⚠️ UI complexity (35 parameters)

### High Risk
- ❌ Mid/Side artifacts (if not implemented correctly)

**Mitigation**: Test Mid/Side with null test (encode → decode should = original)

---

## Competitive Analysis

| Feature | AudioForge MB | FabFilter Pro-MB | iZotope Ozone | Waves C6 |
|---------|---------------|------------------|---------------|----------|
| **Price** | FREE | $149 | $249 | $149 |
| **Bands** | 4 | 6 | 4 | 6 |
| **Crossover Type** | Linkwitz-Riley | Linear Phase | Both | Linkwitz-Riley |
| **Per-Band Controls** | 5 | 8 | 6 | 6 |
| **Mid/Side** | ✅ | ✅ | ✅ | ✅ |
| **FFT Analyzer** | ✅ | ✅ Excellent | ✅ Excellent | ✅ Basic |
| **Solo/Bypass** | ✅ | ✅ | ✅ | ✅ |
| **External Sidechain** | ❌ (v2.0) | ✅ | ❌ | ✅ |
| **Parallel Mode** | ❌ (v2.0) | ✅ | ✅ | ❌ |
| **Presets** | ✅ 10 | ✅ Extensive | ✅ Extensive | ✅ Many |

**Unique Selling Points**:
- **FREE** (competitors are $149-249)
- Linkwitz-Riley crossovers (phase-coherent)
- Mid/Side per band (professional feature)
- Visual FFT analyzer
- Clean, focused UI (less overwhelming than Pro-MB)

---

## Next Steps

**Immediate** (Today):
1. Create `shared/dsp/LinkwitzRiley.h` skeleton
2. Create `shared/dsp/MidSide.h` skeleton
3. Research Linkwitz-Riley filter coefficients
4. Set up CMakeLists.txt (copy from Gate, modify)

**This Week**:
1. Implement LinkwitzRiley crossover network
2. Test band splitting/summing for phase coherence
3. Implement MidSide encoder/decoder
4. Unit test both libraries

**Next Week**:
1. Build BandCompressor class
2. Build MultibandEngine
3. Test 4-band independent compression

**Week 3**:
1. Integrate with PluginProcessor
2. Build UI with FFT analyzer
3. Test in standalone app

**Week 4**:
1. DAW testing
2. AU/PluginVal validation
3. Documentation
4. Commit v1.8.0

---

**Let's build the most powerful free multiband compressor ever created!** 🎚️

**This will complete the AudioForge Mastering Suite and position it as a serious FabFilter/iZotope competitor.** 🚀
