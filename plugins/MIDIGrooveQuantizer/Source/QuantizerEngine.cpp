#include "QuantizerEngine.h"

QuantizerEngine::QuantizerEngine()
    : randomGen_(std::random_device{}()),
      randomDist_(-1.0f, 1.0f) {}

void QuantizerEngine::prepareToPlay(double sampleRate) {
    sampleRate_ = sampleRate;
}

void QuantizerEngine::processBlock(juce::MidiBuffer& midiMessages, int numSamples,
                                    const juce::AudioPlayHead::PositionInfo& posInfo) {
    juce::MidiBuffer processedMidi;

    if (!posInfo.getBpm().hasValue()) {
        // No tempo info, pass through unmodified
        return;
    }

    double bpm = *posInfo.getBpm();
    int64_t totalSamplesProcessed = posInfo.getTimeInSamples().orFallback(0);

    // Process incoming MIDI messages
    for (const auto metadata : midiMessages) {
        auto msg = metadata.getMessage();
        int samplePosition = metadata.samplePosition;

        if (msg.isNoteOn() || msg.isNoteOff()) {
            // Calculate quantized position
            int64_t absoluteSample = totalSamplesProcessed + samplePosition;
            int64_t quantizedSample = calculateQuantizedPosition(absoluteSample, bpm, totalSamplesProcessed);

            // Apply humanization to timing
            if (humanizeTiming_ > 0.0f && msg.isNoteOn()) {
                int64_t maxOffset = (int64_t)(getGridSizeInSamples(bpm) * 0.1 * humanizeTiming_);
                int64_t randomOffset = (int64_t)(getRandomOffset() * maxOffset);
                quantizedSample += randomOffset;
            }

            // Calculate relative position in current block
            int newPosition = (int)(quantizedSample - totalSamplesProcessed);

            // Create potentially modified message
            juce::MidiMessage newMsg = msg;

            if (msg.isNoteOn() && humanizeVelocity_ > 0.0f) {
                int newVelocity = calculateHumanizedVelocity(msg.getVelocity());
                newMsg = juce::MidiMessage::noteOn(msg.getChannel(), msg.getNoteNumber(),
                                                  newVelocity / 127.0f);
            }

            // Only output if within current block
            if (newPosition >= 0 && newPosition < numSamples) {
                processedMidi.addEvent(newMsg, newPosition);
            } else if (newPosition >= numSamples) {
                // Note quantized to future block - would need lookahead buffer
                // For now, output at end of block
                processedMidi.addEvent(newMsg, numSamples - 1);
            } else {
                // Note quantized to past - output immediately
                processedMidi.addEvent(newMsg, 0);
            }
        } else {
            // Pass through non-note messages (CC, pitch bend, etc.)
            processedMidi.addEvent(msg, samplePosition);
        }
    }

    // Replace input MIDI with quantized MIDI
    midiMessages.swapWith(processedMidi);
}

int64_t QuantizerEngine::calculateQuantizedPosition(int64_t originalSample, double bpm,
                                                      int64_t totalSamplesProcessed) {
    int64_t gridSize = getGridSizeInSamples(bpm);

    // Find nearest grid position
    int64_t gridIndex = (originalSample + gridSize / 2) / gridSize;
    int64_t nearestGridSample = gridIndex * gridSize;

    // Apply swing to odd-numbered grid positions (every other beat)
    if (swing_ > 0.0f && (gridIndex % 2) == 1) {
        int64_t swingOffset = (int64_t)(gridSize * swing_ * 0.5);  // Max swing is 50% of grid
        nearestGridSample += swingOffset;
    }

    // Apply quantize strength (partial quantization)
    if (quantizeStrength_ < 1.0f) {
        int64_t offset = nearestGridSample - originalSample;
        nearestGridSample = originalSample + (int64_t)(offset * quantizeStrength_);
    }

    return nearestGridSample;
}

int64_t QuantizerEngine::getGridSizeInSamples(double bpm) {
    double beatsPerSecond = bpm / 60.0;
    double secondsPerBeat = 1.0 / beatsPerSecond;
    double secondsPerGrid = secondsPerBeat;

    switch (gridResolution_) {
        case GridResolution::Quarter:
            secondsPerGrid = secondsPerBeat;
            break;
        case GridResolution::Eighth:
            secondsPerGrid = secondsPerBeat / 2.0;
            break;
        case GridResolution::Sixteenth:
            secondsPerGrid = secondsPerBeat / 4.0;
            break;
        case GridResolution::ThirtySecond:
            secondsPerGrid = secondsPerBeat / 8.0;
            break;
        case GridResolution::EighthTriplet:
            secondsPerGrid = secondsPerBeat / 3.0;
            break;
        case GridResolution::SixteenthTriplet:
            secondsPerGrid = secondsPerBeat / 6.0;
            break;
    }

    return (int64_t)(secondsPerGrid * sampleRate_);
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
