#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>
#include <random>

class QuantizerEngine {
public:
    enum class GridResolution {
        Quarter = 0,       // 1/4 note
        Eighth,            // 1/8 note
        Sixteenth,         // 1/16 note
        ThirtySecond,      // 1/32 note
        EighthTriplet,     // 1/8 triplet
        SixteenthTriplet   // 1/16 triplet
    };

    QuantizerEngine();

    void prepareToPlay(double sampleRate);
    void processBlock(juce::MidiBuffer& midiMessages, int numSamples,
                      const juce::AudioPlayHead::PositionInfo& posInfo);

    // Parameter setters
    void setGridResolution(GridResolution res) { gridResolution_ = res; }
    void setQuantizeStrength(float strength) { quantizeStrength_ = juce::jlimit(0.0f, 1.0f, strength); }
    void setSwing(float swing) { swing_ = juce::jlimit(0.0f, 1.0f, swing); }
    void setHumanizeTiming(float amount) { humanizeTiming_ = juce::jlimit(0.0f, 1.0f, amount); }
    void setHumanizeVelocity(float amount) { humanizeVelocity_ = juce::jlimit(0.0f, 1.0f, amount); }

private:
    struct NoteEvent {
        int noteNumber;
        int velocity;
        int channel;
        int64_t originalSample;
        int64_t quantizedSample;
        bool isNoteOff;
    };

    std::vector<NoteEvent> eventBuffer_;
    double sampleRate_ = 44100.0;
    std::mt19937 randomGen_;
    std::uniform_real_distribution<float> randomDist_;

    // Parameters
    GridResolution gridResolution_ = GridResolution::Sixteenth;
    float quantizeStrength_ = 1.0f;  // 0 = no quantize, 1 = full quantize
    float swing_ = 0.0f;             // 0 = straight, 1 = maximum swing
    float humanizeTiming_ = 0.0f;    // 0 = perfect timing, 1 = max random variation
    float humanizeVelocity_ = 0.0f;  // 0 = original velocity, 1 = max random variation

    // Timing calculations
    int64_t calculateQuantizedPosition(int64_t originalSample, double bpm, int64_t totalSamplesProcessed);
    int64_t getGridSizeInSamples(double bpm);
    int calculateHumanizedVelocity(int originalVelocity);
    float getRandomOffset();
};
