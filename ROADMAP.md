# AudioForge Development Roadmap

## Phase 1: Foundation & Learning (Current)

**Goal**: Build 3 simple plugins to learn JUCE and DSP fundamentals

### Plugin 1: SimpleGain ✅ COMPLETE
- [x] Gain control (-60 to +12 dB)
- [x] Real-time level meter
- [x] Smooth parameter changes
- [x] Clean rotary knob UI
- [x] VST3 + Standalone builds

**What we learned**: Buffer processing, dB conversion, parameter smoothing, basic GUI

### Plugin 2: PanUtil (Next)
**Timeline**: 1-2 weeks

Features:
- [ ] Stereo pan control (-100% L to +100% R)
- [ ] Constant-power panning algorithm
- [ ] Balance mode vs Pan mode
- [ ] Width control (stereo widening/narrowing)
- [ ] Dual meters (L/R)
- [ ] Phase correlation meter

**Learning focus**: Multi-channel processing, stereo imaging, phase relationships

### Plugin 3: BasicSynth
**Timeline**: 2-3 weeks

Features:
- [ ] 3 oscillator waveforms (sine, saw, square)
- [ ] ADSR envelope
- [ ] Low-pass filter with resonance
- [ ] MIDI note handling
- [ ] Polyphony (4-8 voices)
- [ ] Simple visual keyboard

**Learning focus**: MIDI processing, oscillators, filters, voice management

## Phase 2: Core Plugin Suite

**Goal**: Build useful, professional-quality effects

### Plugin 4: CleanDelay
- [ ] Delay time (1ms - 2 seconds)
- [ ] Feedback control
- [ ] Wet/dry mix
- [ ] Sync to DAW tempo
- [ ] Stereo ping-pong mode
- [ ] High-pass filter in feedback path

### Plugin 5: SimpleEQ
- [ ] 3-band parametric EQ
- [ ] High-pass and low-pass filters
- [ ] Visual frequency response curve
- [ ] Spectrum analyzer

### Plugin 6: SimpleComp
- [ ] Threshold, ratio, attack, release
- [ ] Automatic makeup gain
- [ ] Gain reduction meter
- [ ] Soft knee option

## Phase 3: Distribution & Website

**Goal**: Get plugins into users' hands, build community

- [ ] Website with downloads (simple static site)
- [ ] User documentation for each plugin
- [ ] Video tutorials (YouTube)
- [ ] Code-signing certificates (macOS + Windows)
- [ ] Installers (macOS .pkg, Windows .exe)
- [ ] AU (Audio Units) support for macOS
- [ ] Linux builds
- [ ] Beta testing program

## Phase 4: Advanced Plugins

**Goal**: Build unique, creative tools

Ideas:
- Granular synthesis plugin
- Convolution reverb
- Spectral processing tools
- Creative modulation effects
- Wavetable synthesizer

## Phase 5: Monetization

**Goal**: Generate revenue while keeping core plugins free

### Free Tier
- SimpleGain, PanUtil, BasicSynth stay free forever
- CleanDelay, SimpleEQ, SimpleComp free

### Premium Tier ($29-49)
- Advanced synths and effects
- Expansion packs (presets, wavetables)
- Early access to new plugins

### Business Model Options
1. **Pay What You Want** - Free download, optional donation
2. **Freemium** - Free basic plugins, paid advanced ones
3. **Subscription** - $5/month for all premium plugins
4. **One-time Purchase** - Buy individual premium plugins

## Technical Milestones

### Build System
- [x] CMake configuration for JUCE
- [x] VST3 builds
- [ ] AU (Audio Units) support
- [ ] AAX (Pro Tools) support
- [ ] Automated testing
- [ ] CI/CD pipeline (GitHub Actions)

### Code Quality
- [ ] Unit tests for DSP algorithms
- [ ] Integration tests
- [ ] Code coverage reporting
- [ ] Static analysis
- [ ] Performance profiling

### Documentation
- [x] DSP basics guide
- [x] JUCE patterns guide
- [ ] Contributing guidelines
- [ ] API documentation (Doxygen)
- [ ] Plugin user manuals

## Success Metrics

### Phase 1 (Learning)
- 3 working plugins
- Understanding of core DSP concepts
- Comfortable with JUCE framework

### Phase 2 (Quality)
- Plugins that sound professional
- No CPU spikes or crashes
- Positive feedback from beta testers

### Phase 3 (Distribution)
- 100+ downloads in first month
- Active user feedback/bug reports
- Small community forming

### Phase 4 (Revenue)
- First paying customer
- $100/month revenue by month 6
- $500/month revenue by end of year 1

## Next Steps

1. **This week**: Start PanUtil plugin
2. **This month**: Complete PanUtil, start BasicSynth
3. **Next 3 months**: Complete Phase 1, start Phase 2
4. **6 months**: Launch website with first public downloads

## Resources Needed

- Audio interface for testing
- Reference plugins for comparison
- Beta testers (producers/musicians)
- Code signing certificates ($99/year Apple, $200-400 Windows)
- Web hosting for downloads ($5-10/month)
