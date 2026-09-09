#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <cmath>

class KotoEngine
{
public:
    static constexpr int DELAY_BUFFER_SIZE = 8192;

    enum class Scale
    {
        InSen,        // E F A B D (in-sen scale)
        Hirajoshi,    // E F A B♭ D
        Iwato,        // E F A♭ B♭ D♭
        Kumoi,        // E F A B C
        Chromatic     // All semitones
    };

    enum class PlayStyle
    {
        Pluck,     // Normal pluck
        Tremolo,   // Rapid re-plucking
        Scrape     // Scraping across strings
    };

    KotoEngine();

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void reset();
    float processSample(float baseFrequency, float velocity, bool noteOn, int samplesSinceNoteOn);

    void setScale(Scale scale) { scale_ = scale; }
    void setPlayStyle(PlayStyle style) { playStyle_ = style; }
    void setBodyResonance(float amount) { bodyResonance_ = juce::jlimit(0.0f, 1.0f, amount); }
    void setAttack(float attack) { attackTime_ = attack; }
    void setDecay(float decay) { decayTime_ = decay; }
    void setPitchBend(float bend) { pitchBendAmount_ = bend; }
    void setTremoloRate(float rate) { tremoloRate_ = rate; }
    void setTone(float tone) { tone_ = juce::jlimit(0.0f, 1.0f, tone); }

private:
    struct CombFilter
    {
        std::array<float, DELAY_BUFFER_SIZE> buffer{};
        int writePos = 0;
        float delayLength = 0.0f;
        float feedback = 0.994f;

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

    struct ResonantFilter
    {
        float frequency = 440.0f;
        float q = 2.0f;
        float gain = 1.0f;

        float x1 = 0.0f, x2 = 0.0f;
        float y1 = 0.0f, y2 = 0.0f;
        float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
        float a1 = 0.0f, a2 = 0.0f;

        void updateCoefficients(double sampleRate)
        {
            float w0 = juce::MathConstants<float>::twoPi * frequency / static_cast<float>(sampleRate);
            float alpha = std::sin(w0) / (2.0f * q);
            float A = std::sqrt(gain);

            // Peaking EQ coefficients
            b0 = 1.0f + alpha * A;
            b1 = -2.0f * std::cos(w0);
            b2 = 1.0f - alpha * A;
            float a0 = 1.0f + alpha / A;
            a1 = -2.0f * std::cos(w0);
            a2 = 1.0f - alpha / A;

            // Normalize
            b0 /= a0;
            b1 /= a0;
            b2 /= a0;
            a1 /= a0;
            a2 /= a0;
        }

        float process(float input)
        {
            float output = b0 * input + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;

            x2 = x1;
            x1 = input;
            y2 = y1;
            y1 = output;

            return output;
        }

        void reset()
        {
            x1 = x2 = y1 = y2 = 0.0f;
        }
    };

    void applyBodyResonance(float& sample);

    double sampleRate_ = 44100.0;
    Scale scale_ = Scale::InSen;
    PlayStyle playStyle_ = PlayStyle::Pluck;

    // Main string
    CombFilter string_;

    // Body resonance (3 formant peaks)
    ResonantFilter bodyFilter1_;  // ~250Hz
    ResonantFilter bodyFilter2_;  // ~800Hz
    ResonantFilter bodyFilter3_;  // ~2000Hz

    // One-pole tone filter
    float toneLp_ = 0.0f;
    float toneCoeff_ = 0.5f;

    // Parameters
    float bodyResonance_ = 0.5f;
    float attackTime_ = 0.005f;
    float decayTime_ = 0.8f;
    float pitchBendAmount_ = 0.0f;
    float tremoloRate_ = 10.0f;
    float tone_ = 0.6f;

    // State
    float tremoloPhase_ = 0.0f;
    int lastTremoloTrigger_ = 0;
    juce::Random random_;
};
