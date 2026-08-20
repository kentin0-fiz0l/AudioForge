# SimpleGain

A basic gain/volume control plugin - our first learning project.

## What It Does

- Adjustable gain from -60 dB to +12 dB
- Real-time level meter
- Smooth parameter changes (no clicks or pops)
- Clean, simple interface

## What We're Learning

### DSP Concepts
- Audio buffer processing
- Decibel to linear conversion
- Parameter smoothing
- Level metering
- Sample-by-sample processing

### JUCE Framework
- AudioProcessor basics
- AudioParameterFloat
- GUI with AudioProcessorEditor
- Timer-based UI updates
- Thread-safe communication (atomics)

## Building

```bash
cd plugins/SimpleGain
cmake -B build
cmake --build build
```

The built plugin will be in `build/SimpleGain_artefacts/`.

### Installing

Copy the VST3 to your plugin folder:
```bash
# macOS
cp -r build/SimpleGain_artefacts/VST3/SimpleGain.vst3 ~/Library/Audio/Plug-Ins/VST3/

# The standalone app is at:
# build/SimpleGain_artefacts/Standalone/SimpleGain.app
```

## Code Walkthrough

### PluginProcessor.cpp
1. **Constructor**: Sets up the gain parameter (-60 to +12 dB)
2. **prepareToPlay()**: Initializes smoothed gain with 50ms ramp time
3. **processBlock()**: The heart of the plugin
   - Gets current parameter value
   - Converts dB to linear gain
   - Applies smoothed gain to each sample
   - Tracks peak level for metering

### PluginEditor.cpp
1. **Constructor**: Creates the rotary gain slider and meter
2. **paint()**: Draws the UI and animated level meter
3. **timerCallback()**: Updates meter 30 times per second

## Next Steps

After understanding SimpleGain, we'll build:
- **PanUtil**: Stereo panning (learning multi-channel processing)
- **BasicSynth**: Simple synthesizer (learning MIDI and oscillators)
