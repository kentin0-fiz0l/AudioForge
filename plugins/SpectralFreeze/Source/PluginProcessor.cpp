#include "PluginProcessor.h"
#include "PluginEditor.h"

SpectralFreezeProcessor::SpectralFreezeProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Freeze toggle
    addParameter(freezeParam = new juce::AudioParameterBool(
        PARAM_FREEZE,
        "Freeze",
        false));  // Default: not frozen

    // Spectral Blur (0-100%)
    addParameter(blurParam = new juce::AudioParameterFloat(
        PARAM_BLUR,
        "Blur",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        0.0f));  // Default: no blur

    // Low-Cut Frequency (20-20000 Hz)
    addParameter(lowCutParam = new juce::AudioParameterFloat(
        PARAM_LOW_CUT,
        "Low Cut",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f),  // Log scale
        20.0f));  // Default: 20 Hz

    // High-Cut Frequency (20-20000 Hz)
    addParameter(highCutParam = new juce::AudioParameterFloat(
        PARAM_HIGH_CUT,
        "High Cut",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f),  // Log scale
        20000.0f));  // Default: 20 kHz

    // Dry/Wet Mix (0-100%)
    addParameter(dryWetParam = new juce::AudioParameterFloat(
        PARAM_DRY_WET,
        "Dry/Wet",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        100.0f));  // Default: 100% wet

    // Phase 2: Advanced Effects

    // Spectral Stretch (0-100%)
    addParameter(stretchParam = new juce::AudioParameterFloat(
        PARAM_STRETCH,
        "Stretch",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        0.0f));  // Default: no stretch

    // Spectral Shift (-100 to +100, semitones)
    addParameter(shiftParam = new juce::AudioParameterFloat(
        PARAM_SHIFT,
        "Shift",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 1.0f),
        0.0f));  // Default: no shift

    // Spectral Morph (0-100%)
    addParameter(morphParam = new juce::AudioParameterFloat(
        PARAM_MORPH,
        "Morph",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        0.0f));  // Default: no morph

    // Spectral Gate (0-100%)
    addParameter(gateParam = new juce::AudioParameterFloat(
        PARAM_GATE,
        "Gate",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        0.0f));  // Default: no gate

    // Spectral Randomize (0-100%)
    addParameter(randomizeParam = new juce::AudioParameterFloat(
        PARAM_RANDOMIZE,
        "Randomize",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        0.0f));  // Default: no randomize
}

SpectralFreezeProcessor::~SpectralFreezeProcessor()
{
}

void SpectralFreezeProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    spectralProcessor.prepare(sampleRate, samplesPerBlock, getTotalNumInputChannels());
}

void SpectralFreezeProcessor::releaseResources()
{
    spectralProcessor.reset();
}

bool SpectralFreezeProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // Supports mono and stereo
    return layouts.getMainOutputChannelSet() == layouts.getMainInputChannelSet()
        && !layouts.getMainInputChannelSet().isDisabled();
}

void SpectralFreezeProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    // Update parameters
    spectralProcessor.setFreeze(freezeParam->get());
    spectralProcessor.setBlurAmount(blurParam->get() / 100.0f);
    spectralProcessor.setLowCutFreq(lowCutParam->get());
    spectralProcessor.setHighCutFreq(highCutParam->get());
    spectralProcessor.setDryWet(dryWetParam->get() / 100.0f);

    // Phase 2: Advanced Effect Parameters
    spectralProcessor.setStretchAmount(stretchParam->get() / 100.0f);
    spectralProcessor.setShiftAmount(shiftParam->get() / 100.0f);
    spectralProcessor.setMorphAmount(morphParam->get() / 100.0f);
    spectralProcessor.setGateThreshold(gateParam->get() / 100.0f);
    spectralProcessor.setRandomizeAmount(randomizeParam->get() / 100.0f);

    // Process spectral effects
    spectralProcessor.processBlock(buffer);
}

juce::AudioProcessorEditor* SpectralFreezeProcessor::createEditor()
{
    return new SpectralFreezeEditor(*this);
}

void SpectralFreezeProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save plugin state
    juce::MemoryOutputStream stream(destData, true);
    stream.writeBool(freezeParam->get());
    stream.writeFloat(blurParam->get());
    stream.writeFloat(lowCutParam->get());
    stream.writeFloat(highCutParam->get());
    stream.writeFloat(dryWetParam->get());

    // Phase 2: Advanced Effect Parameters
    stream.writeFloat(stretchParam->get());
    stream.writeFloat(shiftParam->get());
    stream.writeFloat(morphParam->get());
    stream.writeFloat(gateParam->get());
    stream.writeFloat(randomizeParam->get());
}

void SpectralFreezeProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore plugin state
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    freezeParam->setValueNotifyingHost(stream.readBool() ? 1.0f : 0.0f);
    blurParam->setValueNotifyingHost(blurParam->convertTo0to1(stream.readFloat()));
    lowCutParam->setValueNotifyingHost(lowCutParam->convertTo0to1(stream.readFloat()));
    highCutParam->setValueNotifyingHost(highCutParam->convertTo0to1(stream.readFloat()));
    dryWetParam->setValueNotifyingHost(dryWetParam->convertTo0to1(stream.readFloat()));

    // Phase 2: Advanced Effect Parameters
    stretchParam->setValueNotifyingHost(stretchParam->convertTo0to1(stream.readFloat()));
    shiftParam->setValueNotifyingHost(shiftParam->convertTo0to1(stream.readFloat()));
    morphParam->setValueNotifyingHost(morphParam->convertTo0to1(stream.readFloat()));
    gateParam->setValueNotifyingHost(gateParam->convertTo0to1(stream.readFloat()));
    randomizeParam->setValueNotifyingHost(randomizeParam->convertTo0to1(stream.readFloat()));
}

//==============================================================================
// Phase 4: Preset Management

bool SpectralFreezeProcessor::savePreset(const juce::String& presetName)
{
    // Get presets directory (create if doesn't exist)
    juce::File presetsDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                                .getChildFile("AudioForge")
                                .getChildFile("SpectralFreeze")
                                .getChildFile("Presets");

    if (!presetsDir.exists())
        presetsDir.createDirectory();

    // Create preset file
    juce::File presetFile = presetsDir.getChildFile(presetName + ".xml");

    // Create XML structure
    juce::XmlElement root("SpectralFreezePreset");
    root.setAttribute("version", "1.0");
    root.setAttribute("name", presetName);

    // Save all parameters
    root.setAttribute("freeze", freezeParam->get());
    root.setAttribute("blur", blurParam->get());
    root.setAttribute("lowCut", lowCutParam->get());
    root.setAttribute("highCut", highCutParam->get());
    root.setAttribute("dryWet", dryWetParam->get());
    root.setAttribute("stretch", stretchParam->get());
    root.setAttribute("shift", shiftParam->get());
    root.setAttribute("morph", morphParam->get());
    root.setAttribute("gate", gateParam->get());
    root.setAttribute("randomize", randomizeParam->get());

    // Write to file
    return root.writeTo(presetFile);
}

bool SpectralFreezeProcessor::loadPreset(const juce::String& presetName)
{
    // Get preset file
    juce::File presetsDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                                .getChildFile("AudioForge")
                                .getChildFile("SpectralFreeze")
                                .getChildFile("Presets");

    juce::File presetFile = presetsDir.getChildFile(presetName + ".xml");

    if (!presetFile.exists())
        return false;

    // Parse XML
    std::unique_ptr<juce::XmlElement> root = juce::parseXML(presetFile);
    if (root == nullptr || root->getTagName() != "SpectralFreezePreset")
        return false;

    // Load all parameters
    freezeParam->setValueNotifyingHost(root->getBoolAttribute("freeze") ? 1.0f : 0.0f);
    blurParam->setValueNotifyingHost(blurParam->convertTo0to1(root->getDoubleAttribute("blur")));
    lowCutParam->setValueNotifyingHost(lowCutParam->convertTo0to1(root->getDoubleAttribute("lowCut")));
    highCutParam->setValueNotifyingHost(highCutParam->convertTo0to1(root->getDoubleAttribute("highCut")));
    dryWetParam->setValueNotifyingHost(dryWetParam->convertTo0to1(root->getDoubleAttribute("dryWet")));
    stretchParam->setValueNotifyingHost(stretchParam->convertTo0to1(root->getDoubleAttribute("stretch")));
    shiftParam->setValueNotifyingHost(shiftParam->convertTo0to1(root->getDoubleAttribute("shift")));
    morphParam->setValueNotifyingHost(morphParam->convertTo0to1(root->getDoubleAttribute("morph")));
    gateParam->setValueNotifyingHost(gateParam->convertTo0to1(root->getDoubleAttribute("gate")));
    randomizeParam->setValueNotifyingHost(randomizeParam->convertTo0to1(root->getDoubleAttribute("randomize")));

    return true;
}

juce::StringArray SpectralFreezeProcessor::getAvailablePresets() const
{
    juce::StringArray presets;

    // Get presets directory
    juce::File presetsDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                                .getChildFile("AudioForge")
                                .getChildFile("SpectralFreeze")
                                .getChildFile("Presets");

    if (!presetsDir.exists())
        return presets;

    // Find all .xml files
    juce::Array<juce::File> presetFiles = presetsDir.findChildFiles(juce::File::findFiles, false, "*.xml");

    for (const auto& file : presetFiles)
        presets.add(file.getFileNameWithoutExtension());

    return presets;
}

void SpectralFreezeProcessor::loadFactoryPreset(int index)
{
    // Factory presets showcasing different spectral effects
    switch (index)
    {
        case 0: // "Init" - Clean slate
            freezeParam->setValueNotifyingHost(0.0f);
            blurParam->setValueNotifyingHost(0.0f);
            lowCutParam->setValueNotifyingHost(lowCutParam->convertTo0to1(20.0f));
            highCutParam->setValueNotifyingHost(highCutParam->convertTo0to1(20000.0f));
            dryWetParam->setValueNotifyingHost(1.0f);
            stretchParam->setValueNotifyingHost(0.0f);
            shiftParam->setValueNotifyingHost(0.5f);  // Center = 0 shift
            morphParam->setValueNotifyingHost(0.0f);
            gateParam->setValueNotifyingHost(0.0f);
            randomizeParam->setValueNotifyingHost(0.0f);
            break;

        case 1: // "Frozen Blur" - Smooth frozen spectrum
            freezeParam->setValueNotifyingHost(1.0f);
            blurParam->setValueNotifyingHost(blurParam->convertTo0to1(75.0f));
            lowCutParam->setValueNotifyingHost(lowCutParam->convertTo0to1(20.0f));
            highCutParam->setValueNotifyingHost(highCutParam->convertTo0to1(20000.0f));
            dryWetParam->setValueNotifyingHost(1.0f);
            stretchParam->setValueNotifyingHost(0.0f);
            shiftParam->setValueNotifyingHost(0.5f);
            morphParam->setValueNotifyingHost(0.0f);
            gateParam->setValueNotifyingHost(0.0f);
            randomizeParam->setValueNotifyingHost(0.0f);
            break;

        case 2: // "Spectral Stretch" - Time-stretched freeze
            freezeParam->setValueNotifyingHost(1.0f);
            blurParam->setValueNotifyingHost(0.0f);
            lowCutParam->setValueNotifyingHost(lowCutParam->convertTo0to1(20.0f));
            highCutParam->setValueNotifyingHost(highCutParam->convertTo0to1(20000.0f));
            dryWetParam->setValueNotifyingHost(1.0f);
            stretchParam->setValueNotifyingHost(stretchParam->convertTo0to1(80.0f));
            shiftParam->setValueNotifyingHost(0.5f);
            morphParam->setValueNotifyingHost(0.0f);
            gateParam->setValueNotifyingHost(0.0f);
            randomizeParam->setValueNotifyingHost(0.0f);
            break;

        case 3: // "Frequency Shifter" - Pitch shift effect
            freezeParam->setValueNotifyingHost(0.0f);
            blurParam->setValueNotifyingHost(0.0f);
            lowCutParam->setValueNotifyingHost(lowCutParam->convertTo0to1(20.0f));
            highCutParam->setValueNotifyingHost(highCutParam->convertTo0to1(20000.0f));
            dryWetParam->setValueNotifyingHost(1.0f);
            stretchParam->setValueNotifyingHost(0.0f);
            shiftParam->setValueNotifyingHost(shiftParam->convertTo0to1(50.0f));  // +50 shift
            morphParam->setValueNotifyingHost(0.0f);
            gateParam->setValueNotifyingHost(0.0f);
            randomizeParam->setValueNotifyingHost(0.0f);
            break;

        case 4: // "Spectral Gate" - Clean/isolate frequencies
            freezeParam->setValueNotifyingHost(0.0f);
            blurParam->setValueNotifyingHost(0.0f);
            lowCutParam->setValueNotifyingHost(lowCutParam->convertTo0to1(20.0f));
            highCutParam->setValueNotifyingHost(highCutParam->convertTo0to1(20000.0f));
            dryWetParam->setValueNotifyingHost(1.0f);
            stretchParam->setValueNotifyingHost(0.0f);
            shiftParam->setValueNotifyingHost(0.5f);
            morphParam->setValueNotifyingHost(0.0f);
            gateParam->setValueNotifyingHost(gateParam->convertTo0to1(50.0f));
            randomizeParam->setValueNotifyingHost(0.0f);
            break;

        case 5: // "Granular Texture" - Phase randomization
            freezeParam->setValueNotifyingHost(1.0f);
            blurParam->setValueNotifyingHost(0.0f);
            lowCutParam->setValueNotifyingHost(lowCutParam->convertTo0to1(20.0f));
            highCutParam->setValueNotifyingHost(highCutParam->convertTo0to1(20000.0f));
            dryWetParam->setValueNotifyingHost(1.0f);
            stretchParam->setValueNotifyingHost(0.0f);
            shiftParam->setValueNotifyingHost(0.5f);
            morphParam->setValueNotifyingHost(0.0f);
            gateParam->setValueNotifyingHost(0.0f);
            randomizeParam->setValueNotifyingHost(randomizeParam->convertTo0to1(75.0f));
            break;

        default:
            loadFactoryPreset(0);  // Default to Init
            break;
    }
}

//==============================================================================
// Plugin factory
#ifndef AUDIOFORGE_TESTS
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SpectralFreezeProcessor();
}
#endif
