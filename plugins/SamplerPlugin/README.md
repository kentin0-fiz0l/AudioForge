# AudioForge Sampler Plugin

Professional multi-sample instrument with key zone mapping, velocity layers, and loop playback.

**Version**: 1.0.0 (Phase 2 - Interactive UI)
**Status**: ✅ Core engine + UI complete, preset system pending

## Features (Phase 2)

### Sample Engine
- ✅ Multi-format support (WAV, AIFF, FLAC)
- ✅ 512 MB memory budget
- ✅ Pitch shifting via playback rate
- ✅ Linear interpolation for smooth playback
- ✅ Loop playback (None, Forward, Ping-Pong)

### Sound Shaping
- ✅ ADSR envelope (amplitude)
- ✅ ADSR envelope (filter modulation)
- ✅ Multi-mode filter (LP, HP, BP, Notch)
- ✅ Filter envelope amount control

### Polyphony
- ✅ 16 voices with voice stealing
- ✅ Key zone mapping
- ✅ Velocity layer support
- ✅ Round-robin playback

### Parameters (20 total)
- **Amplitude Envelope**: Attack, Decay, Sustain, Release
- **Filter**: Cutoff, Resonance, Type, Envelope Amount
- **Filter Envelope**: Attack, Decay, Sustain, Release
- **Sample**: Start, End, Loop Mode, Loop Start, Loop End
- **Global**: Master Volume, Master Tune, Voice Count

## Installation

The plugin is automatically installed to:
- **VST3**: `~/Library/Audio/Plug-Ins/VST3/Sampler.vst3`
- **AU**: `~/Library/Audio/Plug-Ins/Components/Sampler.component`
- **Standalone**: `build/SamplerPlugin_artefacts/Release/Standalone/Sampler.app`

### User Interface (NEW in Phase 2)
- ✅ **Waveform display** with draggable loop markers
- ✅ **Piano keyboard** zone editor (88 keys, colored zones)
- ✅ **Drag & drop** sample loading
- ✅ **Zone management** (add, delete, resize via mouse)
- ✅ **Parameter controls** (ADSR sliders, filter controls)
- ✅ **Memory/zone status** display

## Quick Start

1. **Load the plugin** in your DAW (Logic, Reaper, Ableton)
2. **Drag & drop** an audio file (.wav/.aiff/.flac) onto the waveform display
3. **Create zones** by clicking "Add Zone" or right-clicking the piano keyboard
4. **Resize zones** by dragging the edges on the piano keyboard
5. **Edit loop points** by dragging the yellow markers in the waveform
6. **Play MIDI notes** - zones will trigger samples at correct pitch
7. **Adjust ADSR** and filter to shape the sound

## Building from Source

```bash
cd ~/Projects/Active/AudioForge/plugins/SamplerPlugin
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The build takes ~2-3 minutes and produces all plugin formats.

## Development Roadmap

- [x] **Phase 1** (Week 1): Core engine - DONE ✅
  - Sample loading, playback, ADSR, filter, polyphony
- [x] **Phase 2** (Week 2): Key zone mapping & multi-sample - DONE ✅
  - ZoneEditor UI, drag-and-drop, waveform display, zone management
- [ ] **Phase 3** (Week 3): Advanced features
  - Waveform display, loop markers, presets
- [ ] **Phase 4** (Week 4): UI polish & testing
  - Custom UI, comprehensive tests, documentation
- [ ] **Phase 5** (Week 5): Integration & release
  - Installer, v1.5.0 release, factory samples

## Architecture

```
Sample              → Holds audio data + loop settings
SampleLibrary       → Manages loaded samples (512 MB budget)
KeyZone             → Maps MIDI notes → samples + velocity layers
SamplerVoice        → Playback engine (pitch shift, ADSR, filter)
PluginProcessor     → Main audio processor (16-voice polyphony)
PluginEditor        → UI (basic in Phase 1, enhanced in Phase 4)
```

## Technical Specifications

- **Max sample length**: 30 seconds per sample
- **Memory budget**: 512 MB total
- **Sample formats**: WAV, AIFF, FLAC (16/24/32-bit, mono/stereo)
- **Polyphony**: 16 voices (configurable)
- **Sample rate**: Any (tested at 44.1kHz and 48kHz)
- **CPU usage**: <10% target (per instance at 44.1kHz)

## Known Limitations (Phase 2)

- ❌ Velocity layers require manual configuration (Phase 3)
- ❌ Sample-to-zone assignment manual (Phase 3)
- ❌ No factory presets (Phase 3)
- ❌ No preset browser (Phase 3)
- ❌ No sample library view (Phase 3)

## Testing

To test Phase 1:
1. Load plugin in DAW
2. Check parameter controls work
3. Verify MIDI input triggers voices
4. Test ADSR envelope response
5. Test filter cutoff/resonance
6. Monitor CPU usage (<10% target)

Sample loading will be added in Phase 2 with drag-and-drop UI.

## License

Part of AudioForge v1.5.0 - Professional plugin suite for music production.

---

**Built with**: JUCE 8.x, C++17
**Platform**: macOS (Universal Binary: Intel + Apple Silicon)
**Contact**: AudioForge Development Team
