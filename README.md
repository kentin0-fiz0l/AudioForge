# AudioForge

[![Build and Test](https://github.com/kentin0-fiz0l/AudioForge/actions/workflows/build-test.yml/badge.svg)](https://github.com/kentin0-fiz0l/AudioForge/actions/workflows/build-test.yml)
[![Code Coverage](https://codecov.io/gh/kentin0-fiz0l/AudioForge/branch/main/graph/badge.svg?token=YOUR_CODECOV_TOKEN)](https://codecov.io/gh/kentin0-fiz0l/AudioForge)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Professional-quality DAW plugins, free and open source.

## Overview

AudioForge is a collection of high-quality audio plugins for music production. Starting with essential utilities and effects, we're building a suite of tools that producers actually want to use.

## Current Plugins

### Released (v1.0.0)
- **SimpleGain** - Clean gain/volume control with metering (-60 to +12 dB)
- **PanUtil** - Stereo panning utility with balance and width controls
- **BasicSynth** - Simple subtractive synthesizer with ADSR envelope and filter
- **CleanDelay** - Stereo delay effect with ping-pong mode and tempo sync
- **SimpleEQ** - 3-band parametric EQ with high/low-pass filters
- **SimpleComp** - Compressor with auto makeup gain and soft/hard knee modes
- **WaveShaper** - Waveshaping distortion with 5 curve modes and real-time visualization
- **Saturation** - Multi-mode analog saturation with harmonic enhancement and oversampling
- **Chorus/Flanger** - LFO-based modulation effects with stereo width and real-time visualization

### Planned (Phase 3+)
- Reverb effects
- Advanced modulation
- Creative effects

## Tech Stack

- **Framework**: JUCE 7.x
- **Language**: C++17
- **Formats**: VST3 (AU and AAX coming later)
- **Platforms**: macOS, Windows, Linux

## Project Structure

```
AudioForge/
├── .github/              # GitHub Actions CI/CD workflows
├── JUCE/                 # JUCE framework (submodule)
├── plugins/              # Individual plugin projects
│   ├── SimpleGain/       # ✅ Complete (v1.0.0)
│   └── PanUtil/          # ✅ Complete (v1.0.0)
├── shared/               # Shared DSP library
│   └── dsp/              # Reusable DSP components
│       ├── ParameterSmoothing.h
│       ├── Metering.h
│       ├── PanningAlgorithms.h
│       ├── FilterDesign.h
│       └── Utilities.h
├── tests/                # JUCE UnitTestRunner test suite
├── docs/                 # Documentation and tutorials
└── builds/               # Compiled plugins
```

## Getting Started

### Prerequisites

- CMake 3.15+
- C++17 compatible compiler
  - macOS: Xcode 10+
  - Windows: Visual Studio 2019+
  - Linux: GCC 7+ or Clang 6+

### Build Instructions

```bash
# Clone with JUCE submodule
git clone --recursive https://github.com/yourusername/AudioForge.git

# Or if already cloned
git submodule update --init --recursive

# Build a plugin
cd plugins/SimpleGain
cmake -B build
cmake --build build
```

### Installing Plugins

Built plugins will be in `builds/`. Copy them to your DAW's plugin folder:
- **macOS VST3**: `~/Library/Audio/Plug-Ins/VST3/`
- **Windows VST3**: `C:\Program Files\Common Files\VST3\`
- **Linux VST3**: `~/.vst3/`

### Running Tests

```bash
cd tests
cmake -B build
cmake --build build
./build/TestRunner           # Run all tests
./build/TestRunner -v        # Verbose output
```

Test suite: 123/123 tests passing (100%) | Coverage: See badge above

## Development Roadmap

### Phase 1: Infrastructure Foundation ✅ COMPLETE
- [x] Project setup with JUCE
- [x] SimpleGain plugin (v1.0.0)
- [x] PanUtil plugin (v1.0.0)
- [x] Shared DSP library (5 reusable components)
- [x] Testing framework (JUCE UnitTestRunner, 34 test suites)
- [x] CI/CD pipeline (GitHub Actions, multi-platform builds)
- [x] Documentation

### Phase 2: Core Plugins
- [ ] BasicSynth (oscillators, filters, ADSR)
- [ ] PanUtil (stereo utilities)
- [ ] Simple EQ (beginner-friendly DSP)

### Phase 3: Distribution
- [ ] Website for downloads
- [ ] User documentation
- [ ] Video tutorials
- [ ] Community feedback

### Phase 4: Monetization
- [ ] Premium plugin tier
- [ ] Paid expansion packs
- [ ] Support/licensing options

## Learning Resources

Since this is a learning project for DSP, we're documenting our journey:
- `docs/dsp-basics.md` - DSP fundamentals
- `docs/juce-guide.md` - JUCE framework patterns
- Each plugin includes inline comments explaining the algorithms

## Contributing

This is currently a personal learning project, but feedback and suggestions are welcome via issues.

## License

MIT License - Free for personal and commercial use.

## Contact

- Website: Coming soon
- Issues: GitHub Issues
