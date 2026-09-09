#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <cmath>

class SitarEngine
{
public:
    static constexpr int NUM_SYMPATHETIC_STRINGS = 13;
    static constexpr int DELAY_BUFFER_SIZE = 8192;

    enum class RagaScale
    {
        Chromatic,    // All 12 semitones
        Bhairavi,     // C Db Eb F G Ab Bb
        Yaman,        // C D E F# G A B
        Kafi          // C D Eb F G A Bb
    };

    SitarEngine();

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void reset();
    float processSample(float baseFrequency, float velocity, bool noteOn, int samplesSinceNoteOn);

    void setRagaScale(RagaScale scale) { ragaScale_ = scale; updateSympatheticTuning(); }
    void setJawariAmount(float amount) { jawariAmount_ = juce::jlimit(0.0f, 1.0f, amount); }
    void setSympatheticResonance(float resonance) { sympatheticResonance_ = juce::jlimit(0.0f, 1.0f, resonance); }
    void setAttack(float attack) { attackTime_ = attack; }
    void setDecay(float decay) { decayTime_ = decay; }
    void setPitchBend(float bend) { pitchBendAmount_ = bend; }
    void setVibratoRate(float rate) { vibratoRate_ = rate; }
    void setVibratoDepth(float depth) { vibratoDepth_ = depth; }
    void setDroneLevel(float level) { droneLevel_ = juce::jlimit(0.0f, 1.0f, level); }

private:
    struct CombFilter
    {
        std::array<float, DELAY_BUFFER_SIZE> buffer{};
        int writePos = 0;
        float delayLength = 0.0f;
        float feedback = 0.995f;

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

    void updateSympatheticTuning();
    float applyJawari(float sample);
    std::array<float, NUM_SYMPATHETIC_STRINGS> getScaleRatios();

    double sampleRate_ = 44100.0;
    RagaScale ragaScale_ = RagaScale::Chromatic;

    // Main string (Karplus-Strong)
    CombFilter mainString_;
    float mainStringAmp_ = 1.0f;

    // Sympathetic strings
    std::array<CombFilter, NUM_SYMPATHETIC_STRINGS> sympatheticStrings_;
    std::array<float, NUM_SYMPATHETIC_STRINGS> sympatheticTuning_{};

    // Drone strings (tanpura-like)
    CombFilter droneString1_;  // Tonic
    CombFilter droneString2_;  // Fifth
    float dronePhase1_ = 0.0f;
    float dronePhase2_ = 0.0f;

    // Parameters
    float jawariAmount_ = 0.5f;
    float sympatheticResonance_ = 0.6f;
    float attackTime_ = 0.01f;
    float decayTime_ = 0.7f;
    float pitchBendAmount_ = 0.0f;
    float vibratoRate_ = 5.0f;
    float vibratoDepth_ = 0.0f;
    float droneLevel_ = 0.2f;

    // State
    float vibratoPhase_ = 0.0f;
    juce::Random random_;
};
