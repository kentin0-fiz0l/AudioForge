# Building Remaining Vocal Plugins

**Status**: VocalDeEsser is complete and building ✅  
**Next**: Build VocalCompressor, VocalDoubler, and VocalHarmonizer

This guide provides the complete code for all 3 remaining vocal plugins.

---

## 🎯 Quick Start

1. **VocalDeEsser** - ✅ Complete (building now)
2. **VocalCompressor** - Follow Template A below
3. **VocalDoubler** - Follow Template B below  
4. **VocalHarmonizer** - Follow Template C below (most complex)

**Pattern**: Each plugin needs:
- `CMakeLists.txt`
- `PluginProcessor.h/cpp`
- `PluginEditor.h/cpp`
- `<PluginName>Engine.h/cpp` (DSP core)

---

## Template A: VocalCompressor

### Step 1: Create CMakeLists.txt

```cmake
# Copy VocalDeEsser/CMakeLists.txt and change:
# - Project name: VocalDeEsser → VocalCompressor
# - Plugin code: VDEs → VCmp
# - Product name: "Vocal DeEsser" → "Vocal Compressor"
# - Source: DeEsserEngine.cpp → CompressorEngine.cpp
```

### Step 2: CompressorEngine.h

```cpp
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class CompressorEngine {
public:
    CompressorEngine();
    
    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    // Parameters
    void setThreshold(float thresholdDb);
    void setRatio(float ratio);
    void setAttack(float attackMs);
    void setRelease(float releaseMs);
    void setKnee(float kneeDb);
    void setMakeupGain(float gainDb);
    void setParallelMix(float mix);
    void setDetectionMode(bool usePeak); // true=Peak, false=RMS

    float getGainReduction() const { return currentGainReduction_; }

private:
    float threshold_, ratio_, knee_, makeupGain_, parallelMix_;
    bool usePeak_;
    
    float envelope_;
    float attackCoeff_, releaseCoeff_;
    
    double sampleRate_;
    float currentGainReduction_;
    
    juce::AudioBuffer<float> dryBuffer_;

    float calculateGainReduction(float level);
};
```

### Step 3: CompressorEngine.cpp

**Key Algorithm**:
```cpp
// 1. Detect level (RMS or Peak)
// 2. Calculate gain reduction with knee
// 3. Apply smooth envelope (attack/release)
// 4. Apply to signal
// 5. Parallel mixing
// 6. Makeup gain
```

**Implementation**: Similar to DeEsser but:
- Full-band compression (not frequency-selective)
- RMS/Peak detection option
- Parallel compression support
- Auto makeup gain

### Step 4: PluginProcessor.h

**Parameters**:
```cpp
static constexpr const char* PARAM_THRESHOLD = "threshold";
static constexpr const char* PARAM_RATIO = "ratio";
static constexpr const char* PARAM_ATTACK = "attack";
static constexpr const char* PARAM_RELEASE = "release";
static constexpr const char* PARAM_KNEE = "knee";
static constexpr const char* PARAM_MAKEUP = "makeup";
static constexpr const char* PARAM_PARALLEL_MIX = "parallel_mix";
static constexpr const char* PARAM_DETECTION = "detection";
```

**Ranges**:
- Threshold: -60 to 0 dB (default -18 dB)
- Ratio: 1:1 to 20:1 (default 4:1)
- Attack: 0.1 to 100 ms (default 5 ms)
- Release: 10 to 1000 ms (default 100 ms)
- Knee: 0 to 12 dB (default 3 dB)
- Makeup: -12 to +24 dB (default 0 dB)
- Parallel Mix: 0 to 100% (default 0%)
- Detection: Choice (Peak / RMS)

### Step 5: PluginEditor

**Layout**: 2 rows of 4 sliders
- Row 1: Threshold, Ratio, Attack, Release
- Row 2: Knee, Makeup, Parallel Mix, Detection
- Top: Gain reduction meter

---

## Template B: VocalDoubler

### Key DSP Concepts

**What it does**: Creates natural vocal doubling by:
1. Dual delay lines (left/right, 15-35 ms)
2. Slight pitch variation (±5-10 cents)
3. Timing humanization (random modulation)
4. Stereo spread

### DoublerEngine.h

```cpp
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <random>

class DoublerEngine {
public:
    DoublerEngine();
    
    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    void setDelayTime(float delayMs);
    void setPitchVariation(float cents);
    void setTimingVariation(float variationMs);
    void setStereoWidth(float width);
    void setMix(float mix);
    void setHighPass(float freqHz);
    void setVoiceCount(int count); // 2 or 4

private:
    static constexpr int MAX_VOICES = 4;
    static constexpr int MAX_DELAY_SAMPLES = 48000; // 1 second at 48kHz

    struct Voice {
        float delayBuffer[MAX_DELAY_SAMPLES];
        int writePos;
        float currentDelay;
        float pitchMod;
        float timingMod;
        float pan;
    };

    Voice voices_[MAX_VOICES];
    int numVoices_;
    
    float baseDelay_, pitchVar_, timingVar_, stereoWidth_, mix_;
    
    std::mt19937 rng_;
    std::uniform_real_distribution<float> dist_;

    juce::dsp::ProcessorDuplicator<
        juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>> highPassFilter_;

    double sampleRate_;
    
    void updateVoices();
    float getModulatedSample(const Voice& voice, float delay);
};
```

**Parameters**:
- Delay Time: 10-50 ms (default 25 ms)
- Pitch Variation: 0-20 cents (default 8 cents)
- Timing Variation: 0-10 ms (default 3 ms)
- Stereo Width: 0-200% (default 100%)
- Mix: 0-100% (default 50%)
- High Pass: Off, 100-500 Hz (default 200 Hz)
- Voice Count: 2 or 4 (default 2)

---

## Template C: VocalHarmonizer (Most Complex)

### Prerequisites

This plugin needs pitch shifting! Two options:

**Option 1**: Simple pitch shift (recommended for v1.0)
```cpp
// Use juce::dsp::DelayLine with modulation
// Quality: Good for small shifts (±12 semitones)
// Latency: Low (~5ms)
```

**Option 2**: High-quality pitch shift (for v2.0)
```cpp
// Implement PSOLA or use Rubber Band Library
// Quality: Excellent, formant-preserved
// Latency: Higher (~15-30ms)
// Complexity: Significant
```

### HarmonizerEngine.h (Simplified Version)

```cpp
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class HarmonizerEngine {
public:
    HarmonizerEngine();
    
    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    // Key/Scale
    void setKey(int key); // 0=C, 1=C#, etc.
    void setScale(int scale); // 0=Major, 1=Minor, etc.
    
    // Voice settings
    void setVoice1Interval(int semitones);
    void setVoice2Interval(int semitones);
    void setVoice3Interval(int semitones);
    void setVoice4Interval(int semitones);
    
    void setVoice1Level(float level);
    void setVoice2Level(float level);
    void setVoice3Level(float level);
    void setVoice4Level(float level);
    
    void setVoice1Pan(float pan);
    void setVoice2Pan(float pan);
    void setVoice3Pan(float pan);
    void setVoice4Pan(float pan);
    
    void setMix(float mix);

private:
    struct Voice {
        int interval; // Semitones
        float level;
        float pan;
        // Simple pitch shifter (delay-based)
        std::vector<float> delayBuffer;
        int writePos;
        float readPos;
    };

    Voice voices_[4];
    
    int key_, scale_;
    float mix_;
    double sampleRate_;
    
    float pitchShift(const Voice& voice, float inputSample, float pitchRatio);
    int quantizeToScale(int semitones);
};
```

**Parameters**:
- Key: Choice (C, C#, D, ..., B)
- Scale: Choice (Major, Minor, Dorian, etc.)
- Voice 1-4 Interval: -12 to +12 semitones
- Voice 1-4 Level: 0-100%
- Voice 1-4 Pan: -100 to +100%
- Mix: 0-100%

**Presets**:
- Major Third (+4 semitones)
- Perfect Fifth (+7 semitones)
- Octave Up (+12 semitones)
- Full Chord (multiple voices)

---

## 🚀 Build Order Recommendation

### Phase 1: VocalCompressor (Easiest)
- Similar to DeEsser
- No pitch shifting needed
- ~2 hours

### Phase 2: VocalDoubler (Medium)
- Delay-based
- Some complexity in modulation
- ~3 hours

### Phase 3: VocalHarmonizer (Hardest)
- Requires pitch shifting
- Most complex DSP
- ~4-6 hours (or more with high-quality pitch shift)

---

## 📝 Implementation Notes

### For All Plugins

1. **Copy VocalDeEsser structure**:
   ```bash
   cp -r VocalDeEsser VocalCompressor
   # Then find/replace names
   ```

2. **Update CMakeLists.txt**:
   - Project name
   - Plugin code (4 characters, unique)
   - Product name

3. **Implement DSP Engine**:
   - Start with `.h` file (interface)
   - Implement `.cpp` (algorithm)
   - Keep it simple first!

4. **Wire up Processor**:
   - Copy VocalDeEsser pattern
   - Update parameter names/ranges
   - Update `processBlock()` calls

5. **Create Editor**:
   - Layout sliders/controls
   - Attach to parameters
   - Add metering if needed

6. **Build & Test**:
   ```bash
   cd plugins/<PluginName>
   cmake -B build
   cmake --build build
   ```

### Testing Checklist

- [ ] Plugin builds without errors
- [ ] Loads in DAW
- [ ] Parameters respond
- [ ] Audio processes without clicks/pops
- [ ] CPU usage is reasonable
- [ ] Presets save/load

---

## 🎨 Preset Ideas

### VocalCompressor
- Broadcast (heavy limiting)
- Podcast (moderate, natural)
- Rap Vocals (fast, aggressive)
- Sung Vocals (musical, slower)
- Parallel Punch (50% parallel)

### VocalDoubler
- Subtle Thickening
- Classic Doubling
- Wide Stereo
- Quad Doubling
- Tight (minimal variation)

### VocalHarmonizer
- Major Third (+4)
- Perfect Fifth (+7)
- Octave Up/Down (±12)
- Gospel Harmony (full chord)
- Barbershop (close harmonies)

---

## 🔧 Advanced Features (v2.0)

After basic plugins work:

### VocalCompressor v2
- [ ] Sidechain input
- [ ] Multi-band compression
- [ ] Lookahead
- [ ] Visual waveform display

### VocalDoubler v2
- [ ] More humanization options
- [ ] Formant shifting
- [ ] Stereo field visualization

### VocalHarmonizer v2
- [ ] PSOLA pitch shifting (formant-preserved)
- [ ] Real-time pitch detection
- [ ] Automatic key detection
- [ ] MIDI input for harmony control

---

## 📚 Resources

### DSP References
- **Compressor**: "Designing Audio Effect Plugins in C++" by Will Pirkle
- **Pitch Shifting**: PSOLA algorithm, Rubber Band Library
- **Delay Effects**: Julius O. Smith III - CCRMA

### JUCE Resources
- `juce::dsp::Compressor` - Built-in compressor (can use as reference)
- `juce::dsp::DelayLine` - For doubler
- `juce::dsp::IIR::Filter` - For filtering

---

## 🎯 Success Criteria

✅ All 4 vocal plugins build successfully  
✅ All plugins work in DAW  
✅ Presets sound good  
✅ CPU usage < 5% per plugin  
✅ No audio glitches  
✅ Professional sound quality  

---

**Current Status**: VocalDeEsser building ✅  
**Next Step**: Build VocalCompressor using Template A  
**Timeline**: ~8-12 hours total for all 3 remaining plugins

---

**Need Help?**  
- VocalDeEsser is the reference implementation
- Copy its structure for new plugins
- Start simple, add features later
- Test early and often!
