/*
  PluginProcessor.cpp - Implementation
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

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
        double tempo = bpm.hasValue() ? *bpm : 120.0;
        double beatsPerSample = tempo / 60.0 / sampleRate_;

        // Without a host position, carry on from where the last block ended
        auto ppqPos = position->getPpqPosition();
        double blockStartBeat = ppqPos.hasValue() ? std::max(0.0, *ppqPos) : expectedBlockStartBeat_;
        double blockEndBeat = blockStartBeat + numSamples * beatsPerSample;

        // The host jumped (a loop, a locate): start counting steps afresh
        if (std::abs(blockStartBeat - expectedBlockStartBeat_) > beatsPerSample)
            lastTriggeredStep_ = -1;
        expectedBlockStartBeat_ = blockEndBeat;

        // Swing delays the off-beat 16ths, up to a third of a step (the 16th triplet)
        const double swingDelay = swing_.load() * beatsPerStep / 3.0;
        const int numSteps = currentPattern_.numSteps;

        // Steps are counted from the start of the song and each fires once. A step the
        // last block did not reach (rounding, or swing turned down) fires at sample 0;
        // after a jump, only one starting within a sample of the new position does.
        const double earliestBeat = blockStartBeat - beatsPerSample;
        for (auto step = std::max<juce::int64>(0, static_cast<juce::int64>(earliestBeat / beatsPerStep) - 1); ; ++step)
        {
            const int stepInPattern = static_cast<int>(step % numSteps);
            const double onsetBeat = step * beatsPerStep + (stepInPattern % 2 == 1 ? swingDelay : 0.0);
            if (onsetBeat >= blockEndBeat)
                break;
            if (step <= lastTriggeredStep_ || (lastTriggeredStep_ < 0 && onsetBeat < earliestBeat))
                continue;

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
    state.setAttribute ("style", juce::String (currentPattern_.style));
    state.setAttribute ("complexity", complexity_);
    state.setAttribute ("swing", static_cast<double> (swing_.load()));
    state.setAttribute ("fills", fillsEnabled_);
    state.setAttribute ("enabled", patternEnabled_.load());

    // The pattern itself: generation is random and the grid can be edited
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

    complexity_ = juce::jlimit (1, 10, state->getIntAttribute ("complexity", complexity_));
    swing_ = juce::jlimit (0.0f, 1.0f, static_cast<float> (state->getDoubleAttribute ("swing", swing_.load())));
    fillsEnabled_ = state->getBoolAttribute ("fills", fillsEnabled_);
    patternEnabled_ = state->getBoolAttribute ("enabled", patternEnabled_.load());

    DrumEngine::DrumPattern pattern;
    pattern.numSteps = 16;
    pattern.style = state->getStringAttribute ("style", "Rock").toStdString();

    for (auto* hitXml : state->getChildWithTagNameIterator ("Hit")) {
        const int voice = hitXml->getIntAttribute ("voice", -1);
        const int step = hitXml->getIntAttribute ("step", -1);
        if (voice < 0 || voice >= DrumEngine::NUM_VOICES || step < 0 || step >= pattern.numSteps)
            continue;

        const auto velocity = juce::jlimit (0.0f, 1.0f, static_cast<float> (hitXml->getDoubleAttribute ("velocity", 0.8)));
        pattern.voices[voice].push_back ({ static_cast<DrumEngine::DrumVoice> (voice), step, velocity });
    }

    currentPattern_ = std::move (pattern);
}

void DrumMachineProcessor::generateNewPattern(const std::string& style)
{
    DrumEngine::PatternParams params;
    params.style = style;
    params.complexity = complexity_;
    params.swing = swing_;
    params.fillsEnabled = fillsEnabled_;

    currentPattern_ = drumEngine_.generatePattern(params);
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

    for (int v = 0; v < DrumEngine::NUM_VOICES; ++v) {
        const auto& voiceHits = currentPattern_.voices[v];

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
