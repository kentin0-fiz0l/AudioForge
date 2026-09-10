#include "ArpEngine.h"

ArpEngine::ArpEngine() {}

void ArpEngine::prepareToPlay(double sampleRate, int samplesPerBlock) {
    sampleRate_ = sampleRate;
    totalSamplesProcessed_ = 0;
    nextStepSample_ = 0;
}

void ArpEngine::processBlock(juce::MidiBuffer& midiMessages, int numSamples,
                              const juce::AudioPlayHead::PositionInfo& posInfo) {
    juce::MidiBuffer processedMidi;

    // Process incoming MIDI to update held notes
    for (const auto metadata : midiMessages) {
        auto msg = metadata.getMessage();
        if (msg.isNoteOn()) {
            handleNoteOn(msg.getNoteNumber(), msg.getVelocity(), msg.getChannel());
        } else if (msg.isNoteOff()) {
            handleNoteOff(msg.getNoteNumber(), msg.getChannel());
        }
    }

    // Generate arpeggio notes
    if (!heldNotes_.empty() || latchEnabled_) {
        generateArpeggioNotes(processedMidi, 0, numSamples, posInfo);
    }

    // Replace input MIDI with generated arpeggio
    midiMessages.swapWith(processedMidi);
    totalSamplesProcessed_ += numSamples;
}

void ArpEngine::handleNoteOn(int noteNumber, int velocity, int channel) {
    // Check if note already held
    for (const auto& note : heldNotes_) {
        if (note.noteNumber == noteNumber && note.channel == channel)
            return;
    }

    // Add new note
    heldNotes_.push_back({noteNumber, velocity, channel, noteOrderCounter_++});
    currentChannel_ = channel;
    updatePattern();
}

void ArpEngine::handleNoteOff(int noteNumber, int channel) {
    if (latchEnabled_) return; // Latch mode keeps notes

    // Remove note
    heldNotes_.erase(
        std::remove_if(heldNotes_.begin(), heldNotes_.end(),
            [noteNumber, channel](const HeldNote& n) {
                return n.noteNumber == noteNumber && n.channel == channel;
            }),
        heldNotes_.end()
    );

    if (heldNotes_.empty()) {
        currentStep_ = 0;
        currentPattern_.clear();
    } else {
        updatePattern();
    }
}

void ArpEngine::updatePattern() {
    if (heldNotes_.empty()) {
        currentPattern_.clear();
        return;
    }

    std::vector<int> baseNotes;
    for (const auto& note : heldNotes_)
        baseNotes.push_back(note.noteNumber);

    // Sort based on pattern
    switch (pattern_) {
        case Pattern::Up:
            std::sort(baseNotes.begin(), baseNotes.end());
            break;
        case Pattern::Down:
            std::sort(baseNotes.begin(), baseNotes.end(), std::greater<int>());
            break;
        case Pattern::UpDown:
            std::sort(baseNotes.begin(), baseNotes.end());
            break;
        case Pattern::DownUp:
            std::sort(baseNotes.begin(), baseNotes.end(), std::greater<int>());
            break;
        case Pattern::Random:
            // Keep current order, randomize during playback
            break;
        case Pattern::PlayedOrder:
            std::sort(heldNotes_.begin(), heldNotes_.end(),
                [](const HeldNote& a, const HeldNote& b) {
                    return a.orderReceived < b.orderReceived;
                });
            baseNotes.clear();
            for (const auto& note : heldNotes_)
                baseNotes.push_back(note.noteNumber);
            break;
    }

    // Generate pattern with octaves
    currentPattern_.clear();
    for (int octave = 0; octave < octaves_; ++octave) {
        for (int note : baseNotes) {
            currentPattern_.push_back(note + (octave * 12));
        }
    }

    // For Up-Down and Down-Up patterns, add reverse (without duplicating endpoints)
    if (pattern_ == Pattern::UpDown || pattern_ == Pattern::DownUp) {
        if (currentPattern_.size() > 1) {
            for (int i = (int)currentPattern_.size() - 2; i >= 1; --i) {
                currentPattern_.push_back(currentPattern_[i]);
            }
        }
    }

    // Wrap current step if pattern changed
    if (currentStep_ >= (int)currentPattern_.size())
        currentStep_ = 0;
}

void ArpEngine::generateArpeggioNotes(juce::MidiBuffer& output, int startSample, int endSample,
                                       const juce::AudioPlayHead::PositionInfo& posInfo) {
    if (currentPattern_.empty()) return;

    double bpm = posInfo.getBpm().orFallback(120.0);
    int samplesPerStep = getSamplesPerStep(bpm);
    int gateLength = (int)(samplesPerStep * gate_);

    while (totalSamplesProcessed_ + nextStepSample_ < totalSamplesProcessed_ + endSample) {
        int stepStartSample = (int)(nextStepSample_ - totalSamplesProcessed_);

        if (stepStartSample < 0) stepStartSample = 0;
        if (stepStartSample >= endSample) break;

        // Note off for previous note
        if (currentNoteOn_ >= 0) {
            output.addEvent(juce::MidiMessage::noteOff(currentChannel_, currentNoteOn_),
                          stepStartSample);
        }

        // Note on for current step
        int noteToPlay = currentPattern_[currentStep_];
        if (pattern_ == Pattern::Random) {
            noteToPlay = currentPattern_[juce::Random::getSystemRandom().nextInt((int)currentPattern_.size())];
        }

        int velocity = getVelocityForNote();
        float normalizedVelocity = velocity / 127.0f; // JUCE uses 0.0-1.0 for velocity
        output.addEvent(juce::MidiMessage::noteOn(currentChannel_, noteToPlay, normalizedVelocity),
                       stepStartSample);
        currentNoteOn_ = noteToPlay;

        // Schedule note off
        int noteOffSample = stepStartSample + gateLength;
        if (noteOffSample < endSample) {
            output.addEvent(juce::MidiMessage::noteOff(currentChannel_, noteToPlay),
                          noteOffSample);
            currentNoteOn_ = -1;
        }

        // Advance to next step
        currentStep_ = (currentStep_ + 1) % currentPattern_.size();
        nextStepSample_ += samplesPerStep;
    }
}

int ArpEngine::getSamplesPerStep(double bpm) const {
    double beatsPerSecond = bpm / 60.0;
    double secondsPerBeat = 1.0 / beatsPerSecond;
    double secondsPerStep = secondsPerBeat;

    // Calculate step duration based on rate
    switch (rate_) {
        case Rate::Whole:      secondsPerStep = secondsPerBeat * 4.0; break;
        case Rate::Half:       secondsPerStep = secondsPerBeat * 2.0; break;
        case Rate::Quarter:    secondsPerStep = secondsPerBeat; break;
        case Rate::Eighth:     secondsPerStep = secondsPerBeat / 2.0; break;
        case Rate::Sixteenth:  secondsPerStep = secondsPerBeat / 4.0; break;
        case Rate::ThirtySecond: secondsPerStep = secondsPerBeat / 8.0; break;
        case Rate::QuarterTriplet: secondsPerStep = secondsPerBeat * (2.0 / 3.0); break;
        case Rate::EighthTriplet:  secondsPerStep = secondsPerBeat / 3.0; break;
        case Rate::SixteenthTriplet: secondsPerStep = secondsPerBeat / 6.0; break;
    }

    return (int)(secondsPerStep * sampleRate_);
}

int ArpEngine::getVelocityForNote() const {
    if (useInputVelocity_ && !heldNotes_.empty()) {
        // Average velocity of held notes
        int totalVel = 0;
        for (const auto& note : heldNotes_)
            totalVel += note.velocity;
        return totalVel / (int)heldNotes_.size();
    }
    return fixedVelocity_;
}
