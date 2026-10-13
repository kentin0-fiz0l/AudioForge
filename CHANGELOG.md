# Changelog

All notable changes to AudioForge will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### In Progress (Phase 4)
- Advanced Modulation System (4 LFOs + 2 ADSRs per plugin)
- Modulation matrix
- MPE (MIDI Polyphonic Expression) support
- Visualization components (spectrum analyzer, oscilloscope)

### Under Consideration
- Plugin collaboration features

---

## [2.1.0] - 2026-10-13

### Highlights
- **Installers for macOS, Windows and Linux.** Every release now carries a macOS `.pkg`
  (universal: Apple Silicon and Intel, macOS 11+), a Windows `Setup.exe` and a Linux
  `.tar.gz` with `install.sh`, plus `.zip` versions to copy by hand. All 60 plugins: VST3
  everywhere, AU on macOS, and standalone apps for 53 of them. The installers are not
  code-signed, so macOS asks for "Open Anyway" and Windows for "Run anyway" once.
- **Every instrument at the same level.** The instruments were up to 24 dB apart; they are
  now calibrated to one output level, and effects no longer change the level when loaded.
- **AI Producer for Ableton Live** grew from a track generator into a way to drive a Live
  set from outside: mixer and devices, clips and warping, the Arrangement, locators and
  playback. Its bass now follows the chords.

### Added
- DrumSynth: an open hi-hat and a clap, so it covers a whole house or techno kit, and it
  now follows velocity and timing.
- PanUtil is now a track utility: gain and mono alongside pan and width.
- BasicSynth: bass, pad and lead patches.
- ElectricPiano and Polysynth: an Output Level control.
- AI Producer (`ableton/remote-scripts/AIProducer`, see its README):
  - control Live's mixer, devices and meters from outside, and record listening feedback
    with what Live was doing at the time
  - read clips, set their warping and markers, copy them to the Arrangement and clear it
  - place, list and jump to locators, and move the playhead
  - load instruments it can count on in Live Intro

### Changed
- **Some sessions will sound different:**
  - instrument and effect levels (see Highlights)
  - PhaserFlanger's phaser mode is now a real phaser with feedback
  - BrassSection's Fall-off falls on release
  - GranularEngine: its Time Stretch control is now Scan Speed (saved values are kept), its
    default level went from -28 dB to -4 dB, and its grains spread across the stereo field
- Four plugins have plain names in your DAW's list instead of "AudioForge - Name".
- CleanDelay shows Feedback and Mix as percentages.

### Fixed
- Plugins that were silent or nearly silent: HiHat, FreezeFX's Freeze and SpectralFreeze.
- FreezeFX now holds the level and pitch it froze.
- Limiter: it went over its ceiling, and its lookahead did nothing.
- PlateReverb doubled its input.
- BrassSection produced invalid output on loud Marcato notes.
- 18 plugins read saved settings without checking them.
- Controls that showed the defaults instead of the real settings in CleanDelay, SimpleGain,
  SpectralFreeze, GranularEngine, BasicSynth and PanUtil.
- DrumSynth: a choked hi-hat fades out instead of stepping down.
- GranularEngine's Reverse now plays grains backwards.
- Layout overlaps in PanUtil and GranularEngine.
- Windows builds of OrganEmulator, PadSynth and WavetableSynth.
- AI Producer: Live could not load or trigger it; it now follows Live's transport; clips Live
  warps again after analysing them are kept unwarped; locators off Live's grid no longer
  loop forever.

### Technical Details
- CI builds every plugin on macOS, Windows and Linux, and runs each through a stress host at
  extreme settings and a level check; pull requests build only what they change.
- Releases are built by `.github/workflows/release.yml`; see `DEPLOYMENT.md`.

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

## Version History

| Version | Date | Major Changes |
|---------|------|---------------|
| **2.1.0** | 2026-10-13 | Installers for macOS, Windows and Linux; calibrated levels; AI Producer drives Live |
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
