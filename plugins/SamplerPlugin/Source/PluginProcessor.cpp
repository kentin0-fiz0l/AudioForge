#include "PluginProcessor.h"
#include "PluginEditor.h"

SamplerPluginProcessor::SamplerPluginProcessor()
    : AudioProcessor(BusesProperties()
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

SamplerPluginProcessor::~SamplerPluginProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout SamplerPluginProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Amplitude Envelope (ADSR)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_AMP_ATTACK, "Amp Attack",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f),
        0.01f, "s"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_AMP_DECAY, "Amp Decay",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f),
        0.1f, "s"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_AMP_SUSTAIN, "Amp Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.7f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_AMP_RELEASE, "Amp Release",
        juce::NormalisableRange<float>(0.001f, 10.0f, 0.001f, 0.3f),
        0.3f, "s"));

    // Filter
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_FILTER_CUTOFF, "Filter Cutoff",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f),
        20000.0f, "Hz"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_FILTER_RESONANCE, "Filter Resonance",
        juce::NormalisableRange<float>(0.5f, 10.0f, 0.1f),
        0.707f));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        PARAM_FILTER_TYPE, "Filter Type",
        juce::StringArray{"Low-pass", "High-pass", "Band-pass", "Notch"},
        0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_FILTER_ENV_AMOUNT, "Filter Env Amount",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f),
        0.0f));

    // Filter Envelope
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_FILT_ATTACK, "Filt Attack",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f),
        0.01f, "s"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_FILT_DECAY, "Filt Decay",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f),
        0.1f, "s"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_FILT_SUSTAIN, "Filt Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.7f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_FILT_RELEASE, "Filt Release",
        juce::NormalisableRange<float>(0.001f, 10.0f, 0.001f, 0.3f),
        0.3f, "s"));

    // Sample Parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_SAMPLE_START, "Sample Start",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
        0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_SAMPLE_END, "Sample End",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
        1.0f));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        PARAM_LOOP_MODE, "Loop Mode",
        juce::StringArray{"None", "Forward", "Ping-Pong"},
        0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_LOOP_START, "Loop Start",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
        0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_LOOP_END, "Loop End",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
        1.0f));

    // Global Parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_MASTER_VOLUME, "Master Volume",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.7f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PARAM_MASTER_TUNE, "Master Tune",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f),
        0.0f, "semi"));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        PARAM_VOICE_COUNT, "Voice Count",
        1, 16, 16));

    return { params.begin(), params.end() };
}

void SamplerPluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    setRateAndBufferSizeDetails(sampleRate, samplesPerBlock);
}

void SamplerPluginProcessor::releaseResources()
{
}

void SamplerPluginProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                         juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear unused output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Get current parameter values
    float ampAttack = apvts.getRawParameterValue(PARAM_AMP_ATTACK)->load();
    float ampDecay = apvts.getRawParameterValue(PARAM_AMP_DECAY)->load();
    float ampSustain = apvts.getRawParameterValue(PARAM_AMP_SUSTAIN)->load();
    float ampRelease = apvts.getRawParameterValue(PARAM_AMP_RELEASE)->load();

    float filterCutoff = apvts.getRawParameterValue(PARAM_FILTER_CUTOFF)->load();
    float filterResonance = apvts.getRawParameterValue(PARAM_FILTER_RESONANCE)->load();
    int filterType = static_cast<int>(apvts.getRawParameterValue(PARAM_FILTER_TYPE)->load());
    float filterEnvAmount = apvts.getRawParameterValue(PARAM_FILTER_ENV_AMOUNT)->load();

    float filtAttack = apvts.getRawParameterValue(PARAM_FILT_ATTACK)->load();
    float filtDecay = apvts.getRawParameterValue(PARAM_FILT_DECAY)->load();
    float filtSustain = apvts.getRawParameterValue(PARAM_FILT_SUSTAIN)->load();
    float filtRelease = apvts.getRawParameterValue(PARAM_FILT_RELEASE)->load();

    float masterVolume = apvts.getRawParameterValue(PARAM_MASTER_VOLUME)->load();

    // Process MIDI events
    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();

        if (msg.isNoteOn())
        {
            int midiNote = msg.getNoteNumber();
            float velocity = msg.getVelocity() / 127.0f;

            // Find zone for this note
            const KeyZone* zone = keyZoneMap.getZoneForNote(midiNote);
            if (zone)
            {
                // Get sample index for this velocity
                int sampleIndex = zone->getSampleForVelocity(velocity);
                auto sample = sampleLibrary.getSample(sampleIndex);

                if (sample)
                {
                    // Trigger voice
                    voiceManager.noteOn(midiNote, velocity, currentSampleRate);

                    // Set sample for the most recently triggered voice
                    // (This is a simplified approach - in production you'd track which voice was allocated)
                    auto& voices = voiceManager.getVoices();
                    for (auto& voice : voices)
                    {
                        if (voice.isActive() && voice.getMidiNote() == midiNote)
                        {
                            voice.setSample(sample, zone);
                            break;
                        }
                    }
                }
            }
        }
        else if (msg.isNoteOff())
        {
            voiceManager.noteOff(msg.getNoteNumber());
        }
        else if (msg.isAllNotesOff() || msg.isAllSoundOff())
        {
            voiceManager.allNotesOff();
        }
    }

    // Render audio
    buffer.clear();

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float mixedSample = 0.0f;

        // Process all voices
        for (auto& voice : voiceManager.getVoices())
        {
            if (voice.isActive())
            {
                mixedSample += voice.processSample(
                    currentSampleRate,
                    ampAttack, ampDecay, ampSustain, ampRelease,
                    filterCutoff, filterResonance, filterType,
                    filterEnvAmount,
                    filtAttack, filtDecay, filtSustain, filtRelease
                );
            }
        }

        // Apply master volume
        mixedSample *= masterVolume;

        // Write to output channels
        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
            buffer.setSample(channel, sample, mixedSample);
        }
    }
}

bool SamplerPluginProcessor::loadSample(const juce::File& file)
{
    auto sample = sampleLibrary.loadSample(file);
    return sample != nullptr;
}

void SamplerPluginProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save parameters
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());

    // Add custom sampler state
    auto samplerState = xml->createNewChildElement("SamplerState");

    // Save sample paths (relative to project)
    auto samplesElement = samplerState->createNewChildElement("Samples");
    for (int i = 0; i < sampleLibrary.getNumSamples(); ++i)
    {
        auto sample = sampleLibrary.getSample(i);
        if (sample)
        {
            auto sampleElement = samplesElement->createNewChildElement("Sample");
            sampleElement->setAttribute("index", i);
            // TODO: Store sample file path (need to add to Sample class)
        }
    }

    // Save key zones
    auto zonesElement = samplerState->createNewChildElement("KeyZones");
    for (int i = 0; i < keyZoneMap.getNumZones(); ++i)
    {
        const auto& zone = keyZoneMap.getZones()[i];
        auto zoneElement = zonesElement->createNewChildElement("Zone");
        zoneElement->setAttribute("noteMin", zone.noteMin);
        zoneElement->setAttribute("noteMax", zone.noteMax);
        zoneElement->setAttribute("rootNote", zone.rootNote);

        for (const auto& layer : zone.velocityLayers)
        {
            auto layerElement = zoneElement->createNewChildElement("VelocityLayer");
            layerElement->setAttribute("sampleIndex", layer.sampleIndex);
            layerElement->setAttribute("velocityMin", static_cast<double>(layer.velocityMin));
            layerElement->setAttribute("velocityMax", static_cast<double>(layer.velocityMax));
        }
    }

    copyXmlToBinary(*xml, destData);
}

void SamplerPluginProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr)
    {
        if (xmlState->hasTagName(apvts.state.getType()))
        {
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));

            // Load custom sampler state
            if (auto samplerState = xmlState->getChildByName("SamplerState"))
            {
                // Load samples
                if (auto samplesElement = samplerState->getChildByName("Samples"))
                {
                    // TODO: Implement sample loading from saved paths
                }

                // Load key zones
                if (auto zonesElement = samplerState->getChildByName("KeyZones"))
                {
                    keyZoneMap.clearAllZones();

                    for (auto* zoneElement : zonesElement->getChildIterator())
                    {
                        KeyZone zone;
                        zone.noteMin = zoneElement->getIntAttribute("noteMin", 0);
                        zone.noteMax = zoneElement->getIntAttribute("noteMax", 127);
                        zone.rootNote = zoneElement->getIntAttribute("rootNote", 60);

                        for (auto* layerElement : zoneElement->getChildIterator())
                        {
                            VelocityLayer layer;
                            layer.sampleIndex = layerElement->getIntAttribute("sampleIndex", -1);
                            layer.velocityMin = static_cast<float>(
                                layerElement->getDoubleAttribute("velocityMin", 0.0));
                            layer.velocityMax = static_cast<float>(
                                layerElement->getDoubleAttribute("velocityMax", 1.0));

                            zone.velocityLayers.push_back(layer);
                        }

                        keyZoneMap.addZone(zone);
                    }
                }
            }
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SamplerPluginProcessor();
}

//==============================================================================
// Editor creation
juce::AudioProcessorEditor* SamplerPluginProcessor::createEditor()
{
    return new SamplerPluginEditor(*this);
}
