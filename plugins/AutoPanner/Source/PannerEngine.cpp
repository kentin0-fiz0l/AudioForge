#include "PannerEngine.h"

PannerEngine::PannerEngine() {}

void PannerEngine::prepareToPlay(double sr, int sb) {
    sampleRate_ = sr;
    lfoPhase_ = 0.0f;
}

void PannerEngine::processBlock(juce::AudioBuffer<float>& buf) {
    const int numSamples = buf.getNumSamples();
    const int numChannels = buf.getNumChannels();

    if (numChannels < 2) return; // Stereo only

    for (int sample = 0; sample < numSamples; ++sample) {
        // Get LFO values for left and right channels
        float lfoL = getLFOValue(lfoPhase_);
        float lfoR = getLFOValue(lfoPhase_ + phaseOffset_ * 2.0f * juce::MathConstants<float>::pi);

        // Map LFO to pan position (-1 to +1)
        float panL = lfoL * depth_;
        float panR = lfoR * depth_;

        // Get input samples
        float left = buf.getSample(0, sample);
        float right = buf.getSample(1, sample);

        // Convert stereo to mono for panning
        float mono = (left + right) * 0.5f;

        // Apply equal power panning
        float gainL_L = getPanLaw(-panL);  // Left channel pan
        float gainL_R = getPanLaw(panL);   // Left channel to right
        float gainR_L = getPanLaw(-panR);  // Right channel pan
        float gainR_R = getPanLaw(panR);   // Right channel to right

        // Apply stereo width
        float mid = (left + right) * 0.5f;
        float side = (left - right) * 0.5f * stereoWidth_;

        left = mid + side;
        right = mid - side;

        // Apply autopan
        buf.setSample(0, sample, left * gainL_L);
        buf.setSample(1, sample, right * gainR_R);

        // Update LFO phase
        lfoPhase_ += (2.0f * juce::MathConstants<float>::pi * rate_) / static_cast<float>(sampleRate_);
        if (lfoPhase_ >= 2.0f * juce::MathConstants<float>::pi)
            lfoPhase_ -= 2.0f * juce::MathConstants<float>::pi;
    }
}

float PannerEngine::getLFOValue(float phase) {
    float lfo = 0.0f;
    if (waveform_ == 0) { // Sine
        lfo = std::sin(phase);
    } else if (waveform_ == 1) { // Triangle
        lfo = (2.0f / juce::MathConstants<float>::pi) * std::asin(std::sin(phase));
    } else { // Square
        lfo = std::sin(phase) >= 0.0f ? 1.0f : -1.0f;
    }
    return lfo;
}

float PannerEngine::getPanLaw(float pan) {
    // Equal power panning: -3dB at center
    // pan: -1 (left) to +1 (right)
    float angle = (pan * 0.5f + 0.5f) * juce::MathConstants<float>::halfPi;
    return std::cos(angle);
}
