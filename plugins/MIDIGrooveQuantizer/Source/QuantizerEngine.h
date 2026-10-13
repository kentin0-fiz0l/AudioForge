#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <array>
#include <random>

// Quantizes note-ons to a beat grid taken from the host's PPQ position.
// Notes can move both later and earlier, so all MIDI is delayed by a lookahead
// (getLatencySamples()) that the processor reports to the host. Note-offs move
// by the same amount as their note-on, so note lengths are kept.
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

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void processBlock(juce::MidiBuffer& midiMessages, int numSamples,
                      const juce::Optional<juce::AudioPlayHead::PositionInfo>& posInfo);

    int getLatencySamples() const { return latencySamples_; }

    // Parameter setters
    void setGridResolution(GridResolution res) { gridResolution_ = res; }
    void setQuantizeStrength(float strength) { quantizeStrength_ = juce::jlimit(0.0f, 1.0f, strength); }
    void setSwing(float swing) { swing_ = juce::jlimit(0.0f, 1.0f, swing); }
    void setHumanizeTiming(float amount) { humanizeTiming_ = juce::jlimit(0.0f, 1.0f, amount); }
    void setHumanizeVelocity(float amount) { humanizeVelocity_ = juce::jlimit(0.0f, 1.0f, amount); }

private:
    // A delayed short MIDI message (notes, CCs, pitch bend...), due at an
    // absolute time on the engine's own sample counter.
    struct PendingEvent {
        int64_t dueSample;
        juce::uint8 data[3];
        int size;
    };

    static constexpr int maxPendingEvents = 1024;
    std::array<PendingEvent, maxPendingEvents> pending_;
    int numPending_ = 0;

    // How far each sounding note's note-on was moved, so its note-off moves the same.
    std::array<std::array<int64_t, 128>, 16> noteShift_ {};

    juce::MidiBuffer output_;
    int64_t samplesElapsed_ = 0;
    int latencySamples_ = 0;
    double sampleRate_ = 44100.0;
    std::mt19937 randomGen_;
    std::uniform_real_distribution<float> randomDist_;

    // Parameters
    GridResolution gridResolution_ = GridResolution::Sixteenth;
    float quantizeStrength_ = 1.0f;  // 0 = no quantize, 1 = full quantize
    float swing_ = 0.0f;             // 0 = straight, 1 = maximum swing
    float humanizeTiming_ = 0.0f;    // 0 = perfect timing, 1 = max random variation
    float humanizeVelocity_ = 0.0f;  // 0 = original velocity, 1 = max random variation

    void updateLatency(double bpm, bool isPlaying);
    void schedule(const juce::MidiMessage& msg, int64_t dueSample);
    double quantizeOffsetInBeats(double ppq);
    double getGridSizeInBeats() const;
    int calculateHumanizedVelocity(int originalVelocity);
    float getRandomOffset();
};
