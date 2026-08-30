# AudioForge

[![Build and Test](https://github.com/kentin0-fiz0l/AudioForge/actions/workflows/build-test.yml/badge.svg)](https://github.com/kentin0-fiz0l/AudioForge/actions/workflows/build-test.yml)
[![Code Coverage](https://codecov.io/gh/kentin0-fiz0l/AudioForge/branch/main/graph/badge.svg?token=YOUR_CODECOV_TOKEN)](https://codecov.io/gh/kentin0-fiz0l/AudioForge)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Professional-quality DAW plugins, free and open source.

## Overview

AudioForge is a collection of high-quality audio plugins for music production. Starting with essential utilities and effects, we're building a suite of tools that producers actually want to use.

## Current Plugins

### Core Processing Suite (v1.0.0)
- **SimpleGain** - Clean gain/volume control with metering (-60 to +12 dB)
- **PanUtil** - Stereo panning utility with balance and width controls
- **BasicSynth** - Simple subtractive synthesizer with ADSR envelope and filter
- **CleanDelay** - Stereo delay effect with ping-pong mode and tempo sync
- **SimpleEQ** - 3-band parametric EQ with high/low-pass filters
- **SimpleComp** - Compressor with auto makeup gain and soft/hard knee modes
- **WaveShaper** - Waveshaping distortion with 5 curve modes and real-time visualization
- **Saturation** - Multi-mode analog saturation with harmonic enhancement and oversampling
- **Chorus/Flanger** - LFO-based modulation effects with stereo width and real-time visualization
- **Reverb** - Freeverb-inspired algorithmic reverb with freeze mode and stereo width control
- **FreezeFX** - Spectral freezing plugin with phase evolution, dual-mode algorithms, and real-time visualization

### Advanced Effects Suite (v1.2.0) 🆕
- **GranularEngine** - Real-time granular synthesis with 6 window functions, MIDI control, grain direction control, visualization, and preset system
- **SpectralFreeze** - Professional spectral manipulation with freeze/unfreeze, 5 advanced effects (stretch, shift, morph, gate, randomize), 60 FPS spectrogram visualization, parameter smoothing, and 6 factory presets

### Documentation
- **SpectralFreeze**: [User Manual](docs/SpectralFreeze-Manual.md) - Complete 300+ line guide with creative techniques

### Planned (Phase 3+)
- Code signing for macOS and Windows
- Professional installers (.dmg, .exe, .deb, .rpm)
- Video tutorial series
- Additional creative effects

## Tech Stack

- **Framework**: JUCE 7.x
- **Language**: C++17
- **Formats**: VST3, AU (Audio Units for macOS), Standalone (AAX coming later)
- **Platforms**: macOS, Windows, Linux

## Project Structure

```
AudioForge/
├── .github/              # GitHub Actions CI/CD workflows
├── JUCE/                 # JUCE framework (submodule)
├── plugins/              # Individual plugin projects (12 total)
│   ├── SimpleGain/       # ✅ Core Suite (v1.0.0)
│   ├── PanUtil/          # ✅ Core Suite (v1.0.0)
│   ├── BasicSynth/       # ✅ Core Suite (v1.0.0)
│   ├── CleanDelay/       # ✅ Core Suite (v1.0.0)
│   ├── SimpleEQ/         # ✅ Core Suite (v1.0.0)
│   ├── SimpleComp/       # ✅ Core Suite (v1.0.0)
│   ├── WaveShaper/       # ✅ Core Suite (v1.0.0)
│   ├── Saturation/       # ✅ Core Suite (v1.0.0)
│   ├── ChorusFlanger/    # ✅ Core Suite (v1.0.0)
│   ├── Reverb/           # ✅ Core Suite (v1.0.0)
│   ├── FreezeFX/         # ✅ Core Suite (v1.0.0)
│   ├── GranularEngine/   # 🆕 Advanced Suite (v1.2.0)
│   └── SpectralFreeze/   # 🆕 Advanced Suite (v1.2.0)
├── shared/               # Shared DSP library
│   └── dsp/              # Reusable DSP components
├── tests/                # JUCE UnitTestRunner test suite
├── docs/                 # Documentation and user manuals
│   └── SpectralFreeze-Manual.md  # 300+ line user guide
└── installers/           # Platform-specific installers
    ├── macos/            # DMG installer scripts
    ├── windows/          # NSIS installer spec
    └── linux/            # .deb, .rpm, AppImage scripts
```

## What's New in v1.2.0 🎉

### GranularEngine - Real-Time Granular Synthesis
- **6 Window Functions**: Hann, Gaussian, Triangle, Tukey, Blackman, Kaiser
- **Grain Control**: Size (10-500ms), density (1-50 grains/sec), pitch (-24 to +24 semitones)
- **Direction Control**: Forward, backward, pingpong grain playback
- **MIDI Integration**: Velocity modulation for grain density
- **Preset System**: 10 factory presets + quick save/load
- **60 FPS Visualization**: Real-time waveform, window preview, grain activity

### SpectralFreeze - Professional Spectral Manipulation
- **FFT Engine**: 2048-sample FFT with 75% overlap for high-quality spectral processing
- **Core Features**: Freeze/unfreeze, spectral blur, low/high-cut filters, dry/wet mix
- **Advanced Effects**: Stretch, shift, morph, gate, randomize spectral content
- **60 FPS Spectrogram**: Real-time frequency visualization with frozen spectrum overlay
- **6 Factory Presets**: Frozen Blur, Spectral Stretch, Frequency Shifter, and more
- **Parameter Smoothing**: Click-free parameter changes for live performance
- **Full Documentation**: [300+ line user manual](docs/SpectralFreeze-Manual.md) with creative techniques

---

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

Built plugins are automatically installed to your system plugin folders:
- **macOS VST3**: `~/Library/Audio/Plug-Ins/VST3/`
- **macOS AU**: `~/Library/Audio/Plug-Ins/Components/`
- **Windows VST3**: `C:\Program Files\Common Files\VST3\`
- **Linux VST3**: `~/.vst3/`

All AudioForge plugins are available in both VST3 and AU formats on macOS.

### Running Tests

```bash
cd tests
cmake -B build
cmake --build build
./build/TestRunner           # Run all tests
./build/TestRunner -v        # Verbose output
```

Test suite: 136/136 tests passing (100%) | Coverage: See badge above

### Generating Code Coverage

Generate a detailed HTML coverage report locally:

```bash
./generate_coverage.sh       # Run from AudioForge root directory
```

This script will:
1. Clean previous build artifacts
2. Configure CMake with coverage enabled
3. Build and run the full test suite
4. Generate coverage data with lcov
5. Filter out JUCE framework and test code
6. Create an HTML report in `tests/coverage_report/`

Open the report:
```bash
open tests/coverage_report/index.html
```

**Current Coverage Metrics:**
- **Line Coverage**: 46.3% (1,488 / 3,217 lines)
- **Function Coverage**: 90.5% (3,004 / 3,319 functions)
- **Branch Coverage**: 21.4% (840 / 3,930 branches)

Coverage reports are also generated automatically by our CI/CD pipeline on every push and pull request.

## Development Roadmap

### ✅ v1.0.0 - Core Plugin Suite (COMPLETE)
- [x] 11 production plugins (SimpleGain, PanUtil, BasicSynth, CleanDelay, SimpleEQ, SimpleComp, WaveShaper, Saturation, ChorusFlanger, Reverb, FreezeFX)
- [x] VST3 + AU format support
- [x] CI/CD pipeline (macOS, Windows, Linux)
- [x] Testing framework (136/136 tests passing)
- [x] Shared DSP library

### ✅ v1.2.0 - Advanced Effects (COMPLETE)
- [x] GranularEngine - Real-time granular synthesis with MIDI and presets
- [x] SpectralFreeze - Professional spectral manipulation with visualization
- [x] Full documentation (SpectralFreeze user manual)
- [x] Parameter smoothing and tooltips
- [x] 60 FPS real-time visualization

### 🔧 v1.3.0 - Production Distribution (IN PROGRESS)
- [x] GitHub Discussions enabled
- [x] Updated documentation
- [ ] macOS code signing (Developer ID)
- [ ] Windows installer (.exe with code signing)
- [ ] Linux packages (.deb, .rpm, AppImage)
- [ ] Professional installers for all platforms

### 📅 v1.4.0 - Content & Community (PLANNED)
- [ ] Video tutorial series (7 tutorials, ~87 minutes)
- [ ] Plugin showcase videos
- [ ] Community engagement (100+ downloads)
- [ ] User feedback integration
- [ ] Website updates

### 📅 v1.5.0+ - Future Development
- [ ] Additional creative effects
- [ ] AAX format for Pro Tools
- [ ] Preset marketplace
- [ ] Premium plugin tier (optional)

## Learning Resources

Since this is a learning project for DSP, we're documenting our journey:
- `docs/dsp-basics.md` - DSP fundamentals
- `docs/juce-guide.md` - JUCE framework patterns
- Each plugin includes inline comments explaining the algorithms

## Contributing

AudioForge welcomes community contributions! Here's how to get involved:

- **GitHub Discussions**: Ask questions, share ideas, show off your creations
- **Issues**: Report bugs or request features
- **Pull Requests**: Contribute code (see CONTRIBUTING.md for guidelines)
- **User Manuals**: Help improve documentation
- **Presets**: Share your custom presets with the community

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed contribution guidelines.

## Community

- **Discussions**: [GitHub Discussions](https://github.com/kentin0-fiz0l/AudioForge/discussions) - Ask questions, share tips
- **Issues**: [GitHub Issues](https://github.com/kentin0-fiz0l/AudioForge/issues) - Report bugs, request features
- **Website**: [audioforge.fluxstudio.art](https://audioforge.fluxstudio.art) - Downloads and documentation

## License

MIT License - Free for personal and commercial use.

## Acknowledgments

Built with [JUCE](https://juce.com/) - The premier C++ framework for audio applications.
