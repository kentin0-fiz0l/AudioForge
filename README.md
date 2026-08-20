# AudioForge

Professional-quality DAW plugins, free and open source.

## Overview

AudioForge is a collection of high-quality audio plugins for music production. Starting with essential utilities and effects, we're building a suite of tools that producers actually want to use.

## Current Plugins

### In Development
- **SimpleGain** - Clean gain/volume control with metering (learning project)
- **BasicSynth** - Simple subtractive synthesizer
- **PanUtil** - Stereo panning utility

### Planned
- Reverb effects
- Delay/echo
- EQ
- Compression
- More synths

## Tech Stack

- **Framework**: JUCE 7.x
- **Language**: C++17
- **Formats**: VST3 (AU and AAX coming later)
- **Platforms**: macOS, Windows, Linux

## Project Structure

```
AudioForge/
├── JUCE/                 # JUCE framework (submodule)
├── plugins/              # Individual plugin projects
│   ├── SimpleGain/
│   ├── BasicSynth/
│   └── PanUtil/
├── shared/               # Shared code/utilities
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

## Development Roadmap

### Phase 1: Foundation (Current)
- [x] Project setup
- [ ] JUCE integration
- [ ] SimpleGain plugin (learning DSP basics)
- [ ] Build system and CI
- [ ] Basic documentation

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
