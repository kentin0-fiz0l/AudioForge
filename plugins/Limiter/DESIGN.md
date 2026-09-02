# Limiter Plugin - Design Document

**Version**: 1.0.0  
**Type**: Mastering Limiter  
**Target Release**: v1.6.0  
**Estimated Time**: 15-20 hours

---

## Overview

Professional true peak limiter for mastering and final mix processing. Free alternative to FabFilter Pro-L ($149) and Waves L2 ($150).

**Market Gap**: No comprehensive free limiter exists with true peak detection and professional-grade sound quality.

---

## Core Features

### 1. True Peak Limiting (ITU-R BS.1770-4)
- Prevents intersample peaks (clipping on D/A conversion)
- 4x oversampling for accurate peak detection
- Compliant with streaming platforms (Spotify, Apple Music, YouTube)

### 2. Lookahead Buffer
- 0-10ms lookahead time
- Smooth gain reduction (prevents pumping)
- Latency compensation reported to DAW

### 3. Auto Makeup Gain
- Automatic output level compensation
- Maintains perceived loudness
- Optional manual override

### 4. Visual Feedback
- Input/output meters (peak + RMS)
- Gain reduction meter (real-time GR display)
- True peak indicator (red when >0 dBTP)

---

## Parameters

| Parameter | Range | Default | Unit | Description |
|-----------|-------|---------|------|-------------|
| **Ceiling** | -12 to 0 | -0.3 | dBFS | Maximum output level |
| **Threshold** | -24 to 0 | -6.0 | dB | Level above which limiting starts |
| **Release** | 10 to 1000 | 100 | ms | Gain reduction release time |
| **Lookahead** | 0 to 10 | 5 | ms | Peak anticipation time |
| **Oversampling** | 1x, 2x, 4x | 4x | - | Peak detection quality |
| **Auto Makeup** | On/Off | On | - | Automatic output compensation |
| **Output Trim** | -12 to +12 | 0 | dB | Manual output adjustment |

---

## DSP Architecture

### Signal Flow

```
Input
  ↓
[Oversampler 4x] → Upsample to 176.4/192 kHz
  ↓
[Lookahead Buffer] → Delay 0-10ms
  ↓
[Peak Detector] → Find true peaks
  ↓
[Gain Computer] → Calculate GR (threshold, ceiling)
  ↓
[Gain Smoother] → Apply release curve
  ↓
[Gain Multiplier] → Apply GR to signal
  ↓
[Auto Makeup] → Compensate output level
  ↓
[Downsampler] → Return to original sample rate
  ↓
Output
```

### Key Algorithms

**1. True Peak Detection (ITU-R BS.1770-4)**
```cpp
// 4x oversampling for intersample peak detection
float detectTruePeak(const float* samples, int numSamples) {
    auto upsampled = oversample4x(samples, numSamples);
    return findAbsoluteMaximum(upsampled);
}
```

**2. Gain Reduction Calculation**
```cpp
// Soft knee limiting curve
float calculateGainReduction(float inputLevel, float threshold, float ceiling) {
    if (inputLevel < threshold)
        return 0.0f; // No reduction below threshold
    
    float overThreshold = inputLevel - threshold;
    float ratio = 1000.0f; // Brickwall limiting (∞:1)
    float gainReduction = overThreshold * (1.0f - (1.0f / ratio));
    
    // Ensure output doesn't exceed ceiling
    return std::min(gainReduction, inputLevel - ceiling);
}
```

**3. Smooth Release Envelope**
```cpp
// Exponential release curve (prevents pumping)
float smoothGainReduction(float targetGR, float currentGR, float releaseTimeMs) {
    float attackCoeff = 0.9999f; // Instant attack
    float releaseCoeff = exp(-1.0f / (releaseTimeMs * sampleRate / 1000.0f));
    
    if (targetGR > currentGR)
        return currentGR + (targetGR - currentGR) * (1.0f - attackCoeff);
    else
        return currentGR + (targetGR - currentGR) * (1.0f - releaseCoeff);
}
```

---

## File Structure

```
plugins/Limiter/
├── Source/
│   ├── PluginProcessor.h       (DSP + state management)
│   ├── PluginProcessor.cpp
│   ├── PluginEditor.h          (UI + metering)
│   ├── PluginEditor.cpp
│   ├── LimiterEngine.h         (Core limiting algorithm)
│   ├── LimiterEngine.cpp
│   ├── TruePeakDetector.h      (Oversampling + peak detection)
│   ├── TruePeakDetector.cpp
│   └── LimiterLookAndFeel.h    (Custom UI styling)
├── CMakeLists.txt              (JUCE project configuration)
└── DESIGN.md                   (this file)
```

---

## Implementation Plan

### Phase 1: Core DSP (6-8 hours)
- ✅ Create plugin structure (CMakeLists.txt, basic files)
- ⏳ Implement `TruePeakDetector` (4x oversampling, ITU-R BS.1770-4)
- ⏳ Implement `LimiterEngine` (threshold, ceiling, release, lookahead)
- ⏳ Implement gain reduction smoothing (exponential release curve)
- ⏳ Add auto makeup gain calculation

### Phase 2: UI Design (4-6 hours)
- ⏳ Create parameter controls (rotary knobs for all 7 parameters)
- ⏳ Implement input/output meters (peak + RMS)
- ⏳ Implement gain reduction meter (horizontal bar, 0 to -20 dB)
- ⏳ Add true peak indicator (red light when >0 dBTP)
- ⏳ Custom LookAndFeel (professional dark theme)

### Phase 3: Testing & Optimization (3-4 hours)
- ⏳ PluginVal validation (strict mode, all tests pass)
- ⏳ Performance benchmarking (<5% CPU at 44.1kHz/512)
- ⏳ LUFS compliance testing (using test tones)
- ⏳ Intersample peak testing (verify no overshoots)

### Phase 4: Documentation (2 hours)
- ⏳ User manual (parameter descriptions, usage examples)
- ⏳ Code comments (algorithm explanations)
- ⏳ README update (add Limiter to plugin list)
- ⏳ Capture screenshots (5 types: default, active, extreme, daw, detail)

---

## Success Criteria

**Functional Requirements:**
- ✅ True peak limiting (no intersample peaks >0 dBTP)
- ✅ Adjustable ceiling (-12 to 0 dBFS)
- ✅ Smooth gain reduction (no pumping or distortion)
- ✅ Auto makeup gain (maintains perceived loudness)
- ✅ Visual feedback (meters, GR display)

**Quality Benchmarks:**
- ✅ PluginVal: 100% pass rate (strict mode)
- ✅ CPU Usage: <5% (44.1kHz, 512 samples, macOS)
- ✅ Latency: <10ms (reported to DAW)
- ✅ LUFS Accuracy: ±0.5 LU (compared to Loudness Meter)

**User Experience:**
- ✅ Intuitive controls (7 parameters, clear labeling)
- ✅ Professional appearance (dark theme, clean meters)
- ✅ Instant feedback (real-time metering)
- ✅ Zero-click workflow (presets for common tasks)

---

## Use Cases

### 1. Streaming Mastering
- **Goal**: -14 LUFS integrated loudness (Spotify, Apple Music)
- **Settings**: Ceiling -1.0 dB, Release 100ms, Auto Makeup ON
- **Workflow**: Load limiter on master bus → Adjust threshold until -14 LUFS

### 2. Podcast Mastering
- **Goal**: -16 LUFS integrated loudness (podcast platforms)
- **Settings**: Ceiling -1.0 dB, Release 50ms (fast for speech)
- **Workflow**: Apply to voice track → Match target loudness

### 3. YouTube Mastering
- **Goal**: -13 LUFS integrated loudness (YouTube target)
- **Settings**: Ceiling -0.5 dB, Release 150ms (moderate)
- **Workflow**: Final limiter on master → Check true peak <0 dBTP

---

## Competitive Analysis

| Feature | AudioForge Limiter | FabFilter Pro-L | Waves L2 | LoudMax (Free) |
|---------|-------------------|-----------------|----------|----------------|
| **Price** | Free | $149 | $150 | Free |
| **True Peak** | ✅ 4x Oversampling | ✅ 16x Oversampling | ✅ IRC™ | ❌ No oversampling |
| **Lookahead** | ✅ 0-10ms | ✅ 0-100ms | ✅ Fixed | ❌ None |
| **Auto Makeup** | ✅ Yes | ✅ Yes | ✅ Yes | ❌ No |
| **Metering** | ✅ Peak + RMS + GR | ✅ Advanced | ✅ Basic | ✅ Basic |
| **Oversampling** | ✅ 1x/2x/4x | ✅ up to 16x | ✅ IRC™ | ❌ None |
| **Open Source** | ✅ MIT License | ❌ Proprietary | ❌ Proprietary | ❌ Freeware |

**Positioning**: "80% of Pro-L quality at 0% of the cost, with open-source transparency."

---

## Known Limitations (v1.0.0)

1. **Oversampling**: Limited to 4x (FabFilter Pro-L offers 16x)
   - **Mitigation**: 4x is sufficient for 99% of use cases
   
2. **Lookahead**: Max 10ms (FabFilter Pro-L offers 100ms)
   - **Mitigation**: 5-10ms is industry standard for mastering
   
3. **Release Curves**: Single exponential curve (no advanced attack shaping)
   - **Future**: Add "Fast/Medium/Slow" presets (v1.1.0)
   
4. **Mid/Side Processing**: Not implemented in v1.0.0
   - **Future**: Add M/S mode for advanced stereo control (v1.2.0)

---

## Future Enhancements (v1.1.0+)

### v1.1.0: Advanced Release Curves
- Multiple release modes (Fast/Medium/Slow/Auto)
- Program-dependent release (adapts to audio content)
- Estimated time: 4 hours

### v1.2.0: Mid/Side Processing
- Separate limiting for mid and side channels
- Independent ceiling control per channel
- Estimated time: 6 hours

### v1.3.0: Clipper Mode
- True peak clipper (alternative to limiting)
- Soft/hard clipping algorithms
- Estimated time: 5 hours

---

## Testing Plan

### Unit Tests
- True peak detection accuracy (sine wave sweeps)
- Gain reduction calculation (threshold/ceiling verification)
- Auto makeup gain (verify output level matching)
- Oversampling quality (measure THD+N)

### Integration Tests
- PluginVal (strict mode, all tests)
- DAW compatibility (Logic Pro, Reaper, Ableton)
- Preset recall (save/load parameter state)
- Automation (smooth parameter changes)

### Performance Tests
- CPU usage across sample rates (44.1/48/96 kHz)
- Latency measurement (verify lookahead reporting)
- Memory usage (check for leaks)

---

## Documentation

### User Manual Sections
1. **Introduction**: What is a limiter? When to use it?
2. **Parameter Guide**: Detailed explanation of all 7 parameters
3. **Workflow Examples**: Streaming, podcast, YouTube mastering
4. **Technical Specs**: Algorithm details, oversampling, latency
5. **Troubleshooting**: Common issues and solutions

### Code Documentation
- Doxygen-style comments for all public methods
- Algorithm explanations (peak detection, gain reduction)
- References to ITU-R BS.1770-4 standard

---

## Release Checklist

- [ ] Core DSP implemented and tested
- [ ] UI designed and implemented
- [ ] PluginVal passes (100% strict mode)
- [ ] Performance benchmarked (<5% CPU)
- [ ] User manual written
- [ ] Code commented
- [ ] Screenshots captured (5 types)
- [ ] README updated
- [ ] GitHub release created (v1.6.0)
- [ ] Website updated (Limiter plugin page)
- [ ] Social media announcement

---

**Status**: 🚧 IN PROGRESS  
**Next Action**: Implement TruePeakDetector.h (4x oversampling)  
**Estimated Completion**: 2-3 weeks (15-20 hours)

This plugin transforms AudioForge from "synth collection" to "mastering suite" and fills a critical market gap. Let's build it! 🚀
