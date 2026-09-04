#include "PluginProcessor.h"
#include "PluginEditor.h"

FreezeFXProcessor::FreezeFXProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, juce::Identifier("FreezeFXParameters"),
            {
                // Freeze toggle
                std::make_unique<juce::AudioParameterBool>(
                    PARAM_FREEZE,
                    "Freeze",
                    false),  // Default: not frozen

                // Freeze Mix (0-100%)
                std::make_unique<juce::AudioParameterFloat>(
                    PARAM_FREEZE_MIX,
                    "Freeze Mix",
                    juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
                    1.0f,  // Default: 100% frozen when active
                    juce::String(),
                    juce::AudioProcessorParameter::genericParameter,
                    [](float value, int) { return juce::String((int)(value * 100.0f)) + "%"; }),

                // FFT Size (1024, 2048, 4096, 8192)
                std::make_unique<juce::AudioParameterChoice>(
                    PARAM_FFT_SIZE,
                    "FFT Size",
                    juce::StringArray{"1024", "2048", "4096", "8192"},
                    1),  // Default: 2048

                // Overlap Factor (2x, 4x, 8x)
                std::make_unique<juce::AudioParameterChoice>(
                    PARAM_OVERLAP,
                    "Overlap",
                    juce::StringArray{"2x", "4x", "8x"},
                    1),  // Default: 4x

                // Phase Randomization (0-100%)
                std::make_unique<juce::AudioParameterFloat>(
                    PARAM_PHASE_RANDOM,
                    "Phase Random",
                    juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
                    0.5f,  // Default: 50%
                    juce::String(),
                    juce::AudioProcessorParameter::genericParameter,
                    [](float value, int) { return juce::String((int)(value * 100.0f)) + "%"; }),

                // Phase Speed (0.1-10 Hz)
                std::make_unique<juce::AudioParameterFloat>(
                    PARAM_PHASE_SPEED,
                    "Phase Speed",
                    juce::NormalisableRange<float>(0.1f, 10.0f, 0.1f, 0.3f),
                    1.0f,  // Default: 1 Hz
                    juce::String(),
                    juce::AudioProcessorParameter::genericParameter,
                    [](float value, int) { return juce::String(value, 1) + " Hz"; }),

                // Spectral Blur (0-100%)
                std::make_unique<juce::AudioParameterFloat>(
                    PARAM_SPECTRAL_BLUR,
                    "Spectral Blur",
                    juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
                    0.0f,  // Default: no blur
                    juce::String(),
                    juce::AudioProcessorParameter::genericParameter,
                    [](float value, int) { return juce::String((int)(value * 100.0f)) + "%"; }),

                // High-Pass Frequency (20-20000 Hz)
                std::make_unique<juce::AudioParameterFloat>(
                    PARAM_HIGH_PASS,
                    "High Pass",
                    juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f),
                    20.0f,  // Default: 20 Hz (full range)
                    juce::String(),
                    juce::AudioProcessorParameter::genericParameter,
                    [](float value, int) { return juce::String((int)value) + " Hz"; }),

                // Low-Pass Frequency (20-20000 Hz)
                std::make_unique<juce::AudioParameterFloat>(
                    PARAM_LOW_PASS,
                    "Low Pass",
                    juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f),
                    20000.0f,  // Default: 20 kHz (full range)
                    juce::String(),
                    juce::AudioProcessorParameter::genericParameter,
                    [](float value, int) { return juce::String((int)value) + " Hz"; })
            })
{
}

FreezeFXProcessor::~FreezeFXProcessor()
{
}

void FreezeFXProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Get FFT size from parameter
    auto* fftParam = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(PARAM_FFT_SIZE));
    int fftSizeIndex = fftParam ? fftParam->getIndex() : 1;
    int fftSize = 1024 << fftSizeIndex;  // 1024, 2048, 4096, 8192

    // Get overlap factor
    auto* overlapParam = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(PARAM_OVERLAP));
    int overlapIndex = overlapParam ? overlapParam->getIndex() : 1;
    int overlap = 2 << overlapIndex;  // 2, 4, 8

    // Prepare spectral processor
    spectralProcessor.setFFTSize(fftSize);
    spectralProcessor.setOverlapFactor(overlap);
    spectralProcessor.prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());

    // Configure phase evolver
    phaseEvolver.setRandomizationAmount(apvts.getRawParameterValue(PARAM_PHASE_RANDOM)->load());
    phaseEvolver.setEvolutionSpeed(apvts.getRawParameterValue(PARAM_PHASE_SPEED)->load());

    // Configure frozen spectrum
    frozenSpectrum.setBlurAmount(apvts.getRawParameterValue(PARAM_SPECTRAL_BLUR)->load());
    frozenSpectrum.setFrequencyRange(
        apvts.getRawParameterValue(PARAM_HIGH_PASS)->load(),
        apvts.getRawParameterValue(PARAM_LOW_PASS)->load(),
        sampleRate,
        fftSize);

    // Pre-allocate spectral blending buffers (avoid per-frame allocation)
    int numBins = fftSize / 2 + 1;
    tempFrozenMagnitude.resize(numBins);
    tempFrozenPhase.resize(numBins);

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
    bool freeze = apvts.getRawParameterValue(PARAM_FREEZE)->load() > 0.5f;

    // *** PERFORMANCE OPTIMIZATION: Bypass all FFT processing when freeze is off ***
    // This eliminates 100% of spectral processing overhead when the effect is inactive
    if (!freeze && !wasFrozen)
    {
        // Freeze is off and was off - complete bypass (no FFT!)
        return;
    }

    float freezeMix = apvts.getRawParameterValue(PARAM_FREEZE_MIX)->load();

    // Update phase evolver parameters
    phaseEvolver.setRandomizationAmount(apvts.getRawParameterValue(PARAM_PHASE_RANDOM)->load());
    phaseEvolver.setEvolutionSpeed(apvts.getRawParameterValue(PARAM_PHASE_SPEED)->load());

    // Update frozen spectrum parameters
    frozenSpectrum.setBlurAmount(apvts.getRawParameterValue(PARAM_SPECTRAL_BLUR)->load());

    // Check for freeze trigger (edge detection)
    if (freeze && !wasFrozen)
    {
        // Capture current spectrum on freeze toggle
        frozenSpectrum.captureSpectrum(
            spectralProcessor.getMagnitudeSpectrum(),
            spectralProcessor.getPhaseSpectrum());
        frozenSpectrum.freeze();
    }
    else if (!freeze && wasFrozen)
    {
        frozenSpectrum.unfreeze();
        wasFrozen = false;  // Reset state
        return;  // Exit early - no processing needed when transitioning to bypass
    }
    wasFrozen = freeze;  // Update state for next block

    // Process through spectral processor (only when freeze is active)
    spectralProcessor.processBlock(buffer);

    // TODO: Implement freeze mixing in spectral domain (Phase 2)
    // For now, the frozen effect is applied via spectral callback in processSpectrum()
}

juce::AudioProcessorEditor* FreezeFXProcessor::createEditor()
{
    return new FreezeFXEditor(*this);
}

void FreezeFXProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // APVTS handles state serialization automatically
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void FreezeFXProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // APVTS handles state deserialization automatically
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// Spectral Processing

void FreezeFXProcessor::processSpectrum(std::vector<float>& magnitude, std::vector<float>& phase)
{
    // Get current parameters
    float freezeMix = apvts.getRawParameterValue(PARAM_FREEZE_MIX)->load();
    float deltaTime = 1.0f / getSampleRate() * spectralProcessor.getHopSize();

    // If frozen, blend with frozen spectrum
    if (frozenSpectrum.isFrozen() && freezeMix > 0.0f)
    {
        // Use pre-allocated buffers (no per-frame allocation!)
        // Get frozen spectrum into temp buffers
        frozenSpectrum.getSpectrum(tempFrozenMagnitude, tempFrozenPhase);

        // Blend magnitude: live → frozen based on mix
        for (size_t i = 0; i < magnitude.size(); ++i)
        {
            magnitude[i] = magnitude[i] * (1.0f - freezeMix) + tempFrozenMagnitude[i] * freezeMix;
        }

        // Evolve phase if randomization is enabled
        float phaseRandom = apvts.getRawParameterValue(PARAM_PHASE_RANDOM)->load();
        if (phaseRandom > 0.0f)
        {
            phaseEvolver.evolvePhase(phase, deltaTime);
        }
        else
        {
            // Use frozen phase
            for (size_t i = 0; i < phase.size(); ++i)
            {
                phase[i] = phase[i] * (1.0f - freezeMix) + tempFrozenPhase[i] * freezeMix;
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
