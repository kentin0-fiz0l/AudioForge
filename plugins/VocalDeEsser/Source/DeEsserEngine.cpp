#include "DeEsserEngine.h"
#include <cmath>

DeEsserEngine::DeEsserEngine()
    : bandCoefficients_(new Coefficients(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f)),
      envelope_(0.0f),
      attackCoeff_(0.0f),
      releaseCoeff_(0.0f),
      frequency_(7000.0f),
      bandwidth_(1.5f),
      threshold_(-20.0f),
      ratio_(4.0f),
      mix_(1.0f),
      sampleRate_(44100.0),
      currentGainReduction_(0.0f) {
    for (auto& filter : bandFilters_)
        filter.coefficients = bandCoefficients_;
}

void DeEsserEngine::prepare(double sampleRate, int /*samplesPerBlock*/) {
    sampleRate_ = sampleRate;
    updateFilters();
    reset();
}

void DeEsserEngine::reset() {
    for (auto& filter : bandFilters_)
        filter.reset();
    envelope_ = 0.0f;
    currentGainReduction_ = 0.0f;
}

void DeEsserEngine::setFrequency(float freqHz) {
    freqHz = juce::jlimit(4000.0f, 10000.0f, freqHz);
    if (freqHz != frequency_) {
        frequency_ = freqHz;
        updateFilters();
    }
}

void DeEsserEngine::setBandwidth(float octaves) {
    octaves = juce::jlimit(0.5f, 4.0f, octaves);
    if (octaves != bandwidth_) {
        bandwidth_ = octaves;
        updateFilters();
    }
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
    // Band-pass around the sibilance (unity gain at the centre). Updated in
    // place: the array form of the coefficients does not allocate.
    const float Q = 1.0f / bandwidth_;
    const float frequency = juce::jmin(frequency_, static_cast<float>(sampleRate_) * 0.45f);
    *bandCoefficients_ = juce::dsp::IIR::ArrayCoefficients<float>::makeBandPass(sampleRate_, frequency, Q);
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
    const int numChannels = juce::jmin(buffer.getNumChannels(), kMaxChannels);

    if (numSamples == 0 || numChannels == 0)
        return;

    std::array<float*, kMaxChannels> channelData {};
    for (int ch = 0; ch < numChannels; ++ch)
        channelData[static_cast<size_t>(ch)] = buffer.getWritePointer(ch);

    float maxGainReduction = 0.0f;

    for (int sample = 0; sample < numSamples; ++sample) {
        // Split off the sibilant band and detect its level (linked stereo)
        std::array<float, kMaxChannels> band {};
        float sidechainLevel = 0.0f;
        for (size_t ch = 0; ch < static_cast<size_t>(numChannels); ++ch) {
            band[ch] = bandFilters_[ch].processSample(channelData[ch][sample]);
            sidechainLevel = std::max(sidechainLevel, std::abs(band[ch]));
        }

        // Envelope follower
        if (sidechainLevel > envelope_)
            envelope_ = attackCoeff_ * envelope_ + (1.0f - attackCoeff_) * sidechainLevel;
        else
            envelope_ = releaseCoeff_ * envelope_ + (1.0f - releaseCoeff_) * sidechainLevel;

        const float gainReduction = calculateGainReduction(envelope_);
        maxGainReduction = std::max(maxGainReduction, 1.0f - gainReduction);

        // Output = rest of signal + reduced band = input + (gain - 1) * band,
        // blended with the dry input by the mix amount
        const float bandGain = mix_ * (gainReduction - 1.0f);
        for (size_t ch = 0; ch < static_cast<size_t>(numChannels); ++ch)
            channelData[ch][sample] += bandGain * band[ch];
    }

    currentGainReduction_ = maxGainReduction;
}
