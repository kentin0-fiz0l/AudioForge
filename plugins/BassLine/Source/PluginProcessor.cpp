/*
  PluginProcessor.cpp - Implementation
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../../../shared/dsp/ReverbLevels.h"

// Bass synthesizer voice with filter
class BassSynthVoice : public juce::SynthesiserVoice
{
public:
    BassSynthVoice(BassLineProcessor* proc) : processor(proc) {}

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

        // Reset filter
        filterState = 0.0f;
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
                sample *= envValue * noteVelocity * 0.4f;

                // Apply simple low-pass filter (for bass warmth)
                float cutoff = processor->getFilterCutoff();
                float sampleRate = static_cast<float>(getSampleRate());
                float rc = 1.0f / (2.0f * juce::MathConstants<float>::pi * cutoff);
                float dt = 1.0f / sampleRate;
                float alpha = dt / (rc + dt);
                filterState = filterState + alpha * (sample - filterState);
                sample = filterState;

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
            case 0: // Sine (sub bass)
                return static_cast<float>(std::sin(angle));

            case 1: // Saw (bright bass)
            {
                double normalizedAngle = std::fmod(angle, juce::MathConstants<double>::twoPi);
                return static_cast<float>((normalizedAngle / juce::MathConstants<double>::pi) - 1.0);
            }

            case 2: // Square (hollow bass)
                return (std::sin(angle) >= 0.0) ? 1.0f : -1.0f;

            case 3: // Triangle (warm bass)
            {
                double normalizedAngle = std::fmod(angle, juce::MathConstants<double>::twoPi);
                if (normalizedAngle < juce::MathConstants<double>::pi)
                    return static_cast<float>((normalizedAngle / juce::MathConstants<double>::halfPi) - 1.0);
                else
                    return static_cast<float>(3.0 - (normalizedAngle / juce::MathConstants<double>::halfPi));
            }

            default:
                return static_cast<float>(std::sin(angle));
        }
    }

    BassLineProcessor* processor;
    double currentAngle = 0.0;
    double angleDelta = 0.0;
    float noteVelocity = 0.0f;
    float filterState = 0.0f;
    juce::ADSR envelope;
};

struct BassSynthSound : public juce::SynthesiserSound
{
    bool appliesToNote (int) override { return true; }
    bool appliesToChannel (int) override { return true; }
};

BassLineProcessor::BassLineProcessor()
    : AudioProcessor (BusesProperties()
                     .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
    // Initialize synthesizer with 8 voices
    for (int i = 0; i < 8; ++i)
        synth_.addVoice(new BassSynthVoice(this));

    synth_.addSound(new BassSynthSound());

    // Generate initial bass line
    BassEngine::BassParams params;
    params.style = "Rock";
    params.complexity = 5;
    params.octave = 2;  // C2 (bass register)
    params.usePassingTones = false;
    params.useFifths = true;
    params.useOctaves = false;

    currentBassLine_ = bassEngine_.generateBassLineInKey(48, true, params, 4);  // C2 = 48
}

BassLineProcessor::~BassLineProcessor()
{
}

void BassLineProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sampleRate_ = sampleRate;
    synth_.setCurrentPlaybackSampleRate(sampleRate);

    // Initialize effects
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 2;

    delayLine_.prepare(spec);
    delayLine_.setDelay(static_cast<float>(sampleRate * 0.25f)); // 250ms delay

    juce::dsp::Reverb::Parameters reverbParams;
    reverbParams.roomSize = 0.5f;
    reverbParams.damping = 0.5f;
    const auto reverbLevels = AudioForge::DSP::reverbLevelsForMix(reverbMix_);
    reverbParams.wetLevel = reverbLevels.wet;
    reverbParams.dryLevel = reverbLevels.dry;
    reverb_.setParameters(reverbParams);
}

void BassLineProcessor::releaseResources()
{
}

void BassLineProcessor::processBlock (juce::AudioBuffer<float>& buffer,
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
            if (isPlaying && bassEnabled_)
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

                // Loop the bass line
                double loopStartBeat = std::floor(currentBeat / bassLengthInBeats_) * bassLengthInBeats_;
                double beatInLoop = currentBeat - loopStartBeat;

                int numSamples = buffer.getNumSamples();

                // Generate MIDI notes from bass line
                for (const auto& note : currentBassLine_)
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

    // Apply effects chain: Delay → Reverb
    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();

    // 1. Delay
    if (delayMix_ > 0.0f) {
        for (int channel = 0; channel < numChannels; ++channel) {
            auto* channelData = buffer.getWritePointer(channel);

            for (int sample = 0; sample < numSamples; ++sample) {
                float delaySample = delayLine_.popSample(channel);
                float input = channelData[sample] + delaySample * 0.3f; // Fixed 30% feedback
                delayLine_.pushSample(channel, input);
                channelData[sample] += delaySample * delayMix_;
            }
        }
    }

    // 2. Reverb
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

juce::AudioProcessorEditor* BassLineProcessor::createEditor()
{
    return new BassLineEditor (*this);
}

void BassLineProcessor::getStateInformation (juce::MemoryBlock&)
{
    // Save state
}

void BassLineProcessor::setStateInformation (const void*, int)
{
    // Restore state
}

void BassLineProcessor::generateNewBassLine(const std::string& style, int numBars)
{
    BassEngine::BassParams params;
    params.style = style;
    params.complexity = 5;
    params.octave = octave_;
    params.usePassingTones = (style == "Jazz");
    params.useFifths = (style != "EDM");
    params.useOctaves = false;

    currentBassLine_ = bassEngine_.generateBassLineInKey(keyRoot_, keyIsMajor_, params, numBars);
}

void BassLineProcessor::setWaveform(int waveform)
{
    waveform_ = juce::jlimit(0, 3, waveform);
}

void BassLineProcessor::exportToMidiFile(const juce::File& file)
{
    juce::MidiFile midiFile;
    midiFile.setTicksPerQuarterNote(480);
    juce::MidiMessageSequence track;

    track.addEvent(juce::MidiMessage::tempoMetaEvent(500000), 0);
    track.addEvent(juce::MidiMessage::timeSignatureMetaEvent(4, 4), 0);
    track.addEvent(juce::MidiMessage::textMetaEvent(3, "Bass"), 0);

    int ticksPerBeat = 480;

    for (const auto& note : currentBassLine_) {
        int velocity = note.velocity;
        int timeStamp = static_cast<int>(note.startTime * ticksPerBeat);
        track.addEvent(juce::MidiMessage::noteOn(1, note.pitch, juce::uint8(velocity)), timeStamp);

        int endTime = static_cast<int>((note.startTime + note.duration) * ticksPerBeat);
        track.addEvent(juce::MidiMessage::noteOff(1, note.pitch), endTime);
    }

    track.updateMatchedPairs();
    midiFile.addTrack(track);

    juce::FileOutputStream stream(file);
    if (stream.openedOk()) {
        midiFile.writeTo(stream);
    }
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BassLineProcessor();
}
