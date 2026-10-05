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
            }),
      midiLearnManager_(apvts),
      presetManager_(apvts, "FreezeFX")
{
    // Scan for presets on startup
    presetManager_.scanPresets();
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

    // Each channel freezes its own sound
    int numBins = fftSize / 2 + 1;
    frozenChannels = std::vector<FrozenChannel>(static_cast<size_t>(juce::jmax(1, getTotalNumOutputChannels())));

    for (auto& frozen : frozenChannels)
    {
        frozen.phaseEvolver.setRandomizationAmount(apvts.getRawParameterValue(PARAM_PHASE_RANDOM)->load());
        frozen.phaseEvolver.setEvolutionSpeed(apvts.getRawParameterValue(PARAM_PHASE_SPEED)->load());

        frozen.spectrum.setBlurAmount(apvts.getRawParameterValue(PARAM_SPECTRAL_BLUR)->load());
        frozen.spectrum.setFrequencyRange(
            apvts.getRawParameterValue(PARAM_HIGH_PASS)->load(),
            apvts.getRawParameterValue(PARAM_LOW_PASS)->load(),
            sampleRate,
            fftSize);

        frozen.spectrum.prepare(static_cast<size_t>(numBins));
        frozen.phase.assign(static_cast<size_t>(numBins), 0.0f);
        frozen.phaseAdvance.assign(static_cast<size_t>(numBins), 0.0f);

        // The evolver sizes itself to the first spectrum it is given. Give
        // it one now, so that it does not allocate on the first frozen frame.
        frozen.phaseEvolver.evolvePhase(frozen.phase, 0.0f);
    }

    wasFrozen = false;
    frozenForUI.store(false);

    // Pre-allocate spectral blending buffers (avoid per-frame allocation)
    tempFrozenMagnitude.resize(numBins);

    // Set up spectral processing callback
    spectralProcessor.setSpectralCallback(
        [this](int channel, std::vector<float>& magnitude, std::vector<float>& phase)
        {
            this->processSpectrum(channel, magnitude, phase);
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
        // Freeze is off and was off - bypass (no FFT!). The analysis window is
        // still kept up to date, so there is something to capture the moment
        // freeze is switched on.
        spectralProcessor.pushInput(buffer);
        return;
    }

    // Update phase evolver and frozen spectrum parameters
    const float phaseRandom = apvts.getRawParameterValue(PARAM_PHASE_RANDOM)->load();
    const float phaseSpeed = apvts.getRawParameterValue(PARAM_PHASE_SPEED)->load();
    const float spectralBlur = apvts.getRawParameterValue(PARAM_SPECTRAL_BLUR)->load();

    for (auto& frozen : frozenChannels)
    {
        frozen.phaseEvolver.setRandomizationAmount(phaseRandom);
        frozen.phaseEvolver.setEvolutionSpeed(phaseSpeed);
        frozen.spectrum.setBlurAmount(spectralBlur);
    }

    // Check for freeze trigger (edge detection)
    if (freeze && !wasFrozen)
    {
        // Capture on freeze toggle. No spectrum has been computed while
        // bypassed, so take it from the first frame analysed from here on:
        // that frame ends within one hop of this moment.
        spectralProcessor.clearOutput();

        for (auto& frozen : frozenChannels)
        {
            frozen.capturePending = true;
            frozen.spectrum.freeze();
        }
    }
    else if (!freeze && wasFrozen)
    {
        for (auto& frozen : frozenChannels)
            frozen.spectrum.unfreeze();

        wasFrozen = false;  // Reset state
        frozenForUI.store(false);
        spectralProcessor.pushInput(buffer);
        return;  // Exit early - no processing needed when transitioning to bypass
    }
    wasFrozen = freeze;  // Update state for next block
    frozenForUI.store(freeze);

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

void FreezeFXProcessor::processSpectrum(int channel, std::vector<float>& magnitude, std::vector<float>& phase)
{
    if (! juce::isPositiveAndBelow(channel, static_cast<int>(frozenChannels.size())))
        return;

    auto& frozen = frozenChannels[static_cast<size_t>(channel)];
    const size_t numBins = juce::jmin(magnitude.size(), frozen.phase.size(), tempFrozenMagnitude.size());
    const float twoPi = juce::MathConstants<float>::twoPi;
    const juce::int64 phaseDriftSeed = 0x46726565;

    if (frozen.capturePending)
    {
        frozen.spectrum.captureSpectrum(magnitude);
        frozen.capturePending = false;
        frozen.measurePending = true;

        // Each channel drifts its own way, and the same way on every render
        frozen.phaseEvolver.restart(phaseDriftSeed + channel);

        // Start from the phase the sound had. Until the next frame says
        // better, assume each bin holds a tone at the bin's own frequency,
        // whose phase moves on by bin * hop / fftSize turns in one hop.
        const size_t hop = static_cast<size_t>(spectralProcessor.getHopSize());
        const size_t fftSize = static_cast<size_t>(spectralProcessor.getFFTSize());

        for (size_t bin = 0; bin < numBins; ++bin)
        {
            const float turns = static_cast<float>((bin * hop) % fftSize) / static_cast<float>(fftSize);

            frozen.phase[bin] = phase[bin];
            frozen.phaseAdvance[bin] = std::remainder(twoPi * turns, twoPi);
        }
    }
    else if (frozen.spectrum.isFrozen())
    {
        // One frame on, the input shows how far each bin's phase really
        // moves in a hop, which is the exact frequency of what is in it.
        // Use that wherever the same sound is still there to be measured:
        // not where it has stopped, and not where something louder has
        // started.
        if (frozen.measurePending)
        {
            frozen.measurePending = false;
            const auto& captured = frozen.spectrum.getCapturedMagnitude();

            for (size_t bin = 0; bin < numBins; ++bin)
                if (captured[bin] > 1.0e-6f && magnitude[bin] > 0.25f * captured[bin] && magnitude[bin] < 4.0f * captured[bin])
                    frozen.phaseAdvance[bin] = std::remainder(phase[bin] - frozen.phase[bin], twoPi);
        }

        for (size_t bin = 0; bin < numBins; ++bin)
            frozen.phase[bin] = std::remainder(frozen.phase[bin] + frozen.phaseAdvance[bin], twoPi);

        // Phase Random lets the frozen sound drift from there; at zero the
        // evolver leaves the phase alone
        const float deltaTime = static_cast<float>(spectralProcessor.getHopSize() / getSampleRate());
        frozen.phaseEvolver.evolvePhase(frozen.phase, deltaTime);
    }

    const float freezeMix = apvts.getRawParameterValue(PARAM_FREEZE_MIX)->load();

    if (! frozen.spectrum.isFrozen() || freezeMix <= 0.0f)
        return;

    // The frozen magnitudes, with blur and frequency range applied
    frozen.spectrum.getSpectrum(tempFrozenMagnitude);

    if (freezeMix >= 1.0f)
    {
        for (size_t bin = 0; bin < numBins; ++bin)
        {
            magnitude[bin] = tempFrozenMagnitude[bin];
            phase[bin] = frozen.phase[bin];
        }

        return;
    }

    // Part frozen, part live: add the two as the sounds they are. Blending
    // magnitudes and phases separately gives neither.
    for (size_t bin = 0; bin < numBins; ++bin)
    {
        const float live = magnitude[bin] * (1.0f - freezeMix);
        const float held = tempFrozenMagnitude[bin] * freezeMix;

        const float real = live * std::cos(phase[bin]) + held * std::cos(frozen.phase[bin]);
        const float imaginary = live * std::sin(phase[bin]) + held * std::sin(frozen.phase[bin]);

        magnitude[bin] = std::sqrt(real * real + imaginary * imaginary);
        phase[bin] = std::atan2(imaginary, real);
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
