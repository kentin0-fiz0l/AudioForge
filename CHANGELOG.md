# Changelog

All notable changes to AudioForge will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [2.0.0] - 2026-09-11

### 🎉 Major Features

#### Added
- **Universal Preset Management System**
  - Save/load presets for all 39 APVTS-ready plugins
  - Factory presets included with each plugin
  - Category-based organization (Bass, Lead, Pad, FX, etc.)
  - `.afpreset` JSON format (human-readable)
  - User presets saved to `~/Library/Application Support/AudioForge/{Plugin}/Presets/`
  - Preset browser UI component in all supported plugins

- **Universal MIDI Learn**
  - Right-click any parameter → Assign MIDI CC
  - Visual feedback for mapped parameters
  - Mappings saved with project state
  - Works across all plugin types (synths, effects, MIDI tools)
  - Easy removal via context menu

- **Installation Toolkit**
  - `install-all-plugins.sh` - Batch installation with auto-backup
  - `install-plugin.sh` - Single plugin installation
  - `verify-installation.sh` - Installation verification
  - `cleanup-backups.sh` - Backup management
  - `INSTALLATION.md` - Comprehensive installation guide

- **Documentation**
  - Complete installation guide with DAW-specific instructions
  - Release notes with feature breakdown
  - Troubleshooting section
  - Migration guide from v1.x

### Changed
- **UI Layout:** All plugins increased height by 50px for preset browser
- **State Persistence:** Enhanced XML serialization with MIDI Learn + Preset data
- **Parameter Management:** Standardized on APVTS across all supported plugins

### Fixed
- Constructor initialization order issues across multiple plugins
- XML variable naming consistency (xml vs xmlState)
- Missing PresetBrowser includes in 9 plugins
- DrumSynth PresetManager include path

### Technical Details
- **Plugins with Full Support:** 39/51 (APVTS-ready)
- **Shared Libraries:** PresetManager, PresetBrowser, MIDILearnManager
- **Build System:** Mass build script for all 39 plugins
- **Quality Assurance:** Comprehensive build verification

### Breaking Changes
⚠️ **Preset Format:** v1.x simple parameter storage not compatible  
⚠️ **MIDI Mappings:** New system does not migrate v1.x mappings  
⚠️ **UI Size:** Increased by 50px to accommodate preset browser  

### Migration from v1.x
1. Save v1.x parameter settings manually
2. Install v2.0 (automatic backup of existing plugins)
3. Recreate presets using new preset system
4. Remap MIDI controllers using right-click MIDI Learn

---

## [1.0.0] - 2026-08-31

### Initial Release

#### Added
- **51 Audio Plugins** across 3 categories:
  - 17 Synthesizers
  - 26 Effects
  - 8 MIDI Tools

- **Core Features:**
  - VST3 format
  - JUCE 7.x framework
  - Professional UI with AudioForge theme
  - Real-time audio processing
  - Cross-DAW compatibility

- **Plugin Categories:**
  - Synthesizers: FMSynth, Polysynth, ElectricPiano, OrganEmulator, etc.
  - Effects: PlateReverb, VintageCompressor, TapeDelay, Vocoder, etc.
  - MIDI Tools: MIDIArpeggiator, MIDIChordGenerator, MacroController, etc.

- **Build System:**
  - CMake-based build system
  - Individual plugin builds
  - macOS Universal Binary (Intel + Apple Silicon)

#### Technical Specs
- **Platform:** macOS 10.13+
- **Architecture:** Universal Binary
- **Format:** VST3
- **Sample Rates:** 44.1 kHz - 192 kHz
- **Buffer Sizes:** 32 - 2048 samples

---

## [Unreleased]

### In Progress (Phase 4)
- Advanced Modulation System (4 LFOs + 2 ADSRs per plugin)
- Modulation matrix
- MPE (MIDI Polyphonic Expression) support
- Visualization components (spectrum analyzer, oscilloscope)

### Under Consideration
- Windows VST3 builds
- Linux VST3 builds  
- Audio Units (AU) format
- Standalone applications
- Plugin collaboration features

---

## Version History

| Version | Date | Major Changes |
|---------|------|---------------|
| **2.0.0** | 2026-09-11 | Preset Management + MIDI Learn |
| **1.0.0** | 2026-08-31 | Initial release with 51 plugins |

---

## Contributing

This is a personal project, but bug reports and feature suggestions are welcome!

**Reporting Issues:**
- Use GitHub Issues
- Include plugin name and DAW
- Provide steps to reproduce
- Attach crash logs if applicable

**Suggesting Features:**
- Check existing issues first
- Describe use case
- Explain expected behavior

---

## Credits

**Developer:** Kent Ino (jkino.ji@gmail.com)

**Built With:**
- JUCE Framework 7.x
- CMake build system
- VST3 SDK

**Special Thanks:**
- JUCE team
- Audio plugin development community
- Beta testers
- All users providing feedback

---

## License

© 2026 Kent Ino. All rights reserved.

**Usage:**
- ✅ Personal and commercial music production
- ✅ Live performance and broadcast
- ✅ Client work
- ❌ Binary redistribution
- ❌ Reverse engineering

---

For detailed release notes, see `RELEASE_NOTES_v2.0.md`
