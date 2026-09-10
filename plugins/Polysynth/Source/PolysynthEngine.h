#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>

class PolysynthEngine
{
public:
    static constexpr int MAX_UNISON_VOICES = 6;

    PolysynthEngine();

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void reset();
    float processSample();

    void noteOn(float frequency, float velocity);
    void noteOff();

    // Oscillators
    void setOscMix(float mix) { oscMix_ = juce::jlimit(0.0f, 1.0f, mix); }
    void setDetune(float semitones) { detune_ = semitones; }
    void setUnisonVoices(int voices) { unisonVoices_ = juce::jlimit(1, MAX_UNISON_VOICES, voices); }
    void setUnisonDetune(float cents) { unisonDetune_ = cents; }

    // Filter
    void setFilterCutoff(float cutoff) { filterCutoff_ = juce::jlimit(20.0f, 20000.0f, cutoff); }
    void setFilterResonance(float resonance) { filterResonance_ = juce::jlimit(0.0f, 1.0f, resonance); }
    void setFilterMix(float mix) { filterMix_ = juce::jlimit(0.0f, 1.0f, mix); } // 0=LP, 1=HP

    // Envelope
    void setAttack(float attack) { envelope_.setAttack(attack); }
    void setDecay(float decay) { envelope_.setDecay(decay); }
    void setSustain(float sustain) { envelope_.setSustain(sustain); }
    void setRelease(float release) { envelope_.setRelease(release); }

    // Chorus
    void setChorusDepth(float depth) { chorusDepth_ = depth; }
    void setChorusRate(float rate) { chorusRate_ = rate; }

    bool isActive() const { return envelope_.isActive(); }

private:
    struct ADSREnvelope
    {
        enum class Stage { Idle, Attack, Decay, Sustain, Release };

        double sampleRate = 44100.0;
        Stage stage = Stage::Idle;
        float output = 0.0f;

        float attack = 0.01f;
        float decay = 0.2f;
        float sustain = 0.7f;
        float release = 0.5f;

        void setAttack(float a) { attack = juce::jlimit(0.001f, 2.0f, a); }
        void setDecay(float d) { decay = juce::jlimit(0.001f, 2.0f, d); }
        void setSustain(float s) { sustain = juce::jlimit(0.0f, 1.0f, s); }
        void setRelease(float r) { release = juce::jlimit(0.001f, 5.0f, r); }

        void trigger() { stage = Stage::Attack; }
        void release_() { stage = Stage::Release; }
        void reset() { stage = Stage::Idle; output = 0.0f; }
        bool isActive() const { return stage != Stage::Idle; }

        float process()
        {
            float attackRate = 1.0f / (attack * static_cast<float>(sampleRate));
            float decayRate = 1.0f / (decay * static_cast<float>(sampleRate));
            float releaseRate = 1.0f / (release * static_cast<float>(sampleRate));

            switch (stage)
            {
                case Stage::Attack:
                    output += attackRate;
                    if (output >= 1.0f) { output = 1.0f; stage = Stage::Decay; }
                    break;
                case Stage::Decay:
                    output -= decayRate * (1.0f - sustain);
                    if (output <= sustain) { output = sustain; stage = Stage::Sustain; }
                    break;
                case Stage::Sustain:
                    output = sustain;
                    break;
                case Stage::Release:
                    output -= releaseRate * sustain;
                    if (output <= 0.0f) { output = 0.0f; stage = Stage::Idle; }
                    break;
                case Stage::Idle:
                    output = 0.0f;
                    break;
            }
            return output;
        }
    };

    struct OnePoleFilter
    {
        float state = 0.0f;
        float coeff = 0.5f;
        bool isHighpass = false;

        void reset() { state = 0.0f; }

        float process(float input)
        {
            state += coeff * (input - state);
            return isHighpass ? (input - state) : state;
        }
    };

    float generateOscillator(float& phase, float frequency, bool isSaw);

    double sampleRate_ = 44100.0;

    // Unison oscillators
    float unisonPhases_[MAX_UNISON_VOICES][2]{}; // [voice][osc]
    float unisonDetuneAmounts_[MAX_UNISON_VOICES]{};
    int unisonVoices_ = 1;
    float unisonDetune_ = 10.0f;

    // Oscillator parameters
    float oscMix_ = 0.5f;
    float detune_ = -12.0f;
    float baseFrequency_ = 440.0f;
    float velocity_ = 1.0f;

    // Filter
    OnePoleFilter lpFilter_;
    OnePoleFilter hpFilter_;
    float filterCutoff_ = 2000.0f;
    float filterResonance_ = 0.0f;
    float filterMix_ = 0.0f; // 0=LP only, 1=HP only

    // Envelope
    ADSREnvelope envelope_;

    // Chorus
    float chorusPhase_ = 0.0f;
    float chorusDepth_ = 0.0f;
    float chorusRate_ = 2.0f;
};
