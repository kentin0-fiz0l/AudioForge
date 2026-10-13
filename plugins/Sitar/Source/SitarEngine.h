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

    // A plucked-string delay loop (Karplus-Strong). A one-pole lowpass in
    // the loop makes the high harmonics die sooner than the fundamental, so
    // the tone darkens as the string rings. It is set in hertz, so it darkens
    // the same at every sample rate. A first-order allpass supplies the
    // fraction of a sample that keeps the loop in tune.
    struct CombFilter
    {
        std::array<float, DELAY_BUFFER_SIZE> buffer{};
        int writePos = 0;
        float delayLength = 2.0f;  // buffer plus allpass delay, in samples
        float feedback = 0.995f;
        // Each pass the loop loses about (f / dampingHz)^2 of its power at a
        // frequency f well below it. 0 keeps every harmonic. Used by tune().
        float dampingHz = 0.0f;

        void reset()
        {
            buffer.fill(0.0f);
            writePos = 0;
            allpassIn = allpassOut = lowpassOut = 0.0f;
        }

        // Sets the delay so the loop, lowpass included, rings at `frequency`
        void setFrequency(float frequency, double sampleRate)
        {
            const float period = static_cast<float>(sampleRate) / frequency;
            float lowpassDelay = 0.0f;
            if (lowpassCoeff > 0.0f)
            {
                const float w = juce::MathConstants<float>::twoPi / period;
                lowpassDelay = std::atan2(lowpassCoeff * std::sin(w), 1.0f - lowpassCoeff * std::cos(w)) / w;
            }

            // Shortened if need be to fit the buffer
            delayLength = juce::jlimit(2.0f, static_cast<float>(DELAY_BUFFER_SIZE - 2), period - lowpassDelay);
            wholeDelay = static_cast<int>(delayLength);
            float fraction = delayLength - static_cast<float>(wholeDelay);
            if (fraction < 0.5f)  // the allpass is most accurate for 0.5 to 1.5 samples
            {
                fraction += 1.0f;
                --wholeDelay;
            }
            allpassCoeff = (1.0f - fraction) / (1.0f + fraction);
        }

        // Sets the pitch, and the feedback that makes the fundamental fall by
        // `fall` (0.1 = 20 dB) every `seconds`, making up for what the
        // lowpass takes from it
        void tune(float frequency, float fall, float seconds, double sampleRate)
        {
            const float perPass = std::pow(fall, 1.0f / (frequency * seconds));

            // On high notes with a long Decay the lowpass alone would take more
            // than the fundamental may lose; it is eased to take at most half
            float lowpassGain = 1.0f;
            if (dampingHz > 0.0f)
            {
                const float cutoff = std::max(dampingHz, frequency / std::sqrt(0.5f * (1.0f - perPass * perPass)));
                // Solves a / (1 - a)^2 = k, which gives the loss above at any rate
                const double k = std::pow(sampleRate / (juce::MathConstants<double>::twoPi * cutoff), 2.0);
                lowpassCoeff = static_cast<float>(((2.0 * k + 1.0) - std::sqrt(4.0 * k + 1.0)) / (2.0 * k));

                const float w = juce::MathConstants<float>::twoPi * frequency / static_cast<float>(sampleRate);
                lowpassGain = (1.0f - lowpassCoeff)
                            / std::sqrt(1.0f - 2.0f * lowpassCoeff * std::cos(w) + lowpassCoeff * lowpassCoeff);
            }

            setFrequency(frequency, sampleRate);
            feedback = std::min(perPass / lowpassGain, 0.9999f);
        }

        float process(float input)
        {
            int readPos = writePos - wholeDelay;
            if (readPos < 0) readPos += DELAY_BUFFER_SIZE;

            const float delayed = buffer[static_cast<size_t>(readPos)];
            const float output = allpassCoeff * delayed + allpassIn - allpassCoeff * allpassOut;
            allpassIn = delayed;
            allpassOut = output;

            lowpassOut = output + lowpassCoeff * (lowpassOut - output);
            buffer[static_cast<size_t>(writePos)] = input + lowpassOut * feedback;
            if (++writePos == DELAY_BUFFER_SIZE) writePos = 0;

            return output;
        }

    private:
        float lowpassCoeff = 0.0f;
        int wholeDelay = 1;
        float allpassCoeff = 0.0f;
        float allpassIn = 0.0f, allpassOut = 0.0f, lowpassOut = 0.0f;
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
    void reset();
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
