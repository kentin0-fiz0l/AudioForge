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
- ✅ 32 voices with voice stealing
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

## SFZ instruments

The sampler plays SFZ instruments, such as those in [VSCO 2 Community Edition](https://github.com/sgossner/VSCO-2-CE). A saved state names one with an `sfz` attribute on its root element, and loading the state loads it and every sample it uses:

```xml
<Parameters sfz="/Users/me/vsco2/ViolinEnsSusVib.sfz"/>
```

That is the XML to give the [render host](../../tools/render-host/README.md) with `--state`. While an instrument is loaded, notes play it instead of the key zones, and the state the plugin saves keeps the path. A relative path is taken from the working directory.

What it reads:

| | |
|---|---|
| Headers | `<control>`, `<global>`, `<master>`, `<group>`, `<region>`; a region takes the opcodes of the headers above it and can override them |
| Files | `sample`, `default_path`; paths relative to the .sfz, `\` or `/`, spaces allowed; WAV, AIFF, FLAC |
| Mapping | `lokey`, `hikey`, `key`, `lovel`, `hivel`; keys as numbers or names (`c4` is 60, `f#3`, `eb2`) |
| Pitch | `pitch_keycenter`, `pitch_keytrack`, `tune` (cents), `transpose`; notes away from the key centre are resampled |
| Level | `volume` (dB), `amplitude`, `pan`, `amp_veltrack` (velocity squared, as SFZ does by default) |
| Envelope | `ampeg_attack`, `ampeg_decay`, `ampeg_sustain`, `ampeg_release` |
| Playback | `offset`, `end`, `loop_mode` (`no_loop`, `one_shot`, `loop_continuous`, `loop_sustain`), `loop_start`, `loop_end`; without a `loop_mode`, a WAV's own loop is used |
| Round robin | `seq_length`, `seq_position`, `lorand`, `hirand` |
| Keyswitches | `sw_lokey`, `sw_hikey`, `sw_last`, `sw_default` |

Release-triggered regions (`trigger=release`) are left out, and other opcodes are ignored; what was ignored, and any samples that couldn't be loaded, are written to the log (the terminal, under the render host). A region's envelope opcodes override the Amp envelope parameters; where it has none, attack and release come from those parameters and the sustain is full. `loop_sustain` loops like `loop_continuous`. The filter, Master Volume and Master Tune apply to SFZ instruments too.

The random round robin is seeded the same way every time, so a render comes out the same each time it's made.

### MIDI

Notes start at their exact sample in the block. The sustain pedal (CC64) holds notes, and volume (CC7) and expression (CC11) scale the output on a square-law curve; both start at full.

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
SfzParser           → Reads SFZ text into regions
SfzSampleMap        → An SFZ instrument with its samples: which regions a note plays
PluginProcessor     → Main audio processor (32-voice polyphony)
PluginEditor        → UI (basic in Phase 1, enhanced in Phase 4)
```

## Technical Specifications

- **Max sample length**: 30 seconds per sample
- **Memory budget**: 512 MB total
- **Sample formats**: WAV, AIFF, FLAC (16/24/32-bit, mono/stereo)
- **Polyphony**: 32 voices
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
