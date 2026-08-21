#include "PluginProcessor.h"
#include "PluginEditor.h"

BasicSynthProcessor::BasicSynthProcessor()
    : AudioProcessor(BusesProperties()
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Waveform parameter: Sine, Saw, Square
    addParameter(waveformParam = new juce::AudioParameterChoice(
        PARAM_WAVEFORM,
        "Waveform",
        juce::StringArray{"Sine", "Sawtooth", "Square"},
        0)); // Default: Sine

    // Master volume parameter (0.0 to 1.0)
    addParameter(volumeParam = new juce::AudioParameterFloat(
        PARAM_VOLUME,
        "Volume",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.7f)); // Default: 70%

    // ADSR Envelope parameters
    addParameter(attackParam = new juce::AudioParameterFloat(
        PARAM_ATTACK,
        "Attack",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f), // Skew toward short attacks
        0.01f,  // Default: 10ms
        "s"));

    addParameter(decayParam = new juce::AudioParameterFloat(
        PARAM_DECAY,
        "Decay",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f),
        0.1f,   // Default: 100ms
        "s"));

    addParameter(sustainParam = new juce::AudioParameterFloat(
        PARAM_SUSTAIN,
        "Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.7f)); // Default: 70%

    addParameter(releaseParam = new juce::AudioParameterFloat(
        PARAM_RELEASE,
        "Release",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f),
        0.3f,   // Default: 300ms
        "s"));

    // Filter parameters (for future implementation)
    addParameter(filterCutoffParam = new juce::AudioParameterFloat(
        PARAM_FILTER_CUTOFF,
        "Filter Cutoff",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), // Skew toward low frequencies
        20000.0f, // Default: wide open
        "Hz"));

    addParameter(filterResonanceParam = new juce::AudioParameterFloat(
        PARAM_FILTER_RESONANCE,
        "Filter Resonance",
        juce::NormalisableRange<float>(0.5f, 10.0f, 0.1f),
        0.707f)); // Default: butterworth (no resonance)
}

BasicSynthProcessor::~BasicSynthProcessor()
{
}

void BasicSynthProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Inform base class of sample rate and buffer size
    setRateAndBufferSizeDetails(sampleRate, samplesPerBlock);

    // Initialize smoothed volume
    smoothedVolume.reset(sampleRate, 0.05); // 50ms smoothing
    smoothedVolume.setCurrentAndTargetValue(volumeParam->get());

    // Reset all voices
    for (auto& voice : voices)
        voice.reset();
}

void BasicSynthProcessor::releaseResources()
{
}

void BasicSynthProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Clear output buffer
    buffer.clear();

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Get parameters
    int waveform = waveformParam->getIndex();
    float volume = volumeParam->get();
    float attack = attackParam->get();
    float decay = decayParam->get();
    float sustain = sustainParam->get();
    float release = releaseParam->get();
    float filterCutoff = filterCutoffParam->get();
    float filterResonance = filterResonanceParam->get();

    smoothedVolume.setTargetValue(volume);

    // Process MIDI messages
    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();

        if (msg.isNoteOn())
        {
            handleNoteOn(msg.getNoteNumber(), msg.getVelocity() / 127.0f);
        }
        else if (msg.isNoteOff())
        {
            handleNoteOff(msg.getNoteNumber());
        }
    }

    // Process audio samples
    float peakLevel = 0.0f;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float mixedSample = 0.0f;

        // Sum all active voices
        for (auto& voice : voices)
        {
            if (voice.isActive())
            {
                mixedSample += voice.processSample(getSampleRate(), waveform,
                                                  attack, decay, sustain, release,
                                                  filterCutoff, filterResonance);
            }
        }

        // Apply master volume
        float masterVolume = smoothedVolume.getNextValue();
        mixedSample *= masterVolume;

        // Track peak for metering
        peakLevel = juce::jmax(peakLevel, std::abs(mixedSample));

        // Write to all output channels (mono to stereo)
        for (int channel = 0; channel < numChannels; ++channel)
        {
            buffer.setSample(channel, sample, mixedSample);
        }
    }

    // Update level meter
    levelMeter.updateLevel(peakLevel);
}

juce::AudioProcessorEditor* BasicSynthProcessor::createEditor()
{
    return new BasicSynthEditor(*this);
}

void BasicSynthProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save plugin state
    juce::MemoryOutputStream stream(destData, true);

    stream.writeInt(waveformParam->getIndex());
    stream.writeFloat(volumeParam->get());
    stream.writeFloat(attackParam->get());
    stream.writeFloat(decayParam->get());
    stream.writeFloat(sustainParam->get());
    stream.writeFloat(releaseParam->get());
    stream.writeFloat(filterCutoffParam->get());
    stream.writeFloat(filterResonanceParam->get());
}

void BasicSynthProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore plugin state
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);

    waveformParam->setValueNotifyingHost(static_cast<float>(stream.readInt()) /
                                        (waveformParam->choices.size() - 1));
    volumeParam->setValueNotifyingHost(volumeParam->convertTo0to1(stream.readFloat()));
    attackParam->setValueNotifyingHost(attackParam->convertTo0to1(stream.readFloat()));
    decayParam->setValueNotifyingHost(decayParam->convertTo0to1(stream.readFloat()));
    sustainParam->setValueNotifyingHost(sustainParam->convertTo0to1(stream.readFloat()));
    releaseParam->setValueNotifyingHost(releaseParam->convertTo0to1(stream.readFloat()));
    filterCutoffParam->setValueNotifyingHost(filterCutoffParam->convertTo0to1(stream.readFloat()));
    filterResonanceParam->setValueNotifyingHost(filterResonanceParam->convertTo0to1(stream.readFloat()));
}

//==============================================================================
// Voice management helpers

SynthVoice* BasicSynthProcessor::findFreeVoice()
{
    for (auto& voice : voices)
    {
        if (!voice.isActive())
            return &voice;
    }
    return nullptr;
}

SynthVoice* BasicSynthProcessor::findVoiceForNote(int midiNoteNumber)
{
    for (auto& voice : voices)
    {
        if (voice.isActive() && voice.getMidiNote() == midiNoteNumber)
            return &voice;
    }
    return nullptr;
}

SynthVoice* BasicSynthProcessor::stealVoice()
{
    // Simple round-robin voice stealing
    SynthVoice* voiceToSteal = &voices[nextVoiceIndex];
    nextVoiceIndex = (nextVoiceIndex + 1) % MAX_VOICES;
    return voiceToSteal;
}

void BasicSynthProcessor::handleNoteOn(int midiNoteNumber, float velocity)
{
    // Try to find a free voice
    SynthVoice* voice = findFreeVoice();

    // If no free voice, steal one
    if (voice == nullptr)
        voice = stealVoice();

    // Start the note
    voice->noteOn(midiNoteNumber, velocity, getSampleRate());
}

void BasicSynthProcessor::handleNoteOff(int midiNoteNumber)
{
    // Find the voice playing this note and release it
    SynthVoice* voice = findVoiceForNote(midiNoteNumber);
    if (voice != nullptr)
        voice->noteOff();
}

//==============================================================================
// Plugin factory
#ifndef AUDIOFORGE_TESTS
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BasicSynthProcessor();
}
#endif
