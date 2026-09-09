#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <vector>
#include <cmath>

/**
 * StringEngine - Karplus-Strong string synthesis
 *
 * Physical modeling of bowed/plucked strings using the Karplus-Strong algorithm:
 * - Noise burst excitation (bow attack or pluck)
 * - Tuned delay line for pitch
 * - Low-pass filter for damping
 * - Feedback loop creates natural string decay
 *
 * Features:
 * - 4 string types (Violin, Viola, Cello, Bass)
 * - Ensemble mode (1-4 voices)
 * - Vibrato control
 * - Attack shaping (soft bow, hard attack)
 * - Brightness control (damping)
 */
class StringEngine
{
public:
    enum class StringType
    {
        Violin,  // Bright, high register (G3-E7)
        Viola,   // Warm, alto register (C3-A6)
        Cello,   // Rich, tenor/bass (C2-C6)
        Bass     // Deep, bass register (E1-C5)
    };

    StringEngine();
    ~StringEngine() = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock);

    // String type selection
    void setStringType(StringType type);

    // Tone controls
    void setBrightness(float brightness);       // 0-1, affects damping filter
    void setAttack(float attack);               // 0-1, bow pressure/pluck hardness
    void setDecay(float decay);                 // 0-1, string damping rate
    void setVibratoDepth(float depth);          // 0-1, vibrato intensity
    void setVibratoRate(float rate);            // 0-1, vibrato speed (4-7 Hz)

    // Ensemble
    void setEnsembleSize(int size);             // 1-4 voices

    // Generate one sample
    float processSample(float frequency, float velocity, bool isNoteOn, int samplesSinceNoteOn);

    // Reset on note on
    void reset();

private:
    // Karplus-Strong synthesis
    float processKarplusStrong(float frequency, int voiceIndex);
    void exciteString(int voiceIndex, float velocity);
    float processVibrato(float frequency);

    // Delay line for Karplus-Strong
    static constexpr int MAX_DELAY_SIZE = 4096;
    struct DelayLine
    {
        std::array<float, MAX_DELAY_SIZE> buffer;
        int writePos = 0;
        int readPos = 0;
        int delayLength = 100;
        float damping = 0.995f;
        float prevOutput = 0.0f;
    };

    std::array<DelayLine, 4> delayLines_;  // One per ensemble voice

    // Current settings
    StringType currentString_ = StringType::Violin;

    // Parameters
    float brightness_ = 0.7f;
    float attack_ = 0.5f;
    float decay_ = 0.7f;
    float vibratoDepth_ = 0.0f;
    float vibratoRate_ = 0.5f;
    int ensembleSize_ = 1;

    // Envelope
    float currentAmplitude_ = 0.0f;
    float targetAmplitude_ = 0.0f;
    float attackRate_ = 0.0f;
    float releaseRate_ = 0.0f;

    // Vibrato LFO
    float vibratoPhase_ = 0.0f;

    // Ensemble detuning (cents)
    std::array<float, 4> ensembleDetune_ = {0.0f, -5.0f, 8.0f, -12.0f};

    // Random number generator for string excitation
    juce::Random random_;

    double sampleRate_ = 44100.0;
};
