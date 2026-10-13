/*
  PluginProcessor.cpp - Implementation
*/

#include "PluginProcessor.h"
#include <algorithm>
#include <cmath>
#include "PluginEditor.h"

namespace
{
    const juce::StringArray styles { "Rock", "Hip-Hop", "Jazz", "EDM", "Funk", "Latin", "Blues" };
}

DrumMachineProcessor::DrumMachineProcessor()
    : AudioProcessor (BusesProperties()
                     .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
    // Generate initial pattern
    DrumEngine::PatternParams params;
    params.style = "Rock";
    params.complexity = 5;
    params.swing = 0.0f;
    params.fillsEnabled = false;

    currentPattern_ = drumEngine_.generatePattern(params);
}

DrumMachineProcessor::~DrumMachineProcessor()
{
}

void DrumMachineProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sampleRate_ = sampleRate;
    drumSynth_.prepare(sampleRate, samplesPerBlock);
    resetSequencer();
}

void DrumMachineProcessor::releaseResources()
{
}

void DrumMachineProcessor::resetSequencer()
{
    lastTriggeredStep_ = -1;
    expectedBlockStartBeat_ = 0.0;
    resync_ = true;
}

void DrumMachineProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                         juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    const int numSamples = buffer.getNumSamples();
    int renderedSamples = 0;

    // Get transport info
    juce::Optional<juce::AudioPlayHead::PositionInfo> position;
    if (auto* playHead = getPlayHead())
        position = playHead->getPosition();

    if (position.hasValue() && position->getIsPlaying() && patternEnabled_)
    {
        // Get tempo and beat position
        auto bpm = position->getBpm();
        double tempo = bpm.hasValue() && std::isfinite (*bpm) && *bpm > 0.0 ? *bpm : 120.0;
        double beatsPerSample = tempo / 60.0 / sampleRate_;

        // Without a host position, carry on from where the last block ended. A count-in or
        // pre-roll is negative: step 0 then lands where the position crosses 0.
        auto ppqPos = position->getPpqPosition();
        double blockStartBeat = ppqPos.hasValue() && std::isfinite (*ppqPos) ? *ppqPos : expectedBlockStartBeat_;
        double blockEndBeat = blockStartBeat + numSamples * beatsPerSample;

        // The host jumped (a loop, a locate): start counting steps afresh. A smaller
        // difference is the tempo changing within a block; missed steps catch up at sample 0.
        if (std::abs(blockStartBeat - expectedBlockStartBeat_) > beatsPerStep)
            resetSequencer();
        expectedBlockStartBeat_ = blockEndBeat;

        // If the pattern is being replaced, this block's steps catch up in the next one
        const juce::SpinLock::ScopedTryLockType patternLock (patternLock_);
        if (patternLock.isLocked())
        {
            // Swing delays the off-beat 16ths, up to a third of a step (the 16th triplet)
            const double swingDelay = swing_.load() * beatsPerStep / 3.0;
            const int numSteps = currentPattern_.numSteps;

            // Steps are counted from the start of the song and each fires once. A step the
            // last block did not reach (rounding, tempo drift, or swing turned down) fires at
            // sample 0; after a start or a jump, steps already passed are skipped.
            const double earliestBeat = blockStartBeat - beatsPerSample;
            for (auto step = std::max<juce::int64>(0, static_cast<juce::int64>(std::floor(earliestBeat / beatsPerStep)) - 1); ; ++step)
            {
                const int stepInPattern = static_cast<int>(step % numSteps);
                const double onsetBeat = step * beatsPerStep + (stepInPattern % 2 == 1 ? swingDelay : 0.0);
                if (onsetBeat >= blockEndBeat)
                    break;
                if (step <= lastTriggeredStep_)
                    continue;
                if (resync_ && onsetBeat < earliestBeat)
                {
                    lastTriggeredStep_ = step;
                    continue;
                }

                const int onsetSample = std::max(0, juce::roundToInt((onsetBeat - blockStartBeat) / beatsPerSample));
                if (onsetSample >= numSamples)
                    break;  // Rounds into the next block, which picks it up

                // Render up to the hit so it starts on its own sample
                drumSynth_.renderNextBlock(buffer, renderedSamples, onsetSample - renderedSamples);
                renderedSamples = onsetSample;

                for (const auto& voiceHits : currentPattern_.voices)
                    for (const auto& hit : voiceHits)
                        if (hit.step == stepInPattern)
                            drumSynth_.triggerDrum(hit.voice, hit.velocity);

                lastTriggeredStep_ = step;
            }
            resync_ = false;
        }
    }
    else
    {
        resetSequencer();
    }

    // Render the rest of the block
    drumSynth_.renderNextBlock(buffer, renderedSamples, numSamples - renderedSamples);
}

juce::AudioProcessorEditor* DrumMachineProcessor::createEditor()
{
    return new DrumMachineEditor (*this);
}

void DrumMachineProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::XmlElement state ("DrumMachine");
    state.setAttribute ("complexity", complexity_.load());
    state.setAttribute ("swing", static_cast<double> (swing_.load()));
    state.setAttribute ("fills", fillsEnabled_.load());
    state.setAttribute ("enabled", patternEnabled_.load());

    // The pattern itself: generation is random and the grid can be edited
    const juce::ScopedLock edit (patternEditLock_);
    state.setAttribute ("style", juce::String (currentPattern_.style));
    for (const auto& voiceHits : currentPattern_.voices) {
        for (const auto& hit : voiceHits) {
            auto* hitXml = state.createNewChildElement ("Hit");
            hitXml->setAttribute ("voice", static_cast<int> (hit.voice));
            hitXml->setAttribute ("step", hit.step);
            hitXml->setAttribute ("velocity", static_cast<double> (hit.velocity));
        }
    }

    copyXmlToBinary (state, destData);
}

void DrumMachineProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto state = getXmlFromBinary (data, sizeInBytes);
    if (state == nullptr || ! state->hasTagName ("DrumMachine"))
        return;

    // A project file can be damaged or hand-edited, so nothing from it reaches the audio thread
    // unchecked: settings are kept to their ranges (a non-finite one is ignored), only known
    // styles are taken, hits must be in the grid, and each cell holds at most one hit.
    complexity_ = juce::jlimit (1, 10, state->getIntAttribute ("complexity", complexity_));
    if (const double swing = state->getDoubleAttribute ("swing", swing_.load()); std::isfinite (swing))
        swing_ = juce::jlimit (0.0f, 1.0f, static_cast<float> (swing));
    fillsEnabled_ = state->getBoolAttribute ("fills", fillsEnabled_);
    patternEnabled_ = state->getBoolAttribute ("enabled", patternEnabled_.load());

    const juce::ScopedLock edit (patternEditLock_);
    DrumEngine::DrumPattern pattern;
    pattern.numSteps = 16;
    const auto style = state->getStringAttribute ("style");
    pattern.style = styles.contains (style) ? style.toStdString() : currentPattern_.style;

    for (auto* hitXml : state->getChildWithTagNameIterator ("Hit")) {
        const int voice = hitXml->getIntAttribute ("voice", -1);
        const int step = hitXml->getIntAttribute ("step", -1);
        if (voice < 0 || voice >= DrumEngine::NUM_VOICES || step < 0 || step >= pattern.numSteps)
            continue;

        auto& voiceHits = pattern.voices[voice];
        if (std::any_of (voiceHits.begin(), voiceHits.end(), [step] (const auto& hit) { return hit.step == step; }))
            continue;

        double velocity = hitXml->getDoubleAttribute ("velocity", 0.8);
        if (std::isnan (velocity))
            velocity = 0.8;
        voiceHits.push_back ({ static_cast<DrumEngine::DrumVoice> (voice), step,
                               juce::jlimit (0.0f, 1.0f, static_cast<float> (velocity)) });
    }

    setPattern (std::move (pattern));
}

DrumEngine::DrumPattern DrumMachineProcessor::getCurrentPattern() const
{
    const juce::ScopedLock edit (patternEditLock_);
    return currentPattern_;
}

void DrumMachineProcessor::setPattern (DrumEngine::DrumPattern pattern)
{
    pattern.numSteps = juce::jlimit (1, 16, pattern.numSteps);  // The audio thread wraps steps by it

    const juce::ScopedLock edit (patternEditLock_);
    {
        // Swap rather than assign, so the audio thread never waits on an allocation
        const juce::SpinLock::ScopedLockType lock (patternLock_);
        std::swap (currentPattern_, pattern);
    }
    // The old pattern is freed here, outside the audio thread's lock
}

bool DrumMachineProcessor::setHit (int voice, int step, bool on)
{
    if (voice < 0 || voice >= DrumEngine::NUM_VOICES || step < 0 || step >= 16)
        return false;

    const juce::ScopedLock edit (patternEditLock_);
    auto pattern = currentPattern_;
    auto& voiceHits = pattern.voices[voice];
    const auto it = std::find_if (voiceHits.begin(), voiceHits.end(), [step] (const auto& hit) { return hit.step == step; });
    if ((it != voiceHits.end()) == on)
        return false;

    if (on)
        voiceHits.push_back ({ static_cast<DrumEngine::DrumVoice> (voice), step, 0.8f });
    else
        voiceHits.erase (it);
    setPattern (std::move (pattern));
    return true;
}

void DrumMachineProcessor::toggleHit (int voice, int step)
{
    const juce::ScopedLock edit (patternEditLock_);
    if (! setHit (voice, step, true))
        setHit (voice, step, false);
}

void DrumMachineProcessor::clearPattern()
{
    const juce::ScopedLock edit (patternEditLock_);
    auto pattern = currentPattern_;
    for (auto& voiceHits : pattern.voices)
        voiceHits.clear();
    setPattern (std::move (pattern));
}

void DrumMachineProcessor::generateNewPattern(const std::string& style)
{
    DrumEngine::PatternParams params;
    params.style = style;
    params.complexity = complexity_;
    params.swing = swing_;
    params.fillsEnabled = fillsEnabled_;

    const juce::ScopedLock edit (patternEditLock_);
    setPattern (drumEngine_.generatePattern(params));
}

void DrumMachineProcessor::exportToMidiFile(const juce::File& file)
{
    juce::MidiFile midiFile;
    midiFile.setTicksPerQuarterNote(480);

    juce::MidiMessageSequence track;

    // General MIDI drum note mapping
    std::map<DrumEngine::DrumVoice, int> drumNoteMap;
    drumNoteMap[DrumEngine::KICK] = 36;   // Bass Drum
    drumNoteMap[DrumEngine::SNARE] = 38;  // Snare
    drumNoteMap[DrumEngine::HIHAT] = 42;  // Closed Hi-Hat
    drumNoteMap[DrumEngine::TOM] = 47;    // Mid Tom
    drumNoteMap[DrumEngine::CRASH] = 49;  // Crash Cymbal

    // Set tempo (120 BPM default)
    track.addEvent(juce::MidiMessage::tempoMetaEvent(500000), 0);  // 500000 microseconds per quarter = 120 BPM

    // Set time signature (4/4)
    track.addEvent(juce::MidiMessage::timeSignatureMetaEvent(4, 4), 0);

    // Set track name
    track.addEvent(juce::MidiMessage::textMetaEvent(3, "Drums"), 0);

    // Convert pattern to MIDI
    int ticksPerBeat = 480;  // Quarter note
    int ticksPer16th = ticksPerBeat / 4;

    const auto pattern = getCurrentPattern();
    for (int v = 0; v < DrumEngine::NUM_VOICES; ++v) {
        const auto& voiceHits = pattern.voices[v];

        for (const auto& hit : voiceHits) {
            int midiNote = drumNoteMap[hit.voice];
            int velocity = static_cast<int>(hit.velocity * 127.0f);

            // Note on at step position
            int timeStamp = hit.step * ticksPer16th;
            track.addEvent(juce::MidiMessage::noteOn(10, midiNote, juce::uint8(velocity)), timeStamp);

            // Note off after 1/16th note (short duration for drums)
            track.addEvent(juce::MidiMessage::noteOff(10, midiNote), timeStamp + (ticksPer16th - 1));
        }
    }

    track.updateMatchedPairs();
    midiFile.addTrack(track);

    // Write to file
    juce::FileOutputStream stream(file);
    if (stream.openedOk()) {
        midiFile.writeTo(stream);
    }
}

#ifndef AUDIOFORGE_TESTS
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DrumMachineProcessor();
}
#endif
