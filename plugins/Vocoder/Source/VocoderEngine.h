#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class VocoderEngine {
public:
    VocoderEngine();
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples);

    void setNumBands(int bands) { numBands_ = juce::jlimit(8, 32, bands); }
    void setFormantShift(float shift) { formantShift_ = juce::jlimit(0.5f, 2.0f, shift); }
    void setCarrierLevel(float level) { carrierLevel_ = juce::jlimit(0.0f, 1.0f, level); }
    void setModulatorLevel(float level) { modulatorLevel_ = juce::jlimit(0.0f, 1.0f, level); }
    void setAttack(float attack) { attack_ = juce::jlimit(0.001f, 0.1f, attack); }
    void setRelease(float release) { release_ = juce::jlimit(0.01f, 0.5f, release); }
    void setMix(float mix) { mix_ = juce::jlimit(0.0f, 1.0f, mix); }

    void noteOn(int midiNote, float velocity);
    void noteOff(int midiNote);

    float getBandLevel(int band) const;

private:
    static constexpr int MAX_BANDS = 32;
    static constexpr int MAX_VOICES = 8;

    struct VocoderBand {
        juce::dsp::IIR::Filter<float> modulatorFilter;
        juce::dsp::IIR::Filter<float> carrierFilter;
        float envelopeLevel = 0.0f;
        float centerFreq = 0.0f;
    };

    struct Voice {
        float frequency = 0.0f;
        float velocity = 0.0f;
        int midiNote = -1;
        float phase = 0.0f;
        bool active = false;
    };

    int numBands_ = 16;
    float formantShift_ = 1.0f;
    float carrierLevel_ = 0.8f;
    float modulatorLevel_ = 1.0f;
    float attack_ = 0.01f;
    float release_ = 0.05f;
    float mix_ = 1.0f;

    double sampleRate_ = 44100.0;
    VocoderBand bands_[MAX_BANDS];
    Voice voices_[MAX_VOICES];

    juce::AudioBuffer<float> modulatorBuffer_;
    juce::AudioBuffer<float> carrierBuffer_;

    void initializeBands();
    float generateCarrier();
    void processVocoder(juce::AudioBuffer<float>& buffer);
};
