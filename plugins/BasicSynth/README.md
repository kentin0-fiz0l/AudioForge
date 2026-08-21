# BasicSynth

A simple subtractive synthesizer for learning synthesis fundamentals.

## Overview

BasicSynth is a polyphonic subtractive synthesizer designed as an educational tool for understanding the core principles of sound synthesis. It features essential synthesis components in a clean, intuitive interface.

**Version:** 0.1.0
**Format:** VST3, Standalone
**Polyphony:** 8 voices
**Platform:** macOS (Apple Silicon & Intel)

## Features

### Oscillator
- **3 Waveforms**: Sine, Sawtooth, Square
- **Frequency Range**: Full MIDI note range (A0-C8)
- **Anti-Aliasing**: Basic waveform generation (PolyBLEP available for future use)

### ADSR Envelope
- **Attack**: 1ms - 2s (controls amplitude ramp-up)
- **Decay**: 1ms - 2s (controls decay to sustain level)
- **Sustain**: 0-100% (held level while note is pressed)
- **Release**: 1ms - 5s (controls fade-out after note release)

### Low-Pass Filter
- **Cutoff Frequency**: 20 Hz - 20 kHz (logarithmic scale)
- **Resonance (Q)**: 0.5 - 10.0
  - 0.707 = Butterworth (flat frequency response)
  - Higher values = resonant peak at cutoff frequency
- **Type**: 2nd-order Biquad low-pass
- **Filter per Voice**: Independent filtering for each polyphonic voice

### Voice Management
- **8-Voice Polyphony**: Up to 8 simultaneous notes
- **Voice Stealing**: Round-robin allocation when exceeding 8 voices
- **Per-Voice State**: Each voice maintains independent oscillator, envelope, and filter state

### Additional Features
- **Master Volume**: 0-100% output level control
- **Output Metering**: Real-time level visualization
- **State Management**: Full state save/restore for DAW sessions

## Parameter Reference

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Waveform | Sine / Sawtooth / Square | Sine | Oscillator waveform shape |
| Volume | 0.0 - 1.0 | 0.7 (70%) | Master output volume |
| Attack | 0.001s - 2.0s | 0.01s (10ms) | Envelope attack time |
| Decay | 0.001s - 2.0s | 0.1s (100ms) | Envelope decay time |
| Sustain | 0.0 - 1.0 | 0.7 (70%) | Envelope sustain level |
| Release | 0.001s - 5.0s | 0.3s (300ms) | Envelope release time |
| Filter Cutoff | 20 Hz - 20 kHz | 20 kHz (wide open) | Low-pass filter cutoff frequency |
| Filter Resonance | 0.5 - 10.0 | 0.707 (Butterworth) | Filter resonance (Q factor) |

## Installation

### macOS

1. Download `BasicSynth.vst3` from the [AudioForge website](https://audioforge.fluxstudio.art)
2. Copy to your VST3 plugins folder:
   ```bash
   cp -r BasicSynth.vst3 ~/Library/Audio/Plug-Ins/VST3/
   ```
3. Restart your DAW
4. BasicSynth will appear in your synth/instrument list

### Standalone App

1. Download `BasicSynth.app`
2. Copy to your Applications folder
3. Launch the standalone app to use without a DAW

## Usage Examples

### Pad Sound
- **Waveform**: Sine or Sawtooth
- **Attack**: 500ms - 1s (slow attack)
- **Decay**: 300ms - 500ms
- **Sustain**: 60-80%
- **Release**: 1s - 2s (long release)
- **Filter Cutoff**: 500Hz - 2kHz
- **Filter Resonance**: 1.0 - 2.0

### Bass Sound
- **Waveform**: Sawtooth or Square
- **Attack**: 1ms - 10ms (fast attack)
- **Decay**: 100ms - 300ms
- **Sustain**: 0-20% (short sustain)
- **Release**: 50ms - 200ms
- **Filter Cutoff**: 200Hz - 800Hz
- **Filter Resonance**: 2.0 - 4.0

### Pluck Sound
- **Waveform**: Any
- **Attack**: 1ms (instant)
- **Decay**: 200ms - 500ms
- **Sustain**: 0% (no sustain)
- **Release**: 100ms - 300ms
- **Filter Cutoff**: 1kHz - 5kHz
- **Filter Resonance**: 0.707 - 2.0

### Lead Sound
- **Waveform**: Sawtooth or Square
- **Attack**: 10ms - 50ms
- **Decay**: 100ms - 200ms
- **Sustain**: 70-90%
- **Release**: 200ms - 500ms
- **Filter Cutoff**: 3kHz - 10kHz
- **Filter Resonance**: 3.0 - 6.0 (resonant sweep)

## Technical Details

### Audio Processing
- **Sample Rate**: Any (tested at 44.1kHz, 48kHz, 96kHz)
- **Buffer Size**: Any (tested at 64-2048 samples)
- **Bit Depth**: 32-bit float internal processing
- **Latency**: Zero latency (real-time processing)

### DSP Architecture
- **Oscillator**: Direct digital synthesis with phase accumulation
- **Envelope**: Linear attack/release, linear decay
- **Filter**: Biquad IIR with Direct Form II Transposed
- **Voice Mixing**: Simple additive mixing with per-voice output

### Performance
- **CPU Usage**: ~1-2% per voice at 48kHz on Apple M1
- **Memory**: ~10MB total (VST3 plugin)
- **Optimization**: Filter coefficients cached and only recalculated when parameters change

## Building from Source

### Prerequisites
- CMake 3.15+
- C++17 compatible compiler
- JUCE Framework 7.x (included as submodule)

### Build Steps
```bash
cd plugins/BasicSynth
cmake -B build
cmake --build build

# VST3 output: build/BasicSynth_artefacts/VST3/BasicSynth.vst3
# Standalone: build/BasicSynth_artefacts/Standalone/BasicSynth.app
```

### Running Tests
```bash
cd tests
cmake -B build
cmake --build build
./build/TestRunner
```

## Roadmap

### v0.2.0 (Planned)
- [ ] Filter envelope modulation
- [ ] LFO for cutoff modulation
- [ ] Additional filter types (high-pass, band-pass)
- [ ] Velocity sensitivity controls
- [ ] Preset system

### v0.3.0 (Planned)
- [ ] Second oscillator with detune
- [ ] Oscillator mix control
- [ ] Sub-oscillator
- [ ] Noise generator

### v1.0.0 (Planned)
- [ ] Audio Units (AU) support
- [ ] Windows VST3 builds
- [ ] MPE (MIDI Polyphonic Expression) support
- [ ] Modulation matrix

## License

Part of the AudioForge plugin suite.
See the main AudioForge LICENSE file for details.

## Credits

**Development**: AudioForge Project
**DSP Library**: AudioForge shared DSP components
**Framework**: JUCE by Raw Material Software
**Testing**: JUCE UnitTest framework

## Support

- **Website**: https://audioforge.fluxstudio.art
- **Issues**: https://github.com/audioforge/audioforge/issues
- **Documentation**: https://audioforge.fluxstudio.art/docs

## Changelog

### v0.1.0 (2026-08-21)
- Initial release
- 3 oscillator waveforms (sine, sawtooth, square)
- ADSR envelope generator
- 8-voice polyphony with voice stealing
- Low-pass filter with resonance control
- Master volume with output metering
- VST3 and Standalone formats for macOS
