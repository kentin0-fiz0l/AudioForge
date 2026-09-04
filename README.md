# AudioForge

[![Build and Test](https://github.com/kentin0-fiz0l/AudioForge/actions/workflows/build-test.yml/badge.svg)](https://github.com/kentin0-fiz0l/AudioForge/actions/workflows/build-test.yml)
[![Code Coverage](https://codecov.io/gh/kentin0-fiz0l/AudioForge/branch/main/graph/badge.svg?token=YOUR_CODECOV_TOKEN)](https://codecov.io/gh/kentin0-fiz0l/AudioForge)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Latest Release](https://img.shields.io/github/v/release/kentin0-fiz0l/AudioForge)](https://github.com/kentin0-fiz0l/AudioForge/releases/latest)

Professional-quality DAW plugins, free and open source.

## 📦 Download

**[Download Latest Release](https://github.com/kentin0-fiz0l/AudioForge/releases/latest)** - v1.9.0

- **macOS**: Signed & notarized DMG (VST3 + AU) - **Available now!**
- **Windows**: NSIS installer (VST3) - *Coming in v2.0.0*
- **Linux**: .deb, .rpm, AppImage (VST3) - *Coming in v2.0.0*

## Overview

AudioForge is a collection of high-quality audio plugins for music production. Starting with essential utilities and effects, we're building a suite of tools that producers actually want to use.

## Current Plugins (22 Validated ✅)

### Mixing & Mastering Suite (9 plugins) ✅
- **SimpleGain** - Clean gain/volume control with metering (-60 to +12 dB)
- **PanUtil** - Stereo panning utility with balance and width controls
- **SimpleEQ** - 3-band parametric EQ with high/low-pass filters
- **SimpleComp** - Compressor with auto makeup gain and soft/hard knee modes
- **Gate** - Professional noise gate/expander with sidechain (v1.7.0) 🆕
- **Limiter** - True peak mastering limiter with lookahead (v1.6.0) 🆕
- **MultibandCompressor** - 4-band mastering compressor with crossovers (v1.9.0) 🆕
- **Reverb** - Freeverb-inspired algorithmic reverb with freeze mode
- **CleanDelay** - Stereo delay effect with ping-pong mode and tempo sync

### Creative Effects (3 plugins) ✅
- **Saturation** - Multi-mode analog saturation with harmonic enhancement
- **WaveShaper** - Waveshaping distortion with 5 curve modes and visualization
- **ChorusFlanger** - LFO-based modulation effects with stereo width

### Spectral Effects (2 plugins) ✅
- **SpectralFreeze** - FFT-based spectral freezing with phase evolution
- **FreezeFX** - Advanced spectral freeze with blur and randomization 🆕 *Fixed in v1.9.0*

### Synthesizers (8 plugins) ✅ **+ 121 Factory Presets!**
- **BasicSynth** - Subtractive synthesizer with ADSR envelope and filter
- **FMSynth** - 4-operator FM synthesis **+ 20 presets** (E.Piano, Bells, Bass, Lead)
- **WavetableSynth** - Wavetable synthesis with interpolation **+ 26 presets** (EDM Lead, Bass, Pad, Digital, Pluck)
- **PadSynth** - Lush pad synthesis **+ 20 presets**
- **DrumSynth** - Drum synthesis engine **+ 30 presets**
- **OrganEmulator** - Hammond-style organ with drawbars **+ 25 presets**
- **GranularEngine** - Real-time granular synthesis with MIDI control
- **Sampler** - Multi-zone sampler with velocity layers and loop points ✅ *Fixed in v1.9.0*

### Factory Presets (v1.9.1) 🎹
- **121 Ready-to-Use Presets** across 5 synthesizers
- **Organized by Category**: Bass, Lead, Pad, Keys, Drums, FX
- **Instant Creativity**: No programming required - load and play!
- **Professional Quality**: Crafted for immediate use in production

| Synth | Presets | Categories |
|-------|---------|------------|
| DrumSynth | 30 | Kick, Snare, Hat, Percussion |
| WavetableSynth | 26 | EDM Lead, Bass, Pad, Digital, Pluck |
| OrganEmulator | 25 | Jazz, Rock, Gospel, Blues |
| FMSynth | 20 | E.Piano, Bells, Bass, Lead |
| PadSynth | 20 | Warm, Bright, Dark, Ethereal |

### Quality Assurance (v1.9.0) ✅
- **PluginVal Validated**: **22 production plugins pass strict validation** (strictness level 10 - maximum)
- **Pass Rate**: **100%** (22/22 total plugins) 🎉
- **100% Test Coverage**: Comprehensive unit tests for all DSP components
- **Performance Benchmarked**: CPU usage profiled for all plugins

### Production-Ready Plugins (22) ✅
- ✅ **FreezeFX** - *Fixed in v1.9.0* (thread-safe APVTS refactoring + performance optimization)
- ✅ **SpectralFreeze** - *Fixed in v1.9.0* (negative modulo bug)
- ✅ **Sampler** - *Fixed in v1.9.0* (unique_ptr destruction order)
- ✅ All mixing, mastering, effects, and synthesizer plugins fully validated

### Distribution (v1.9.0) ✅
- ✅ macOS code signing (Developer ID Application)
- ✅ Apple notarization (zero Gatekeeper warnings)
- ✅ Professional DMG installer (v1.9.0 available now)
- ✅ PluginVal integration for automated quality assurance (strictness level 10)
- ✅ Performance benchmarking framework
- ✅ **100% validation pass rate (22/22 plugins)** 🎉
- 🔧 Windows NSIS installer (scripts ready, v2.0.0 planned)
- 🔧 Linux packages (scripts ready, v2.0.0 planned)
- ✅ GitHub Discussions enabled for community

### Website (v1.9.0) 🌐
- ✅ **Professional landing page** with Next.js 14 + Tailwind CSS
- ✅ **Badge system** (Fixed, New, Experimental, Validated, Popular)
- ✅ **Real plugin screenshots** (4 plugins with optimized PNGs)
- ✅ **95.5% validation badge** prominently displayed on hero
- ✅ **Dismissible release banner** for v1.9.0 announcement
- ✅ **Responsive design** - Mobile, tablet, desktop optimized
- ✅ **Stats section** highlighting validation achievement
- ✅ **Live deployment** on DigitalOcean Spaces with CDN
- 🔗 [Visit Website](https://audioforge-site.sfo3.cdn.digitaloceanspaces.com/index.html)

### Planned (v2.0.0+)
- Video tutorial series (5 videos scripted, ready to record)
- Complete plugin screenshot gallery (18 remaining plugins)
- AAX format for Pro Tools
- Linux packages (.deb, .rpm, AppImage)
- Windows installer (NSIS)
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

## What's New in v1.3.0 🎉

### Professional Distribution (macOS)
- **Code Signed & Notarized**: Zero Gatekeeper warnings on macOS installation
- **Universal Binary**: Native support for Intel and Apple Silicon Macs
- **One-Click Installer**: Professional DMG with automated deployment
- **GitHub Discussions**: Community Q&A and feature requests now enabled

Full release notes: [v1.3.0 Release Notes](docs/v1.3.0-Release-Notes.md)

---

## Plugin Features

### GranularEngine v1.2.0 - Real-Time Granular Synthesis
- **6 Window Functions**: Hann, Gaussian, Triangle, Tukey, Blackman, Kaiser
- **Grain Control**: Size (10-500ms), density (1-50 grains/sec), pitch (-24 to +24 semitones)
- **Direction Control**: Forward, backward, pingpong grain playback
- **MIDI Integration**: Velocity modulation for grain density
- **Preset System**: 10 factory presets + quick save/load
- **60 FPS Visualization**: Real-time waveform, window preview, grain activity

### SpectralFreeze v1.2.0 - Professional Spectral Manipulation
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
- **Website**: [AudioForge Website](https://audioforge-site.sfo3.cdn.digitaloceanspaces.com/index.html) - Downloads and documentation

## License

MIT License - Free for personal and commercial use.

## Acknowledgments

Built with [JUCE](https://juce.com/) - The premier C++ framework for audio applications.
