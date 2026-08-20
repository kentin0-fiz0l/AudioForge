# PanUtil

Stereo panning and width control utility plugin.

## What It Does

- **Pan Control**: Position audio anywhere in the stereo field (-100% L to +100% R)
- **Width Control**: Adjust stereo width (0% = mono, 100% = normal, 200% = ultra-wide)
- **Dual Modes**:
  - **Pan Mode**: Constant-power panning (no volume drop in center)
  - **Balance Mode**: Attenuates one channel, preserves the other
- **Visual Feedback**: See pan position and L/R meters in real-time

## What We're Learning

### DSP Concepts
- **Stereo processing** (L/R channels)
- **Constant-power panning** algorithm
- **M/S (Mid/Side) processing** for width control
- **Dual-channel metering**
- **Pan vs Balance** behavior

### JUCE Framework
- Multi-channel buffer processing
- ComboBox parameter (mode selector)
- Advanced GUI with visualization
- Multiple sliders and controls

## Algorithm Details

### Constant-Power Panning
```cpp
// Convert pan (-1 to +1) to angle (0 to π/2)
float angle = (pan + 1.0) * 0.25 * π;
leftGain = cos(angle);
rightGain = sin(angle);
```

This ensures perceived loudness stays constant as you pan.

### Width Control (M/S Processing)
```cpp
// Convert L/R to M/S
mid = (L + R) * 0.5;    // Mono sum
side = (L - R) * 0.5;   // Stereo difference

// Scale side by width
side *= width;  // 0.0 = mono, 1.0 = normal, 2.0 = wide

// Convert back to L/R
L = mid + side;
R = mid - side;
```

## Building

```bash
cd plugins/PanUtil
cmake -B build
cmake --build build --config Release
```

## Using PanUtil

### Pan Control
- **Center (0)**: Equal left/right
- **Left (-1.0)**: 100% left channel
- **Right (+1.0)**: 100% right channel
- **Double-click** to reset to center

### Width Control
- **0%**: Mono (L and R identical)
- **100%**: Normal stereo
- **200%**: Wide stereo (exaggerated difference)
- **Double-click** to reset to 100%

### Mode Selection
- **Pan Mode**: Best for mono sources (vocals, single instruments)
  - Uses constant-power panning
  - No volume drop in center
- **Balance Mode**: Best for stereo sources (full mixes)
  - Attenuates one channel
  - Preserves stereo image

## Use Cases

1. **Fix off-center recordings** - Center a vocal that was recorded slightly to one side
2. **Create space in mix** - Pan instruments to different positions
3. **Stereo widening** - Make synths/pads sound bigger with width >100%
4. **Mono compatibility** - Check mix at 0% width
5. **Creative effects** - Extreme panning/width for special effects

## Next Steps

After understanding PanUtil, we'll build **BasicSynth** to learn MIDI processing and sound synthesis!
