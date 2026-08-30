# Changelog

All notable changes to AudioForge will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- User manuals (PDF) for all plugins (planned v1.2.0)
- Video tutorials (YouTube) (planned v1.2.0)
- Download analytics tracking (planned v1.3.0)

---

## [1.5.0] - 2026-08-30

### Enhanced BasicSynth - Professional Synthesizer Upgrade 🎹

Major feature release transforming BasicSynth from simple learning tool to professional-grade instrument.

#### Added - Synthesis Engine
- **OscillatorBank**: 5 independent oscillators per voice (up from 1)
  - Per-oscillator level, detune (±12 semitones), and pulse width controls
  - Automatic mixing with normalization to prevent clipping
- **NoiseGenerator**: White and pink noise (Paul Kellett's algorithm)
- **Pulse Waveform**: Adjustable pulse width (5-95% duty cycle)
- **8-voice polyphony**: 40 total oscillators at full capacity

#### Added - Modulation System
- **2 LFOs per voice** with 5 waveform shapes:
  - Sine, Triangle, Sawtooth, Square, Sample & Hold
  - Rate control (0.1-10 Hz)
  - Tempo sync capability
- **4-slot Modulation Matrix** for flexible routing:
  - Sources: LFO1, LFO2, Amp Envelope, Filter Envelope, Velocity, Mod Wheel
  - Destinations: Pitch, Filter Cutoff, PWM, Amplitude, Pan Position
  - Additive modulation (multiple sources can target same destination)

#### Added - Envelope System
- **Independent amp and filter envelopes** (separate ADSR controls)
- **ADSREnvelope class**: Reusable envelope generator
  - Linear attack/release, exponential decay
  - Proper voice deactivation when amp envelope completes
- **Filter envelope**: Faster response (0.5x attack, 0.8x decay, 0.7x release) for snappy filtering

#### Added - Multi-Mode Filter
- **State-variable filter** with 4 filter types:
  - Low-pass (classic warm synth sound)
  - High-pass (remove low frequencies)
  - Band-pass (emphasize middle frequencies)
  - Notch (scooped sound, LP + HP combined)
- **Filter envelope modulation** (±2 octaves)
- **Resonance control** (Q = 0.5-20)
- Smooth transitions between filter types

#### Added - Professional Effects Chain
- **Stereo Chorus**:
  - Modulated delay lines (5-30ms range)
  - LFO-based delay time modulation with 90° phase offset for stereo width
  - Parameters: Rate (0.1-10 Hz), Depth (0-1), Mix (0-1)
- **Algorithmic Reverb**:
  - Schroeder reverb (4 parallel comb filters + 2 series allpass filters)
  - One-pole lowpass damping for high-frequency roll-off
  - Parameters: Room Size (0-1), Damping (0-1), Mix (0-1)
- **Saturation/Distortion**:
  - 3 waveshaping modes: Soft Clip (tanh), Hard Clip (clamp), Tube (asymmetric)
  - Automatic gain compensation
  - Parameters: Drive (0-1 → 1x-10x gain), Mix (0-1), Type (0-2)
- **Signal flow**: Input → Saturation → Chorus → Reverb → Stereo Output

#### Added - Enhanced User Interface
- **Expanded window size** (700x650, up from 500x480)
- **Filter type selector** (ComboBox for LP/HP/BP/Notch)
- **Effects controls**:
  - Chorus Mix slider (purple theme)
  - Reverb Mix slider (blue theme)
  - Saturation Drive slider (red theme)
- **Organized layout** with section headers (ENVELOPE, FILTER, EFFECTS)
- **Color-coded controls** for easy identification

#### Changed
- **Mono to stereo conversion**: BasicSynth now outputs stereo (effects chain)
- **Voice class architecture**: Integrated OscillatorBank, LFOs, ModulationMatrix, ADSREnvelopes
- **Filter implementation**: Replaced simple low-pass with multi-mode state-variable filter

#### Technical Details
- Added 18 new classes (~2,150 lines of C++17 code)
- Modified 6 core files (Voice, Processor, Editor, Oscillator, CMakeLists)
- Sample-accurate envelope processing
- < 3ms total effects latency
- Optimized for real-time performance

#### Files Added
- `OscillatorBank.h/cpp` - Multi-oscillator management
- `NoiseGenerator.h/cpp` - White/pink noise generation
- `LFO.h/cpp` - Low-frequency oscillator
- `ModulationMatrix.h/cpp` - Flexible modulation routing
- `ADSREnvelope.h/cpp` - Reusable envelope generator
- `MultiModeFilter.h/cpp` - State-variable filter
- `Chorus.h/cpp` - Stereo chorus effect
- `Reverb.h/cpp` - Algorithmic reverb
- `Saturation.h/cpp` - Waveshaping distortion
- `EffectsChain.h/cpp` - Effects chain manager

#### Known Limitations
- Oscillator bank, LFO, and modulation matrix use defaults (no per-parameter UI yet)
- Advanced effects parameters accessible via parameter list (not all exposed in UI)
- Preset saving/loading not implemented

#### Future Plans (v1.6.0+)
- Oscillator bank UI (per-oscillator controls)
- LFO shape selector UI
- Visual modulation matrix routing display
- Additional effects (delay, phaser)
- Preset management system

**Release**: [v1.5.0 on GitHub](https://github.com/kentin0-fiz0l/AudioForge/releases/tag/v1.5.0)

---

## [1.1.0] - TBD (In Development)

### Added
- **macOS Code Signing**: Plugins and DMG installer now signed with Developer ID
- **macOS Notarization**: DMG installer notarized by Apple (eliminates Gatekeeper warnings)
- **Windows NSIS Installer**: One-click installer for Windows 10/11
- **Windows Code Signing**: Installer and plugins signed with Authenticode certificate
- **Linux .deb Package**: Ubuntu/Debian package for easy installation
- **Linux .rpm Package**: Fedora/RHEL package for easy installation
- **Linux AppImage**: Universal Linux installer (no dependencies)
- **Installation Guides**: Comprehensive docs for Windows and Linux
- **Community Guidelines**: CONTRIBUTING.md, CODE_OF_CONDUCT.md, SECURITY.md
- **GitHub Issue Templates**: Bug report and feature request templates
- **CHANGELOG.md**: This changelog file

### Changed
- Updated ROADMAP.md to reflect v1.0.0 completion and future plans
- build-installer.sh now supports automatic code signing (macOS)

### Security
- macOS plugins signed with hardened runtime (required for notarization)
- Windows plugins signed with Authenticode (eliminates SmartScreen warnings after reputation builds)
- Security disclosure policy established (SECURITY.md)

### Documentation
- docs/signing-macos.md: Complete guide for Apple Developer setup and notarization
- docs/signing-windows.md: Complete guide for Authenticode signing and SmartScreen
- docs/install-linux.md: Installation guide for all Linux distributions
- docs/install-windows.md: Installation guide for Windows 10/11

---

## [1.0.1] - 2026-08-23

### Fixed
- Version consistency: BasicSynth, CleanDelay, and SimpleEQ now correctly show v1.0.0 (were incorrectly showing v0.1.0)

---

## [1.0.0] - 2026-08-XX (Initial Release)

🎉 **First public release of AudioForge!**

### Added

#### Plugins (10 total)
1. **SimpleGain** - Clean gain/volume control with real-time level meter
2. **PanUtil** - Stereo panning and width control with phase correlation meter
3. **BasicSynth** - Subtractive synthesizer with 3 oscillators (sine/saw/square), ADSR envelope, and low-pass filter
4. **CleanDelay** - Stereo delay with ping-pong mode and tempo sync
5. **SimpleEQ** - 3-band parametric equalizer with high-pass and low-pass filters
6. **SimpleComp** - Compressor with threshold, ratio, attack, release, and automatic makeup gain
7. **WaveShaper** - Waveshaping distortion with multiple curve types
8. **Saturation** - Multi-mode analog-style saturation (tape, tube, transistor)
9. **ChorusFlanger** - LFO-based modulation effects
10. **Reverb** - Freeverb-inspired algorithmic reverb with room size and damping controls

#### Features
- **VST3 Support**: All plugins available as VST3 format
- **AU Support**: macOS plugins available as Audio Units
- **Cross-Platform**: macOS, Windows, and Linux support
- **Free and Open Source**: GPL-3.0 license

#### Distribution
- **macOS DMG Installer**: Automatic installation script for VST3 and AU plugins
- **CI/CD Pipeline**: GitHub Actions building for all platforms
- **Website**: Deployed at https://audioforge.fluxstudio.art

#### Testing
- **136 Tests**: Comprehensive test suite with 100% pass rate
- **Code Coverage**: 46.3% line coverage, 90.5% function coverage
- **Test Types**: Unit tests, integration tests, DSP validation tests

#### Documentation
- **README.md**: Project overview and quick start guide
- **ROADMAP.md**: Development roadmap and future plans
- **docs/dsp-basics.md**: DSP fundamentals guide
- **docs/juce-patterns.md**: JUCE framework patterns guide
- **Website Documentation**: User guides for all 10 plugins

---

## [0.3.0] - 2026-XX-XX (Beta)

### Added
- WaveShaper, Saturation, ChorusFlanger, Reverb plugins
- Automated installer script (macOS)
- CI/CD pipeline with GitHub Actions

---

## [0.2.0] - 2026-XX-XX (Beta)

### Added
- CleanDelay, SimpleEQ, SimpleComp plugins
- Shared DSP library (AudioForgeDSP)
- Integration tests with pytest

---

## [0.1.0] - 2026-XX-XX (Alpha)

### Added
- SimpleGain, PanUtil, BasicSynth plugins
- JUCE framework integration
- CMake build system
- Google Test integration for unit tests

---

## Release Notes Format

### Version Numbers

AudioForge follows [Semantic Versioning](https://semver.org/):

- **Major** (x.0.0): Breaking changes, major new features
- **Minor** (1.x.0): New features, backward-compatible
- **Patch** (1.0.x): Bug fixes, backward-compatible

### Change Categories

- **Added**: New features or plugins
- **Changed**: Changes to existing functionality
- **Deprecated**: Features that will be removed in future versions
- **Removed**: Removed features
- **Fixed**: Bug fixes
- **Security**: Security fixes or improvements

---

## Links

- **Releases**: https://github.com/username/AudioForge/releases
- **Issues**: https://github.com/username/AudioForge/issues
- **Discussions**: https://github.com/username/AudioForge/discussions
- **Website**: https://audioforge.fluxstudio.art

---

[Unreleased]: https://github.com/username/AudioForge/compare/v1.0.1...HEAD
[1.1.0]: https://github.com/username/AudioForge/compare/v1.0.1...v1.1.0
[1.0.1]: https://github.com/username/AudioForge/compare/v1.0.0...v1.0.1
[1.0.0]: https://github.com/username/AudioForge/releases/tag/v1.0.0
