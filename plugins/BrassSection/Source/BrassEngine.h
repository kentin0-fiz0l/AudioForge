#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <cmath>

/**
 * BrassEngine - Professional brass section synthesis
 *
 * Features:
 * - 3 brass instrument types (Trumpet, Trombone, Saxophone)
 * - 4 articulations (Sustain, Staccato, Marcato, Fall-off)
 * - Breath noise synthesis for realism
 * - Dynamic filtering (brightness increases with volume)
 * - Expression control via mod wheel
 * - Vibrato with rate and depth controls
 */
class BrassEngine
{
public:
    enum class InstrumentType
    {
        Trumpet,    // Bright, penetrating, strong high harmonics
        Trombone,   // Warm, rich, strong fundamentals
        Saxophone   // Reedy, complex harmonics, vocal quality
    };

    enum class Articulation
    {
        Sustain,    // Smooth, connected notes
        Staccato,   // Short, detached notes
        Marcato,    // Accented, emphasized attack
        FallOff     // Pitch bend down at end of note
    };

    BrassEngine();
    ~BrassEngine() = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock);

    // Instrument and articulation selection
    void setInstrumentType(InstrumentType type);
    void setArticulation(Articulation articulation);

    // Tone controls
    void setBrightness(float brightness);     // 0-1, affects filter cutoff
    void setBreathNoise(float amount);        // 0-1, air/breath sound
    void setVibratoDepth(float depth);        // 0-1, vibrato intensity
    void setVibratoRate(float rate);          // 0-1, vibrato speed (4-8 Hz)

    // Expression (usually from mod wheel)
    void setExpression(float expression);     // 0-1, controls dynamics

    // Section size (number of voices)
    void setSectionSize(int size);            // 1-4, unison voices

    // Generate one sample
    float processSample(float frequency, float velocity, bool isNoteOn, int samplesSinceNoteOn);

    // Reset on note on
    void reset();

private:
    // Wavetable generation for different instruments
    void generateTrumpetWavetable();
    void generateTromboneWavetable();
    void generateSaxophoneWavetable();

    // Helpers
    float interpolateWavetable(float phase);
    float generateBreathNoise();
    float processFiltering(float sample, float cutoffMod);
    float processVibrato(float frequency);

    // Wavetable
    static constexpr int WAVETABLE_SIZE = 2048;
    std::array<float, WAVETABLE_SIZE> wavetable_;
    float phase_ = 0.0f;
    float phaseIncrement_ = 0.0f;

    // Current settings
    InstrumentType currentInstrument_ = InstrumentType::Trumpet;
    Articulation currentArticulation_ = Articulation::Sustain;

    // Parameters
    float brightness_ = 0.7f;
    float breathNoise_ = 0.2f;
    float vibratoDepth_ = 0.0f;
    float vibratoRate_ = 0.5f;  // 0-1 maps to 4-8 Hz
    float expression_ = 1.0f;
    int sectionSize_ = 1;

    // Envelope
    float currentAmplitude_ = 0.0f;
    float targetAmplitude_ = 0.0f;
    float attackRate_ = 0.0f;
    float releaseRate_ = 0.0f;

    // Vibrato LFO
    float vibratoPhase_ = 0.0f;

    // Filter (state variable filter)
    float filterCutoff_ = 1000.0f;
    float filterResonance_ = 0.7f;
    float filterLP_ = 0.0f;
    float filterBP_ = 0.0f;
    float filterHP_ = 0.0f;

    // Breath noise
    juce::Random random_;
    float noiseLP_ = 0.0f;  // Low-pass filtered noise

    // Section (multiple detuned voices)
    std::array<float, 4> sectionPhases_ = {0.0f, 0.0f, 0.0f, 0.0f};
    std::array<float, 4> sectionDetune_ = {0.0f, -0.003f, 0.005f, -0.007f};

    double sampleRate_ = 44100.0;
};
