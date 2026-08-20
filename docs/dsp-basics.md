# DSP Basics for Plugin Development

This document covers fundamental DSP concepts as we learn them through building plugins.

## Audio Fundamentals

### Sample Rate
- Common rates: 44.1kHz (CD), 48kHz (video), 96kHz (hi-res)
- Sample rate determines maximum frequency (Nyquist: sample_rate / 2)
- All our plugins should handle any sample rate dynamically

### Bit Depth
- 16-bit (CD quality), 24-bit (professional), 32-bit float (DAW internal)
- JUCE handles this for us, we work with floats

### Buffers
- Audio processed in blocks (typically 64-512 samples)
- JUCE gives us `AudioBuffer<float>` with channels
- We process samples in a loop

## Basic DSP Operations

### Gain (Volume Control)
```cpp
// Simplest effect: multiply each sample
for (int sample = 0; sample < numSamples; ++sample) {
    buffer[sample] = buffer[sample] * gainLinear;
}
```

### Decibels to Linear
```cpp
// Convert dB to linear gain
float linearGain = std::pow(10.0f, gainDb / 20.0f);

// Convert linear to dB
float gainDb = 20.0f * std::log10(linearGain);
```

### Panning (Stereo)
```cpp
// Constant power pan (-45dB to +45dB)
float leftGain = std::cos(panAngle);
float rightGain = std::sin(panAngle);
```

## Audio Processing Blocks

### 1. Process Block (JUCE)
Every plugin has a `processBlock()` function called by the DAW:
```cpp
void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midi) {
    // 1. Get parameters
    // 2. Process audio
    // 3. Update meters/UI
}
```

### 2. Parameter Smoothing
Never apply parameter changes instantly - causes clicks:
```cpp
// Use linear smoothing
currentGain += (targetGain - currentGain) * 0.01f;
```

### 3. Buffering Strategy
- Process per sample vs per block
- Per sample: More accurate modulation, more CPU
- Per block: Faster, fine for most effects

## Common Effects

### Delay
```cpp
// Circular buffer
delayBuffer[(writePos++) % bufferSize] = input;
output = delayBuffer[(writePos - delaySamples) % bufferSize];
```

### Simple Filter (One-pole lowpass)
```cpp
// Exponential moving average
output = output + cutoff * (input - output);
```

## Learning Path

1. **SimpleGain** - Learn buffer processing, parameters, dB conversion
2. **PanUtil** - Stereo processing, constant-power panning
3. **BasicSynth** - Oscillators, ADSR envelopes, MIDI
4. **SimpleDelay** - Circular buffers, feedback
5. **EQ** - Filters, frequency response

## Resources

- [JUCE Tutorials](https://juce.com/learn/tutorials)
- [The Audio Programmer (YouTube)](https://www.youtube.com/c/TheAudioProgrammer)
- "Designing Audio Effect Plugins in C++" by Will Pirkle
- [musicdsp.org](https://www.musicdsp.org/) - DSP algorithms

## Common Pitfalls

1. **Not initializing variables** - Causes pops and noise
2. **Ignoring sample rate** - Delay times, cutoffs must scale
3. **Forgetting to smooth parameters** - Causes clicks
4. **Buffer overruns** - Check array bounds
5. **Denormal numbers** - Can kill CPU, flush to zero
