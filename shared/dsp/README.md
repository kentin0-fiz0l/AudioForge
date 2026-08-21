# AudioForge DSP Library

A header-only C++ library containing reusable DSP components for AudioForge audio plugins.

## Components

### 1. Utilities.h
Common DSP utility functions:
- **dB/Linear conversion**: `dbToLinear()`, `linearToDb()`
- **Range mapping**: `mapNormalizedToRange()`, `mapRangeToNormalized()`
- **Clipping/saturation**: `hardClip()`, `softClip()`
- **Frequency conversion**: `frequencyToNormalized()`, `normalizedToFrequency()`
- **Denormal handling**: `isDenormal()`, `flushDenormal()`
- **Interpolation**: `lerp()`

### 2. ParameterSmoothing.h
Click-free parameter smoothing using exponential decay:
```cpp
AudioForge::DSP::ParameterSmoothing<float> smoother;
smoother.reset(sampleRate, 0.05); // 50ms ramp time
smoother.setTargetValue(newValue);

for (int i = 0; i < numSamples; ++i) {
    float smoothValue = smoother.getNextValue();
    // Use smoothValue...
}
```

**Features**:
- Configurable ramp time
- Template class (float/double)
- Efficient exponential smoothing
- State queries: `isSmoothing()`, `getCurrentValue()`

### 3. Metering.h
Audio level metering for plugin displays:

**PeakMeter**: Peak level tracking with decay
```cpp
AudioForge::DSP::PeakMeter meter;
meter.reset(sampleRate, 1.0); // 1 second decay time
meter.processBuffer(samples, numSamples);
float peakDb = meter.getPeakLevelDb();
```

**RMSMeter**: RMS level calculation with sliding window
```cpp
AudioForge::DSP::RMSMeter meter;
meter.reset(sampleRate, 0.3); // 300ms window
meter.processBuffer(samples, numSamples);
float rmsDb = meter.getRMSLevelDb();
```

**ThreadSafeMeter**: Lock-free audio-to-GUI communication
```cpp
AudioForge::DSP::ThreadSafeMeter meter;
// Audio thread:
meter.updateLevel(level);
// GUI thread:
float level = meter.getLevel();
```

### 4. PanningAlgorithms.h
Stereo panning and width control:

**Panning Laws**:
- `constantPowerPan()`: Constant-power sin/cos law (professional standard)
- `linearPan()`: Linear crossfade
- `balance()`: Reduce one channel (for existing stereo)

**Stereo Width**:
- `applyStereoWidth()`: M/S processing for width control (0.0 = mono, 1.0 = normal, 2.0 = wide)
- `applyPanAndWidth()`: Combined pan + width (efficient)

**Utilities**:
- `calculateStereoCorrelation()`: Measure L/R correlation
- `calculateHaasDelaySamples()`: Precedence delay for widening

### 5. FilterDesign.h
Biquad filter coefficient calculator:

**Filter Types**:
- Low-pass: `makeLowPass(freq, sampleRate, Q)`
- High-pass: `makeHighPass(freq, sampleRate, Q)`
- Band-pass: `makeBandPass(freq, sampleRate, Q)`
- Notch: `makeNotch(freq, sampleRate, Q)`
- Low-shelf: `makeLowShelf(freq, sampleRate, gainDb, Q)`
- High-shelf: `makeHighShelf(freq, sampleRate, gainDb, Q)`
- Parametric EQ: `makePeakingEQ(freq, sampleRate, gainDb, Q)`
- All-pass: `makeAllPass(freq, sampleRate, Q)`

**BiquadFilter Processor**:
```cpp
auto coeffs = AudioForge::DSP::FilterDesign::makeLowPass(1000.0f, 48000.0f, 0.707f);
AudioForge::DSP::BiquadFilter filter;
filter.setCoefficients(coeffs);
filter.processBuffer(samples, numSamples);
```

## Usage

### CMake Integration
```cmake
# Add shared library to plugin
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../shared ${CMAKE_CURRENT_BINARY_DIR}/shared)

# Link against DSP library
target_link_libraries(YourPlugin PRIVATE
    juce::juce_audio_utils
    AudioForgeDSP)
```

### Include in Code
```cpp
// Include everything:
#include <dsp/AudioForgeDSP.h>

// Or include individual headers:
#include <dsp/ParameterSmoothing.h>
#include <dsp/PanningAlgorithms.h>
```

## Design Principles

1. **Header-only**: All code in headers for easy integration
2. **Namespace**: All code in `AudioForge::DSP` namespace
3. **JUCE integration**: Uses JUCE types where appropriate (`jlimit`, `MathConstants`, etc.)
4. **Template-friendly**: Generic types where useful (e.g., `ParameterSmoothing<T>`)
5. **Well-documented**: Clear comments and usage examples
6. **Performance**: Optimized algorithms, minimal overhead
7. **Thread-safe**: Lock-free communication for meters

## Current Users

- **SimpleGain**: Uses `ParameterSmoothing` and `ThreadSafeMeter`
- **PanUtil**: Uses `ParameterSmoothing`, `PanningAlgorithms`, and `ThreadSafeMeter`

## Future Components

Planned additions for upcoming plugins:
- **Envelope Followers**: Attack/release envelope detection
- **Compressor DSP**: Dynamic range compression algorithms
- **Delay Lines**: Circular buffers, interpolation
- **FFT Utilities**: Windowing, overlap-add processing
- **Modulation**: LFO, envelope generators
