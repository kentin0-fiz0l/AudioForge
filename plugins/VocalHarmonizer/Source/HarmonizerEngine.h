#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <vector>

class HarmonizerEngine {
public:
    enum class Scale {
        Chromatic = 0,
        Major,
        Minor,
        Dorian,
        Phrygian,
        Lydian,
        Mixolydian,
        Aeolian,
        Locrian,
        HarmonicMinor,
        MelodicMinor,
        Pentatonic
    };

    HarmonizerEngine();
    ~HarmonizerEngine() = default;

    void prepare(double sampleRate, int samplesPerBlock);
    void reset();
    void process(juce::AudioBuffer<float>& buffer);

    // Parameter setters
    void setVoiceCount(int count);              // 1-4 voices
    void setVoicePitch(int voiceIndex, float semitones); // -24 to +24 semitones
    void setVoicePan(int voiceIndex, float pan); // -100 to +100
    void setScale(Scale scale);
    void setFormantShift(float semitones);      // -12 to +12 semitones
    void setHighPass(float freqHz);             // 20-500 Hz
    void setLowPass(float freqHz);              // 2000-20000 Hz
    void setMix(float mix);                     // 0-1
    void setOutputGain(float gainDb);           // -12 to +12 dB

private:
    struct Voice {
        juce::AudioBuffer<float> delayBuffer;
        int writePos = 0;
        float pitchShift = 0.0f;        // In semitones
        float panPosition = 0.0f;       // -1 to +1
        float readSpeed = 1.0f;         // Playback speed for pitch shifting
        float readPos = 0.0f;           // Fractional read position

        // Formant preservation buffers
        juce::AudioBuffer<float> formantBuffer;
    };

    float quantizePitchToScale(float semitones);
    void applyFormantPreservation(Voice& voice, const juce::AudioBuffer<float>& input,
                                  juce::AudioBuffer<float>& output);
    float readInterpolated(const Voice& voice, int channel, float offset);

    double sampleRate_ = 44100.0;
    int voiceCount_ = 2;
    std::vector<Voice> voices_;

    Scale currentScale_ = Scale::Chromatic;
    float formantShift_ = 0.0f;
    float mix_ = 0.5f;
    float outputGain_ = 1.0f;

    static constexpr int kMaxDelayMs = 100;
    int maxDelaySamples_ = 4410; // 100ms at 44.1kHz

    // Filters for each voice
    std::vector<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                                 juce::dsp::IIR::Coefficients<float>>> highPassFilters_;
    std::vector<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                                 juce::dsp::IIR::Coefficients<float>>> lowPassFilters_;

    float highPassFreq_ = 100.0f;
    float lowPassFreq_ = 12000.0f;

    // Scale definitions (semitone offsets from root)
    static const std::vector<int>& getScaleIntervals(Scale scale);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HarmonizerEngine)
};
