#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

/**
 * Vocal Compressor Engine
 *
 * Full-band compressor optimized for vocal processing with:
 * - RMS/Peak detection, linked across channels (the louder channel drives both)
 * - Soft/hard knee
 * - Parallel compression
 * - Manual makeup gain
 */
class VocalCompressorEngine {
public:
    VocalCompressorEngine();

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
    static constexpr int kMaxChannels = 2;
    static constexpr int kRmsWindowSize = 512;
    float rmsWindow_[kMaxChannels][kRmsWindowSize];
    int rmsIndex_;

    double sampleRate_;
    float currentGainReduction_;
    float outputLevel_;

    juce::AudioBuffer<float> dryBuffer_;

    // Helper functions
    float detectLevel(int channel, float sample);
    float calculateGainReduction(float level);
};
