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

## Post-v1.3.0: Future Directions

Once v1.1.0-v1.3.0 is complete, AudioForge will have a solid professional foundation. Next steps depend on user feedback and project goals:

### Option A: New Plugin Development (Phase 4)
**Focus**: Expand the plugin suite with advanced creative tools

**Potential Plugins**:
- Granular synthesis plugin (texture/grain manipulation)
- Spectral gate/compressor (frequency-based dynamics)
- Creative modulation (ring mod, bit crusher, sample rate reduction)
- Wavetable synthesizer (modern digital synth)
- Convolution reverb (impulse response loading)

**Timeline**: 3-4 months (1 plugin per month)
**Investment**: Time only (no additional costs)

### Option B: Platform Expansion
**Focus**: Reach more users via additional formats and platforms

**Potential Platforms**:
- AAX format for Pro Tools (requires iLok Developer account ~$300)
- iOS/iPadOS AUv3 plugins (requires Apple Developer + iOS app development)
- Web Audio plugins via WebAssembly (experimental, browser-based)

**Timeline**: 2-3 months per platform
**Investment**: $300-500 for AAX, $99/year for iOS

### Option C: Monetization Preparation (Phase 5)
**Focus**: Build revenue streams while keeping core plugins free

**Revenue Models**:
1. **Freemium**: Free basic plugins, paid advanced ones ($29-49 per premium plugin)
2. **Preset Packs**: Expansion libraries ($9-19 per pack)
3. **Patreon/Sponsors**: Ongoing support tiers ($5-50/month)
4. **Pay What You Want**: Free download, optional donation

**Free Tier** (永久免费):
- SimpleGain, PanUtil, BasicSynth
- CleanDelay, SimpleEQ, SimpleComp
- All current 10 plugins remain free

**Premium Tier** (Future plugins only):
- Advanced synths (wavetable, granular)
- Specialized effects (spectral tools, convolution reverb)
- Professional mastering suite

**Timeline**: 1-2 months setup (pricing, payment processing, licensing)
**Investment**: Payment processor fees (Stripe 2.9% + $0.30)

### Option D: Community Growth & Marketing
**Focus**: Build user base and developer community

**Initiatives**:
- Discord server for real-time support and community
- Monthly development livestreams (Twitch/YouTube)
- Plugin showcases from users (social proof, demos)
- Blog posts on DSP techniques (SEO, educational content)
- Outreach to music production communities (Reddit, forums)

**Timeline**: Ongoing (2-4 hours/week)
**Investment**: Time only

**Recommended**: After v1.3.0 release, collect user feedback via GitHub Discussions and surveys. Let the community vote on what comes next.

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
| v1.0.0 | 10 plugins + website | COMPLETED | Time only |
| v1.0.1 | Version fixes | COMPLETED | Time only |
| v1.1.0 | Code signing + installers | 3 weeks (15-20 hrs) | $300-500/year |
| v1.2.0 | Documentation + videos | 2 weeks (10-15 hrs) | Time only |
| v1.3.0 | Community infrastructure | 1 week (5-10 hrs) | Time only |
| **Total** | **Professional foundation** | **6 weeks (30-45 hrs)** | **$300-500** |

**Post-v1.3.0**: Community feedback determines next direction (new plugins, platforms, monetization, or marketing)

---

## Contributing

Want to help shape AudioForge's future? Here's how:

1. **Try the plugins** and report bugs via GitHub Issues
2. **Share feedback** on what features/plugins you'd like to see
3. **Join discussions** about roadmap priorities
4. **Contribute code** once CONTRIBUTING.md is available (v1.2.0)

See [CLAUDE.md](CLAUDE.md) for development setup and project structure.

---

**Last Updated**: 2026-08-23 (v1.0.1)
