#include "PluginProcessor.h"
#include "PluginEditor.h"

FreezeFXProcessor::FreezeFXProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Freeze toggle
    addParameter(freezeParam = new juce::AudioParameterBool(
        PARAM_FREEZE,
        "Freeze",
        false));  // Default: not frozen

    // Freeze Mix (0-100%)
    addParameter(freezeMixParam = new juce::AudioParameterFloat(
        PARAM_FREEZE_MIX,
        "Freeze Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        1.0f));  // Default: 100% frozen when active

    // FFT Size (1024, 2048, 4096, 8192)
    addParameter(fftSizeParam = new juce::AudioParameterChoice(
        PARAM_FFT_SIZE,
        "FFT Size",
        juce::StringArray{"1024", "2048", "4096", "8192"},
        1));  // Default: 2048

    // Overlap Factor (2x, 4x, 8x)
    addParameter(overlapParam = new juce::AudioParameterChoice(
        PARAM_OVERLAP,
        "Overlap",
        juce::StringArray{"2x", "4x", "8x"},
        1));  // Default: 4x

    // Phase Randomization (0-100%)
    addParameter(phaseRandomParam = new juce::AudioParameterFloat(
        PARAM_PHASE_RANDOM,
        "Phase Random",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f));  // Default: 50%

    // Phase Speed (0.1-10 Hz)
    addParameter(phaseSpeedParam = new juce::AudioParameterFloat(
        PARAM_PHASE_SPEED,
        "Phase Speed",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.1f, 0.3f),
        1.0f));  // Default: 1 Hz

    // Spectral Blur (0-100%)
    addParameter(spectralBlurParam = new juce::AudioParameterFloat(
        PARAM_SPECTRAL_BLUR,
        "Spectral Blur",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f));  // Default: no blur

    // High-Pass Frequency (20-20000 Hz)
    addParameter(highPassParam = new juce::AudioParameterFloat(
        PARAM_HIGH_PASS,
        "High Pass",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f),
        20.0f));  // Default: 20 Hz (full range)

    // Low-Pass Frequency (20-20000 Hz)
    addParameter(lowPassParam = new juce::AudioParameterFloat(
        PARAM_LOW_PASS,
        "Low Pass",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f),
        20000.0f));  // Default: 20 kHz (full range)
}

FreezeFXProcessor::~FreezeFXProcessor()
{
}

void FreezeFXProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Get FFT size from parameter
    int fftSizeIndex = fftSizeParam->getIndex();
    int fftSize = 1024 << fftSizeIndex;  // 1024, 2048, 4096, 8192

    // Get overlap factor
    int overlapIndex = overlapParam->getIndex();
    int overlap = 2 << overlapIndex;  // 2, 4, 8

    // Prepare spectral processor
    spectralProcessor.setFFTSize(fftSize);
    spectralProcessor.setOverlapFactor(overlap);
    spectralProcessor.prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());

    // Configure phase evolver
    phaseEvolver.setRandomizationAmount(phaseRandomParam->get());
    phaseEvolver.setEvolutionSpeed(phaseSpeedParam->get());

    // Configure frozen spectrum
    frozenSpectrum.setBlurAmount(spectralBlurParam->get());
    frozenSpectrum.setFrequencyRange(
        highPassParam->get(),
        lowPassParam->get(),
        sampleRate,
        fftSize);

    // Set up spectral processing callback
    spectralProcessor.setSpectralCallback(
        [this](std::vector<float>& magnitude, std::vector<float>& phase)
        {
            this->processSpectrum(magnitude, phase);
        });
}

void FreezeFXProcessor::releaseResources()
{
}

bool FreezeFXProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // Supports mono and stereo
    return layouts.getMainOutputChannelSet() == layouts.getMainInputChannelSet()
        && !layouts.getMainInputChannelSet().isDisabled();
}

void FreezeFXProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    // Get parameters
    bool freeze = freezeParam->get();
    float freezeMix = freezeMixParam->get();

    // Update phase evolver parameters
    phaseEvolver.setRandomizationAmount(phaseRandomParam->get());
    phaseEvolver.setEvolutionSpeed(phaseSpeedParam->get());

    // Update frozen spectrum parameters
    frozenSpectrum.setBlurAmount(spectralBlurParam->get());

    // Check for freeze trigger
    static bool wasFrozen = false;
    if (freeze && !wasFrozen)
    {
        // Capture current spectrum
        frozenSpectrum.captureSpectrum(
            spectralProcessor.getMagnitudeSpectrum(),
            spectralProcessor.getPhaseSpectrum());
        frozenSpectrum.freeze();
    }
    else if (!freeze && wasFrozen)
    {
        frozenSpectrum.unfreeze();
    }
    wasFrozen = freeze;

    // Process through spectral processor
    // TODO: Implement freeze mixing in spectral domain (Phase 2)
    spectralProcessor.processBlock(buffer);

    // For now, just apply dry/wet mix at audio level (temporary)
    if (!freeze)
    {
        // Not frozen - normal pass-through
        return;
    }

    // Frozen - apply mix
    // (This is a placeholder - proper spectral mixing happens in Phase 2)
}

juce::AudioProcessorEditor* FreezeFXProcessor::createEditor()
{
    return new FreezeFXEditor(*this);
}

void FreezeFXProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save plugin state
    juce::MemoryOutputStream stream(destData, true);

    stream.writeBool(freezeParam->get());
    stream.writeFloat(freezeMixParam->get());
    stream.writeInt(fftSizeParam->getIndex());
    stream.writeInt(overlapParam->getIndex());
    stream.writeFloat(phaseRandomParam->get());
    stream.writeFloat(phaseSpeedParam->get());
    stream.writeFloat(spectralBlurParam->get());
    stream.writeFloat(highPassParam->get());
    stream.writeFloat(lowPassParam->get());
}

void FreezeFXProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore plugin state
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);

    freezeParam->setValueNotifyingHost(stream.readBool() ? 1.0f : 0.0f);
    freezeMixParam->setValueNotifyingHost(freezeMixParam->convertTo0to1(stream.readFloat()));
    fftSizeParam->setValueNotifyingHost(static_cast<float>(stream.readInt()) / (fftSizeParam->choices.size() - 1));
    overlapParam->setValueNotifyingHost(static_cast<float>(stream.readInt()) / (overlapParam->choices.size() - 1));
    phaseRandomParam->setValueNotifyingHost(phaseRandomParam->convertTo0to1(stream.readFloat()));
    phaseSpeedParam->setValueNotifyingHost(phaseSpeedParam->convertTo0to1(stream.readFloat()));
    spectralBlurParam->setValueNotifyingHost(spectralBlurParam->convertTo0to1(stream.readFloat()));
    highPassParam->setValueNotifyingHost(highPassParam->convertTo0to1(stream.readFloat()));
    lowPassParam->setValueNotifyingHost(lowPassParam->convertTo0to1(stream.readFloat()));
}

//==============================================================================
// Spectral Processing

void FreezeFXProcessor::processSpectrum(std::vector<float>& magnitude, std::vector<float>& phase)
{
    // Get current parameters
    float freezeMix = freezeMixParam->get();
    float deltaTime = 1.0f / getSampleRate() * spectralProcessor.getHopSize();

    // If frozen, blend with frozen spectrum
    if (frozenSpectrum.isFrozen() && freezeMix > 0.0f)
    {
        std::vector<float> frozenMagnitude(magnitude.size());
        std::vector<float> frozenPhase(phase.size());

        // Get frozen spectrum
        frozenSpectrum.getSpectrum(frozenMagnitude, frozenPhase);

        // Blend magnitude: live → frozen based on mix
        for (size_t i = 0; i < magnitude.size(); ++i)
        {
            magnitude[i] = magnitude[i] * (1.0f - freezeMix) + frozenMagnitude[i] * freezeMix;
        }

        // Evolve phase if randomization is enabled
        float phaseRandom = phaseRandomParam->get();
        if (phaseRandom > 0.0f)
        {
            phaseEvolver.evolvePhase(phase, deltaTime);
        }
        else
        {
            // Use frozen phase
            for (size_t i = 0; i < phase.size(); ++i)
            {
                phase[i] = phase[i] * (1.0f - freezeMix) + frozenPhase[i] * freezeMix;
            }
        }
    }
}

//==============================================================================
// Plugin factory
#ifndef AUDIOFORGE_TESTS
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FreezeFXProcessor();
}
#endif
