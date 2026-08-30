# FreezeFX - Spectral Freezing Plugin

## Overview

FreezeFX is an advanced real-time spectral freezing audio effect plugin that captures and manipulates frequency-domain content. It uses Short-Time Fourier Transform (STFT) to freeze audio in the spectral domain while allowing continuous phase evolution, creating ambient, textured, and evolving soundscapes from any audio source.

**Format Support:** VST3, AU (Audio Unit)
**Platform:** macOS (tested on macOS 15.2)
**Framework:** JUCE 8.x
**Language:** C++17

---

## Features

### Core Functionality

1. **Real-Time Spectral Freezing**
   - Captures magnitude and phase spectrum at freeze moment
   - Smooth blending between live and frozen audio (0-100% mix)
   - Maintains spectral characteristics while allowing timbral evolution

2. **Dual-Mode Phase Evolution**
   - **Smooth Mode** (0-50% randomization): Random walk with spectral coherence
     - Natural-sounding ambient textures
     - Correlation with neighboring frequency bins (10% influence)
     - Bass frequencies evolve slower than highs
   - **Aggressive Mode** (50-100% randomization): LFO modulation + strong randomization
     - Chaotic, dense textures
     - Sine-wave modulation per frequency bin
     - Frequency-dependent scaling

3. **Spectral Processing**
   - **Spectral Blur**: Box blur (1-6 bin radius) smooths magnitude spectrum
   - **Frequency Range Filtering**: Selectable high-pass and low-pass filtering
   - Adjustable FFT size (1024, 2048, 4096, 8192 samples)
   - Overlap factor (2x, 4x, 8x) for smooth reconstruction

4. **Real-Time Visualization**
   - Live magnitude spectrum display (512 bins)
   - Color-coded: Green (live), Cyan (frozen)
   - dB scale (-60dB to 0dB) with log frequency axis
   - 60 FPS refresh rate

---

## Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| **Freeze** | On/Off | Off | Captures and freezes current spectrum |
| **Freeze Mix** | 0-100% | 100% | Blend between live and frozen audio |
| **FFT Size** | 1024-8192 | 2048 | Frequency resolution (larger = more detail) |
| **Overlap** | 2x-8x | 4x | Overlap factor for smooth reconstruction |
| **Phase Random** | 0-100% | 50% | Amount of phase randomization/evolution |
| **Phase Speed** | 0.1-10 Hz | 1.0 Hz | Rate of phase evolution over time |
| **Spectral Blur** | 0-100% | 0% | Smoothing applied to magnitude spectrum |
| **High Pass** | 20-20000 Hz | 20 Hz | Low-frequency cutoff (log scale) |
| **Low Pass** | 20-20000 Hz | 20 kHz | High-frequency cutoff (log scale) |

---

## Usage Guide

### Basic Freeze Effect

1. Load FreezeFX on an audio track or send/return
2. Play audio through the plugin (you'll see green spectrum)
3. Click **FREEZE** button when you want to capture the spectrum
4. Spectrum turns cyan - audio is now frozen in frequency domain
5. Adjust **Freeze Mix** to blend frozen and live audio

### Creating Ambient Textures

1. Freeze a rich harmonic source (pad, voice, guitar sustain)
2. Set **Phase Random** to 20-40% (smooth evolution)
3. Set **Phase Speed** to 0.5-2 Hz (slow evolution)
4. Optionally add **Spectral Blur** (10-30%) for smoothness
5. Result: Evolving ambient texture that maintains original timbre

### Chaotic Soundscapes

1. Freeze any audio source
2. Set **Phase Random** to 70-100% (aggressive mode)
3. Set **Phase Speed** to 2-10 Hz (fast evolution)
4. Use **High Pass** and **Low Pass** to sculpt frequency range
5. Result: Dense, constantly-shifting texture

### Spectral Filtering

1. Freeze audio with full frequency content
2. Adjust **High Pass** (e.g., 200 Hz) to remove lows
3. Adjust **Low Pass** (e.g., 5 kHz) to remove highs
4. Use **Spectral Blur** to smooth transitions between bins
5. Result: Filtered frozen texture (different from traditional EQ)

---

## Technical Architecture

### Signal Flow

```
Input Audio
    ↓
[FFT Engine] (SpectralProcessor.cpp)
    ↓ (magnitude + phase)
[Freeze Logic] (PluginProcessor::processSpectrum)
    ├─→ [FrozenSpectrum] (captures/stores spectrum)
    ├─→ [PhaseEvolver] (evolves phase over time)
    └─→ [Spectral Filtering] (blur + frequency range)
    ↓ (modified magnitude + phase)
[Inverse FFT]
    ↓
Output Audio
```

### Key Components

**SpectralProcessor** (`SpectralProcessor.h/cpp`)
- Overlap-add STFT with Hann windowing
- Callback-based spectral manipulation
- Supports FFT sizes: 1024-8192 samples
- Overlap factors: 2x, 4x, 8x

**FrozenSpectrum** (`FrozenSpectrum.h/cpp`)
- Captures and stores magnitude/phase spectra
- Applies spectral blur (box blur algorithm)
- Frequency range filtering (zeros bins outside range)
- Converts Hz to FFT bin indices

**PhaseEvolver** (`PhaseEvolver.h/cpp`)
- Smooth mode: Random walk with spectral coherence
- Aggressive mode: LFO modulation + strong randomization
- Frequency-dependent evolution (bass slower, highs faster)
- Phase wrapping to [-π, π]

**PluginProcessor** (`PluginProcessor.h/cpp`)
- Main audio processor and parameter management
- Lambda callback for spectral processing
- Freeze trigger detection and blending logic

**PluginEditor** (`PluginEditor.h/cpp`)
- 500x600 UI with 9 parameter controls
- Real-time spectrum visualizer (60 FPS)
- Timer-based bidirectional parameter synchronization

---

## Build Instructions

### Prerequisites

- JUCE 8.x framework
- CMake 3.15+
- C++17 compiler (Xcode 14+, GCC 9+, or MSVC 2019+)

### macOS Build

```bash
cd plugins/FreezeFX
cmake -B build
cmake --build build

# Install to system plugin directories
cp -r build/FreezeFX_artefacts/Release/VST3/FreezeFX.vst3 ~/Library/Audio/Plug-Ins/VST3/
cp -r build/FreezeFX_artefacts/Release/AU/FreezeFX.component ~/Library/Audio/Plug-Ins/Components/
```

### Windows Build

```bash
cd plugins\FreezeFX
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Release

# Install to system plugin directories
copy build\FreezeFX_artefacts\Release\VST3\FreezeFX.vst3 "C:\Program Files\Common Files\VST3\"
```

### Linux Build

```bash
cd plugins/FreezeFX
cmake -B build
cmake --build build

# Install to system plugin directories
cp -r build/FreezeFX_artefacts/Release/VST3/FreezeFX.vst3 ~/.vst3/
```

---

## Development Phases

FreezeFX was developed in 5 phases over 2 days:

**Phase 1: FFT Setup** (Feb 2024)
- STFT engine with overlap-add
- FFT → IFFT pass-through (transparent)
- Basic UI skeleton

**Phase 2: Spectral Freezing** (Feb 2024)
- Freeze/unfreeze logic
- Magnitude blending
- Callback-based spectral processing

**Phase 3: Enhanced Phase Evolution** (Feb 2024)
- Dual-mode phase evolution algorithm
- Frequency-dependent weighting
- Spectral coherence for smooth mode

**Phase 4: Spectral Filtering** (Aug 2026)
- Box blur implementation
- Frequency range filtering (high-pass/low-pass)
- Hz to FFT bin conversion

**Phase 5: UI & Visualization** (Aug 2026)
- Complete parameter control hookup
- Real-time spectrum visualizer
- 60 FPS UI refresh with Timer

---

## Use Cases

### Music Production

- **Ambient Pads**: Freeze synthesizer chords for evolving textures
- **Vocal Effects**: Freeze voice sustains for otherworldly backgrounds
- **Transitional Elements**: Freeze end of phrase for smooth fade transitions
- **Soundscape Design**: Layer multiple frozen sources for dense atmospheres

### Sound Design

- **Drone Creation**: Freeze percussive hits for sustained drones
- **Texture Generation**: Evolve frozen spectra into new timbres
- **Cinematic Effects**: Create tension with slowly-evolving frozen audio
- **Granular Alternative**: Spectral domain freezing vs. time-domain granular

### Live Performance

- **Looping Alternative**: Freeze audio without traditional loop boundaries
- **Improvisation Tool**: Capture moments and evolve them in real-time
- **Transitional Effect**: Smooth transitions between song sections
- **Textural Layer**: Add frozen layers while playing live

---

## Algorithm Details

### Phase Evolution

**Smooth Mode** (randomAmount < 0.5):
```cpp
// Random walk with drift
float drift = (random() * 2.0f - 1.0f) * 0.01f * randomAmount;
phaseIncrement[i] += drift;
phaseIncrement[i] = clamp(phaseIncrement[i], -evolutionSpeed, evolutionSpeed);

// Spectral coherence (10% neighbor influence)
float neighborAvg = (phase[i-1] + phase[i+1]) * 0.5f;
phase[i] = phase[i] * 0.9f + neighborAvg * 0.1f * randomAmount;
```

**Aggressive Mode** (randomAmount >= 0.5):
```cpp
// Strong random offset + LFO modulation
float randomOffset = (random() * 2.0f - 1.0f) * randomAmount * 0.3f;
float modulation = sin(phaseAccumulator[i]) * randomAmount * freqFactor;
phase[i] += randomOffset + modulation * timeStep;
```

### Spectral Blur

Box blur with radius based on `blurAmount`:
```cpp
int radius = (int)(blurAmount * 5.0f) + 1;  // 1-6 bins
magnitude[i] = average(magnitude[i-radius : i+radius]);
```

### Frequency Range Filtering

Convert Hz to FFT bin index:
```cpp
int bin = (int)((frequencyHz / sampleRate) * fftSize);
if (bin < lowBin || bin > highBin)
    magnitude[bin] = 0.0f;
```

---

## Performance

- **Latency**: ~43ms @ 44.1kHz with 2048 FFT and 4x overlap (FFT size / sample rate)
- **CPU Usage**: ~5-10% on Apple M1 (single core)
- **Memory**: ~2 MB for FFT buffers and frozen spectra
- **UI Refresh**: 60 FPS (16.67ms frame time)

---

## Known Limitations

1. **FFT Latency**: Inherent latency due to FFT window size (not suitable for real-time monitoring)
2. **Transient Smearing**: Percussive transients may smear due to overlap-add
3. **Parameter Automation**: Some DAWs may have issues with FFT size changes during playback
4. **Mono Processing**: Processes left/right channels identically (no mid/side processing yet)

---

## Future Enhancements

- **Stereo Width Control**: Independent L/R phase evolution or mid/side processing
- **Preset System**: Save/recall frozen spectra and parameter settings
- **MIDI Learn**: Map MIDI controllers to parameters
- **Spectral Gate**: Threshold-based spectral filtering
- **Multi-Band Freeze**: Independent freeze per frequency band
- **Morph Between Frozen States**: Crossfade between multiple captured spectra

---

## Credits

**Developer**: AudioForge Team
**Framework**: JUCE 8.x (https://juce.com)
**License**: MIT (see LICENSE file in repository root)

**Algorithm Inspiration**:
- Spectral delay/freezing concepts from academic research
- STFT overlap-add implementation based on JUCE DSP tutorials
- Phase vocoder techniques for spectral manipulation

---

## Version History

- **v1.0.0** (Aug 2026): Initial release
  - All 5 development phases complete
  - VST3 and AU formats
  - macOS support (code-signed and notarized)

---

## Support

For bug reports, feature requests, or questions:
- GitHub Issues: https://github.com/kentin0-fiz0l/AudioForge/issues
- Discussions: https://github.com/kentin0-fiz0l/AudioForge/discussions

---

## License

FreezeFX is part of the AudioForge plugin suite and is released under the MIT License.
See the LICENSE file in the repository root for details.
