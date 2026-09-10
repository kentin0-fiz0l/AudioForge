#include "DelayEngine.h"
void DelayEngine::prepareToPlay(double sr, int) {
    sampleRate_ = sr;
    int maxDelay = static_cast<int>(sr * 2.5);
    for (int ch = 0; ch < 2; ++ch) {
        delayBuffer_[ch].resize(maxDelay, 0.0f);
        lpFilter_[ch] = 0.0f;
    }
    writePos_ = 0;
    wowPhase_ = flutterPhase_ = 0.0f;
}
void DelayEngine::processBlock(juce::AudioBuffer<float>& buf) {
    int numCh = buf.getNumChannels();
    int numSamp = buf.getNumSamples();
    int bufSize = delayBuffer_[0].size();
    float delayMs = time_;
    
    for (int s = 0; s < numSamp; ++s) {
        wowPhase_ += 0.5f * 6.28318f / static_cast<float>(sampleRate_);
        if (wowPhase_ > 6.28318f) wowPhase_ -= 6.28318f;
        flutterPhase_ += 6.0f * 6.28318f / static_cast<float>(sampleRate_);
        if (flutterPhase_ > 6.28318f) flutterPhase_ -= 6.28318f;
        
        float modulation = (std::sin(wowPhase_) * wow_ * 15.0f) + (std::sin(flutterPhase_) * flutter_ * 5.0f);
        float actualDelay = (delayMs + modulation) / 1000.0f * static_cast<float>(sampleRate_);
        int delaySamples = static_cast<int>(actualDelay);
        
        for (int ch = 0; ch < numCh; ++ch) {
            float input = buf.getSample(ch, s);
            int readPos = (writePos_ - delaySamples + bufSize) % bufSize;
            float delayed = delayBuffer_[ch][readPos];
            lpFilter_[ch] += 0.3f * (delayed - lpFilter_[ch]);
            delayed = lpFilter_[ch];
            float saturated = std::tanh(delayed * (1.0f + saturation_ * 2.0f)) / (1.0f + saturation_);
            delayBuffer_[ch][writePos_] = input + saturated * feedback_;
            float output = input * (1.0f - mix_) + delayed * mix_;
            buf.setSample(ch, s, output);
        }
        writePos_ = (writePos_ + 1) % bufSize;
    }
}
