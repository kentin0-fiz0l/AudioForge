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
    // Decay: seconds for the main string to fall 20 dB while the note is held
    void setDecay(float decay) { decayTime_ = decay; }
    void setPitchBend(float bend) { pitchBendAmount_ = bend; }
    void setVibratoRate(float rate) { vibratoRate_ = rate; }
    void setVibratoDepth(float depth) { vibratoDepth_ = depth; }

    // True once a released note has faded out
    bool isReleased() const { return releaseLevel_ < 0.001f; }

    // A plucked-string delay loop (Karplus-Strong). With damping above zero
    // the loop runs through a two-point lowpass, so the high harmonics die
    // sooner than the fundamental and the tone darkens as the string rings.
    struct CombFilter
    {
        std::array<float, DELAY_BUFFER_SIZE> buffer{};
        int writePos = 0;
        float delayLength = 0.0f;
        float feedback = 0.995f;
        float damping = 0.0f;  // 0 keeps every harmonic, 0.5 is classic Karplus-Strong
        float lastOutput = 0.0f;

        void reset()
        {
            buffer.fill(0.0f);
            writePos = 0;
            lastOutput = 0.0f;
        }

        // Sets the pitch, and the feedback that makes the string fall by
        // `fall` (0.1 = 20 dB) every `seconds`
        void tune(float frequency, float fall, float seconds, double sampleRate)
        {
            float period = static_cast<float>(sampleRate) / frequency;
            delayLength = period - damping;  // the lowpass adds `damping` samples of delay
            feedback = std::pow(fall, period / (seconds * static_cast<float>(sampleRate)));
        }

        float process(float input)
        {
            int readPos = static_cast<int>(writePos - delayLength);
            if (readPos < 0) readPos += DELAY_BUFFER_SIZE;

            float output = buffer[readPos % DELAY_BUFFER_SIZE];
            float filtered = output * (1.0f - damping) + lastOutput * damping;
            lastOutput = output;
            buffer[writePos] = input + filtered * feedback;
            writePos = (writePos + 1) % DELAY_BUFFER_SIZE;

            return output;
        }
    };

private:
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

    // Parameters
    float jawariAmount_ = 0.5f;
    float sympatheticResonance_ = 0.6f;
    float attackTime_ = 0.01f;
    float decayTime_ = 0.7f;
    float pitchBendAmount_ = 0.0f;
    float vibratoRate_ = 5.0f;
    float vibratoDepth_ = 0.0f;

    // State
    float vibratoPhase_ = 0.0f;
    float releaseLevel_ = 1.0f;
    float releaseCoeff_ = 0.0f;
    juce::Random random_;
};

// The tanpura drone: a tonic and a fifth string, tuned to C3 and G3, the Sa
// and Pa of the raga scales. There is one for the whole instrument, not one
// per voice. It is plucked as soon as a note starts, re-plucked every few
// seconds while notes sound, and rings on after they stop.
class SitarDrone
{
public:
    void prepareToPlay(double sampleRate);
    void setLevel(float level) { level_ = juce::jlimit(0.0f, 1.0f, level); }

    // Adds the drone to the buffer. noteOnSample is where the block's first
    // note starts, or -1; notesSounding says whether any note still sounds.
    void render(juce::AudioBuffer<float>& buffer, int noteOnSample, bool notesSounding);

private:
    struct DroneString
    {
        float frequency;
        float pluckIntervalSeconds;
        SitarEngine::CombFilter loop;
        int samplesToPluck = 0;
        int excitationLeft = 0;
    };

    std::array<DroneString, 2> strings_ { { { 130.81f, 4.0f }, { 130.81f * 1.5f, 5.0f } } };
    double sampleRate_ = 44100.0;
    float level_ = 0.2f;
    bool cycling_ = false;
    juce::Random random_;
};
