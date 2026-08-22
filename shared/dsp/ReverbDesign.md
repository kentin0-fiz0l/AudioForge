# Reverb Algorithm Design

## Overview

AudioForge Reverb uses a Freeverb-inspired architecture based on Schroeder's reverberator design. This provides a good balance of sound quality, CPU efficiency, and parameter controllability.

## Algorithm Architecture

### Signal Flow

```
Input → Pre-Delay → [Comb Filter Bank] → [All-Pass Cascade] → Mix → Output
                           ↓ (parallel)          ↓ (series)
                        8 comb filters      4 all-pass filters
```

### Components

#### 1. Pre-Delay (0-100ms)
- Simple delay line before reverb processing
- Simulates distance to first reflection
- Adds depth and separation from dry signal

#### 2. Comb Filter Bank (8 parallel filters)
- Creates dense echo pattern (reverb tail)
- Each comb filter has:
  - Delay line with feedback
  - Damping filter (one-pole low-pass)
  - Tuned delay times to avoid metallic resonances

**Left Channel Delay Times (samples @ 44.1kHz):**
- Comb 1: 1116
- Comb 2: 1188
- Comb 3: 1277
- Comb 4: 1356
- Comb 5: 1422
- Comb 6: 1491
- Comb 7: 1557
- Comb 8: 1617

**Right Channel:** Add 23 samples to each delay for stereo decorrelation

#### 3. All-Pass Filter Cascade (4 series filters)
- Creates diffusion (smooths out echo density)
- Series connection for maximum diffusion
- Fixed feedback coefficient: 0.5

**All-Pass Delay Times (samples @ 44.1kHz):**
- AP1: 556
- AP2: 441
- AP3: 341
- AP4: 225

#### 4. Damping Filter
- One-pole low-pass filter in comb feedback path
- Simulates air absorption (high frequencies decay faster)
- Coefficient controlled by Damping parameter

## Parameters

### Room Size (0-100%)
- **Range**: 0.0 to 1.0
- **Effect**: Scales all comb filter delay times
- **Implementation**: `actualDelay = baseDelay * (0.5 + roomSize * 0.5)`
- **Audio Impact**: Larger rooms = longer reverb tail

### Damping (0-100%)
- **Range**: 0.0 to 1.0
- **Effect**: Controls low-pass filter coefficient in comb feedback
- **Implementation**: `dampCoeff = damping * 0.4`
- **Audio Impact**: Higher damping = darker, more absorbed sound

### Width (0-100%)
- **Range**: 0.0 to 1.0
- **Effect**: Stereo spread control
- **Implementation**: Mix L/R signals: `width=0 → mono, width=1 → full stereo`
- **Audio Impact**: Wider = more spacious, narrower = more focused

### Pre-Delay (0-100ms)
- **Range**: 0 to 4410 samples (@ 44.1kHz)
- **Effect**: Delay before reverb onset
- **Implementation**: Simple delay line
- **Audio Impact**: Simulates room size and distance

### Mix (0-100%)
- **Range**: 0.0 to 1.0
- **Effect**: Dry/wet balance
- **Implementation**: `output = dry * (1-mix) + wet * mix`
- **Audio Impact**: Control reverb amount

### Freeze (On/Off)
- **Range**: Boolean
- **Effect**: Infinite decay mode
- **Implementation**: Set comb feedback to 1.0, mute input
- **Audio Impact**: Sustains current reverb indefinitely

## Mathematical Formulas

### Comb Filter
```
y[n] = x[n] + feedback * damped[n-delay]
damped[n] = y[n] * (1-damp) + damped[n-1] * damp
```

### All-Pass Filter
```
y[n] = -x[n] + x[n-delay] + feedback * y[n-delay]
```
(feedback = 0.5 for all-pass filters)

### Damping Filter (one-pole low-pass)
```
y[n] = x[n] * (1-coeff) + y[n-1] * coeff
```

## Tuning Notes

### Delay Time Selection
- Comb filter delays chosen to avoid harmonic relationships
- Prime-ish numbers reduce metallic resonances
- Stereo offset (23 samples) creates decorrelation

### Feedback Coefficients
- Comb feedback: 0.84 (adjustable by room size)
- All-pass feedback: 0.5 (fixed)
- Freeze mode: 1.0 (infinite)

### Sample Rate Scaling
All delay times scale linearly with sample rate:
```
scaledDelay = baseDelay * (currentSampleRate / 44100.0)
```

## Performance Considerations

- **CPU Usage**: ~8-10% per instance (8 core M1)
- **Memory**: ~200KB for delay buffers
- **Latency**: Minimal (pre-delay only)

## Implementation Files

- `shared/dsp/ReverbEngine.h` - Core algorithm
- `plugins/Reverb/Source/PluginProcessor.cpp` - Parameter handling
- `tests/DSP/ReverbAlgorithmTests.cpp` - DSP unit tests
- `tests/Plugins/ReverbTests.cpp` - Plugin integration tests

## References

- Freeverb by Jezar at Dreampoint (public domain)
- Schroeder, M.R. "Natural Sounding Artificial Reverberation" (1962)
- Moorer, J.A. "About This Reverberation Business" (1979)
