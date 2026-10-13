#include "QuantizerEngine.h"

namespace {
    // Note-on/off checks on a raw short message: -1 if it is not that kind of message,
    // else channel * 128 + note.
    int noteKey(const juce::uint8* data, int size, bool noteOn) {
        if (size != 3) return -1;
        const int type = data[0] & 0xf0;
        const bool isOn = type == 0x90 && data[2] > 0;
        const bool isOff = type == 0x80 || (type == 0x90 && data[2] == 0);
        return (noteOn ? isOn : isOff) ? (data[0] & 0x0f) * 128 + data[1] : -1;
    }
}

QuantizerEngine::QuantizerEngine()
    : randomGen_(std::random_device{}()),
      randomDist_(-1.0f, 1.0f) {
    lastNoteOnDue_.fill(noNoteOn);
}

void QuantizerEngine::prepareToPlay(double sampleRate, int samplesPerBlock) {
    sampleRate_ = sampleRate;
    numPending_ = 0;
    samplesElapsed_ = 0;
    noteShift_ = {};
    lastNoteOnDue_.fill(noNoteOn);
    output_.ensureSize((size_t) (maxPendingEvents + samplesPerBlock) * 16);
    updateLatency(120.0, false);  // first guess until the host reports a tempo
}

void QuantizerEngine::processBlock(juce::MidiBuffer& midiMessages, int numSamples,
                                    const juce::Optional<juce::AudioPlayHead::PositionInfo>& posInfo) {
    const int64_t blockStart = samplesElapsed_;
    const int64_t blockEnd = blockStart + numSamples;
    samplesElapsed_ = blockEnd;
    output_.clear();

    const bool hasTempo = posInfo.hasValue() && posInfo->getBpm().hasValue()
                          && posInfo->getPpqPosition().hasValue()
                          && std::isfinite(*posInfo->getBpm()) && *posInfo->getBpm() >= 1.0;
    const bool isPlaying = hasTempo && posInfo->getIsPlaying();

    if (hasTempo)
        updateLatency(*posInfo->getBpm(), isPlaying);

    if (!isPlaying) {
        // No running beat grid to quantize against: release anything still
        // delayed, then pass the input straight through.
        flushPending();

        for (const auto metadata : midiMessages) {
            // A note-off for this note must not reuse an older note's shift if play starts
            const int key = noteKey(metadata.data, metadata.numBytes, true);
            if (key >= 0)
                noteShift_[(size_t) key / 128][(size_t) key % 128] = 0;
            output_.addEvent(metadata.data, metadata.numBytes, metadata.samplePosition);
        }
    } else {
        const double samplesPerBeat = sampleRate_ * 60.0 / *posInfo->getBpm();
        const double blockStartPpq = *posInfo->getPpqPosition();

        for (const auto metadata : midiMessages) {
            if (metadata.numBytes > 3) {
                // SysEx is not delayed: it is rare and can't be queued without allocating
                output_.addEvent(metadata.data, metadata.numBytes, metadata.samplePosition);
                continue;
            }

            auto msg = metadata.getMessage();
            const int64_t inputDue = blockStart + metadata.samplePosition + latencySamples_;
            int64_t shift = 0;

            if (msg.isNoteOn()) {
                const double ppq = blockStartPpq + metadata.samplePosition / samplesPerBeat;
                shift = juce::jmax((int64_t) -latencySamples_,
                                   (int64_t) std::llround(quantizeOffsetInBeats(ppq) * samplesPerBeat));

                // Never move a note-on to or before the previous same-pitch note-on
                // (humanized timing or a host loop can swap them), so that note keeps a length
                const auto key = (size_t) (msg.getChannel() - 1) * 128 + (size_t) msg.getNoteNumber();
                shift = juce::jmax(shift, lastNoteOnDue_[key] + 1 - inputDue);
                lastNoteOnDue_[key] = inputDue + shift;
                noteShift_[(size_t) msg.getChannel() - 1][(size_t) msg.getNoteNumber()] = shift;

                if (humanizeVelocity_ > 0.0f)
                    msg = juce::MidiMessage::noteOn(msg.getChannel(), msg.getNoteNumber(),
                                                    (juce::uint8) calculateHumanizedVelocity(msg.getVelocity()));
            } else if (msg.isNoteOff()) {
                // Move the note-off by the same amount as its note-on to keep the note length
                shift = noteShift_[(size_t) msg.getChannel() - 1][(size_t) msg.getNoteNumber()];
            }

            schedule(msg, inputDue + shift);
        }

        // Emit events due in this block; keep the rest, in order
        int kept = 0;
        for (int i = 0; i < numPending_; ++i) {
            const auto& event = pending_[(size_t) i];
            if (event.dueSample < blockEnd)
                output_.addEvent(event.data, event.size, (int) juce::jmax((int64_t) 0, event.dueSample - blockStart));
            else
                pending_[(size_t) kept++] = event;
        }
        numPending_ = kept;
    }

    midiMessages.clear();
    midiMessages.addEvents(output_, 0, -1, 0);
}

void QuantizerEngine::schedule(const juce::MidiMessage& msg, int64_t dueSample) {
    if (numPending_ == maxPendingEvents) {
        // Queue full: release the oldest event now rather than drop it, so no note-off is lost
        output_.addEvent(pending_[0].data, pending_[0].size, 0);
        std::move(pending_.begin() + 1, pending_.end(), pending_.begin());
        --numPending_;
    }

    // A note-on moved earlier than the previous same-pitch note's queued note-off
    // would be cut short by it: pull that note-off back to the new note-on. It is
    // earlier in the queue, so at the same sample it still comes out first, and it
    // stays after its own note-on, which is always due before this one.
    const int key = noteKey(msg.getRawData(), msg.getRawDataSize(), true);
    if (key >= 0)
        for (int i = 0; i < numPending_; ++i) {
            auto& queued = pending_[(size_t) i];
            if (queued.dueSample > dueSample && noteKey(queued.data, queued.size, false) == key)
                queued.dueSample = dueSample;
        }

    auto& event = pending_[(size_t) numPending_++];
    event.dueSample = dueSample;
    event.size = msg.getRawDataSize();
    std::copy_n(msg.getRawData(), event.size, event.data);
}

void QuantizerEngine::flushPending() {
    if (numPending_ == 0)
        return;

    // Everything queued comes out now, so no queued note-on limits later ones
    lastNoteOnDue_.fill(noNoteOn);

    // A note whose note-on and note-off are both still queued has not started yet.
    // Releasing both at once would make a zero-length note, so drop the pair.
    // Walk backwards, remembering the nearest later queued note-off of each note.
    std::array<int16_t, 16 * 128> laterOff;
    laterOff.fill(-1);
    for (int i = numPending_ - 1; i >= 0; --i) {
        auto& event = pending_[(size_t) i];
        if (const int offKey = noteKey(event.data, event.size, false); offKey >= 0) {
            laterOff[(size_t) offKey] = (int16_t) i;
        } else if (const int onKey = noteKey(event.data, event.size, true);
                   onKey >= 0 && laterOff[(size_t) onKey] >= 0) {
            pending_[(size_t) laterOff[(size_t) onKey]].size = 0;
            event.size = 0;
            laterOff[(size_t) onKey] = -1;
        }
    }

    for (int i = 0; i < numPending_; ++i)
        if (pending_[(size_t) i].size > 0)
            output_.addEvent(pending_[(size_t) i].data, pending_[(size_t) i].size, 0);
    numPending_ = 0;
}

void QuantizerEngine::updateLatency(double bpm, bool isPlaying) {
    // Notes can be pulled earlier by up to half a grid step, plus up to 10% of a step
    // of timing humanization. Round up to a power of two so tempo changes rarely
    // change the latency, and only shrink it while stopped, as the host may need
    // to re-align when the latency changes.
    const double samplesPerBeat = sampleRate_ * 60.0 / bpm;
    const int required = juce::nextPowerOfTwo((int) std::ceil(0.6 * getGridSizeInBeats() * samplesPerBeat));

    if (required > latencySamples_ || (!isPlaying && required != latencySamples_))
        latencySamples_ = required;
}

double QuantizerEngine::quantizeOffsetInBeats(double ppq) {
    const double grid = getGridSizeInBeats();
    const double gridIndex = std::round(ppq / grid);
    double target = gridIndex * grid;

    // Apply swing to odd-numbered grid positions (the off-beats)
    if (swing_ > 0.0f && ((int64_t) gridIndex & 1) != 0)
        target += grid * swing_ * 0.5;  // Max swing is 50% of grid

    // Apply quantize strength (partial quantization)
    double offset = (target - ppq) * quantizeStrength_;

    if (humanizeTiming_ > 0.0f)
        offset += getRandomOffset() * 0.1 * humanizeTiming_ * grid;

    return offset;
}

double QuantizerEngine::getGridSizeInBeats() const {
    switch (gridResolution_) {
        case GridResolution::Quarter:          return 1.0;
        case GridResolution::Eighth:           return 1.0 / 2.0;
        case GridResolution::Sixteenth:        return 1.0 / 4.0;
        case GridResolution::ThirtySecond:     return 1.0 / 8.0;
        case GridResolution::EighthTriplet:    return 1.0 / 3.0;
        case GridResolution::SixteenthTriplet: return 1.0 / 6.0;
    }
    return 1.0 / 4.0;
}

int QuantizerEngine::calculateHumanizedVelocity(int originalVelocity) {
    if (humanizeVelocity_ <= 0.0f) {
        return originalVelocity;
    }

    // Apply random variation to velocity
    int maxVariation = (int)(30 * humanizeVelocity_);  // Max ±30 at full humanize
    int variation = (int)(getRandomOffset() * maxVariation);
    int newVelocity = originalVelocity + variation;

    return juce::jlimit(1, 127, newVelocity);
}

float QuantizerEngine::getRandomOffset() {
    return randomDist_(randomGen_);
}
