#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>

class ArpEngine {
public:
    enum class Pattern {
        Up = 0,
        Down,
        UpDown,
        DownUp,
        Random,
        PlayedOrder
    };

    enum class Rate {
        Whole = 0,      // 1/1
        Half,           // 1/2
        Quarter,        // 1/4
        Eighth,         // 1/8
        Sixteenth,      // 1/16
        ThirtySecond,   // 1/32
        QuarterTriplet, // 1/4T
        EighthTriplet,  // 1/8T
        SixteenthTriplet // 1/16T
    };

    ArpEngine();

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void processBlock(juce::MidiBuffer& midiMessages, int numSamples,
                      const juce::AudioPlayHead::PositionInfo& posInfo);

    // Parameter setters
    void setPattern(Pattern p) { pattern_ = p; }
    void setRate(Rate r) { rate_ = r; }
    void setOctaves(int octaves) { octaves_ = juce::jlimit(1, 4, octaves); }
    void setGate(float gate) { gate_ = juce::jlimit(0.0f, 1.0f, gate); }
    void setSwing(float swing) { swing_ = juce::jlimit(0.0f, 1.0f, swing); }
    void setLatch(bool enabled) { latchEnabled_ = enabled; }
    void setVelocityMode(bool useInput) { useInputVelocity_ = useInput; }
    void setFixedVelocity(int vel) { fixedVelocity_ = juce::jlimit(1, 127, vel); }

private:
    // Note management
    struct HeldNote {
        int noteNumber;
        int velocity;
        int channel;
        int orderReceived; // For "played order" pattern
    };

    std::vector<HeldNote> heldNotes_;
    int noteOrderCounter_ = 0;

    // Arpeggio state
    std::vector<int> currentPattern_;
    int currentStep_ = 0;
    int currentNoteOn_ = -1;
    int currentChannel_ = 1;

    // Timing
    double sampleRate_ = 44100.0;
    int64_t nextStepSample_ = 0;
    int64_t totalSamplesProcessed_ = 0;

    // Parameters
    Pattern pattern_ = Pattern::Up;
    Rate rate_ = Rate::Sixteenth;
    int octaves_ = 1;
    float gate_ = 0.8f;
    float swing_ = 0.0f;
    bool latchEnabled_ = false;
    bool useInputVelocity_ = true;
    int fixedVelocity_ = 100;

    // Internal methods
    void handleNoteOn(int noteNumber, int velocity, int channel);
    void handleNoteOff(int noteNumber, int channel);
    void updatePattern();
    void generateArpeggioNotes(juce::MidiBuffer& output, int startSample, int endSample,
                               const juce::AudioPlayHead::PositionInfo& posInfo);
    int getSamplesPerStep(double bpm) const;
    int getVelocityForNote() const;
};
