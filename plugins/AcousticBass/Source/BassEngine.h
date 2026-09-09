#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <cmath>

/**
 * BassEngine - Hybrid acoustic/electric bass synthesis
 *
 * Combines wavetable, physical modeling, and sample-based techniques:
 * - Strong fundamental for low-end weight
 * - Harmonic overtones that decay faster (pluck characteristic)
 * - Playing technique simulation (fingered, picked, slapped)
 * - Fret noise and string buzz
 * - Tone shaping (pickup position, EQ)
 */
class BassEngine
{
public:
    enum class BassType
    {
        Upright,     // Deep, woody acoustic upright bass
        Electric     // Bright, punchy electric bass
    };

    enum class PlayStyle
    {
        Fingered,    // Smooth, warm tone
        Picked,      // Bright, defined attack
        Slapped      // Percussive, aggressive
    };

    BassEngine();
    ~BassEngine() = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock);

    void setBassType(BassType type);
    void setPlayStyle(PlayStyle style);
    void setTone(float tone);              // 0-1, brightness/pickup position
    void setAttack(float attack);          // 0-1, pluck/pick hardness
    void setDecay(float decay);            // 0-1, string damping
    void setFretNoise(float amount);       // 0-1, fret/string noise
    void setSlideAmount(float amount);     // 0-1, slide/glide

    float processSample(float frequency, float velocity, bool isNoteOn, int samplesSinceNoteOn);
    void reset();

private:
    void generateUprightWavetable();
    void generateElectricWavetable();
    float interpolateWavetable(float phase);
    float generateFretNoise(int samplesSinceNoteOn);
    float processFiltering(float sample);

    static constexpr int WAVETABLE_SIZE = 2048;
    std::array<float, WAVETABLE_SIZE> wavetable_;
    float phase_ = 0.0f;
    float phaseIncrement_ = 0.0f;

    BassType currentType_ = BassType::Electric;
    PlayStyle currentStyle_ = PlayStyle::Fingered;

    float tone_ = 0.5f;
    float attack_ = 0.5f;
    float decay_ = 0.7f;
    float fretNoise_ = 0.1f;
    float slideAmount_ = 0.0f;

    // Dual envelope (fundamental + harmonics decay separately)
    float fundamentalAmp_ = 0.0f;
    float harmonicAmp_ = 0.0f;
    float targetAmp_ = 0.0f;

    // Filter
    float filterLP_ = 0.0f;
    float filterPrev_ = 0.0f;

    juce::Random random_;
    double sampleRate_ = 44100.0;
};
