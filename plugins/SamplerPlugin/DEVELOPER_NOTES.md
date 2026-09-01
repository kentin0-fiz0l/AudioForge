# AudioForge Sampler Plugin - Developer Notes

## Architecture Overview

The AudioForge Sampler is built on JUCE 8.x and follows modern C++17 practices with a clear separation of concerns:

```
┌─────────────────────────────────────────────────┐
│                PluginEditor (UI)                │
│  ┌──────────┬───────────┬──────────────────┐  │
│  │Waveform  │ZoneEditor │VelocityLayerEditor│  │
│  │Display   │           │SampleBrowser      │  │
│  └──────────┴───────────┴──────────────────┘  │
├─────────────────────────────────────────────────┤
│           PluginProcessor (DSP)                 │
│  ┌──────────────┬─────────────────────────┐   │
│  │VoiceManager  │AudioProcessorValueTree  │   │
│  │<16 voices>   │State (APVTS)            │   │
│  └──────────────┴─────────────────────────┘   │
├─────────────────────────────────────────────────┤
│              Domain Models                      │
│  ┌──────┬─────────┬──────────┬──────────┐    │
│  │Sample│Sample   │KeyZone   │Sampler   │    │
│  │      │Library  │Map       │Voice     │    │
│  └──────┴─────────┴──────────┴──────────┘    │
└─────────────────────────────────────────────────┘
```

---

## Core Components

### 1. Sample (`Source/Sample.h/cpp`)

**Purpose**: Encapsulates a single audio sample with metadata and loop settings.

**Key Responsibilities**:
- Load audio from `AudioFormatReader`
- Store audio data in stereo `AudioBuffer<float>`
- Manage loop points and loop mode
- Calculate memory footprint

**API**:
```cpp
Sample(juce::AudioFormatReader& reader, double maxLengthSecs = 30.0);

const juce::AudioBuffer<float>& getAudioData() const;
double getSampleRate() const;
int getLengthInSamples() const;
size_t getSizeInBytes() const;

void setLoopPoints(double startSec, double endSec);
void setLoopMode(LoopMode mode);
juce::Range<double> getLoopPoints() const;
```

**Implementation Details**:

**Loop Modes**:
```cpp
enum class LoopMode {
    None = 0,      // Play once
    Forward = 1,   // Loop forward
    PingPong = 2   // Loop forward then backward
};
```

**Memory Calculation**:
```cpp
size_t getSizeInBytes() const {
    return audioData.getNumChannels() *
           audioData.getNumSamples() *
           sizeof(float);
}
```

**Thread Safety**: Sample objects are immutable after construction (safe for multi-threaded access).

---

### 2. SampleLibrary (`Source/SampleLibrary.h/cpp`)

**Purpose**: Manages loaded samples with memory budget enforcement.

**Key Responsibilities**:
- Load samples from files (WAV/AIFF/FLAC)
- Enforce 512 MB memory budget
- Provide shared ownership via `shared_ptr`
- Track total memory usage

**API**:
```cpp
std::shared_ptr<Sample> loadSample(const juce::File& file);
void unloadSample(int index);
void clearAllSamples();

std::shared_ptr<Sample> getSample(int index) const;
int getNumSamples() const;
size_t getTotalMemoryUsed() const;
juce::String getLastError() const;
```

**Memory Budget**:
```cpp
static constexpr size_t MAX_MEMORY_BYTES = 512 * 1024 * 1024;  // 512 MB

std::shared_ptr<Sample> loadSample(const juce::File& file) {
    auto newSample = std::make_shared<Sample>(*reader);
    size_t newTotal = totalMemoryUsed + newSample->getSizeInBytes();

    if (newTotal > MAX_MEMORY_BYTES) {
        lastError = "Memory limit exceeded";
        return nullptr;
    }

    samples.push_back(newSample);
    totalMemoryUsed = newTotal;
    return newSample;
}
```

**Supported Formats**:
- WAV (all bit depths: 16, 24, 32, float)
- AIFF (Apple format)
- FLAC (lossless compression)

**Not Supported**:
- MP3 (requires `juce_audio_formats` with MP3 support)

---

### 3. KeyZone & KeyZoneMap (`Source/KeyZone.h/cpp`)

**Purpose**: Maps MIDI note ranges to samples with velocity layer support.

**Data Structures**:
```cpp
struct VelocityLayer {
    int sampleIndex;           // Index into SampleLibrary
    float velocityMin;         // 0.0 - 1.0 (MIDI velocity / 127)
    float velocityMax;         // 0.0 - 1.0
};

struct KeyZone {
    int noteMin;               // MIDI note (0-127)
    int noteMax;               // MIDI note (0-127)
    int rootNote;              // Pitch reference
    std::vector<VelocityLayer> velocityLayers;

    int roundRobinCount = 1;
    int currentRoundRobin = 0;

    int getSampleForVelocity(float velocity) const;
};

class KeyZoneMap {
    void addZone(const KeyZone& zone);
    void removeZone(int zoneIndex);
    const KeyZone* getZoneForNote(int midiNote) const;
    std::vector<KeyZone>& getZones();
};
```

**Velocity Layer Lookup**:
```cpp
int KeyZone::getSampleForVelocity(float velocity) const {
    for (const auto& layer : velocityLayers) {
        if (velocity >= layer.velocityMin &&
            velocity <= layer.velocityMax) {
            return layer.sampleIndex;
        }
    }
    return -1;  // No layer found
}
```

**Note Lookup**:
```cpp
const KeyZone* KeyZoneMap::getZoneForNote(int midiNote) const {
    for (const auto& zone : zones) {
        if (midiNote >= zone.noteMin && midiNote <= zone.noteMax) {
            return &zone;
        }
    }
    return nullptr;  // Note not mapped
}
```

**Design Decision**: Overlapping zones are allowed. First matching zone is used.

---

### 4. SamplerVoice (`Source/SamplerVoice.h/cpp`)

**Purpose**: Implements VoiceManager interface for polyphonic sample playback.

**VoiceManager Interface**:
```cpp
void noteOn(int midiNote, float velocity, double sampleRate);
void noteOff();
bool isActive() const;
int getMidiNote() const;
float getLevel() const;
void reset();
```

**Processing**:
```cpp
float processSample(double sampleRate,
                   const SampleLibrary& library,
                   const KeyZoneMap& keyZones,
                   float masterVolume,
                   float masterTune);
```

**Pitch Shifting Algorithm**:
```cpp
static double calculatePlaybackRate(int midiNote, int rootNote,
                                    double sampleRate, double targetSampleRate) {
    // Equal temperament: freq = 440 * 2^((note-69)/12)
    double targetFreq = 440.0 * std::pow(2.0, (midiNote - 69) / 12.0);
    double rootFreq = 440.0 * std::pow(2.0, (rootNote - 69) / 12.0);

    double pitchRatio = targetFreq / rootFreq;
    double sampleRateRatio = targetSampleRate / sampleRate;

    return pitchRatio * sampleRateRatio;
}
```

**Linear Interpolation**:
```cpp
float getInterpolatedSample(const juce::AudioBuffer<float>& buffer,
                           int channel, double position) const {
    int pos = (int)position;
    float frac = position - pos;

    if (pos < 0 || pos + 1 >= buffer.getNumSamples())
        return 0.0f;

    float sample1 = buffer.getSample(channel, pos);
    float sample2 = buffer.getSample(channel, pos + 1);

    return sample1 * (1.0f - frac) + sample2 * frac;
}
```

**Loop Playback**:
```cpp
// Inside processSample()
if (inLoop) {
    if (loopMode == Sample::LoopMode::Forward) {
        if (playbackPosition >= loopEnd) {
            playbackPosition = loopStart;
        }
    }
    else if (loopMode == Sample::LoopMode::PingPong) {
        if (playbackPosition >= loopEnd && loopDirection == 1) {
            loopDirection = -1;
            playbackPosition = loopEnd;
        }
        else if (playbackPosition <= loopStart && loopDirection == -1) {
            loopDirection = 1;
            playbackPosition = loopStart;
        }
    }
}
```

**Envelope & Filter**:
```cpp
// Dual ADSRs: amplitude + filter
ADSREnvelope ampEnvelope;
ADSREnvelope filterEnvelope;
MultiModeFilter filter;

// Process
float ampEnv = ampEnvelope.getNextValue(deltaTime);
float filtEnv = filterEnvelope.getNextValue(deltaTime);

float modCutoff = baseCutoff * (1.0f + envAmount * filtEnv);
filter.setParameters(modCutoff, resonance, sampleRate);

float output = sample * velocity * ampEnv;
output = filter.processSample(output, channel);
```

---

### 5. PluginProcessor (`Source/PluginProcessor.h/cpp`)

**Purpose**: Main audio processor with MIDI handling and parameter management.

**Parameters (20 total)**:
```cpp
// Amplitude Envelope
PARAM_AMP_ATTACK    = "amp_attack"     // 0.001 - 2.0s
PARAM_AMP_DECAY     = "amp_decay"      // 0.001 - 2.0s
PARAM_AMP_SUSTAIN   = "amp_sustain"    // 0.0 - 1.0
PARAM_AMP_RELEASE   = "amp_release"    // 0.001 - 5.0s

// Filter
PARAM_FILTER_CUTOFF     = "filter_cutoff"      // 20 - 20000 Hz
PARAM_FILTER_RESONANCE  = "filter_resonance"   // 0.1 - 10.0
PARAM_FILTER_TYPE       = "filter_type"        // 0-3 (LP/HP/BP/Notch)
PARAM_FILTER_ENV_AMOUNT = "filter_env_amount"  // 0.0 - 1.0

// Filter Envelope
PARAM_FILT_ATTACK   = "filt_attack"
PARAM_FILT_DECAY    = "filt_decay"
PARAM_FILT_SUSTAIN  = "filt_sustain"
PARAM_FILT_RELEASE  = "filt_release"

// Sample
PARAM_SAMPLE_START  = "sample_start"   // 0.0 - 1.0
PARAM_SAMPLE_END    = "sample_end"     // 0.0 - 1.0
PARAM_LOOP_MODE     = "loop_mode"      // 0-2 (None/Forward/PingPong)
PARAM_LOOP_START    = "loop_start"     // 0.0 - 1.0
PARAM_LOOP_END      = "loop_end"       // 0.0 - 1.0

// Global
PARAM_MASTER_VOLUME = "master_volume"  // 0.0 - 1.0
PARAM_MASTER_TUNE   = "master_tune"    // -24 - +24 semitones
PARAM_VOICE_COUNT   = "voice_count"    // 1 - 32
```

**Voice Management**:
```cpp
AudioForge::VoiceManager<SamplerVoice, 16> voiceManager;

void prepareToPlay(double sampleRate, int samplesPerBlock) override {
    voiceManager.prepareToPlay(sampleRate, samplesPerBlock);
}

void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override {
    // Update all voices with current parameters
    for (auto& voice : voiceManager.getVoices()) {
        voice.setEnvelope(attack, decay, sustain, release);
        voice.setFilter(cutoff, resonance, filterType);
    }

    // Render voices
    voiceManager.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}
```

**State Serialization**:
```cpp
void getStateInformation(MemoryBlock& destData) override {
    auto state = apvts.copyState();
    auto xml = state.createXml();

    // Add custom sampler state
    auto samplerState = xml->createNewChildElement("SamplerState");

    // Save sample paths (relative to project directory)
    auto samplesXml = samplerState->createNewChildElement("Samples");
    for (int i = 0; i < sampleLibrary.getNumSamples(); ++i) {
        auto sample = sampleLibrary.getSample(i);
        auto sampleXml = samplesXml->createNewChildElement("Sample");
        sampleXml->setAttribute("path", sample->getFilePath());
        sampleXml->setAttribute("index", i);
    }

    // Save key zones
    auto zonesXml = samplerState->createNewChildElement("KeyZones");
    for (const auto& zone : keyZoneMap.getZones()) {
        auto zoneXml = zonesXml->createNewChildElement("Zone");
        zoneXml->setAttribute("noteMin", zone.noteMin);
        zoneXml->setAttribute("noteMax", zone.noteMax);
        zoneXml->setAttribute("rootNote", zone.rootNote);

        for (const auto& layer : zone.velocityLayers) {
            auto layerXml = zoneXml->createNewChildElement("VelocityLayer");
            layerXml->setAttribute("sampleIndex", layer.sampleIndex);
            layerXml->setAttribute("velocityMin", layer.velocityMin);
            layerXml->setAttribute("velocityMax", layer.velocityMax);
        }
    }

    copyXmlToBinary(*xml, destData);
}
```

---

## UI Components

### 6. WaveformDisplay (`Source/WaveformDisplay.h/cpp`)

**Purpose**: Displays audio waveform with editable loop markers.

**Features**:
- Min/max waveform rendering
- Draggable loop markers (yellow handles)
- Zoom controls (Cmd+wheel horizontal, Shift+wheel vertical)
- Sample info display

**Drawing Pipeline**:
```cpp
void paint(Graphics& g) override {
    auto bounds = getLocalBounds().toFloat();

    drawWaveform(g, bounds);
    drawLoopMarkers(g, bounds);
    drawSampleInfo(g, bounds);
}

void drawWaveform(Graphics& g, Rectangle<float> bounds) {
    if (!currentSample) return;

    const auto& audioData = currentSample->getAudioData();
    int numSamples = audioData.getNumSamples();

    // Calculate visible range based on zoom
    int startSample = (int)(scrollPosition * numSamples);
    int endSample = std::min(startSample + (int)(numSamples / zoomLevel), numSamples);

    // Draw min/max waveform
    Path waveformPath;
    for (int x = 0; x < bounds.getWidth(); ++x) {
        int sampleIndex = jmap(x, 0.0f, bounds.getWidth(),
                              (float)startSample, (float)endSample);

        // Find min/max in window around sample
        float minVal = 0.0f, maxVal = 0.0f;
        findMinMaxInWindow(audioData, sampleIndex, minVal, maxVal);

        float minY = jmap(minVal, -verticalZoom, verticalZoom,
                         bounds.getBottom(), bounds.getTop());
        float maxY = jmap(maxVal, -verticalZoom, verticalZoom,
                         bounds.getBottom(), bounds.getTop());

        waveformPath.addLineSegment(Line<float>(x, minY, x, maxY), 1.0f);
    }

    g.setColour(Colours::lightblue);
    g.strokePath(waveformPath, PathStrokeType(1.0f));
}
```

**Listener Pattern**:
```cpp
class Listener {
    virtual void loopPointsChanged(double loopStartSec, double loopEndSec) = 0;
};

void mouseDrag(const MouseEvent& e) override {
    if (isDraggingLoopStart) {
        double newLoopStart = xToSeconds(e.x, getLocalBounds().toFloat());
        currentSample->setLoopPoints(newLoopStart, currentSample->getLoopPoints().getEnd());

        listeners.call([&](Listener& l) {
            l.loopPointsChanged(newLoopStart, currentSample->getLoopPoints().getEnd());
        });
    }
}
```

---

### 7. ZoneEditor (`Source/ZoneEditor.h/cpp`)

**Purpose**: Visual piano keyboard with colored key zones.

**Features**:
- 88 keys (A0 to C8)
- 8 distinct zone colors
- Drag edges to resize zones
- Right-click menu (Add/Delete/Set Root)
- Yellow markers for root notes

**Layout**:
```cpp
void paint(Graphics& g) override {
    auto bounds = getLocalBounds();

    // Draw piano keyboard background
    drawPianoKeys(g, bounds);

    // Draw colored zones on top
    for (int i = 0; i < keyZoneMap.getNumZones(); ++i) {
        const auto& zone = keyZoneMap.getZones()[i];

        Colour zoneColor = getZoneColor(i);
        g.setColour(zoneColor.withAlpha(0.5f));

        float startX = getNoteX(zone.noteMin);
        float endX = getNoteX(zone.noteMax + 1);

        g.fillRect(startX, 0.0f, endX - startX, (float)bounds.getHeight());

        // Draw root note marker
        if (zone.rootNote >= zone.noteMin && zone.rootNote <= zone.noteMax) {
            float rootX = getNoteX(zone.rootNote);
            g.setColour(Colours::yellow);
            g.fillRect(rootX, 0.0f, 2.0f, (float)bounds.getHeight());
        }
    }
}
```

**Hit Testing**:
```cpp
bool isNearZoneEdge(int zoneIndex, int midiNote, bool& isLeftEdge) const {
    const auto& zone = keyZoneMap.getZones()[zoneIndex];

    if (std::abs(midiNote - zone.noteMin) <= 1) {
        isLeftEdge = true;
        return true;
    }
    if (std::abs(midiNote - zone.noteMax) <= 1) {
        isLeftEdge = false;
        return true;
    }
    return false;
}
```

---

### 8. VelocityLayerEditor (`Source/VelocityLayerEditor.h/cpp`)

**Purpose**: Visual editor for velocity layers with draggable boundaries.

**Features**:
- Vertical velocity scale (0-127)
- Colored layer rectangles
- Draggable boundaries between layers
- Sample index display in each layer

**Boundary Dragging**:
```cpp
void mouseDrag(const MouseEvent& e) override {
    if (draggingBoundaryIndex >= 0 && currentZone) {
        float newVelocity = yToVelocity(e.y);
        newVelocity = jlimit(0.0f, 1.0f, newVelocity);

        // Update layer boundaries
        if (draggingBoundaryIndex < currentZone->velocityLayers.size()) {
            currentZone->velocityLayers[draggingBoundaryIndex].velocityMax = newVelocity;
        }
        if (draggingBoundaryIndex + 1 < currentZone->velocityLayers.size()) {
            currentZone->velocityLayers[draggingBoundaryIndex + 1].velocityMin = newVelocity;
        }

        repaint();
    }
}
```

---

### 9. SampleBrowser (`Source/SampleBrowser.h/cpp`)

**Purpose**: List view of all loaded samples with drag-and-drop support.

**ListBoxModel Implementation**:
```cpp
int getNumRows() override {
    return sampleLibrary.getNumSamples();
}

void paintListBoxItem(int rowNumber, Graphics& g,
                     int width, int height, bool rowIsSelected) override {
    if (rowIsSelected) {
        g.fillAll(Colour(0xff4a9eff).withAlpha(0.3f));
    }

    auto sample = sampleLibrary.getSample(rowNumber);
    if (!sample) return;

    double lengthSec = sample->getLengthInSamples() / sample->getSampleRate();
    size_t sizeKB = sample->getSizeInBytes() / 1024;

    String info = String::formatted("[%d] Sample %d (%.1fs, %d KB)",
                                   rowNumber, rowNumber, lengthSec, (int)sizeKB);

    g.setColour(Colours::white);
    g.drawText(info, 5, 0, width - 10, height,
              Justification::centredLeft, true);
}
```

---

## Testing Strategy

### Unit Tests (`Source/SamplerTests.cpp`)

**Test Categories**:
- Sample loading and management
- Key zone mapping and lookup
- Velocity layer selection
- Voice allocation and playback
- State serialization
- Performance benchmarks

**Running Tests**:
```bash
cd /Users/kentino/Projects/Active/AudioForge/plugins/SamplerPlugin
cmake -B build -DBUILD_TESTING=ON
cmake --build build
./build/SamplerPluginTests
```

**Expected Output**:
```
Running test: Sampler Plugin Tests
  Test: Sample: Load WAV file ... PASSED
  Test: Sample: Loop points ... PASSED
  Test: Sample: Memory calculation ... PASSED
  ...
  30/30 tests passed
```

---

## Performance Characteristics

### CPU Usage

**16-Voice Polyphony** (measured at 44.1 kHz, 512 sample buffer):
- Sample playback: ~0.5% per voice (linear interpolation)
- Filter processing: ~1% per voice (biquad IIR)
- Envelope processing: ~0.1% per voice

**Total**: ~8-10% CPU for 16 simultaneous voices

**Optimization Opportunities**:
- SIMD for sample processing (AVX2, NEON)
- Voice pre-rendering in background thread
- Sample rate conversion cache

### Memory Usage

**Sample Memory**: 512 MB budget
- Typical: 100-200 MB for full instrument library
- Max: 512 MB (enforced by SampleLibrary)

**Plugin Overhead**: ~5 MB
- Code + UI resources: ~2 MB
- JUCE framework: ~3 MB

**Total RAM**: ~200-500 MB typical

---

## Future Enhancements

### v1.6.0 Roadmap

**User Presets**:
- Save/load custom presets
- Preset browser with search
- Tag-based organization

**Round-Robin**:
- Multiple samples per layer
- Cycle through variations

**Sample Editing**:
- Normalize, reverse, trim
- Pitch detection and auto-tuning
- Non-destructive edits

**Modulation Matrix**:
- LFOs modulating cutoff, volume, pan
- Velocity → filter, envelope parameters

**Effects**:
- Built-in reverb, delay
- Per-zone FX send

---

## Build Instructions

### Prerequisites

- JUCE 8.x
- CMake 3.15+
- C++17 compiler (Xcode 12+ / GCC 9+ / MSVC 2019+)

### Build Steps

```bash
cd plugins/SamplerPlugin
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Install to system plugin folder
cp -r build/SamplerPlugin_artefacts/VST3/Sampler.vst3 \
     ~/Library/Audio/Plug-Ins/VST3/
```

### Build Targets

- `SamplerPlugin`: Main plugin (VST3, AU, Standalone)
- `SamplerPluginTests`: Unit tests
- `SamplerPlugin_Standalone`: Standalone app

---

## Code Style Guidelines

**Naming Conventions**:
- Classes: `PascalCase`
- Functions: `camelCase`
- Private members: `camelCase`
- Constants: `UPPER_SNAKE_CASE`

**File Organization**:
- One class per .h/.cpp file pair
- Header guards: `#pragma once`
- Include order: Standard library → JUCE → AudioForge → Local

**Comments**:
- Doxygen-style for public API
- Inline comments for complex logic
- TODO/FIXME for future work

---

## Contributing

See main AudioForge CONTRIBUTING.md for guidelines.

**Sampler-Specific**:
- Add tests for new features
- Update USER_GUIDE.md for UI changes
- Maintain memory budget enforcement
- Preserve thread safety in audio callback

---

## License

GPL-3.0 (same as AudioForge project)
