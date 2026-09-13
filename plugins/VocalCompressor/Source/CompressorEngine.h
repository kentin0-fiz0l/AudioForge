#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

/**
 * Vocal Compressor Engine
 *
 * Full-band compressor optimized for vocal processing with:
 * - RMS/Peak detection
 * - Soft/hard knee
 * - Parallel compression
 * - Auto makeup gain
 */
class CompressorEngine {
public:
    CompressorEngine();

    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    // Parameters
    void setThreshold(float thresholdDb);
    void setRatio(float ratio);
    void setAttack(float attackMs);
    void setRelease(float releaseMs);
    void setKnee(float kneeDb);
    void setMakeupGain(float gainDb);
    void setParallelMix(float mix);       // 0-1
    void setDetectionMode(bool usePeak);  // true=Peak, false=RMS

    // Metering
    float getGainReduction() const { return currentGainReduction_; }
    float getOutputLevel() const { return outputLevel_; }

private:
    // Parameters
    float threshold_;
    float ratio_;
    float knee_;
    float makeupGain_;
    float parallelMix_;
    bool usePeak_;

    // State
    float envelope_;
    float attackCoeff_;
    float releaseCoeff_;
    float rmsWindow_[512];
    int rmsIndex_;

    double sampleRate_;
    float currentGainReduction_;
    float outputLevel_;

    juce::AudioBuffer<float> dryBuffer_;

    // Helper functions
    float detectLevel(float sample);
    float calculateGainReduction(float level);
    float applyKnee(float inputDb, float thresholdDb, float kneeDb);
};
