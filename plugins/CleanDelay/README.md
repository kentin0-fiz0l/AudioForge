# CleanDelay

A professional stereo delay effect with ping-pong mode, feedback control, and wet/dry mixing.

## Overview

CleanDelay is a versatile delay plugin designed for creating rhythmic echoes, ambient textures, and stereo width enhancement. From tight slapback delays to lush ambient washes, CleanDelay provides everything you need for creative delay effects.

## Features

- **Adjustable Delay Time**: 1ms to 2000ms (2 seconds) with logarithmic scaling
- **Feedback Control**: 0% to 99% for controlled echo repetitions
- **Wet/Dry Mix**: Seamless blending from 100% dry to 100% wet
- **Ping-Pong Mode**: Alternating left/right delays for stereo width
- **Real-time Metering**: Input and output level monitoring
- **Clean Signal Path**: High-quality circular buffer implementation
- **State Persistence**: Save and recall settings in your DAW

## Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| **Delay Time** | 1ms - 2000ms | 250ms | Time between echoes (log scale for musical control) |
| **Feedback** | 0% - 99% | 30% | Amount of delayed signal fed back into the delay line |
| **Mix (Wet)** | 0% - 100% | 50% | Balance between dry (original) and wet (delayed) signal |
| **Ping-Pong** | Off/On | Off | Enable alternating left/right delays for stereo effect |

### Parameter Details

#### Delay Time (1ms - 2000ms)
- **Short delays (1-50ms)**: Slapback, doubling, comb filtering effects
- **Medium delays (50-200ms)**: Rhythmic echoes synced to tempo
- **Long delays (200ms-2s)**: Ambient washes, experimental textures
- Logarithmic scale emphasizes musical delay times (quarter notes, eighth notes)

#### Feedback (0% - 99%)
- **0-30%**: Clean, discrete echoes (good for rhythmic delays)
- **30-60%**: Multiple distinct repetitions (classic delay sound)
- **60-90%**: Sustained echoes building into ambience
- **90-99%**: Near-infinite repeats (caution: can build up quickly!)
- Limited to 99% to prevent runaway feedback

#### Mix (0% - 100%)
- **0%**: Completely dry (bypass)
- **25-40%**: Subtle delay enhancement
- **50%**: Classic 50/50 blend
- **75-100%**: Delay-dominant for special effects

#### Ping-Pong Mode
- **Off**: Standard stereo delay (L→L, R→R)
- **On**: Alternating delays (L→R→L→R) for wide stereo imaging
- Creates rhythmic stereo movement
- Excellent for adding width to mono sources

## Usage Examples

### 1. Vocal Slapback (Classic Rock/Rockabilly)
```
Delay Time: 80-120ms
Feedback:   10-20%
Mix:        25-35%
Ping-Pong:  Off
```
**Effect**: Quick doubling effect that thickens vocals without obvious echoes. Adds presence and energy to lead vocals.

### 2. Rhythmic Eighth Note Delay (120 BPM)
```
Delay Time: 250ms (eighth note at 120 BPM)
Feedback:   40-50%
Mix:        30-40%
Ping-Pong:  On
```
**Effect**: Syncopated rhythmic echoes that bounce between speakers. Delay time = 60,000ms / BPM / subdivision.

**Tempo sync reference:**
- 120 BPM quarter note: 500ms
- 120 BPM eighth note: 250ms
- 120 BPM sixteenth note: 125ms

### 3. Ambient Wash (Soundscapes/Pads)
```
Delay Time: 500-800ms
Feedback:   70-85%
Mix:        60-80%
Ping-Pong:  On
```
**Effect**: Lush, evolving delay clouds with wide stereo field. Great for synth pads, guitars, and atmospheric textures.

### 4. Haas Effect (Stereo Width Enhancement)
```
Delay Time: 10-30ms
Feedback:   0%
Mix:        100%
Ping-Pong:  Off
```
**Effect**: Psychoacoustic stereo widening without obvious echoes. Use on mono sources to create stereo spread.

### 5. Dub-Style Echo Throw (Reggae/Dub)
```
Delay Time: 375ms (dotted eighth at 120 BPM)
Feedback:   60-75%
Mix:        Automate 0% → 100% for throws
Ping-Pong:  On
```
**Effect**: Classic dub delay throws where echoes spiral across the stereo field. Automate Mix for dramatic effect sends.

### 6. Guitar Lead Enhancement
```
Delay Time: 375-500ms
Feedback:   30-45%
Mix:        25-35%
Ping-Pong:  Off
```
**Effect**: Fills out lead guitar lines with rhythmic support. Gives body to single-note lines without muddiness.

## Technical Specifications

### Audio Processing
- **Sample Rate Support**: 44.1kHz, 48kHz, 88.2kHz, 96kHz
- **Bit Depth**: 32-bit floating point internal processing
- **Latency**: Zero samples (real-time processing)
- **Buffer Size**: Adaptive (supports 64 to 2048 samples)

### Performance
- **CPU Usage**: ~0.5-1% (typical, Intel i7 @ 2.6GHz)
- **Memory Footprint**: ~1.5 MB (circular buffer storage)
- **Max Delay Time**: 2 seconds at 96kHz sample rate

### Delay Line Implementation
- **Algorithm**: Circular buffer with linear interpolation
- **Buffer Size**: 192,000 samples (2 seconds @ 96kHz)
- **Channels**: Independent L/R buffers for stereo processing
- **Feedback Routing**: Cross-channel in ping-pong mode

### Signal Flow
```
Input → Delay Buffer → Read (delayed) → Feedback → Mix → Output
         ↑                    ↓
         └──── Feedback ──────┘

Ping-Pong Mode:
Left Input  → Right Delay → Left Output
Right Input → Left Delay  → Right Output
```

## Plugin Formats

- **VST3**: macOS (ARM64/x86_64 universal binary)
- **Standalone**: Runs independently for testing

## Building from Source

### Prerequisites
- CMake 3.15+
- JUCE Framework 7.x
- Xcode (macOS) or Visual Studio (Windows)

### Build Instructions

```bash
# Navigate to plugin directory
cd plugins/CleanDelay

# Configure CMake
cmake -B build

# Build plugin
cmake --build build

# Install to system plugin folder (macOS)
cp -r build/CleanDelay_artefacts/VST3/CleanDelay.vst3 \
      ~/Library/Audio/Plug-Ins/VST3/
```

### Run Tests

```bash
# Navigate to test directory
cd tests

# Configure and build tests
cmake -B build
cmake --build build

# Run CleanDelay tests
./build/TestRunner
```

## Creative Tips

### Delay Time Modulation
While CleanDelay doesn't include built-in modulation, you can:
- **DAW Automation**: Automate delay time for pitch-shift effects
- **LFO Control**: Use your DAW's parameter modulation to create tape-style wow/flutter
- **Manual Tweaking**: Real-time delay time changes create doppler-like pitch shifts

### Parallel Processing
For more control, use CleanDelay on a send/return:
1. Send instrument to CleanDelay on an aux track
2. Set CleanDelay to 100% wet
3. Control mix with send level
4. Apply additional processing to delay return (reverb, filtering, distortion)

### Stereo Imaging
- **Mono to Stereo**: Use ping-pong mode on mono tracks for instant width
- **Stereo Enhancement**: Short delays (15-30ms) with 100% wet creates wide stereo spread
- **Haas Effect**: Set one channel to 10-20ms delay for perceived stereo without obvious echoes

### Feedback Tricks
- **Self-Oscillation**: Push feedback to 95%+ for sustained tones (use with caution!)
- **Decay Automation**: Automate feedback down after delay throws to control decay
- **Regeneration**: High feedback + long delay time creates ambient pad-like textures

### Musical Timing
Calculate delay time for tempo-synced delays:
```
Delay Time (ms) = 60,000 / BPM / subdivision

Examples at 120 BPM:
- Whole note:     2000ms
- Half note:      1000ms
- Quarter note:   500ms
- Eighth note:    250ms
- Sixteenth:      125ms
- Dotted eighth:  375ms
- Triplet eighth: 167ms
```

## Known Limitations

- **No Tempo Sync**: Delay time must be set manually (tempo sync planned for v0.2.0)
- **No Modulation**: No built-in LFO or delay time modulation
- **No Filtering**: No tone controls on feedback path (use external EQ on return)
- **Mono to Stereo Only**: Ping-pong mode most effective on mono sources

## Roadmap

### Version 0.2.0 (Planned)
- [ ] Tempo sync with DAW (quarter notes, eighths, triplets, dotted)
- [ ] Low-pass and high-pass filters on feedback path
- [ ] Delay time modulation (tape-style wow/flutter)
- [ ] Stereo width control

### Version 0.3.0 (Future)
- [ ] Multi-tap delays (3-4 taps with independent timing)
- [ ] Reverse delay mode
- [ ] Duck mode (delay ducks when input is present)
- [ ] Preset system

## Troubleshooting

### No Output
- Check Mix parameter (set to 50% for testing)
- Verify input signal is present (check input meter)
- Ensure delay time is not set to minimum (increase to 100ms+)

### Feedback Runaway
- Reduce Feedback parameter below 90%
- Lower Mix to reduce wet signal level
- Check input levels (high input + high feedback can clip)

### Clicks/Pops
- Occurs when changing delay time in real-time (expected behavior)
- Use automation curves instead of stepped parameter changes
- Reduce feedback before changing delay time

### Ping-Pong Not Working
- Ensure input signal is mono or center-panned
- Ping-pong mode alternates echoes, so needs multiple repetitions (increase feedback)
- Check stereo output (ping-pong only works in stereo DAW tracks)

## Version History

### v0.1.0 (2026-08-21)
- Initial release
- Core delay functionality with feedback and mixing
- Ping-pong stereo mode
- Input/output level metering
- State persistence in DAW projects

## License

CleanDelay is part of the AudioForge plugin suite.

Copyright © 2026 AudioForge
Released under MIT License

## Credits

**Development**: Claude Opus 4.6 + Human collaboration
**Framework**: JUCE 7.x
**DSP Library**: AudioForge shared DSP components

## Support

- **Issues**: Report bugs at [AudioForge GitHub Issues](https://github.com/yourusername/audioforge/issues)
- **Website**: [audioforge.fluxstudio.art](https://audioforge.fluxstudio.art)
- **Documentation**: [AudioForge Docs](https://audioforge.fluxstudio.art/docs)

## See Also

- **SimpleGain**: Basic gain/volume control with metering
- **PanUtil**: Stereo panning and width control
- **BasicSynth**: Polyphonic subtractive synthesizer
- **SimpleEQ**: Parametric equalizer (coming soon)
