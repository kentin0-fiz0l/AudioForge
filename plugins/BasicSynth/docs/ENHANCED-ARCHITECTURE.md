# Enhanced BasicSynth - Architecture Design

**Version**: v1.5.0
**Status**: Planning
**Goal**: Transform BasicSynth into a powerful subtractive synthesizer

---

## Overview

Enhanced BasicSynth builds on the existing BasicSynth plugin, adding:
- **5 oscillators** (vs. 1 current) with PWM and sub-oscillator
- **Noise generator** for texture
- **2 LFOs** with tempo sync and multiple shapes
- **Filter envelope** (dedicated ADSR)
- **Modulation matrix** (4 slots)
- **Built-in effects** (chorus, delay, reverb)

---

## Current Architecture (BasicSynth v1.0)

### Components:
```
SynthVoice
├── Oscillator (1x)
│   ├── Waveforms: Sine, Sawtooth, Square
│   └── Phase-based generation
├── BiquadFilter (low-pass)
│   ├── Cutoff frequency
│   └── Resonance (Q)
└── ADSR Envelope (amplitude only)
    ├── Attack, Decay, Sustain, Release
    └── Controls oscillator output level
```

### Limitations:
- Single oscillator per voice
- No modulation (LFOs, mod matrix)
- No filter envelope
- No PWM or noise
- No built-in effects
- Basic waveforms only

---

## Enhanced Architecture (v1.5.0)

### Voice Structure:
```
EnhancedSynthVoice
├── Oscillator Bank (5x)
│   ├── Osc 1-3: Main oscillators (sine/saw/square/pulse)
│   ├── Osc 4: Sub-oscillator (-1 octave)
│   └── Osc 5: Noise generator (white/pink)
│
├── Modulation Sources
│   ├── LFO 1 (rate, depth, shape, sync)
│   ├── LFO 2 (rate, depth, shape, sync)
│   ├── Amp Envelope (ADSR)
│   └── Filter Envelope (ADSR)
│
├── Modulation Matrix (4 slots)
│   ├── Source → Destination mapping
│   ├── Amount control per slot
│   └── Available destinations: pitch, filter, PWM, amp
│
├── Filter Section
│   ├── Multi-mode filter (LP/HP/BP/Notch, 12/24dB)
│   ├── Cutoff frequency
│   ├── Resonance with self-oscillation
│   └── Filter envelope routing
│
└── Effects Chain
    ├── Chorus (rate, depth, mix)
    ├── Delay (time, feedback, mix)
    └── Reverb (size, damping, mix)
```

---

## Implementation Plan

### Phase 1: Multiple Oscillators (Week 1, Days 1-3)

**Files to create/modify:**
```
Source/
├── Oscillator.h/cpp (enhance existing)
├── OscillatorBank.h/cpp (NEW)
├── NoiseGenerator.h/cpp (NEW)
└── Voice.h/cpp (update)
```

**OscillatorBank class:**
```cpp
class OscillatorBank
{
public:
    static constexpr int NUM_OSCILLATORS = 5;

    struct OscillatorConfig
    {
        float level;          // Volume (0.0-1.0)
        float detune;         // Semitones (-12 to +12)
        float pulseWidth;     // For pulse wave (0.0-1.0)
        int waveform;         // 0=sine, 1=saw, 2=square, 3=pulse
        bool enabled;
    };

    void setFrequency(float frequency, double sampleRate);
    void configure(int oscIndex, const OscillatorConfig& config);
    float getNextSample(); // Mix all enabled oscillators

private:
    Oscillator oscillators[NUM_OSCILLATORS];
    OscillatorConfig configs[NUM_OSCILLATORS];
};
```

**NoiseGenerator class:**
```cpp
class NoiseGenerator
{
public:
    enum class NoiseType
    {
        White,
        Pink
    };

    float getNextSample(NoiseType type);

private:
    float pinkState[7] = {0}; // For pink noise filtering
    juce::Random random;
};
```

**Changes to Voice.h:**
```cpp
class EnhancedSynthVoice
{
private:
    OscillatorBank oscillatorBank;
    NoiseGenerator noiseGenerator;
    // ... rest of voice state
};
```

---

### Phase 2: LFOs and Modulation (Week 1, Days 4-5)

**Files to create:**
```
Source/
├── LFO.h/cpp (NEW)
└── ModulationMatrix.h/cpp (NEW)
```

**LFO class:**
```cpp
class LFO
{
public:
    enum class Shape
    {
        Sine,
        Triangle,
        Sawtooth,
        Square,
        SampleAndHold
    };

    void setRate(float rateHz);
    void setTempoSync(bool sync, float bpm, float division);
    void setShape(Shape shape);
    float getNextSample(double sampleRate);
    void reset();

private:
    float phase = 0.0f;
    float rate = 1.0f;
    Shape shape = Shape::Sine;
    bool tempoSynced = false;
    juce::Random random; // For S&H
};
```

**ModulationMatrix class:**
```cpp
class ModulationMatrix
{
public:
    enum class Source
    {
        LFO1,
        LFO2,
        AmpEnvelope,
        FilterEnvelope,
        Velocity,
        ModWheel
    };

    enum class Destination
    {
        Pitch,
        FilterCutoff,
        PWM,
        Amplitude,
        PanPosition
    };

    struct Slot
    {
        Source source;
        Destination destination;
        float amount; // -1.0 to +1.0
        bool enabled;
    };

    static constexpr int NUM_SLOTS = 4;

    void setSlot(int index, const Slot& slot);
    float getModulation(Destination dest, const SourceValues& sources);

private:
    Slot slots[NUM_SLOTS];
};
```

---

### Phase 3: Filter Envelope (Week 1, Day 6-7)

**Files to modify:**
```
Source/
└── Voice.h/cpp (add filter envelope)
```

**Add to Voice:**
```cpp
class EnhancedSynthVoice
{
private:
    // Separate envelopes for amp and filter
    ADSREnvelope ampEnvelope;
    ADSREnvelope filterEnvelope;

    // Filter envelope routing
    float filterEnvelopeAmount = 0.5f; // 0.0-1.0
};
```

**ADSR Envelope class (shared by both):**
```cpp
class ADSREnvelope
{
public:
    void trigger();
    void release();
    float getNextValue(float deltaTime);

    void setAttack(float seconds);
    void setDecay(float seconds);
    void setSustain(float level);
    void setRelease(float seconds);

private:
    enum class Stage { Off, Attack, Decay, Sustain, Release };
    Stage stage = Stage::Off;
    float level = 0.0f;
    float time = 0.0f;
    // ... ADSR parameters
};
```

---

### Phase 4: Enhanced Filter (Week 2, Days 1-2)

**Files to create:**
```
Source/
└── MultiModeFilter.h/cpp (NEW)
```

**MultiModeFilter class:**
```cpp
class MultiModeFilter
{
public:
    enum class Mode
    {
        LowPass12dB,
        LowPass24dB,
        HighPass12dB,
        HighPass24dB,
        BandPass,
        Notch
    };

    void setMode(Mode mode);
    void setCutoff(float frequency, double sampleRate);
    void setResonance(float q); // 0.5 to 20.0
    float processSample(float input);
    void reset();

private:
    Mode mode = Mode::LowPass24dB;
    AudioForge::DSP::BiquadFilter filter1; // For 12dB
    AudioForge::DSP::BiquadFilter filter2; // For 24dB (cascade)
};
```

---

### Phase 5: Built-in Effects (Week 2, Days 3-5)

**Files to create:**
```
Source/
├── ChorusEffect.h/cpp (NEW)
├── DelayEffect.h/cpp (NEW)
└── ReverbEffect.h/cpp (NEW)
```

**Effect interfaces:**
```cpp
class ChorusEffect
{
public:
    void setRate(float hz);
    void setDepth(float depth); // 0.0-1.0
    void setMix(float mix);     // 0.0-1.0
    void processStereo(float& left, float& right, double sampleRate);
};

class DelayEffect
{
public:
    void setTime(float seconds);
    void setFeedback(float feedback); // 0.0-0.95
    void setMix(float mix);
    void processStereo(float& left, float& right, double sampleRate);
};

class ReverbEffect
{
public:
    void setSize(float size);       // 0.0-1.0
    void setDamping(float damping); // 0.0-1.0
    void setMix(float mix);
    void processStereo(float& left, float& right, double sampleRate);
};
```

---

### Phase 6: UI Updates (Week 2, Days 6-7)

**UI Layout:**
```
┌─────────────────────────────────────────────────┐
│ Enhanced BasicSynth                             │
├─────────────────────────────────────────────────┤
│ OSCILLATORS                                     │
│ ┌───┬───┬───┬───┬───┐                          │
│ │Osc│Osc│Osc│Sub│Noi│                          │
│ │ 1 │ 2 │ 3 │ 4 │ 5 │                          │
│ └───┴───┴───┴───┴───┘                          │
│ Level | Detune | PW | Waveform                 │
├─────────────────────────────────────────────────┤
│ FILTER                                          │
│ Mode: [LP24▾] Cutoff [====●====] Resonance     │
│ Envelope: A D S R  Amount [====●====]          │
├─────────────────────────────────────────────────┤
│ LFOs                                            │
│ LFO1: Rate [===●===] Shape [Sine▾] Sync □      │
│ LFO2: Rate [===●===] Shape [Sine▾] Sync □      │
├─────────────────────────────────────────────────┤
│ MODULATION MATRIX                               │
│ Slot 1: [LFO1▾] → [Cutoff▾] Amount [==●==]    │
│ Slot 2: [LFO2▾] → [Pitch▾]  Amount [==●==]    │
│ Slot 3: [Env1▾] → [PWM▾]    Amount [==●==]    │
│ Slot 4: [Env2▾] → [Amp▾]    Amount [==●==]    │
├─────────────────────────────────────────────────┤
│ EFFECTS                                         │
│ Chorus [On □] Delay [On □] Reverb [On □]       │
│ Mix controls for each                           │
└─────────────────────────────────────────────────┘
```

---

## Parameter List (Total: ~50 parameters)

### Oscillators (5 × 5 = 25 params):
- Osc 1-5: Level, Detune, Pulse Width, Waveform, Enable

### Filter (6 params):
- Mode, Cutoff, Resonance, Envelope Amount, Keyboard Tracking

### Envelopes (2 × 4 = 8 params):
- Amp/Filter: Attack, Decay, Sustain, Release

### LFOs (2 × 4 = 8 params):
- LFO 1/2: Rate, Shape, Sync, Depth

### Modulation Matrix (4 × 3 = 12 params):
- Slot 1-4: Source, Destination, Amount

### Effects (3 × 3 = 9 params):
- Chorus/Delay/Reverb: Mix, Param1, Param2

**Total**: ~68 parameters (will need preset management!)

---

## Testing Strategy

### Unit Tests:
- [ ] OscillatorBank: Mix multiple oscillators correctly
- [ ] NoiseGenerator: White/pink noise generation
- [ ] LFO: All waveforms, tempo sync
- [ ] ModulationMatrix: Routing and scaling
- [ ] Envelopes: ADSR timing accuracy
- [ ] MultiModeFilter: All filter modes
- [ ] Effects: Chorus/Delay/Reverb processing

### Integration Tests:
- [ ] Voice allocation with new architecture
- [ ] Polyphony (8 voices) performance
- [ ] Parameter smoothing (no clicks/pops)
- [ ] Preset save/load
- [ ] DAW automation

### Performance Tests:
- [ ] CPU usage <5% (8 voices, all effects on)
- [ ] Latency <5ms
- [ ] No audio dropouts

---

## Migration Path

### Backward Compatibility:
- Keep existing BasicSynth parameters as defaults
- Map old presets to new parameter structure:
  - Old waveform → Osc 1 waveform
  - Old filter → New filter (LP24 mode)
  - Old envelope → Amp envelope
- Provide "Classic" preset that matches v1.0 sound

### Upgrade Path:
1. Copy BasicSynth/ to EnhancedBasicSynth/
2. Implement new classes incrementally
3. Test each phase independently
4. Keep BasicSynth v1.0 available as separate plugin

---

## Timeline

| Phase | Task | Duration | Deliverable |
|-------|------|----------|-------------|
| 1 | Multiple Oscillators | 3 days | 5 oscillators + noise |
| 2 | LFOs & Mod Matrix | 2 days | 2 LFOs, 4 mod slots |
| 3 | Filter Envelope | 1 day | Dedicated filter ADSR |
| 4 | Enhanced Filter | 2 days | Multi-mode filter |
| 5 | Built-in Effects | 3 days | Chorus, delay, reverb |
| 6 | UI Updates | 2 days | New interface layout |
| **Total** | | **13 days** | **~2 weeks** |

---

## Success Criteria

- [ ] 5 oscillators working with independent control
- [ ] 2 LFOs with tempo sync
- [ ] Modulation matrix routing functional
- [ ] Filter envelope shaping filter cutoff
- [ ] Built-in effects (chorus, delay, reverb)
- [ ] 100+ presets showcasing capabilities
- [ ] <5% CPU usage (8 voices, effects on)
- [ ] Comprehensive unit test coverage

---

**Status**: Ready for implementation
**Next Step**: Begin Phase 1 (Multiple Oscillators)
