#include "CompressorEngine.h"
#include <cmath>

VocalCompressorEngine::VocalCompressorEngine()
    : threshold_(-18.0f),
      ratio_(4.0f),
      knee_(3.0f),
      makeupGain_(0.0f),
      parallelMix_(0.0f),
      usePeak_(false),
      envelope_(0.0f),
      attackCoeff_(0.0f),
      releaseCoeff_(0.0f),
      rmsIndex_(0),
      sampleRate_(44100.0),
      currentGainReduction_(0.0f),
      outputLevel_(0.0f) {

    reset();
}

void VocalCompressorEngine::prepare(double sampleRate, int samplesPerBlock) {
    sampleRate_ = sampleRate;

    dryBuffer_.setSize(2, samplesPerBlock);

    reset();
}

void VocalCompressorEngine::reset() {
    envelope_ = 0.0f;
    currentGainReduction_ = 0.0f;
    outputLevel_ = 0.0f;
    rmsIndex_ = 0;
    for (auto& window : rmsWindow_)
        std::fill(std::begin(window), std::end(window), 0.0f);
}

void VocalCompressorEngine::setThreshold(float thresholdDb) {
    threshold_ = juce::jlimit(-60.0f, 0.0f, thresholdDb);
}

void VocalCompressorEngine::setRatio(float ratio) {
    ratio_ = juce::jlimit(1.0f, 20.0f, ratio);
}

void VocalCompressorEngine::setAttack(float attackMs) {
    attackMs = juce::jlimit(0.1f, 100.0f, attackMs);
    attackCoeff_ = std::exp(-1.0f / (attackMs * 0.001f * static_cast<float>(sampleRate_)));
}

void VocalCompressorEngine::setRelease(float releaseMs) {
    releaseMs = juce::jlimit(10.0f, 1000.0f, releaseMs);
    releaseCoeff_ = std::exp(-1.0f / (releaseMs * 0.001f * static_cast<float>(sampleRate_)));
}

void VocalCompressorEngine::setKnee(float kneeDb) {
    knee_ = juce::jlimit(0.0f, 12.0f, kneeDb);
}

void VocalCompressorEngine::setMakeupGain(float gainDb) {
    makeupGain_ = juce::jlimit(-12.0f, 24.0f, gainDb);
}

void VocalCompressorEngine::setParallelMix(float mix) {
    parallelMix_ = juce::jlimit(0.0f, 1.0f, mix);
}

void VocalCompressorEngine::setDetectionMode(bool usePeak) {
    usePeak_ = usePeak;
}

float VocalCompressorEngine::detectLevel(int channel, float sample) {
    if (usePeak_) {
        // Peak detection
        return std::abs(sample);
    } else {
        // RMS detection with a 512-sample window per channel
        float* window = rmsWindow_[channel];
        window[rmsIndex_] = sample * sample;

        float sum = 0.0f;
        for (int i = 0; i < kRmsWindowSize; ++i)
            sum += window[i];

        return std::sqrt(sum / static_cast<float>(kRmsWindowSize));
    }
}

float VocalCompressorEngine::calculateGainReduction(float level) {
    const float overDb = juce::Decibels::gainToDecibels(level, -60.0f) - threshold_;
    const float slope = 1.0f - 1.0f / ratio_;
    float reductionDb = 0.0f;

    if (2.0f * overDb >= knee_) {
        // Above the knee (or hard knee): follow the ratio line
        reductionDb = slope * overDb;
    } else if (2.0f * overDb > -knee_) {
        // Inside the soft knee: quadratic blend from no reduction to the ratio line
        const float intoKnee = overDb + knee_ / 2.0f;
        reductionDb = slope * intoKnee * intoKnee / (2.0f * knee_);
    }

    return juce::Decibels::decibelsToGain(-reductionDb);
}

void VocalCompressorEngine::process(juce::AudioBuffer<float>& buffer) {
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    if (numSamples == 0 || numChannels == 0)
        return;

    // Store dry signal for parallel processing
    dryBuffer_.setSize(numChannels, numSamples, false, false, true);
    for (int ch = 0; ch < numChannels; ++ch)
        dryBuffer_.copyFrom(ch, 0, buffer, ch, 0, numSamples);

    float maxGainReduction = 0.0f;
    float maxOutput = 0.0f;

    // Process each sample
    for (int sample = 0; sample < numSamples; ++sample) {
        // Detect level (stereo-linked)
        float detectedLevel = 0.0f;
        for (int ch = 0; ch < std::min(numChannels, kMaxChannels); ++ch) {
            float level = detectLevel(ch, buffer.getSample(ch, sample));
            detectedLevel = std::max(detectedLevel, level);
        }
        rmsIndex_ = (rmsIndex_ + 1) % kRmsWindowSize;

        // Envelope follower
        if (detectedLevel > envelope_)
            envelope_ = attackCoeff_ * envelope_ + (1.0f - attackCoeff_) * detectedLevel;
        else
            envelope_ = releaseCoeff_ * envelope_ + (1.0f - releaseCoeff_) * detectedLevel;

        // Calculate gain reduction
        float gainReduction = calculateGainReduction(envelope_);
        maxGainReduction = std::max(maxGainReduction, 1.0f - gainReduction);

        // Apply compression and makeup gain
        float makeupGainLinear = juce::Decibels::decibelsToGain(makeupGain_);

        for (int ch = 0; ch < numChannels; ++ch) {
            float wetSample = buffer.getSample(ch, sample) * gainReduction * makeupGainLinear;
            float drySample = dryBuffer_.getSample(ch, sample);

            // Parallel mix
            float outputSample = wetSample * (1.0f - parallelMix_) + drySample * parallelMix_;

            buffer.setSample(ch, sample, outputSample);
            maxOutput = std::max(maxOutput, std::abs(outputSample));
        }
    }

    currentGainReduction_ = maxGainReduction;
    outputLevel_ = maxOutput;
}
