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
}

void DrumMachineProcessor::releaseResources()
{
}

void DrumMachineProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                         juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    // Get transport info
    auto playHead = getPlayHead();
    if (playHead != nullptr)
    {
        auto position = playHead->getPosition();
        if (position.hasValue())
        {
            auto isPlaying = position->getIsPlaying();
            if (isPlaying && patternEnabled_)
            {
                // Get tempo and beat position
                auto bpm = position->getBpm();
                double tempo = bpm.hasValue() ? *bpm : 120.0;

                double beatsPerSecond = tempo / 60.0;
                double beatsPerSample = beatsPerSecond / sampleRate_;

                auto ppqPos = position->getPpqPosition();
                double currentBeat = ppqPos.hasValue() ? *ppqPos : 0.0;
                if (currentBeat < 0.0)
                    currentBeat = 0.0;

                // Loop the pattern (4 beats = 16 steps)
                double loopStartBeat = std::floor(currentBeat / patternLengthInBeats_) * patternLengthInBeats_;
                double beatInLoop = currentBeat - loopStartBeat;

                int numSamples = buffer.getNumSamples();

                // Process each sample to detect step triggers
                for (int sample = 0; sample < numSamples; ++sample) {
                    double sampleBeat = beatInLoop + (sample * beatsPerSample);

                    // Convert beat to step (16 steps per 4 beats)
                    int currentStep = static_cast<int>((sampleBeat / patternLengthInBeats_) * 16.0) % 16;

                    // Trigger drums when entering a new step
                    if (currentStep != lastProcessedStep_) {
                        lastProcessedStep_ = currentStep;

                        // Get all hits at this step
                        auto hits = drumEngine_.getHitsAtStep(currentPattern_, currentStep);

                        // Trigger each drum hit
                        for (const auto& hit : hits) {
                            drumSynth_.triggerDrum(hit.voice, hit.velocity);
                        }
                    }
                }
            } else {
                // Reset step counter when not playing
                lastProcessedStep_ = -1;
            }
        }
    }

    // Render drum synth audio
    drumSynth_.renderNextBlock(buffer, 0, buffer.getNumSamples());
}

juce::AudioProcessorEditor* DrumMachineProcessor::createEditor()
{
    return new DrumMachineEditor (*this);
}

void DrumMachineProcessor::getStateInformation (juce::MemoryBlock&)
{
    // Save state
}

void DrumMachineProcessor::setStateInformation (const void*, int)
{
    // Restore state
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

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DrumMachineProcessor();
}
