#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <cmath>

class ShakuhachiEngine
{
public:
    static constexpr int DELAY_BUFFER_SIZE = 8192;

    ShakuhachiEngine();

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void reset();
    float processSample(float baseFrequency, float velocity, bool noteOn, int samplesSinceNoteOn);

    void setBreathPressure(float pressure) { breathPressure_ = juce::jlimit(0.0f, 1.0f, pressure); }
    void setEmbouchure(float emb) { embouchure_ = juce::jlimit(-1.0f, 1.0f, emb); } // -1 = meri, +1 = kari
    void setAirNoise(float amount) { airNoise_ = juce::jlimit(0.0f, 1.0f, amount); }
    void setAttack(float attack) { attackTime_ = attack; }
    void setRelease(float release) { releaseTime_ = release; }
    void setVibratoRate(float rate) { vibratoRate_ = rate; }
    void setVibratoDepth(float depth) { vibratoDepth_ = depth; }
    void setBrightness(float brightness) { brightness_ = juce::jlimit(0.0f, 1.0f, brightness); }

private:
    struct Waveguide
    {
        std::array<float, DELAY_BUFFER_SIZE> buffer{};
        int writePos = 0;
        float delayLength = 0.0f;
        float feedback = 0.98f;

        void reset()
        {
            buffer.fill(0.0f);
            writePos = 0;
        }

        float process(float input)
        {
            int readPos = static_cast<int>(writePos - delayLength);
            if (readPos < 0) readPos += DELAY_BUFFER_SIZE;

            float output = buffer[readPos % DELAY_BUFFER_SIZE];
            buffer[writePos] = input + output * feedback;
            writePos = (writePos + 1) % DELAY_BUFFER_SIZE;

            return output;
        }
    };

    struct OnePoleFilter
    {
        float state = 0.0f;
        float coeff = 0.5f;

        void reset() { state = 0.0f; }

        float process(float input)
        {
            state += coeff * (input - state);
            return state;
        }
    };

    float generateBreathNoise();
    float applyBreathAttack(float sample, int samplesSinceNoteOn);

    double sampleRate_ = 44100.0;

    // Waveguide (bamboo tube resonator)
    Waveguide tube_;

    // Filters
    OnePoleFilter toneLp_;         // Tone shaping
    OnePoleFilter breathLp_;       // Breath noise filtering

    // Parameters
    float breathPressure_ = 0.7f;
    float embouchure_ = 0.0f;      // -1 to +1 (meri to kari)
    float airNoise_ = 0.4f;
    float attackTime_ = 0.2f;
    float releaseTime_ = 0.3f;
    float vibratoRate_ = 5.0f;
    float vibratoDepth_ = 0.0f;
    float brightness_ = 0.5f;

    // State
    float vibratoPhase_ = 0.0f;
    float releaseEnvelope_ = 1.0f;
    juce::Random random_;
};
