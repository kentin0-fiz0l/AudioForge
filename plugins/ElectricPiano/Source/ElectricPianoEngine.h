#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <cmath>

/**
 * ElectricPianoEngine - Rhodes/Wurlitzer synthesis engine
 *
 * Synthesis approach:
 * - Wavetable oscillator with EP-specific waveforms
 * - Velocity-sensitive amplitude and tone
 * - Mechanical noise (key-off thump, release)
 * - Built-in tremolo, chorus, and reverb effects
 */
class ElectricPianoEngine
{
public:
    enum class EPModel
    {
        Rhodes,      // Classic Fender Rhodes tine sound
        Wurlitzer,   // Wurlitzer 200 reed sound
        Hybrid       // Mix of both
    };

    ElectricPianoEngine();
    ~ElectricPianoEngine() = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void setModel(EPModel model);
    void setTone(float tone);           // 0-1, controls brightness
    void setDecay(float decay);         // 0-1, controls release time
    void setMechanicalNoise(float amount); // 0-1, key-off thump
    void setVelocityCurve(float curve); // 0-1, velocity response

    // Effects
    void setTremoloDepth(float depth);
    void setTremoloRate(float rate);
    void setChorusDepth(float depth);
    void setReverbMix(float mix);

    // Generate one sample for a given frequency and velocity
    float processSample(float frequency, float velocity, bool isNoteOn);

    // Reset oscillator phase (call on note on)
    void reset();

private:
    // Wavetable generation
    void generateRhodesWavetable();
    void generateWurlitzerWavetable();
    void generateHybridWavetable();

    // DSP helpers
    float interpolateWavetable(float phase);
    float processEffects(float sample);

    // Wavetable
    static constexpr int WAVETABLE_SIZE = 2048;
    std::array<float, WAVETABLE_SIZE> wavetable_;
    float phase_ = 0.0f;
    float phaseIncrement_ = 0.0f;

    // Parameters
    EPModel currentModel_ = EPModel::Rhodes;
    float tone_ = 0.5f;
    float decay_ = 0.5f;
    float mechanicalNoise_ = 0.2f;
    float velocityCurve_ = 0.5f;

    // Effects parameters
    float tremoloDepth_ = 0.0f;
    float tremoloRate_ = 5.0f;
    float chorusDepth_ = 0.0f;
    float reverbMix_ = 0.0f;

    // Effects state
    float tremoloPhase_ = 0.0f;
    std::array<float, 4> chorusDelayBuffer_{};
    int chorusWritePos_ = 0;
    float reverbBuffer_ = 0.0f;

    // Envelope
    float currentAmplitude_ = 0.0f;
    float targetAmplitude_ = 0.0f;
    float releaseRate_ = 0.0f;

    // Mechanical noise
    juce::Random random_;
    float noiseLevel_ = 0.0f;

    double sampleRate_ = 44100.0;
};
