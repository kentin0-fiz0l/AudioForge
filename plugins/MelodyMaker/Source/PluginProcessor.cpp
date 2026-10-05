/*
  PluginProcessor.cpp - Implementation
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../../../shared/dsp/ReverbLevels.h"

// Improved synthesizer voice with waveforms and ADSR
class ImprovedSynthVoice : public juce::SynthesiserVoice
{
public:
    ImprovedSynthVoice(MelodyMakerProcessor* proc) : processor(proc) {}

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override
    {
        currentAngle = 0.0;
        noteVelocity = velocity;

        auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        auto cyclesPerSample = cyclesPerSecond / getSampleRate();
        angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;

        // Start ADSR envelope
        envelope.setSampleRate(getSampleRate());
        envelope.setParameters({processor->getAttack(), processor->getDecay(),
                               processor->getSustain(), processor->getRelease()});
        envelope.noteOn();
    }

    void stopNote (float, bool allowTailOff) override
    {
        if (allowTailOff)
            envelope.noteOff();
        else
        {
            clearCurrentNote();
            angleDelta = 0.0;
        }
    }

    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (angleDelta != 0.0)
        {
            while (--numSamples >= 0)
            {
                // Generate waveform
                float sample = generateWaveform(currentAngle, processor->getWaveform());

                // Apply ADSR envelope
                float envValue = envelope.getNextSample();
                sample *= envValue * noteVelocity * 0.3f;

                // Output to all channels
                for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                    outputBuffer.addSample(i, startSample, sample);

                currentAngle += angleDelta;
                ++startSample;

                // Stop if envelope finished
                if (!envelope.isActive())
                {
                    clearCurrentNote();
                    angleDelta = 0.0;
                    break;
                }
            }
        }
    }

private:
    float generateWaveform(double angle, int waveType)
    {
        switch (waveType)
        {
            case 0: // Sine
                return (float)std::sin(angle);

            case 1: // Saw
            {
                double normalizedAngle = std::fmod(angle, juce::MathConstants<double>::twoPi);
                return (float)((normalizedAngle / juce::MathConstants<double>::pi) - 1.0);
            }

            case 2: // Square
                return (std::sin(angle) >= 0.0) ? 1.0f : -1.0f;

            case 3: // Triangle
            {
                double normalizedAngle = std::fmod(angle, juce::MathConstants<double>::twoPi);
                if (normalizedAngle < juce::MathConstants<double>::pi)
                    return (float)((normalizedAngle / juce::MathConstants<double>::halfPi) - 1.0);
                else
                    return (float)(3.0 - (normalizedAngle / juce::MathConstants<double>::halfPi));
            }

            default:
                return (float)std::sin(angle);
        }
    }

    MelodyMakerProcessor* processor;
    double currentAngle = 0.0;
    double angleDelta = 0.0;
    float noteVelocity = 0.0f;
    juce::ADSR envelope;
};

struct SineWaveSound : public juce::SynthesiserSound
{
    bool appliesToNote (int) override { return true; }
    bool appliesToChannel (int) override { return true; }
};

MelodyMakerProcessor::MelodyMakerProcessor()
    : AudioProcessor (BusesProperties()
                     .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
    // Initialize synthesizer with 8 voices
    for (int i = 0; i < 8; ++i)
        synth_.addVoice(new ImprovedSynthVoice(this));

    synth_.addSound(new SineWaveSound());

    // Generate initial melody
    MelodyEngine::MelodyParams params;
    params.style = "Pop";
    params.noteDensity = 5;
    params.minPitch = 60;
    params.maxPitch = 84;
    params.rhythmComplexity = 5;
    params.useChordTones = true;
    params.useSyncopation = false;

    currentMelody_ = melodyEngine_.generateMelodyInScale(60, true, params, 4);
}

MelodyMakerProcessor::~MelodyMakerProcessor()
{
}

void MelodyMakerProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sampleRate_ = sampleRate;
    synth_.setCurrentPlaybackSampleRate(sampleRate);

    // Initialize effects
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 2;

    delayLine_.prepare(spec);
    delayLine_.setDelay(static_cast<float>(sampleRate * delayTime_));

    filter_.prepare(spec);
    filter_.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    filter_.setCutoffFrequency(static_cast<float>(filterCutoff_));
    filter_.setResonance(filterResonance_);

    juce::dsp::Reverb::Parameters reverbParams;
    reverbParams.roomSize = 0.5f;
    reverbParams.damping = 0.5f;
    const auto reverbLevels = AudioForge::DSP::reverbLevelsForMix(reverbMix_);
    reverbParams.wetLevel = reverbLevels.wet;
    reverbParams.dryLevel = reverbLevels.dry;
    reverb_.setParameters(reverbParams);

    delayBuffer_.setSize(2, samplesPerBlock);
}

void MelodyMakerProcessor::releaseResources()
{
}

void MelodyMakerProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                         juce::MidiBuffer& midiMessages)
{
    buffer.clear();
    internalMidiBuffer_.clear();

    // Get transport info
    auto playHead = getPlayHead();
    if (playHead != nullptr)
    {
        auto position = playHead->getPosition();
        if (position.hasValue())
        {
            auto isPlaying = position->getIsPlaying();
            if (isPlaying && melodyEnabled_)
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

                // Loop the melody
                double loopStartBeat = std::floor(currentBeat / melodyLengthInBeats_) * melodyLengthInBeats_;
                double beatInLoop = currentBeat - loopStartBeat;

                int numSamples = buffer.getNumSamples();

                // Generate MIDI notes from melody
                for (const auto& note : currentMelody_)
                {
                    double noteStartBeat = note.startTime;
                    double noteEndBeat = note.startTime + note.duration;

                    // Check if this note should start in this buffer
                    if (noteStartBeat >= beatInLoop && noteStartBeat < beatInLoop + (numSamples * beatsPerSample))
                    {
                        double beatOffset = noteStartBeat - beatInLoop;
                        int sampleOffset = static_cast<int>(beatOffset / beatsPerSample);
                        sampleOffset = juce::jlimit(0, numSamples - 1, sampleOffset);

                        internalMidiBuffer_.addEvent(
                            juce::MidiMessage::noteOn(1, note.pitch, static_cast<juce::uint8>(note.velocity)),
                            sampleOffset
                        );
                    }

                    // Check if this note should end in this buffer
                    if (noteEndBeat >= beatInLoop && noteEndBeat < beatInLoop + (numSamples * beatsPerSample))
                    {
                        double beatOffset = noteEndBeat - beatInLoop;
                        int sampleOffset = static_cast<int>(beatOffset / beatsPerSample);
                        sampleOffset = juce::jlimit(0, numSamples - 1, sampleOffset);

                        internalMidiBuffer_.addEvent(
                            juce::MidiMessage::noteOff(1, note.pitch, static_cast<juce::uint8>(0)),
                            sampleOffset
                        );
                    }
                }
            }
        }
    }

    // Render audio from synthesizer
    synth_.renderNextBlock(buffer, internalMidiBuffer_, 0, buffer.getNumSamples());

    // Apply effects chain: Filter → Delay → Reverb
    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();

    // 1. Filter
    for (int channel = 0; channel < numChannels; ++channel) {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < numSamples; ++sample) {
            channelData[sample] = filter_.processSample(channel, channelData[sample]);
        }
    }

    // 2. Delay
    if (delayMix_ > 0.0f) {
        delayBuffer_.makeCopyOf(buffer, true);
        for (int channel = 0; channel < numChannels; ++channel) {
            auto* channelData = buffer.getWritePointer(channel);
            auto* delayData = delayBuffer_.getWritePointer(channel);

            for (int sample = 0; sample < numSamples; ++sample) {
                float delaySample = delayLine_.popSample(channel);
                float input = channelData[sample] + delaySample * delayFeedback_;
                delayLine_.pushSample(channel, input);
                channelData[sample] += delaySample * delayMix_;
            }
        }
    }

    // 3. Reverb
    if (reverbMix_ > 0.0f) {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);

        juce::dsp::Reverb::Parameters params;
        params.roomSize = 0.5f;
        params.damping = 0.5f;
        const auto reverbLevels = AudioForge::DSP::reverbLevelsForMix(reverbMix_);
        params.wetLevel = reverbLevels.wet;
        params.dryLevel = reverbLevels.dry;
        reverb_.setParameters(params);

        reverb_.process(context);
    }
}

juce::AudioProcessorEditor* MelodyMakerProcessor::createEditor()
{
    return new MelodyMakerEditor (*this);
}

void MelodyMakerProcessor::getStateInformation (juce::MemoryBlock&)
{
    // Save state
}

void MelodyMakerProcessor::setStateInformation (const void*, int)
{
    // Restore state
}

void MelodyMakerProcessor::generateNewMelody(const std::string& style, int numBars)
{
    MelodyEngine::MelodyParams params;
    params.style = style;
    params.noteDensity = 5;
    params.minPitch = keyRoot_;
    params.maxPitch = keyRoot_ + 24; // 2 octaves
    params.rhythmComplexity = 5;
    params.useChordTones = true;
    params.useSyncopation = (style == "Jazz" || style == "Funk");

    currentMelody_ = melodyEngine_.generateMelodyInScale(keyRoot_, keyIsMajor_, params, numBars);
}

void MelodyMakerProcessor::setWaveform(int waveform)
{
    waveform_ = juce::jlimit(0, 3, waveform);
}

void MelodyMakerProcessor::exportToMidiFile(const juce::File& file)
{
    juce::MidiFile midiFile;
    midiFile.setTicksPerQuarterNote(480);

    juce::MidiMessageSequence track;

    // Set tempo (120 BPM default)
    track.addEvent(juce::MidiMessage::tempoMetaEvent(500000), 0);

    // Set time signature (4/4)
    track.addEvent(juce::MidiMessage::timeSignatureMetaEvent(4, 4), 0);

    // Set track name
    track.addEvent(juce::MidiMessage::textMetaEvent(3, "Melody"), 0);

    // Convert melody to MIDI
    int ticksPerBeat = 480;

    for (const auto& note : currentMelody_) {
        int velocity = note.velocity;

        // Note on
        int timeStamp = static_cast<int>(note.startTime * ticksPerBeat);
        track.addEvent(juce::MidiMessage::noteOn(1, note.pitch, juce::uint8(velocity)), timeStamp);

        // Note off
        int endTime = static_cast<int>((note.startTime + note.duration) * ticksPerBeat);
        track.addEvent(juce::MidiMessage::noteOff(1, note.pitch), endTime);
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
    return new MelodyMakerProcessor();
}
