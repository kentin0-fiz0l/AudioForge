#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../../../shared/utils/AudioFileSafety.h"
#include <map>
#include <utility>

SamplerPluginProcessor::SamplerPluginProcessor()
    : AudioProcessor(BusesProperties()
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts(*this, nullptr, "Parameters", createParameterLayout())
    , midiLearnManager_(apvts)
    , presetManager_(apvts, "SamplerPlugin")
{
    presetManager_.scanPresets();
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

    controllerGain.reset(sampleRate, 0.02);
    controllerGain.setCurrentAndTargetValue(channelVolume * expression);
}

void SamplerPluginProcessor::releaseResources()
{
}

void SamplerPluginProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                         juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    const juce::ScopedTryLock lock(instrumentLock);
    if (!lock.isLocked())
        return;  // The instrument is being changed

    // Notes start where they fall in the block
    int position = 0;
    for (const auto metadata : midiMessages)
    {
        const int eventPosition = juce::jlimit(position, buffer.getNumSamples(), metadata.samplePosition);
        renderVoices(buffer, position, eventPosition);
        position = eventPosition;
        handleMidi(metadata.getMessage());
    }

    renderVoices(buffer, position, buffer.getNumSamples());
}

void SamplerPluginProcessor::handleMidi(const juce::MidiMessage& msg)
{
    if (msg.isNoteOn())
    {
        startNote(msg.getNoteNumber(), msg.getVelocity());
    }
    else if (msg.isNoteOff())
    {
        if (sustainPedal)
            heldByPedal[(size_t) msg.getNoteNumber()] = true;
        else
            releaseNote(msg.getNoteNumber());
    }
    else if (msg.isAllNotesOff() || msg.isAllSoundOff())
    {
        voiceManager.allNotesOff();
        sustainPedal = false;
        heldByPedal.fill(false);
    }
    else if (msg.isSustainPedalOn())
    {
        sustainPedal = true;
    }
    else if (msg.isSustainPedalOff())
    {
        sustainPedal = false;
        for (int note = 0; note < 128; ++note)
            if (std::exchange(heldByPedal[(size_t) note], false))
                releaseNote(note);
    }
    else if (msg.isControllerOfType(7) || msg.isControllerOfType(11))
    {
        // Volume and expression, on the usual square-law curve
        const float value = msg.getControllerValue() / 127.0f;
        (msg.getControllerNumber() == 7 ? channelVolume : expression) = value * value;
        controllerGain.setTargetValue(channelVolume * expression);
    }
}

void SamplerPluginProcessor::startNote(int midiNote, int velocity)
{
    // The same key struck again lets its last note ring out
    heldByPedal[(size_t) midiNote] = false;
    releaseNote(midiNote);

    const float masterTune = apvts.getRawParameterValue(PARAM_MASTER_TUNE)->load();
    std::vector<SamplerVoice::Playback> playbacks;

    if (sfzMap != nullptr && !sfzMap->isEmpty())
    {
        playbacks = sfzMap->noteOn(midiNote, velocity, random.nextFloat(), masterTune);
    }
    else if (const KeyZone* zone = keyZoneMap.getZoneForNote(midiNote))
    {
        // Get sample index for this velocity
        const float level = velocity / 127.0f;
        if (auto sample = sampleLibrary.getSample(zone->getSampleForVelocity(level)))
        {
            SamplerVoice::Playback playback;
            playback.sample = sample;
            playback.pitchRatio = std::pow(2.0, (midiNote - zone->rootNote + masterTune) / 12.0);
            playback.gainLeft = playback.gainRight = level;
            playback.loopMode = sample->getLoopMode();
            playback.loopStart = sample->getLoopStartSamples();
            playback.loopEnd = sample->getLoopEndSamples();
            playbacks.push_back(playback);
        }
    }

    for (const auto& playback : playbacks)
        findVoiceToStart().start(midiNote, playback, currentSampleRate);
}

void SamplerPluginProcessor::releaseNote(int midiNote)
{
    voiceManager.noteOff(midiNote);
}

SamplerVoice& SamplerPluginProcessor::findVoiceToStart()
{
    auto& voices = voiceManager.getVoices();

    for (auto& voice : voices)
        if (!voice.isActive())
            return voice;

    // All in use: take the quietest, a released one if there is one
    SamplerVoice* quietest = nullptr;
    for (auto& voice : voices)
    {
        if (quietest == nullptr
            || voice.isReleased() > quietest->isReleased()
            || (voice.isReleased() == quietest->isReleased() && voice.getLevel() < quietest->getLevel()))
            quietest = &voice;
    }
    return *quietest;
}

void SamplerPluginProcessor::renderVoices(juce::AudioBuffer<float>& buffer, int startSample, int endSample)
{
    if (endSample <= startSample)
        return;

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

    const int numChannels = buffer.getNumChannels();

    for (int sample = startSample; sample < endSample; ++sample)
    {
        float left = 0.0f, right = 0.0f;

        // Process all voices
        for (auto& voice : voiceManager.getVoices())
        {
            if (voice.isActive())
            {
                voice.processSample(
                    currentSampleRate,
                    ampAttack, ampDecay, ampSustain, ampRelease,
                    filterCutoff, filterResonance, filterType,
                    filterEnvAmount,
                    filtAttack, filtDecay, filtSustain, filtRelease,
                    left, right
                );
            }
        }

        // Apply master volume and the MIDI volume and expression
        const float gain = masterVolume * controllerGain.getNextValue();

        // Write to output channels
        if (numChannels == 1)
        {
            buffer.setSample(0, sample, 0.5f * (left + right) * gain);
        }
        else if (numChannels > 1)
        {
            buffer.setSample(0, sample, left * gain);
            buffer.setSample(1, sample, right * gain);
        }
    }
}

bool SamplerPluginProcessor::loadSample(const juce::File& file)
{
    auto sample = sampleLibrary.loadSample(file);
    return sample != nullptr;
}

bool SamplerPluginProcessor::loadSfz(const juce::File& file, juce::String& error)
{
    // Loaded off the lock, so the audio thread waits only for the swap
    auto map = std::make_unique<SfzSampleMap>();
    const bool loaded = map->load(file, error);

    for (const auto& warning : map->getWarnings())
        juce::Logger::writeToLog("Sampler: " + file.getFileName() + ": " + warning);

    if (!loaded)
    {
        juce::Logger::writeToLog("Sampler: " + error);
        return false;
    }

    const juce::ScopedLock lock(instrumentLock);
    voiceManager.allNotesOff();
    sustainPedal = false;
    heldByPedal.fill(false);
    sfzMap = std::move(map);
    return true;
}

void SamplerPluginProcessor::clearSfz()
{
    const juce::ScopedLock lock(instrumentLock);
    voiceManager.allNotesOff();
    sustainPedal = false;
    heldByPedal.fill(false);
    sfzMap.reset();
}

void SamplerPluginProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save parameters
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());

    // The SFZ instrument, if there is one
    xml->removeAttribute("sfz");
    if (sfzMap != nullptr)
        xml->setAttribute("sfz", sfzMap->getFile().getFullPathName());

    // Add custom sampler state
    auto samplerState = xml->createNewChildElement("SamplerState");

    // Save sample paths
    auto samplesElement = samplerState->createNewChildElement("Samples");
    for (int i = 0; i < sampleLibrary.getNumSamples(); ++i)
    {
        auto sample = sampleLibrary.getSample(i);
        if (sample)
        {
            auto sampleElement = samplesElement->createNewChildElement("Sample");
            sampleElement->setAttribute("index", i);
            sampleElement->setAttribute("path", sample->getSourceFile().getFullPathName());
            sampleElement->setAttribute("loopMode", static_cast<int>(sample->getLoopMode()));
            sampleElement->setAttribute("loopStart", sample->getLoopStartSamples());
            sampleElement->setAttribute("loopEnd", sample->getLoopEndSamples());
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

    xml->addChildElement(presetManager_.saveToXml().release());
    copyXmlToBinary(*xml, destData);
}

void SamplerPluginProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    // A state that did not parse, or that belongs to something else, is ignored
    if (xmlState == nullptr || !xmlState->hasTagName(apvts.state.getType()))
        return;

    apvts.replaceState(juce::ValueTree::fromXml(*xmlState));

    // The SFZ instrument: a relative path is taken from the working directory
    const auto sfzPath = xmlState->getStringAttribute("sfz");
    if (AudioForge::FileSafety::isNetworkPath(sfzPath))
    {
        juce::Logger::writeToLog("Sampler: network paths aren't loaded: " + sfzPath);
        clearSfz();
    }
    else if (sfzPath.isNotEmpty())
    {
        juce::String error;
        loadSfz(juce::File::getCurrentWorkingDirectory().getChildFile(sfzPath), error);
    }
    else
    {
        clearSfz();
    }

    // Load custom sampler state
    auto samplerState = xmlState->getChildByName("SamplerState");
    if (samplerState == nullptr)
        return;

    const juce::ScopedLock lock(instrumentLock);

    // Load samples. One that's gone is left out, and the zones that used
    // it lose it rather than play another
    std::map<int, int> newIndex;
    if (auto samplesElement = samplerState->getChildByName("Samples"))
    {
        voiceManager.allNotesOff();
        sampleLibrary.clearAllSamples();

        for (auto* sampleElement : samplesElement->getChildWithTagNameIterator("Sample"))
        {
            const auto path = sampleElement->getStringAttribute("path");
            if (AudioForge::FileSafety::isNetworkPath(path) || !juce::File::isAbsolutePath(path))
            {
                juce::Logger::writeToLog("Sampler: can't restore sample " + path + " (network or not absolute)");
                continue;
            }

            auto sample = sampleLibrary.loadSample(juce::File(path));
            if (sample == nullptr)
            {
                juce::Logger::writeToLog("Sampler: can't restore sample " + path + ": " + sampleLibrary.getLastError());
                continue;
            }

            sample->setLoopMode(static_cast<Sample::LoopMode>(
                juce::jlimit(0, 2, sampleElement->getIntAttribute("loopMode", 0))));
            sample->setLoopPointsInSamples(sampleElement->getIntAttribute("loopStart", 0),
                                           sampleElement->getIntAttribute("loopEnd", sample->getLengthInSamples() - 1));
            newIndex[sampleElement->getIntAttribute("index", -1)] = sampleLibrary.getNumSamples() - 1;
        }
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
                const int savedIndex = layerElement->getIntAttribute("sampleIndex", -1);
                const auto found = newIndex.find(savedIndex);
                layer.sampleIndex = found != newIndex.end() ? found->second : -1;
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

#ifndef AUDIOFORGE_TESTS
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SamplerPluginProcessor();
}
#endif

//==============================================================================
// Editor creation
juce::AudioProcessorEditor* SamplerPluginProcessor::createEditor()
{
    return new SamplerPluginEditor(*this);
}
