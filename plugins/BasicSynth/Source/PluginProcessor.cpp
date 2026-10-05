#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "FactoryPatches.h"

namespace
{
    // What the state was before it held every parameter: the waveform index
    // and seven values, with nothing to mark it
    constexpr int legacyStateSize = 32;

    const char* const stateTag = "BasicSynthState";
    const char* const programAttribute = "program";

    void setShownValue(juce::RangedAudioParameter& param, float value)
    {
        param.setValueNotifyingHost(param.convertTo0to1(value));
    }
}

BasicSynthProcessor::BasicSynthProcessor()
    : AudioProcessor(BusesProperties()
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // A new instance starts as the first factory patch
    const auto& init = BasicSynthPatches::all[0];

    // Waveform parameter: Sine, Saw, Square
    addParameter(waveformParam = new juce::AudioParameterChoice(
        PARAM_WAVEFORM,
        "Waveform",
        juce::StringArray{"Sine", "Sawtooth", "Square"},
        init.waveform));

    // Master volume parameter (0.0 to 1.0)
    addParameter(volumeParam = new juce::AudioParameterFloat(
        PARAM_VOLUME,
        "Volume",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        init.volume));

    // ADSR Envelope parameters
    addParameter(attackParam = new juce::AudioParameterFloat(
        PARAM_ATTACK,
        "Attack",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f), // Skew toward short attacks
        init.attack,
        "s"));

    addParameter(decayParam = new juce::AudioParameterFloat(
        PARAM_DECAY,
        "Decay",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f),
        init.decay,
        "s"));

    addParameter(sustainParam = new juce::AudioParameterFloat(
        PARAM_SUSTAIN,
        "Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        init.sustain));

    addParameter(releaseParam = new juce::AudioParameterFloat(
        PARAM_RELEASE,
        "Release",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f),
        init.release,
        "s"));

    // Filter parameters
    addParameter(filterCutoffParam = new juce::AudioParameterFloat(
        PARAM_FILTER_CUTOFF,
        "Filter Cutoff",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), // Skew toward low frequencies
        init.filterCutoff,
        "Hz"));

    addParameter(filterResonanceParam = new juce::AudioParameterFloat(
        PARAM_FILTER_RESONANCE,
        "Filter Resonance",
        juce::NormalisableRange<float>(0.5f, 10.0f, 0.1f),
        init.filterResonance));

    addParameter(filterTypeParam = new juce::AudioParameterChoice(
        PARAM_FILTER_TYPE,
        "Filter Type",
        juce::StringArray("Low-pass", "High-pass", "Band-pass", "Notch"),
        init.filterType));

    // Chorus parameters
    addParameter(chorusRateParam = new juce::AudioParameterFloat(
        PARAM_CHORUS_RATE,
        "Chorus Rate",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.1f),
        init.chorusRate));

    addParameter(chorusDepthParam = new juce::AudioParameterFloat(
        PARAM_CHORUS_DEPTH,
        "Chorus Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        init.chorusDepth));

    addParameter(chorusMixParam = new juce::AudioParameterFloat(
        PARAM_CHORUS_MIX,
        "Chorus Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        init.chorusMix));

    // Reverb parameters
    addParameter(reverbSizeParam = new juce::AudioParameterFloat(
        PARAM_REVERB_SIZE,
        "Reverb Size",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        init.reverbSize));

    addParameter(reverbDampingParam = new juce::AudioParameterFloat(
        PARAM_REVERB_DAMPING,
        "Reverb Damping",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        init.reverbDamping));

    addParameter(reverbMixParam = new juce::AudioParameterFloat(
        PARAM_REVERB_MIX,
        "Reverb Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        init.reverbMix));

    // Saturation parameters
    addParameter(saturationDriveParam = new juce::AudioParameterFloat(
        PARAM_SATURATION_DRIVE,
        "Saturation Drive",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        init.saturationDrive));

    addParameter(saturationMixParam = new juce::AudioParameterFloat(
        PARAM_SATURATION_MIX,
        "Saturation Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        init.saturationMix));

    addParameter(saturationTypeParam = new juce::AudioParameterChoice(
        PARAM_SATURATION_TYPE,
        "Saturation Type",
        juce::StringArray("Soft Clip", "Hard Clip", "Tube"),
        init.saturationType));
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

    // Prepare effects chain
    effectsChain.prepare(sampleRate, samplesPerBlock);

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
                int filterType = filterTypeParam->getIndex();
                mixedSample += voice.processSample(getSampleRate(), waveform,
                                                  attack, decay, sustain, release,
                                                  filterCutoff, filterResonance,
                                                  filterType);
            }
        }

        // Apply master volume
        float masterVolume = smoothedVolume.getNextValue();
        mixedSample *= masterVolume;

        // Update effects parameters
        effectsChain.setSaturation(
            saturationDriveParam->get(),
            saturationMixParam->get(),
            saturationTypeParam->getIndex());

        effectsChain.setChorus(
            chorusRateParam->get(),
            chorusDepthParam->get(),
            chorusMixParam->get());

        effectsChain.setReverb(
            reverbSizeParam->get(),
            reverbDampingParam->get(),
            reverbMixParam->get());

        // Process through effects chain (mono in, stereo out)
        float leftSample, rightSample;
        effectsChain.processSample(mixedSample, leftSample, rightSample);

        // Track peak for metering (use max of left/right)
        float peakSample = juce::jmax(std::abs(leftSample), std::abs(rightSample));
        peakLevel = juce::jmax(peakLevel, peakSample);

        // Write stereo output
        if (numChannels >= 2)
        {
            buffer.setSample(0, sample, leftSample);   // Left
            buffer.setSample(1, sample, rightSample);  // Right
        }
        else if (numChannels == 1)
        {
            // Mono output: average left and right
            buffer.setSample(0, sample, (leftSample + rightSample) * 0.5f);
        }
    }

    // Update level meter
    levelMeter.updateLevel(peakLevel);
}

double BasicSynthProcessor::getTailLengthSeconds() const
{
    // The reverb keeps ringing after the last voice has stopped. Hosts use
    // this to decide how long to keep processing once the input goes quiet.
    if (reverbMixParam->get() <= 0.0f)
        return 0.0;

    return Reverb::decayTimeSeconds(reverbSizeParam->get());
}

juce::AudioProcessorEditor* BasicSynthProcessor::createEditor()
{
    return new BasicSynthEditor(*this);
}

//==============================================================================
// Factory patches, offered to the host as programs

int BasicSynthProcessor::getNumPrograms()
{
    return static_cast<int>(BasicSynthPatches::all.size());
}

const juce::String BasicSynthProcessor::getProgramName(int index)
{
    if (! juce::isPositiveAndBelow(index, getNumPrograms()))
        return {};

    return BasicSynthPatches::all[static_cast<size_t>(index)].name;
}

void BasicSynthProcessor::setCurrentProgram(int index)
{
    if (! juce::isPositiveAndBelow(index, getNumPrograms()))
        return;

    const auto& patch = BasicSynthPatches::all[static_cast<size_t>(index)];
    currentProgram = index;

    setShownValue(*waveformParam, static_cast<float>(patch.waveform));
    setShownValue(*volumeParam, patch.volume);
    setShownValue(*attackParam, patch.attack);
    setShownValue(*decayParam, patch.decay);
    setShownValue(*sustainParam, patch.sustain);
    setShownValue(*releaseParam, patch.release);
    setShownValue(*filterCutoffParam, patch.filterCutoff);
    setShownValue(*filterResonanceParam, patch.filterResonance);
    setShownValue(*filterTypeParam, static_cast<float>(patch.filterType));
    setShownValue(*chorusRateParam, patch.chorusRate);
    setShownValue(*chorusDepthParam, patch.chorusDepth);
    setShownValue(*chorusMixParam, patch.chorusMix);
    setShownValue(*reverbSizeParam, patch.reverbSize);
    setShownValue(*reverbDampingParam, patch.reverbDamping);
    setShownValue(*reverbMixParam, patch.reverbMix);
    setShownValue(*saturationDriveParam, patch.saturationDrive);
    setShownValue(*saturationMixParam, patch.saturationMix);
    setShownValue(*saturationTypeParam, static_cast<float>(patch.saturationType));
}

//==============================================================================
// State

void BasicSynthProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Every parameter under its ID, as the value its control shows, so a
    // later change to a range or to the order of parameters does not
    // disturb a saved set
    juce::XmlElement state(stateTag);
    state.setAttribute(programAttribute, currentProgram.load());

    for (auto* param : getParameters())
        if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*>(param))
            state.setAttribute(ranged->getParameterID(),
                               static_cast<double>(ranged->convertFrom0to1(ranged->getValue())));

    copyXmlToBinary(state, destData);
}

void BasicSynthProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (data == nullptr || sizeInBytes <= 0)
        return;

    const auto state = getXmlFromBinary(data, sizeInBytes);

    if (state == nullptr)
    {
        if (sizeInBytes == legacyStateSize)
            setLegacyState(data);
        return;
    }

    if (! state->hasTagName(stateTag))
        return;

    // The patch is only remembered here, not loaded: the values saved with
    // it include whatever was changed after it was chosen
    const int program = state->getIntAttribute(programAttribute, currentProgram.load());
    if (juce::isPositiveAndBelow(program, getNumPrograms()))
        currentProgram = program;

    for (auto* param : getParameters())
        if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*>(param))
            if (state->hasAttribute(ranged->getParameterID()))
                setShownValue(*ranged, static_cast<float>(state->getDoubleAttribute(ranged->getParameterID())));
}

void BasicSynthProcessor::setLegacyState(const void* data)
{
    juce::MemoryInputStream stream(data, static_cast<size_t>(legacyStateSize), false);

    // The old state held a waveform, but the waveform was never used: every
    // note was a sawtooth. Keep an old set sounding the way it did.
    stream.readInt();
    setShownValue(*waveformParam, static_cast<float>(BasicSynthPatches::all[0].waveform));

    for (auto* param : { volumeParam, attackParam, decayParam, sustainParam,
                         releaseParam, filterCutoffParam, filterResonanceParam })
        setShownValue(*param, stream.readFloat());
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
