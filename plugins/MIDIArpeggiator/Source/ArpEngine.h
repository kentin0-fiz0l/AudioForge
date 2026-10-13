#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <array>
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
    void setPattern(Pattern p);
    void setRate(Rate r) { rate_ = r; }
    void setOctaves(int octaves);
    void setGate(float gate) { gate_ = juce::jlimit(0.0f, 1.0f, gate); }
    void setSwing(float swing) { swing_ = juce::jlimit(0.0f, 1.0f, swing); }
    void setLatch(bool enabled);
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

    static constexpr int maxHeldNotes = 128;

    std::vector<HeldNote> heldNotes_;  // the notes being arpeggiated (latched or held)
    std::array<std::array<bool, 128>, 16> keyDown_ {}; // keys physically down, per channel
    int numKeysDown_ = 0;
    int noteOrderCounter_ = 0;

    // Arpeggio state (vectors are reserved up front so the audio thread never allocates)
    std::vector<int> baseNotes_;
    std::vector<int> currentPattern_;
    int currentStep_ = 0;
    int currentNoteOn_ = -1;
    int currentChannel_ = 1;
    juce::MidiBuffer outputMidi_;

    // Timing, in quarter notes. Steps sit on a grid of stepBeats; while the host
    // plays the grid follows its PPQ, otherwise it free-runs from the first note.
    double sampleRate_ = 44100.0;
    double freeRunBeat_ = 0.0;
    double expectedBlockBeat_ = 0.0; // where this block starts if nothing jumped
    double lastStepBeats_ = 0.0;
    int64_t lastStepIndex_ = -1;     // grid index of the last step played
    double noteOffBeat_ = 0.0;
    bool wasRunning_ = false;
    bool wasHostPlaying_ = false;

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
    void generateArpeggioNotes(juce::MidiBuffer& output, int numSamples,
                               const juce::AudioPlayHead::PositionInfo& posInfo);
    void stopCurrentNote(juce::MidiBuffer& output, int samplePosition);
    double getStepLengthInBeats() const;
    int getVelocityForNote() const;
};
