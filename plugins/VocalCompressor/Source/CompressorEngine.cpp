#include "CompressorEngine.h"
#include <cmath>

CompressorEngine::CompressorEngine()
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

    std::fill(std::begin(rmsWindow_), std::end(rmsWindow_), 0.0f);
}

void CompressorEngine::prepare(double sampleRate, int samplesPerBlock) {
    sampleRate_ = sampleRate;

    dryBuffer_.setSize(2, samplesPerBlock);

    reset();
}

void CompressorEngine::reset() {
    envelope_ = 0.0f;
    currentGainReduction_ = 0.0f;
    outputLevel_ = 0.0f;
    rmsIndex_ = 0;
    std::fill(std::begin(rmsWindow_), std::end(rmsWindow_), 0.0f);
}

void CompressorEngine::setThreshold(float thresholdDb) {
    threshold_ = juce::jlimit(-60.0f, 0.0f, thresholdDb);
}

void CompressorEngine::setRatio(float ratio) {
    ratio_ = juce::jlimit(1.0f, 20.0f, ratio);
}

void CompressorEngine::setAttack(float attackMs) {
    attackMs = juce::jlimit(0.1f, 100.0f, attackMs);
    attackCoeff_ = std::exp(-1.0f / (attackMs * 0.001f * static_cast<float>(sampleRate_)));
}

void CompressorEngine::setRelease(float releaseMs) {
    releaseMs = juce::jlimit(10.0f, 1000.0f, releaseMs);
    releaseCoeff_ = std::exp(-1.0f / (releaseMs * 0.001f * static_cast<float>(sampleRate_)));
}

void CompressorEngine::setKnee(float kneeDb) {
    knee_ = juce::jlimit(0.0f, 12.0f, kneeDb);
}

void CompressorEngine::setMakeupGain(float gainDb) {
    makeupGain_ = juce::jlimit(-12.0f, 24.0f, gainDb);
}

void CompressorEngine::setParallelMix(float mix) {
    parallelMix_ = juce::jlimit(0.0f, 1.0f, mix);
}

void CompressorEngine::setDetectionMode(bool usePeak) {
    usePeak_ = usePeak;
}

float CompressorEngine::detectLevel(float sample) {
    float absValue = std::abs(sample);

    if (usePeak_) {
        // Peak detection
        return absValue;
    } else {
        // RMS detection with 512-sample window
        rmsWindow_[rmsIndex_] = sample * sample;
        rmsIndex_ = (rmsIndex_ + 1) % 512;

        float sum = 0.0f;
        for (int i = 0; i < 512; ++i)
            sum += rmsWindow_[i];

        return std::sqrt(sum / 512.0f);
    }
}

float CompressorEngine::applyKnee(float inputDb, float thresholdDb, float kneeDb) {
    if (kneeDb <= 0.0f) {
        // Hard knee
        return inputDb;
    }

    // Soft knee
    float kneeStart = thresholdDb - kneeDb / 2.0f;
    float kneeEnd = thresholdDb + kneeDb / 2.0f;

    if (inputDb < kneeStart) {
        return inputDb;
    } else if (inputDb > kneeEnd) {
        return inputDb;
    } else {
        // Smooth curve in knee region
        float x = (inputDb - kneeStart) / kneeDb;
        float curve = x * x / 2.0f; // Quadratic curve
        return inputDb - curve * kneeDb;
    }
}

float CompressorEngine::calculateGainReduction(float level) {
    // Convert to dB
    float levelDb = juce::Decibels::gainToDecibels(level, -60.0f);

    // Apply knee
    float kneedLevel = applyKnee(levelDb, threshold_, knee_);

    if (kneedLevel > threshold_) {
        // Calculate gain reduction
        float excess = kneedLevel - threshold_;
        float reductionDb = excess * (1.0f - 1.0f / ratio_);
        return juce::Decibels::decibelsToGain(-reductionDb);
    }

    return 1.0f; // No reduction
}

void CompressorEngine::process(juce::AudioBuffer<float>& buffer) {
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
        for (int ch = 0; ch < numChannels; ++ch) {
            float level = detectLevel(buffer.getSample(ch, sample));
            detectedLevel = std::max(detectedLevel, level);
        }

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
