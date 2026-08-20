# JUCE Framework Guide

Quick reference for JUCE patterns used in AudioForge plugins.

## Project Structure

Every JUCE plugin has:
```
PluginName/
├── CMakeLists.txt          # Build configuration
├── Source/
│   ├── PluginProcessor.h   # Audio processing logic
│   ├── PluginProcessor.cpp
│   ├── PluginEditor.h      # GUI
│   └── PluginEditor.cpp
```

## CMakeLists.txt Template

```cmake
cmake_minimum_required(VERSION 3.15)
project(PLUGIN_NAME VERSION 1.0.0)

# Add JUCE
add_subdirectory(../../JUCE JUCE)

juce_add_plugin(PluginName
    PLUGIN_MANUFACTURER_CODE Manu
    PLUGIN_CODE Plg1
    FORMATS VST3
    PRODUCT_NAME "Plugin Name")

target_sources(PluginName PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp)

target_compile_definitions(PluginName PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0)

target_link_libraries(PluginName PRIVATE
    juce::juce_audio_utils)
```

## Audio Processor Pattern

```cpp
class MyProcessor : public AudioProcessor {
public:
    MyProcessor() {
        // Initialize parameters
        addParameter(gain = new AudioParameterFloat(
            "gain", "Gain", 0.0f, 1.0f, 0.5f));
    }

    void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midi) override {
        // Get parameter value
        float gainValue = gain->get();

        // Process each channel
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
            float* channelData = buffer.getWritePointer(channel);

            // Process each sample
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
                channelData[sample] *= gainValue;
            }
        }
    }

private:
    AudioParameterFloat* gain;
};
```

## Parameters

### AudioParameterFloat
```cpp
addParameter(new AudioParameterFloat(
    "paramID",              // Unique ID
    "Parameter Name",       // Display name
    0.0f,                   // Min
    1.0f,                   // Max
    0.5f));                 // Default
```

### AudioParameterChoice (dropdown)
```cpp
addParameter(new AudioParameterChoice(
    "mode",
    "Mode",
    StringArray{"Option 1", "Option 2", "Option 3"},
    0));  // Default index
```

## GUI (Editor)

```cpp
class MyEditor : public AudioProcessorEditor {
public:
    MyEditor(MyProcessor& p) : AudioProcessorEditor(&p), processor(p) {
        // Add slider
        addAndMakeVisible(gainSlider);
        gainSlider.setRange(0.0, 1.0);
        gainSlider.onValueChange = [this] {
            processor.setGain(gainSlider.getValue());
        };

        setSize(400, 300);
    }

    void paint(Graphics& g) override {
        g.fillAll(Colours::black);
        g.setColour(Colours::white);
        g.drawText("My Plugin", getLocalBounds(), Justification::centred);
    }

    void resized() override {
        gainSlider.setBounds(50, 100, 300, 50);
    }

private:
    MyProcessor& processor;
    Slider gainSlider;
};
```

## Common JUCE Utilities

### dB Conversion
```cpp
#include <juce_audio_basics/juce_audio_basics.h>

float linear = Decibels::decibelsToGain(dbValue);
float db = Decibels::gainToDecibels(linearValue);
```

### Smoothing
```cpp
#include <juce_dsp/juce_dsp.h>

SmoothedValue<float> smoothedGain;
smoothedGain.reset(sampleRate, 0.05); // 50ms ramp
smoothedGain.setTargetValue(newGain);

// In processBlock:
float currentGain = smoothedGain.getNextValue();
```

### Thread Safety
```cpp
// Parameters are thread-safe
// But for custom variables:
CriticalSection lock;

{
    const ScopedLock sl(lock);
    // Access shared data
}
```

## Building Plugins

```bash
# Configure
cmake -B build

# Build
cmake --build build

# Install to system
cmake --install build
```

## Debugging Tips

1. Use `DBG("message")` for console output
2. Enable assertions in Debug builds
3. Use Xcode/Visual Studio debugger
4. Test in actual DAW, not just standalone
5. Check CPU usage in DAW's performance meter
