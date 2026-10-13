#include "ArpEngine.h"

ArpEngine::ArpEngine() {
    heldNotes_.reserve(maxHeldNotes);
    baseNotes_.reserve(maxHeldNotes);
    currentPattern_.reserve(maxHeldNotes * 4 * 2); // 4 octaves, up and back down
}

void ArpEngine::prepareToPlay(double sampleRate, int /*samplesPerBlock*/) {
    sampleRate_ = sampleRate;
    outputMidi_.ensureSize(4096);
    wasRunning_ = false;
}

void ArpEngine::processBlock(juce::MidiBuffer& midiMessages, int numSamples,
                              const juce::AudioPlayHead::PositionInfo& posInfo) {
    outputMidi_.clear();

    // Process incoming MIDI to update held notes
    for (const auto metadata : midiMessages) {
        auto msg = metadata.getMessage();
        if (msg.isNoteOn()) {
            handleNoteOn(msg.getNoteNumber(), msg.getVelocity(), msg.getChannel());
        } else if (msg.isNoteOff()) {
            handleNoteOff(msg.getNoteNumber(), msg.getChannel());
        }
    }

    generateArpeggioNotes(outputMidi_, numSamples, posInfo);

    // Replace input MIDI with generated arpeggio
    midiMessages.swapWith(outputMidi_);
}

void ArpEngine::setPattern(Pattern p) {
    if (p == pattern_) return;
    pattern_ = p;
    updatePattern(); // takes effect from the next step
}

void ArpEngine::setOctaves(int octaves) {
    octaves = juce::jlimit(1, 4, octaves);
    if (octaves == octaves_) return;
    octaves_ = octaves;
    updatePattern();
}

void ArpEngine::setLatch(bool enabled) {
    if (enabled == latchEnabled_) return;
    latchEnabled_ = enabled;

    if (!enabled) {
        // Drop the latched notes whose keys are no longer down
        heldNotes_.erase(
            std::remove_if(heldNotes_.begin(), heldNotes_.end(),
                [this](const HeldNote& n) { return !keyDown_[n.channel - 1][n.noteNumber]; }),
            heldNotes_.end()
        );
        updatePattern();
    }
}

void ArpEngine::handleNoteOn(int noteNumber, int velocity, int channel) {
    // With latch on, the first key of a fresh chord replaces the latched one
    if (latchEnabled_ && numKeysDown_ == 0)
        heldNotes_.clear();

    auto& keyDown = keyDown_[channel - 1][noteNumber];
    if (!keyDown) {
        keyDown = true;
        ++numKeysDown_;
    }

    // Check if note already held
    for (const auto& note : heldNotes_) {
        if (note.noteNumber == noteNumber && note.channel == channel)
            return;
    }

    if ((int)heldNotes_.size() < maxHeldNotes) {
        heldNotes_.push_back({noteNumber, velocity, channel, noteOrderCounter_++});
        currentChannel_ = channel;
    }
    updatePattern();
}

void ArpEngine::handleNoteOff(int noteNumber, int channel) {
    auto& keyDown = keyDown_[channel - 1][noteNumber];
    if (keyDown) {
        keyDown = false;
        --numKeysDown_;
    }

    if (latchEnabled_) return; // Latch mode keeps notes

    // Remove note
    heldNotes_.erase(
        std::remove_if(heldNotes_.begin(), heldNotes_.end(),
            [noteNumber, channel](const HeldNote& n) {
                return n.noteNumber == noteNumber && n.channel == channel;
            }),
        heldNotes_.end()
    );

    updatePattern();
}

void ArpEngine::updatePattern() {
    if (heldNotes_.empty()) {
        currentPattern_.clear();
        currentStep_ = 0;
        return;
    }

    auto& baseNotes = baseNotes_;
    baseNotes.clear();
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

void ArpEngine::generateArpeggioNotes(juce::MidiBuffer& output, int numSamples,
                                       const juce::AudioPlayHead::PositionInfo& posInfo) {
    if (currentPattern_.empty()) {
        stopCurrentNote(output, 0);
        wasRunning_ = false;
        return;
    }

    const double samplesPerBeat = sampleRate_ * 60.0 / posInfo.getBpm().orFallback(120.0);
    const double stepBeats = getStepLengthInBeats();
    const auto hostPpq = posInfo.getPpqPosition();
    const bool hostPlaying = posInfo.getIsPlaying() && hostPpq.hasValue();

    // Re-lock to the grid when the arp starts, the transport starts or stops, the
    // rate changes, or the host jumps (loop, seek). Free-running starts at beat 0.
    bool resync = !wasRunning_ || hostPlaying != wasHostPlaying_ || stepBeats != lastStepBeats_;
    if (!hostPlaying && resync)
        freeRunBeat_ = 0.0;

    const double blockBeat = hostPlaying ? *hostPpq : freeRunBeat_;
    resync = resync || std::abs(blockBeat - expectedBlockBeat_) > 1.0e-3;

    if (resync) {
        stopCurrentNote(output, 0);
        lastStepIndex_ = (int64_t)std::ceil(blockBeat / stepBeats - 1.0e-9) - 1;
    }

    auto sampleAt = [&](double beat) { return (int)std::round((beat - blockBeat) * samplesPerBeat); };
    const double swingBeats = swing_ * 0.5 * stepBeats; // off-beats move up to half a step late

    while (true) {
        const int64_t stepIndex = lastStepIndex_ + 1;
        const double stepBeat = (double)stepIndex * stepBeats + ((stepIndex & 1) ? swingBeats : 0.0);
        const int stepSample = juce::jmax(0, sampleAt(stepBeat));
        if (stepSample >= numSamples) break;

        // Note off for previous note, at the end of its gate or at the latest now
        if (currentNoteOn_ >= 0)
            stopCurrentNote(output, juce::jlimit(0, stepSample, sampleAt(noteOffBeat_)));

        // Note on for current step
        int noteToPlay = currentPattern_[currentStep_];
        if (pattern_ == Pattern::Random) {
            noteToPlay = currentPattern_[juce::Random::getSystemRandom().nextInt((int)currentPattern_.size())];
        }

        int velocity = getVelocityForNote();
        float normalizedVelocity = velocity / 127.0f; // JUCE uses 0.0-1.0 for velocity
        output.addEvent(juce::MidiMessage::noteOn(currentChannel_, noteToPlay, normalizedVelocity),
                        stepSample);
        currentNoteOn_ = noteToPlay;
        noteOffBeat_ = stepBeat + gate_ * stepBeats;

        // Advance to next step
        currentStep_ = (currentStep_ + 1) % (int)currentPattern_.size();
        lastStepIndex_ = stepIndex;
    }

    // Note off that falls in this block after the last step
    if (currentNoteOn_ >= 0) {
        const int noteOffSample = sampleAt(noteOffBeat_);
        if (noteOffSample < numSamples)
            stopCurrentNote(output, juce::jmax(0, noteOffSample));
    }

    expectedBlockBeat_ = blockBeat + numSamples / samplesPerBeat;
    freeRunBeat_ = expectedBlockBeat_;
    lastStepBeats_ = stepBeats;
    wasHostPlaying_ = hostPlaying;
    wasRunning_ = true;
}

void ArpEngine::stopCurrentNote(juce::MidiBuffer& output, int samplePosition) {
    if (currentNoteOn_ < 0) return;
    output.addEvent(juce::MidiMessage::noteOff(currentChannel_, currentNoteOn_), samplePosition);
    currentNoteOn_ = -1;
}

double ArpEngine::getStepLengthInBeats() const {
    switch (rate_) {
        case Rate::Whole:            return 4.0;
        case Rate::Half:             return 2.0;
        case Rate::Quarter:          return 1.0;
        case Rate::Eighth:           return 1.0 / 2.0;
        case Rate::Sixteenth:        return 1.0 / 4.0;
        case Rate::ThirtySecond:     return 1.0 / 8.0;
        case Rate::QuarterTriplet:   return 2.0 / 3.0;
        case Rate::EighthTriplet:    return 1.0 / 3.0;
        case Rate::SixteenthTriplet: return 1.0 / 6.0;
    }
    return 1.0 / 4.0;
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
