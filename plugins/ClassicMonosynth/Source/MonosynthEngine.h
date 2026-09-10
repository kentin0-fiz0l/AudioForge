#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>

class MonosynthEngine
{
public:
    MonosynthEngine();

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void reset();
    float processSample(float targetFrequency);

    // Oscillators
    void setOsc1Level(float level) { osc1Level_ = level; }
    void setOsc2Level(float level) { osc2Level_ = level; }
    void setOsc3Level(float level) { osc3Level_ = level; }
    void setOsc2Detune(float semitones) { osc2Detune_ = semitones; }
    void setOsc3Detune(float semitones) { osc3Detune_ = semitones; }
    void setPulseWidth(float width) { pulseWidth_ = juce::jlimit(0.01f, 0.99f, width); }
    void setOscSync(bool enabled) { oscSync_ = enabled; }

    // Filter
    void setFilterCutoff(float cutoff) { filterCutoff_ = juce::jlimit(20.0f, 20000.0f, cutoff); }
    void setFilterResonance(float resonance) { filterResonance_ = juce::jlimit(0.0f, 1.0f, resonance); }
    void setFilterEnvAmount(float amount) { filterEnvAmount_ = amount; }

    // Envelopes
    void setFilterAttack(float attack) { filterEnv_.setAttack(attack); }
    void setFilterDecay(float decay) { filterEnv_.setDecay(decay); }
    void setFilterSustain(float sustain) { filterEnv_.setSustain(sustain); }
    void setFilterRelease(float release) { filterEnv_.setRelease(release); }

    void setAmpAttack(float attack) { ampEnv_.setAttack(attack); }
    void setAmpDecay(float decay) { ampEnv_.setDecay(decay); }
    void setAmpSustain(float sustain) { ampEnv_.setSustain(sustain); }
    void setAmpRelease(float release) { ampEnv_.setRelease(release); }

    // LFOs
    void setLfo1Rate(float rate) { lfo1Rate_ = rate; }
    void setLfo1Amount(float amount) { lfo1Amount_ = amount; }
    void setLfo2Rate(float rate) { lfo2Rate_ = rate; }
    void setLfo2Amount(float amount) { lfo2Amount_ = amount; }

    // Glide
    void setGlideTime(float time) { glideTime_ = time; }

    void noteOn();
    void noteOff();

private:
    struct ADSREnvelope
    {
        enum class Stage { Idle, Attack, Decay, Sustain, Release };

        double sampleRate = 44100.0;
        Stage stage = Stage::Idle;
        float output = 0.0f;

        float attack = 0.01f;
        float decay = 0.1f;
        float sustain = 0.7f;
        float release = 0.3f;

        void setAttack(float a) { attack = juce::jlimit(0.001f, 2.0f, a); }
        void setDecay(float d) { decay = juce::jlimit(0.001f, 2.0f, d); }
        void setSustain(float s) { sustain = juce::jlimit(0.0f, 1.0f, s); }
        void setRelease(float r) { release = juce::jlimit(0.001f, 5.0f, r); }

        void trigger()
        {
            stage = Stage::Attack;
        }

        void release_()
        {
            stage = Stage::Release;
        }

        void reset()
        {
            stage = Stage::Idle;
            output = 0.0f;
        }

        float process()
        {
            float attackRate = 1.0f / (attack * static_cast<float>(sampleRate));
            float decayRate = 1.0f / (decay * static_cast<float>(sampleRate));
            float releaseRate = 1.0f / (release * static_cast<float>(sampleRate));

            switch (stage)
            {
                case Stage::Attack:
                    output += attackRate;
                    if (output >= 1.0f)
                    {
                        output = 1.0f;
                        stage = Stage::Decay;
                    }
                    break;

                case Stage::Decay:
                    output -= decayRate * (1.0f - sustain);
                    if (output <= sustain)
                    {
                        output = sustain;
                        stage = Stage::Sustain;
                    }
                    break;

                case Stage::Sustain:
                    output = sustain;
                    break;

                case Stage::Release:
                    output -= releaseRate * sustain;
                    if (output <= 0.0f)
                    {
                        output = 0.0f;
                        stage = Stage::Idle;
                    }
                    break;

                case Stage::Idle:
                    output = 0.0f;
                    break;
            }

            return output;
        }
    };

    struct MoogLadderFilter
    {
        float stage[4] = {0, 0, 0, 0};
        float stageZ1[4] = {0, 0, 0, 0};
        float stageTanh[3] = {0, 0, 0};

        double sampleRate = 44100.0;
        float cutoff = 1000.0f;
        float resonance = 0.0f;

        void reset()
        {
            for (int i = 0; i < 4; ++i)
            {
                stage[i] = 0.0f;
                stageZ1[i] = 0.0f;
            }
            for (int i = 0; i < 3; ++i)
                stageTanh[i] = 0.0f;
        }

        float process(float input)
        {
            // Moog ladder filter approximation
            float f = cutoff / static_cast<float>(sampleRate);
            f = juce::jlimit(0.0f, 0.45f, f);

            float fb = resonance * 4.0f;
            input -= stage[3] * fb;
            input *= 0.3f;

            // 4-stage cascade
            for (int pole = 0; pole < 4; ++pole)
            {
                stage[pole] = stageZ1[pole] + f * (std::tanh(input) - std::tanh(stageZ1[pole]));
                stageZ1[pole] = stage[pole];
                input = stage[pole];
            }

            return stage[3];
        }
    };

    float generateOscillator(float& phase, float frequency, int waveform, float pw);
    void updateGlide(float targetFrequency);

    double sampleRate_ = 44100.0;

    // Oscillators
    float osc1Phase_ = 0.0f;
    float osc2Phase_ = 0.0f;
    float osc3Phase_ = 0.0f;
    float osc1Level_ = 0.5f;
    float osc2Level_ = 0.5f;
    float osc3Level_ = 0.0f;
    float osc2Detune_ = -12.0f; // Semitones
    float osc3Detune_ = 12.0f;
    float pulseWidth_ = 0.5f;
    bool oscSync_ = false;
    float syncMaster_ = 0.0f;

    // Filter
    MoogLadderFilter filter_;
    float filterCutoff_ = 1000.0f;
    float filterResonance_ = 0.0f;
    float filterEnvAmount_ = 2000.0f;

    // Envelopes
    ADSREnvelope filterEnv_;
    ADSREnvelope ampEnv_;

    // LFOs
    float lfo1Phase_ = 0.0f;
    float lfo2Phase_ = 0.0f;
    float lfo1Rate_ = 5.0f;
    float lfo2Rate_ = 3.0f;
    float lfo1Amount_ = 0.0f;
    float lfo2Amount_ = 0.0f;

    // Glide
    float glideTime_ = 0.0f;
    float currentFrequency_ = 0.0f;
};
