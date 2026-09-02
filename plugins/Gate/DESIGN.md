# Gate/Expander Plugin - Design Document

**Version**: 1.0.0  
**Target Release**: v1.7.0  
**Timeline**: 2-3 weeks, 12-15 hours  
**Status**: Planning  

---

## Executive Summary

**AudioForge Gate** is a professional-grade noise gate and expander designed for mixing engineers who need precise dynamic control. Unlike basic gates that abruptly silence audio, this plugin offers:

- **Dual modes**: Hard gate (instant silence) and soft expander (gradual reduction)
- **Sidechain input**: Trigger from external audio (e.g., kick drum triggers bass gate)
- **Sidechain filtering**: Isolate specific frequencies to trigger gating (e.g., only snare hits)
- **Lookahead**: Prevents cutting off transients (0-10ms)
- **Visual feedback**: Real-time gain reduction metering

### Market Positioning

**Competitors**:
- TDR Nova (free, but multi-tool with basic gate)
- ReaGate (REAPER bundled, basic features)
- Waves C1 Gate ($79)
- FabFilter Pro-G ($149)

**AudioForge Gate** (FREE):
- Dedicated gate/expander (not a multi-tool)
- Sidechain filtering (missing from most free gates)
- Lookahead buffer (professional feature)
- Clean, focused UI

**Target Users**:
- Drum mixing (kick, snare, toms isolation)
- Vocal cleanup (remove breath noise, room ambience)
- Guitar/bass gating (remove amp hum)
- Podcast production (remove background noise)

---

## Technical Architecture

### DSP Components

#### 1. **Envelope Follower**
Tracks the input signal's amplitude over time.

**Algorithm**: RMS (Root Mean Square) or Peak detection
- **RMS**: Averages signal energy over a window (smoother, better for sustained sounds)
- **Peak**: Tracks instantaneous peaks (faster, better for transients)

**Implementation**:
```cpp
class EnvelopeFollower {
    float rmsWindow = 10.0f; // ms
    float peakHoldTime = 10.0f; // ms
    
    float detectRMS(const float* samples, int numSamples);
    float detectPeak(const float* samples, int numSamples);
};
```

**Shared Code**: Create `shared/dsp/EnvelopeFollower.h` for reuse in Compressor/Limiter

---

#### 2. **Sidechain Processor**
Allows external audio to control gating (e.g., kick drum triggers bass gate).

**Features**:
- **External input**: Route another track as sidechain
- **Filtering**: High-pass/low-pass to isolate trigger frequency
  - Example: Filter sidechain to 2-5kHz to detect only snare hits
- **Listen mode**: Hear the filtered sidechain signal

**Implementation**:
```cpp
class SidechainProcessor {
    juce::dsp::IIR::Filter<float> highPassFilter;
    juce::dsp::IIR::Filter<float> lowPassFilter;
    
    void setHighPassFreq(float hz);
    void setLowPassFreq(float hz);
    float processSidechain(const float* samples, int numSamples);
};
```

**Shared Code**: Create `shared/dsp/Sidechain.h` for reuse in Compressor/De-Esser

---

#### 3. **Gain Reduction Calculator**
Determines how much to reduce the signal based on threshold and mode.

**Gate Mode** (Hard):
```
if (inputLevel < threshold) {
    gainReduction = 0.0; // Full silence
} else {
    gainReduction = 1.0; // Full volume
}
```

**Expander Mode** (Soft):
```
if (inputLevel < threshold) {
    // Gradual reduction based on ratio
    float excessDb = threshold - inputLevel;
    float reductionDb = excessDb * (ratio - 1.0);
    gainReduction = dBToGain(-reductionDb);
} else {
    gainReduction = 1.0; // Full volume
}
```

**Ratio Examples**:
- **Gate**: Infinite ratio (instant silence below threshold)
- **Expander**: 2:1 (gentle), 4:1 (medium), 10:1 (aggressive)

---

#### 4. **Envelope Shaper**
Smooths gain reduction changes to prevent clicks/pops.

**Parameters**:
- **Attack**: How fast to open gate after signal exceeds threshold (0.1-100ms)
- **Hold**: How long to keep gate open after signal drops below threshold (0-1000ms)
- **Release**: How fast to close gate after hold period (10-1000ms)

**Implementation**:
```cpp
class EnvelopeShaper {
    enum State { Closed, Attack, Open, Hold, Release };
    State currentState = Closed;
    
    float attackCoeff;
    float releaseCoeff;
    float holdTimeMs;
    
    float process(float targetGain, float currentGain);
};
```

**Attack/Release Coefficients** (exponential smoothing):
```cpp
float timeConstantToCoeff(float timeMs, double sampleRate) {
    return exp(-1.0 / (timeMs * 0.001 * sampleRate));
}
```

---

#### 5. **Lookahead Buffer**
Delays the audio signal so the gate "sees ahead" and doesn't cut off transients.

**How it works**:
1. Analyze the input signal 0-10ms ahead
2. Calculate gain reduction from lookahead signal
3. Apply gain reduction to delayed main signal

**Implementation** (same as Limiter):
```cpp
class LookaheadBuffer {
    juce::AudioBuffer<float> buffer;
    int writePosition = 0;
    int readPosition = 0;
    
    void setLookaheadMs(float ms, double sampleRate);
    void process(juce::AudioBuffer<float>& audio);
};
```

**Shared Code**: Extract `shared/dsp/LookaheadBuffer.h` from Limiter

---

### Signal Flow Diagram

```
Input Audio
    ↓
[Lookahead Buffer] ──→ Delayed Audio ──┐
    ↓                                   ↓
[Sidechain Input?] ─→ [Sidechain Filter] ─→ [Envelope Follower]
    ↓                                          ↓
[Detection: RMS/Peak] ─────────────────→ Input Level
    ↓
[Gain Reduction Calculator] ← Threshold, Ratio, Range
    ↓
[Envelope Shaper] ← Attack, Hold, Release
    ↓
Gain Reduction ──────────────────────────→ [Apply to Delayed Audio]
    ↓
Output Audio
```

---

## Parameters

### Core Parameters (8 total)

| Parameter | Range | Default | Unit | Description |
|-----------|-------|---------|------|-------------|
| **Threshold** | -60 to 0 | -30 | dB | Signal level to open gate |
| **Ratio** | 1:1 to ∞:1 | 10:1 | ratio | Gate: ∞:1, Expander: 2:1-20:1 |
| **Range** | 0 to -96 | -60 | dB | Max gain reduction (0 = full mute, -96 = complete silence) |
| **Attack** | 0.1 to 100 | 1.0 | ms | Time to open gate |
| **Hold** | 0 to 1000 | 50 | ms | Time to keep gate open |
| **Release** | 10 to 1000 | 100 | ms | Time to close gate |
| **Lookahead** | 0 to 10 | 2.0 | ms | Preview time to prevent transient loss |
| **Mode** | Gate/Expand | Gate | - | Hard gate or soft expander |

### Sidechain Parameters (4 total)

| Parameter | Range | Default | Unit | Description |
|-----------|-------|---------|------|-------------|
| **Sidechain Enable** | On/Off | Off | - | Use external input to trigger |
| **SC High-Pass** | 20 to 2000 | 80 | Hz | Filter low frequencies from sidechain |
| **SC Low-Pass** | 200 to 20k | 8000 | Hz | Filter high frequencies from sidechain |
| **SC Listen** | On/Off | Off | - | Monitor filtered sidechain signal |

### Detection Parameters (2 total)

| Parameter | Range | Default | Unit | Description |
|-----------|-------|---------|------|-------------|
| **Detection** | RMS/Peak | RMS | - | RMS (smoother) or Peak (faster) |
| **RMS Window** | 1 to 50 | 10 | ms | Averaging window for RMS detection |

**Total**: 14 parameters

---

## User Interface

### Layout (600x400 pixels)

```
┌─────────────────────────────────────────────────────────────┐
│  AudioForge Gate                        [Gate/Expander Mode]│
│  Dynamic Noise Control                                       │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  Threshold    Ratio      Attack     Hold      Release        │
│     ◯         ◯          ◯          ◯          ◯            │
│   -30 dB     10:1       1.0 ms     50 ms     100 ms         │
│                                                               │
│  Range       Lookahead    [Detection: RMS ▼]                 │
│     ◯          ◯          RMS Window: 10 ms                  │
│   -60 dB      2.0 ms                                         │
│                                                               │
├─────────────────────────────────────────────────────────────┤
│  Sidechain                                                    │
│  [✓] Enable    SC HP: 80 Hz    SC LP: 8000 Hz  [Listen]     │
│                                                               │
├─────────────────────────────────────────────────────────────┤
│  Input: ▓▓▓▓▓▓▓░░░░   GR: ░░░░░░░░░░   Output: ▓▓▓▓░░░░░░  │
│          -12 dB             -6 dB               -18 dB       │
└─────────────────────────────────────────────────────────────┘
```

### Visual Feedback

**Meters** (30Hz refresh):
- **Input**: Shows signal level before gating
- **GR (Gain Reduction)**: Shows how much signal is reduced
- **Output**: Shows signal level after gating

**Mode Toggle**:
- **Gate**: Infinite ratio, hard knee
- **Expander**: Adjustable ratio, soft knee

**Sidechain Section**:
- Collapsible panel
- Visual indicator when sidechain is active
- Listen button to monitor filtered sidechain

---

## Implementation Plan

### Phase 1: Core DSP (Week 1, 6-8 hours)

**Files to create**:
1. `shared/dsp/EnvelopeFollower.h/.cpp` (80 lines)
   - RMS detection
   - Peak detection
   - Configurable window size

2. `shared/dsp/Sidechain.h/.cpp` (120 lines)
   - High-pass/low-pass filtering
   - External input routing
   - Listen mode

3. `plugins/Gate/Source/GateEngine.h/.cpp` (200 lines)
   - Gain reduction calculator
   - Envelope shaper (attack/hold/release)
   - Lookahead buffer integration
   - Mode switching (gate/expander)

**Milestones**:
- ✅ Envelope follower detects signal level
- ✅ Sidechain filtering works
- ✅ Gain reduction calculated correctly
- ✅ Attack/hold/release smooth transitions

---

### Phase 2: Plugin Integration (Week 2, 4-5 hours)

**Files to create**:
4. `plugins/Gate/Source/PluginProcessor.h/.cpp` (150 lines)
   - AudioProcessorValueTreeState for 14 parameters
   - Sidechain bus configuration
   - Integration with GateEngine

5. `plugins/Gate/Source/PluginEditor.h/.cpp` (250 lines)
   - 8 rotary sliders (threshold, ratio, range, attack, hold, release, lookahead, RMS window)
   - 2 toggle buttons (mode, sidechain enable)
   - 2 combo boxes (detection type, sidechain listen)
   - 2 frequency sliders (SC HP, SC LP)
   - 3 meters (input, GR, output)

**Milestones**:
- ✅ All parameters mapped correctly
- ✅ Sidechain input configured
- ✅ UI responsive (30Hz refresh)

---

### Phase 3: Testing & Validation (Week 2-3, 2-3 hours)

**Test Cases**:
1. **Drum gating**: Remove tom bleed, preserve attack
2. **Vocal cleanup**: Remove breath noise without cutting words
3. **Sidechain triggering**: Kick drum triggers bass gate
4. **Frequency-selective**: Hi-hat triggers only on 8kHz+ energy
5. **Lookahead effectiveness**: Transients preserved at 0ms, 2ms, 5ms, 10ms

**Validation**:
- Apple AU validation (`auval -v aufx Gate Afge`)
- PluginVal (Steinberg strict mode)
- CPU usage benchmark (<5% target)

**DAW Testing**:
- Load in Ableton/Logic/Reaper
- Test parameter automation
- Test sidechain routing
- Verify no clicks/pops

---

## Technical Challenges & Solutions

### Challenge 1: Click-free gating
**Problem**: Abrupt gain changes cause audible clicks

**Solution**: 
- Exponential attack/release curves (not linear)
- Lookahead to predict transients
- Minimum attack time (0.1ms) to prevent instant changes

---

### Challenge 2: Sidechain latency
**Problem**: Sidechain input may be delayed, causing mis-timed gating

**Solution**:
- Use DAW's automatic delay compensation
- Report plugin latency via `getTailLengthSeconds()`
- Lookahead buffer compensates for processing delay

---

### Challenge 3: RMS vs. Peak trade-off
**Problem**: RMS is smooth but slow, Peak is fast but noisy

**Solution**:
- Offer both detection modes
- Default to RMS (10ms window) for general use
- Peak mode for transient-heavy material (drums)

---

## Code Quality Standards

### Shared DSP Libraries
**Goal**: Build reusable components for future plugins

**Create**:
1. `shared/dsp/EnvelopeFollower.h` → Used in Compressor, De-Esser, Limiter
2. `shared/dsp/Sidechain.h` → Used in Compressor, De-Esser, Multiband Comp
3. Extract `shared/dsp/LookaheadBuffer.h` from Limiter → Reuse here

**Benefits**:
- Consistent behavior across plugins
- Less code duplication
- Easier to test and optimize

---

### Unit Tests (Optional, +2 hours)
**Test scenarios**:
1. Envelope follower accuracy (RMS vs. Peak)
2. Sidechain filter frequency response
3. Attack/release curve shapes
4. Lookahead buffer delay accuracy

**Files**: `plugins/Gate/Tests/GateEngineTest.cpp`

---

## Performance Targets

| Metric | Target | Measurement |
|--------|--------|-------------|
| **CPU Usage** | <3% | 1 instance, 1000 samples, 44.1kHz |
| **Latency** | Lookahead ms | Reported via `getTailLengthSeconds()` |
| **Memory** | <10 MB | Lookahead buffer allocation |
| **UI Refresh** | 30 Hz | Timer callback |

---

## Future Enhancements (v2.0)

### Advanced Features (Defer to later)
1. **Hysteresis**: Separate thresholds for opening/closing (prevents "chattering")
2. **Ducking Mode**: Invert behavior (reduce signal when sidechain is loud)
3. **Stereo Linking**: Link left/right channels (or process independently)
4. **Visual Envelope**: Display gain reduction curve over time
5. **MIDI Triggering**: Gate triggered by MIDI notes (experimental)

---

## Success Criteria

**Functional**:
- ✅ Cleanly gates drum tracks without cutting transients
- ✅ Removes vocal breath noise without artifacts
- ✅ Sidechain triggering works reliably
- ✅ Frequency-selective gating isolates target sounds
- ✅ No clicks/pops during operation

**Technical**:
- ✅ Passes Apple AU validation
- ✅ Passes PluginVal strict mode
- ✅ CPU usage <3% per instance
- ✅ Works in Ableton, Logic, Reaper

**User Experience**:
- ✅ UI intuitive for beginners
- ✅ Advanced features accessible for pros
- ✅ Presets cover common use cases
- ✅ Visual feedback helps understanding

---

## Presets (10 included)

**Drums**:
1. Kick Drum (fast attack, short hold)
2. Snare (medium attack, medium hold)
3. Toms (medium attack, long hold)
4. Hi-Hat (fast attack, very short hold)

**Vocals**:
5. Vocal Cleanup (medium attack, long hold, gentle ratio)
6. Podcast (slow attack, long hold, aggressive ratio)

**Instruments**:
7. Bass Guitar (medium attack, medium hold)
8. Electric Guitar (fast attack, short hold)

**Creative**:
9. Sidechain Pump (sidechain enabled, fast attack/release)
10. Rhythmic Gate (very fast attack/release, synced to tempo)

---

## Documentation Requirements

### User Manual (5-7 pages)
1. What is a gate? What is an expander?
2. When to use Gate vs. Expander mode
3. Parameter explanations with examples
4. Sidechain routing tutorial (per DAW)
5. Common use cases (drums, vocals, bass)
6. Troubleshooting (clicks, mis-timed gating)

### Developer Notes
1. Signal flow diagram
2. DSP algorithm explanations
3. Shared library usage
4. Parameter ranges and defaults
5. Performance optimization notes

---

## Timeline Summary

| Week | Phase | Hours | Deliverables |
|------|-------|-------|--------------|
| 1 | Core DSP | 6-8 | EnvelopeFollower, Sidechain, GateEngine |
| 2 | Plugin Integration | 4-5 | PluginProcessor, PluginEditor |
| 2-3 | Testing & Validation | 2-3 | AU/PluginVal pass, DAW testing |

**Total**: 12-16 hours over 2-3 weeks

---

## Risk Assessment

### Low Risk
- ✅ Envelope detection (well-understood algorithm)
- ✅ Sidechain filtering (JUCE IIR filters)
- ✅ Parameter mapping (same as Limiter)

### Medium Risk
- ⚠️ Click-free gating (requires careful attack/release tuning)
- ⚠️ Sidechain routing (DAW-dependent configuration)

### High Risk
- ❌ None (all components proven in Limiter)

---

## Competitive Analysis

| Feature | AudioForge Gate | TDR Nova | ReaGate | Waves C1 | FabFilter Pro-G |
|---------|-----------------|----------|---------|----------|-----------------|
| **Price** | FREE | FREE | FREE | $79 | $149 |
| **Gate Mode** | ✅ | ✅ | ✅ | ✅ | ✅ |
| **Expander Mode** | ✅ | ✅ | ❌ | ✅ | ✅ |
| **Sidechain** | ✅ | ✅ | ✅ | ✅ | ✅ |
| **SC Filtering** | ✅ HP+LP | ✅ EQ | ❌ | ✅ HP | ✅ Full EQ |
| **Lookahead** | ✅ 0-10ms | ❌ | ❌ | ❌ | ✅ 0-20ms |
| **Detection Modes** | ✅ RMS+Peak | ✅ RMS | ✅ RMS | ✅ RMS+Peak | ✅ RMS+Peak+Many |
| **Visual Metering** | ✅ 3 meters | ✅ Basic | ✅ Basic | ✅ Good | ✅ Excellent |
| **Presets** | ✅ 10 | ✅ Many | ✅ Few | ✅ Many | ✅ Extensive |

**Unique Selling Points**:
- Only free gate with lookahead buffer
- Dedicated tool (not multi-purpose like TDR Nova)
- Sidechain filtering (missing from ReaGate)
- Clean, modern UI

---

## Next Steps

**Immediate** (Today):
1. Create `shared/dsp/EnvelopeFollower.h` skeleton
2. Create `shared/dsp/Sidechain.h` skeleton
3. Create `plugins/Gate/Source/GateEngine.h` skeleton
4. Set up CMakeLists.txt (copy from Limiter, modify)

**This Week**:
1. Implement EnvelopeFollower (RMS + Peak)
2. Implement Sidechain filtering
3. Implement GateEngine core logic
4. Unit test each component

**Next Week**:
1. Integrate with PluginProcessor
2. Build UI (PluginEditor)
3. Test in standalone app
4. Validate with auval

**Week 3**:
1. DAW testing
2. PluginVal validation
3. Documentation
4. Commit v1.7.0

---

**Let's build a professional gate that makes free plugins look premium!** 🎚️
