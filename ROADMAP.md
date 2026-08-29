# AudioForge Development Roadmap

**Current Status**: v1.0.1 released with 10 professional plugins, macOS DMG installer, and deployed website

## Completed Phases ✅

### Phase 1: Foundation & Learning (COMPLETED)
Built 3 foundational plugins to learn JUCE and DSP fundamentals:
- ✅ **SimpleGain**: Gain control, level meter, smooth parameters
- ✅ **PanUtil**: Stereo panning, width control, phase correlation
- ✅ **BasicSynth**: 3 oscillators (sine/saw/square), ADSR envelope, polyphony

**What we learned**: Buffer processing, multi-channel audio, MIDI handling, voice management

### Phase 2: Core Plugin Suite (COMPLETED)
Built professional-quality effects:
- ✅ **CleanDelay**: Tempo-synced delay with ping-pong mode
- ✅ **SimpleEQ**: 3-band parametric EQ with visual frequency response
- ✅ **SimpleComp**: Dynamics processor with auto makeup gain
- ✅ **WaveShaper**: Waveshaping distortion with multiple curve types
- ✅ **Saturation**: Analog-style tape/tube saturation
- ✅ **ChorusFlanger**: Modulation effects with LFO control
- ✅ **Reverb**: Algorithmic reverb with room size and damping

**Total**: 10 professional audio plugins

### Phase 3: Distribution & Website (COMPLETED)
Got plugins into users' hands:
- ✅ Website deployed at https://audioforge.fluxstudio.art
- ✅ macOS DMG installer with install/uninstall scripts
- ✅ VST3 + AU format support
- ✅ CI/CD pipeline (GitHub Actions) building for macOS, Windows, Linux
- ✅ Comprehensive test suite (136 tests, 100% pass rate)
- ✅ Code coverage tracking (46.3% line, 90.5% function)
- ✅ User documentation for all plugins on website

**Current Gaps**:
- ⚠️ Plugins are unsigned (macOS Gatekeeper warnings, Windows SmartScreen blocks)
- ⚠️ Windows/Linux have no installers (manual .vst3 file copying required)
- ⚠️ Missing user manuals (PDF), video tutorials, contributing guidelines

---

## Current Focus: v1.1.0 - Professional Distribution

**Goal**: Eliminate security warnings and provide one-click installers for all platforms

**Timeline**: 3 weeks (15-20 hours)

### Code Signing & Notarization
- [ ] Purchase Apple Developer Program membership ($99/year)
- [ ] Generate Developer ID Application certificate
- [ ] Update build-installer.sh to sign all plugins with `codesign`
- [ ] Notarize DMG installer with Apple (`xcrun notarytool`)
- [ ] Test on macOS 10.15+ (Catalina through Sonoma)

### Windows Installer
- [ ] Create NSIS installer script (`installers/windows/audioforge.nsi`)
- [ ] Purchase Windows code signing certificate ($200-400, EV recommended)
- [ ] Sign plugins and installer executable with `signtool`
- [ ] Test on Windows 10/11 (fresh VM installs)

### Linux Packages
- [ ] Create .deb package for Ubuntu/Debian
- [ ] Create .rpm package for Fedora/RHEL
- [ ] Create AppImage for universal Linux distribution
- [ ] Test on Ubuntu 22.04/24.04 and Fedora 39/40

**Success Criteria**:
- macOS users install with zero security warnings
- Windows users install via .exe without SmartScreen blocks (if EV cert)
- Linux users can install via package manager or AppImage
- Download page shows installers for all platforms

**Required Investment**: $300-500/year (Apple Developer + Windows cert)

---

## v1.2.0 - Documentation & Community

**Goal**: Provide comprehensive documentation and build community infrastructure

**Timeline**: 2 weeks (10-15 hours)

### User Manuals (PDF)
- [ ] Create user manual template (PDF format)
- [ ] Write manuals for all 10 plugins covering:
  - Plugin overview
  - Parameter descriptions
  - Use cases and production tips
  - Troubleshooting
- [ ] Add manual downloads to website
- [ ] Generate EPUB versions for accessibility

### Video Tutorials
- [ ] Record 3-5 video tutorials:
  1. Installation guide (all platforms)
  2. SimpleGain walkthrough
  3. BasicSynth tutorial
  4. SimpleComp usage
  5. Full plugin suite overview (optional)
- [ ] Edit and publish to YouTube
- [ ] Embed videos on website /videos page
- [ ] Create YouTube playlist

### Platform Guides & Contributing
- [ ] Write `docs/install-windows.md` (Windows 10/11 installation)
- [ ] Write `docs/install-linux.md` (Ubuntu, Fedora, AppImage instructions)
- [ ] Create `CONTRIBUTING.md` (development setup, coding standards, PR process)
- [ ] Add plugin screenshots to website (5 per plugin, 50 total)

**Success Criteria**:
- User manual PDF available for all 10 plugins
- 3-5 video tutorials published and embedded on website
- Windows and Linux installation guides accessible on website
- CONTRIBUTING.md encourages external contributors

---

## v1.3.0 - Growth Foundation

**Goal**: Establish infrastructure for community engagement and user feedback

**Timeline**: 1 week (5-10 hours)

### Community Infrastructure
- [ ] Create `CHANGELOG.md` with v1.0.0-v1.3.0 history
- [ ] Enable GitHub Discussions for community Q&A
- [ ] Create issue templates:
  - `.github/ISSUE_TEMPLATE/bug_report.md`
  - `.github/ISSUE_TEMPLATE/feature_request.md`
- [ ] Add `CODE_OF_CONDUCT.md` (community guidelines)
- [ ] Add `SECURITY.md` (security disclosure policy)

### Analytics & Tracking
- [ ] Set up privacy-respecting download analytics (Plausible or Umami)
- [ ] Track platform distribution (macOS/Windows/Linux)
- [ ] Add download counters to website
- [ ] Create analytics dashboard

**Success Criteria**:
- CHANGELOG.md exists with version history
- GitHub Discussions enabled for community Q&A
- Download tracking shows platform distribution
- Bug report and feature request templates active
- At least 3 external users engage (issues, discussions, PRs)

---

## v1.4.0 - Bug Fixes & Polish

**Goal**: Address user feedback from v1.1.0 and improve stability

**Timeline**: 2 weeks (10-15 hours)

### Bug Fixes & Improvements
- [ ] Fix any critical bugs reported by users
- [ ] Performance optimization based on profiling
- [ ] UI/UX improvements from user feedback
- [ ] DAW compatibility fixes (if needed)
- [ ] Update documentation based on common questions

### Preset Management System
- [ ] Implement preset save/load functionality
- [ ] Create default preset banks for all plugins
- [ ] Add preset browser UI
- [ ] Export/import preset packs

### MIDI Learn
- [ ] Right-click parameter → "Learn MIDI CC"
- [ ] MIDI CC mapping persistence
- [ ] MIDI automation support in all plugins

**Success Criteria**:
- Zero critical bugs from v1.1.0
- Preset system working in all plugins
- MIDI learn functional across all parameters

---

## v1.5.0 - Enhanced BasicSynth 🎹

**Goal**: Expand BasicSynth into a powerful subtractive synthesizer

**Timeline**: 1-2 weeks (8-12 hours)

### New Features
- [ ] **Additional Oscillators**:
  - Add 2 more oscillators (5 total)
  - Pulse width modulation (PWM)
  - Noise generator
  - Sub-oscillator (-1 octave)

- [ ] **Enhanced Filters**:
  - Additional filter types (notch, band-pass, high-pass 24dB)
  - Filter envelope (dedicated ADSR)
  - Filter keyboard tracking
  - Resonance self-oscillation

- [ ] **Modulation**:
  - 2 LFOs (rate, depth, shape: sine/saw/square/random)
  - LFO sync to tempo
  - Modulation matrix (4 slots)
  - Velocity sensitivity per parameter

- [ ] **Effects**:
  - Built-in chorus/delay/reverb
  - Distortion/saturation stage
  - Effect send levels

### Technical Implementation
- Reuse existing BasicSynth voice architecture
- Add modulation routing system
- Implement filter envelope generator
- Create effect processing chain

**Success Criteria**:
- 5 oscillators with PWM and noise
- Multi-mode filters with envelope
- 2 LFOs with tempo sync
- Built-in effects chain

---

## v1.6.0 - Wavetable Synthesizer 🌊

**Goal**: Modern wavetable synth for digital/EDM production

**Timeline**: 2-3 weeks (15-20 hours)

### Core Features
- [ ] **Wavetable Engine**:
  - 50+ built-in wavetables
  - Wavetable morphing (position parameter)
  - User wavetable import (WAV files)
  - 2 wavetable oscillators per voice

- [ ] **Synthesis**:
  - Unison mode (up to 7 voices per oscillator)
  - Detune and spread controls
  - Phase modulation / FM
  - Ring modulation

- [ ] **Modulation System**:
  - 4 LFOs (one-shot, sync, free-running)
  - 3 envelopes (amp, filter, mod)
  - 8-slot modulation matrix
  - Macro controls (4 knobs)

- [ ] **Effects**:
  - Multi-mode filter (LP/HP/BP/Notch, 12/24dB)
  - Built-in chorus, phaser, delay, reverb
  - Distortion and bit crushing

### Technical Implementation
- Wavetable rendering engine (interpolation)
- Voice allocation and unison
- Modulation routing system
- Preset management for wavetables

**Success Criteria**:
- 50+ wavetables with morphing
- Unison mode with 7 voices
- 8-slot modulation matrix
- Professional wavetable import

---

## v1.7.0 - FM Synthesizer 🔔

**Goal**: Classic FM synthesis (DX7-style) for bells, bass, pads

**Timeline**: 2-3 weeks (15-20 hours)

### Core Features
- [ ] **FM Engine**:
  - 6 operators (sine wave generators)
  - 32 algorithms (operator routing configurations)
  - Feedback per operator
  - Ratio and fine-tune controls

- [ ] **Modulation**:
  - Operator envelopes (4-stage: ADSR)
  - Velocity sensitivity per operator
  - LFO modulation (pitch, amp, filter)
  - Keyboard scaling (level, rate)

- [ ] **Sound Design**:
  - Algorithm visualization
  - Operator level controls
  - Ratio coarse/fine tuning
  - Fixed frequency mode

- [ ] **Effects**:
  - Filter (LP/HP)
  - Chorus and reverb
  - Distortion

### Technical Implementation
- Operator synthesis engine
- Algorithm routing matrix
- Envelope generator (4-stage)
- Preset library (100+ classic FM sounds)

**Success Criteria**:
- 6 operators with 32 algorithms
- Classic DX7-style sound
- 100+ preset library
- Algorithm visualization

---

## v1.8.0 - Sample-Based Instrument 🎵

**Goal**: Versatile sampler for loading WAV/AIFF files

**Timeline**: 2-3 weeks (15-20 hours)

### Core Features
- [ ] **Sample Engine**:
  - Load WAV/AIFF files (mono/stereo)
  - Multi-sample support (up to 128 samples)
  - Key mapping and velocity layers
  - Sample playback modes (one-shot, loop, reverse)

- [ ] **Synthesis**:
  - Pitch/time stretching
  - Sample start/end points
  - Loop crossfading
  - Pitch envelope

- [ ] **Modulation**:
  - Filter (LP/HP/BP, resonance)
  - Amp envelope (ADSR)
  - Filter envelope (ADSR)
  - 2 LFOs (pitch, filter, amp)

- [ ] **Key Mapping**:
  - Drag-and-drop sample assignment
  - Auto-map samples across keyboard
  - Velocity layer switching
  - Round-robin sample playback

### Technical Implementation
- AudioFormatReader integration (JUCE)
- Sample buffer management
- Key/velocity zone mapping
- Time-stretching algorithm

**Success Criteria**:
- Load and play WAV/AIFF files
- Multi-sample key mapping
- Velocity layers (4+ layers)
- Loop crossfading

---

## v1.9.0 - Granular Synthesizer ✨

**Goal**: Texture-based synthesis for experimental/ambient sounds

**Timeline**: 3-4 weeks (20-25 hours)

### Core Features
- [ ] **Granular Engine**:
  - Load audio files or use built-in wavetables
  - Grain size (1ms - 500ms)
  - Grain density (grains per second)
  - Grain playback position (random, sequential, scan)

- [ ] **Grain Parameters**:
  - Pitch randomization (±semitones)
  - Pan randomization (stereo spread)
  - Envelope shape (Gaussian, triangle, Hann window)
  - Reverse grains probability

- [ ] **Modulation**:
  - Scan position automation (LFO, envelope)
  - Density modulation
  - Pitch modulation
  - Filter per grain

- [ ] **Effects**:
  - Built-in freeze mode (infinite grain loop)
  - Grain delay (feedback)
  - Reverb and chorus

### Technical Implementation
- Grain scheduler and playback engine
- Windowing functions (overlap-add)
- Random parameter generation
- Real-time grain synthesis

**Success Criteria**:
- Smooth grain playback (100+ grains/sec)
- Freeze mode for infinite textures
- Scan position automation
- Low CPU usage (<10% on modern CPU)

---

## v1.10.0 - Physical Modeling Synthesizer 🎻

**Goal**: Realistic acoustic instrument simulation (strings, winds)

**Timeline**: 3-4 weeks (20-25 hours)

### Core Features
- [ ] **Physical Models**:
  - Plucked string (Karplus-Strong algorithm)
  - Bowed string (friction model)
  - Blown pipe (flute, clarinet models)
  - Struck string (piano-like)

- [ ] **Exciter Parameters**:
  - Pluck/bow position
  - Pluck/bow hardness
  - Breath pressure (for winds)
  - Strike velocity

- [ ] **Resonator Parameters**:
  - String/tube length (pitch)
  - Damping (sustain)
  - Stiffness (timbre)
  - Body resonance

- [ ] **Modulation**:
  - Vibrato (pitch LFO)
  - Tremolo (amp LFO)
  - Pressure envelope (dynamics)
  - Expression control (MIDI CC)

### Technical Implementation
- Waveguide synthesis (delay lines)
- Non-linear filters (for realistic behavior)
- Excitation models (impulse, noise, periodic)
- Resonator models (comb filter, all-pass)

**Success Criteria**:
- 4 physical models (string, bow, wind, strike)
- Realistic acoustic sound
- Expressive performance (velocity, aftertouch)
- Low latency (<10ms)

---

## Long-Term Vision: Comprehensive Synthesis Suite

**Total Instruments**: 16 plugins (10 current + 6 new)

### Free Tier (永久免费):
- SimpleGain, PanUtil, BasicSynth (v1.0)
- CleanDelay, SimpleEQ, SimpleComp
- WaveShaper, Saturation, ChorusFlanger, Reverb

### Advanced Tier (Future consideration):
- Enhanced BasicSynth (v1.5.0)
- Wavetable Synthesizer (v1.6.0)
- FM Synthesizer (v1.7.0)
- Sample-Based Instrument (v1.8.0)
- Granular Synthesizer (v1.9.0)
- Physical Modeling (v1.10.0)

**Note**: All plugins remain open source (GPL-3.0). Advanced tier refers to potential future premium support, presets, or expansion packs.

---

## Post-v1.10.0: Platform & Monetization

### Platform Expansion
- AAX format for Pro Tools (~$300 iLok Developer account)
- iOS/iPadOS AUv3 plugins
- Web Audio / WASM plugins

### Monetization Options
1. **Freemium**: Free basic plugins, paid advanced ones ($29-49 each)
2. **Preset Packs**: Expansion libraries ($9-19 per pack)
3. **Patreon/Sponsors**: Ongoing support tiers ($5-50/month)
4. **Pay What You Want**: Free download, optional donation

### Community Growth
- Discord server for real-time support
- Monthly development livestreams (Twitch/YouTube)
- Plugin showcases from users
- Blog posts on DSP techniques
- Outreach to music production communities

---

## Technical Milestones

### Build System ✅
- ✅ CMake configuration for JUCE
- ✅ VST3 builds
- ✅ AU (Audio Units) support
- ✅ CI/CD pipeline (GitHub Actions)
- ✅ Automated testing (136 tests, 100% pass rate)
- ⬜ AAX (Pro Tools) support (Phase 4 Option B)

### Code Quality ✅
- ✅ Unit tests for DSP algorithms
- ✅ Integration tests
- ✅ Code coverage reporting (46.3% line, 90.5% function)
- ✅ Static analysis (cppcheck)
- ⬜ Performance profiling (Phase 4 optimization)

### Documentation 🔄
- ✅ DSP basics guide
- ✅ JUCE patterns guide
- ✅ Website plugin documentation
- ⬜ User manuals (PDF) - v1.2.0
- ⬜ Contributing guidelines - v1.2.0
- ⬜ API documentation (Doxygen) - Phase 4

---

## Success Metrics

### v1.0.0 (COMPLETED) ✅
- ✅ 10 working professional plugins
- ✅ Website deployed with documentation
- ✅ macOS DMG installer
- ✅ CI/CD pipeline building for 3 platforms
- ✅ 100% test pass rate

### v1.1.0 (In Progress)
- [ ] macOS users install with zero security warnings
- [ ] Windows users install via .exe without SmartScreen blocks
- [ ] Linux users can install via package manager
- [ ] All 10 plugins show consistent v1.0.0 version

### v1.2.0 (Planned)
- [ ] User manual PDF available for all 10 plugins
- [ ] 3-5 video tutorials published on YouTube
- [ ] Windows and Linux installation guides on website
- [ ] Plugin screenshots (5 per plugin) visible on website

### v1.3.0 (Planned)
- [ ] CHANGELOG.md with version history
- [ ] GitHub Discussions enabled
- [ ] Download tracking shows platform distribution
- [ ] At least 3 external contributors engaged

### Overall Success (Post v1.3.0)
- [ ] 100+ total downloads across all platforms
- [ ] Platform distribution: 60% macOS, 30% Windows, 10% Linux
- [ ] Zero unresolved security/signing issues
- [ ] 5+ GitHub stars, 3+ contributors
- [ ] Positive user feedback on installation experience

---

## Resources Needed

### Already Acquired ✅
- ✅ JUCE framework (free, GPL3 or indie license)
- ✅ CI/CD via GitHub Actions (free for public repos)
- ✅ Web hosting (Netlify free tier)
- ✅ Development tools (CMake, Git, VS Code)

### Needed for v1.1.0 ($300-500/year)
- [ ] Apple Developer Program membership - $99/year
- [ ] Windows code signing certificate - $200-400/year
  - Standard certificate: ~$200 (SmartScreen warnings on first run)
  - EV certificate: ~$400 (instant SmartScreen trust, recommended)

### Optional for Future Phases
- AAX (Pro Tools) support: ~$300 iLok Developer account
- Video recording/editing software: OBS Studio (free), DaVinci Resolve (free)
- Analytics platform: Plausible (~$9/month) or Umami (self-hosted, free)

---

## Timeline Summary

| Version | Focus | Timeline | Investment |
|---------|-------|----------|------------|
| v1.0.0 | 10 plugins + website | COMPLETED ✅ | Time only |
| v1.0.1 | Version fixes | COMPLETED ✅ | Time only |
| v1.1.0 | Code signing + installers | 3 weeks (15-20 hrs) | $300-500/year |
| v1.1.1 | Windows + Linux builds | 2 weeks (10-15 hrs) | Time only |
| v1.2.0 | Documentation + videos | 2 weeks (10-15 hrs) | Time only |
| v1.3.0 | Community infrastructure | 1 week (5-10 hrs) | Time only |
| v1.4.0 | Bug fixes + presets + MIDI | 2 weeks (10-15 hrs) | Time only |
| v1.5.0 | Enhanced BasicSynth | 1-2 weeks (8-12 hrs) | Time only |
| v1.6.0 | Wavetable Synthesizer | 2-3 weeks (15-20 hrs) | Time only |
| v1.7.0 | FM Synthesizer | 2-3 weeks (15-20 hrs) | Time only |
| v1.8.0 | Sample-Based Instrument | 2-3 weeks (15-20 hrs) | Time only |
| v1.9.0 | Granular Synthesizer | 3-4 weeks (20-25 hrs) | Time only |
| v1.10.0 | Physical Modeling | 3-4 weeks (20-25 hrs) | Time only |
| **Phase 1** | **Foundation (v1.0-v1.3)** | **8 weeks (40-50 hrs)** | **$300-500** |
| **Phase 2** | **Instrument Suite (v1.4-v1.10)** | **18-23 weeks (100-135 hrs)** | **Time only** |
| **Total** | **16-plugin synthesis suite** | **26-31 weeks (140-185 hrs)** | **$300-500** |

**Long-term investment**: ~6-7 months of development for comprehensive synthesis suite

---

## Contributing

Want to help shape AudioForge's future? Here's how:

1. **Try the plugins** and report bugs via GitHub Issues
2. **Share feedback** on what features/plugins you'd like to see
3. **Join discussions** about roadmap priorities
4. **Contribute code** once CONTRIBUTING.md is available (v1.2.0)

See [CLAUDE.md](CLAUDE.md) for development setup and project structure.

---

**Last Updated**: 2026-08-27 (v1.1.1 in progress, comprehensive instrument suite planned)
