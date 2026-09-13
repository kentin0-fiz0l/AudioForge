#include "DeEsserEngine.h"
#include <cmath>

DeEsserEngine::DeEsserEngine()
    : envelope_(0.0f),
      attackCoeff_(0.0f),
      releaseCoeff_(0.0f),
      frequency_(7000.0f),
      bandwidth_(1.5f),
      threshold_(-20.0f),
      ratio_(4.0f),
      mix_(1.0f),
      sampleRate_(44100.0),
      currentGainReduction_(0.0f) {
}

void DeEsserEngine::prepare(double sampleRate, int samplesPerBlock) {
    sampleRate_ = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 2;

    highPassFilter_.prepare(spec);
    lowPassFilter_.prepare(spec);
    bandPassFilter_.prepare(spec);

    updateFilters();
    reset();
}

void DeEsserEngine::reset() {
    highPassFilter_.reset();
    lowPassFilter_.reset();
    bandPassFilter_.reset();
    envelope_ = 0.0f;
    currentGainReduction_ = 0.0f;
}

void DeEsserEngine::setFrequency(float freqHz) {
    frequency_ = juce::jlimit(4000.0f, 10000.0f, freqHz);
    updateFilters();
}

void DeEsserEngine::setBandwidth(float octaves) {
    bandwidth_ = juce::jlimit(0.5f, 4.0f, octaves);
    updateFilters();
}

void DeEsserEngine::setThreshold(float thresholdDb) {
    threshold_ = juce::jlimit(-60.0f, 0.0f, thresholdDb);
}

void DeEsserEngine::setRatio(float ratio) {
    ratio_ = juce::jlimit(1.0f, 10.0f, ratio);
}

void DeEsserEngine::setAttack(float attackMs) {
    attackMs = juce::jlimit(0.1f, 10.0f, attackMs);
    attackCoeff_ = std::exp(-1.0f / (attackMs * 0.001f * static_cast<float>(sampleRate_)));
}

void DeEsserEngine::setRelease(float releaseMs) {
    releaseMs = juce::jlimit(10.0f, 500.0f, releaseMs);
    releaseCoeff_ = std::exp(-1.0f / (releaseMs * 0.001f * static_cast<float>(sampleRate_)));
}

void DeEsserEngine::setMix(float mix) {
    mix_ = juce::jlimit(0.0f, 1.0f, mix);
}

void DeEsserEngine::updateFilters() {
    // Calculate low and high frequencies for the band
    const float Q = 1.0f / bandwidth_;

    // Bandpass filter for sidechain (detects sibilance)
    auto bandPass = Coefficients::makeBandPass(sampleRate_, frequency_, Q);
    bandPassFilter_.state = *bandPass;

    // High-pass and low-pass for splitting signal
    const float lowFreq = frequency_ / std::pow(2.0f, bandwidth_ / 2.0f);
    const float highFreq = frequency_ * std::pow(2.0f, bandwidth_ / 2.0f);

    auto highPass = Coefficients::makeHighPass(sampleRate_, lowFreq);
    auto lowPass = Coefficients::makeLowPass(sampleRate_, highFreq);

    highPassFilter_.state = *highPass;
    lowPassFilter_.state = *lowPass;
}

float DeEsserEngine::calculateGainReduction(float sidechainLevel) {
    // Convert to dB
    const float levelDb = juce::Decibels::gainToDecibels(sidechainLevel);

    if (levelDb > threshold_) {
        // Calculate gain reduction
        const float excess = levelDb - threshold_;
        const float reductionDb = excess * (1.0f - 1.0f / ratio_);
        return juce::Decibels::decibelsToGain(-reductionDb);
    }

    return 1.0f; // No reduction
}

void DeEsserEngine::process(juce::AudioBuffer<float>& buffer) {
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    if (numSamples == 0 || numChannels == 0)
        return;

    // Create a copy for dry signal
    juce::AudioBuffer<float> dryBuffer(numChannels, numSamples);
    for (int ch = 0; ch < numChannels; ++ch)
        dryBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);

    // Create sidechain signal (bandpass filtered)
    juce::AudioBuffer<float> sidechainBuffer(numChannels, numSamples);
    for (int ch = 0; ch < numChannels; ++ch)
        sidechainBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);

    juce::dsp::AudioBlock<float> sidechainBlock(sidechainBuffer);
    juce::dsp::ProcessContextReplacing<float> sidechainContext(sidechainBlock);
    bandPassFilter_.process(sidechainContext);

    // Process each sample
    float maxGainReduction = 0.0f;

    for (int sample = 0; sample < numSamples; ++sample) {
        // Analyze sidechain (sibilance detection)
        float sidechainLevel = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch) {
            float value = std::abs(sidechainBuffer.getSample(ch, sample));
            sidechainLevel = std::max(sidechainLevel, value);
        }

        // Envelope follower
        if (sidechainLevel > envelope_)
            envelope_ = attackCoeff_ * envelope_ + (1.0f - attackCoeff_) * sidechainLevel;
        else
            envelope_ = releaseCoeff_ * envelope_ + (1.0f - releaseCoeff_) * sidechainLevel;

        // Calculate gain reduction
        float gainReduction = calculateGainReduction(envelope_);
        maxGainReduction = std::max(maxGainReduction, 1.0f - gainReduction);

        // Apply gain reduction to high frequency content only
        juce::AudioBuffer<float> highBuffer(numChannels, 1);
        for (int ch = 0; ch < numChannels; ++ch) {
            highBuffer.setSample(ch, 0, buffer.getSample(ch, sample));
        }

        // Split into high and low
        juce::dsp::AudioBlock<float> highBlock(highBuffer);
        juce::dsp::ProcessContextReplacing<float> highContext(highBlock);

        // Process high band with compression
        for (int ch = 0; ch < numChannels; ++ch) {
            float highSample = buffer.getSample(ch, sample);

            // Simple approximation: apply gain reduction to original signal
            // (In production, would use proper crossover)
            float processedSample = highSample * gainReduction;

            // Mix dry/wet
            float outputSample = dryBuffer.getSample(ch, sample) * (1.0f - mix_) +
                                processedSample * mix_;

            buffer.setSample(ch, sample, outputSample);
        }
    }

    currentGainReduction_ = maxGainReduction;
}
