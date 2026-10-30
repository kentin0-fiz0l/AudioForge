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
        float feedback = 0.98f;

        // Loop length = intDelay whole samples + a first-order allpass whose
        // phase delay at the fundamental supplies the fractional remainder
        int intDelay = 1;
        float allpassCoeff = 0.0f;
        float allpassState = 0.0f;

        void reset()
        {
            buffer.fill(0.0f);
            writePos = 0;
            allpassState = 0.0f;
        }

        // Sets the loop length in samples (sampleRate / frequency)
        void setDelay(float delaySamples)
        {
            const float delay = juce::jlimit(4.0f, static_cast<float>(DELAY_BUFFER_SIZE - 2), delaySamples);

            // Keep the allpass delay in [0.5, 1.5), where it is well behaved
            intDelay = static_cast<int>(std::floor(delay - 0.5f));
            const float fraction = delay - static_cast<float>(intDelay);

            // Thiran's (1 - d) / (1 + d) is exact only near DC; this form gives
            // phase delay of exactly d at the fundamental, so the loop's phase
            // there is a whole cycle and the note is in tune at any length
            const float omega = juce::MathConstants<float>::twoPi / delay;
            allpassCoeff = juce::jlimit(-0.99f, 0.99f,
                                        std::sin(omega * (1.0f - fraction) * 0.5f)
                                            / std::sin(omega * (1.0f + fraction) * 0.5f));
        }

        float process(float input)
        {
            int readPos = writePos - intDelay;
            if (readPos < 0) readPos += DELAY_BUFFER_SIZE;
            const int prevPos = (readPos == 0) ? DELAY_BUFFER_SIZE - 1 : readPos - 1;

            // First-order allpass: y[n] = a * x[n] + x[n-1] - a * y[n-1]
            float output = allpassCoeff * (buffer[readPos] - allpassState) + buffer[prevPos];
            allpassState = output;

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
