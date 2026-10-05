#include "ShimmerEngine.h"

void ShimmerEngine::prepareToPlay(double sr, int sb) {
    sampleRate_ = sr;
    juce::dsp::ProcessSpec spec{sr, static_cast<juce::uint32>(sb), 2};
    reverb_.prepare(spec);
    int bufSize = static_cast<int>(sr * 0.5); // 500ms for pitch shifting
    for (int ch = 0; ch < 2; ++ch) {
        pitchBuffer_[ch].resize(bufSize, 0.0f);
    }
    pitchWritePos_ = 0;
    pitchPhase_ = 0.0f;
    dryBuffer_.setSize(2, sb);
}

void ShimmerEngine::processBlock(juce::AudioBuffer<float>& buf) {
    int numCh = juce::jmin(buf.getNumChannels(), 2); // The reverb and the pitch buffers are stereo
    int numSamp = buf.getNumSamples();
    int bufSize = pitchBuffer_[0].size();
    
    // Keep a dry copy. The buffer was sized in prepareToPlay, so this does
    // not allocate unless the host sends a larger block than it promised.
    dryBuffer_.setSize(numCh, numSamp, false, false, true);
    for (int ch = 0; ch < numCh; ++ch)
        dryBuffer_.copyFrom(ch, 0, buf, ch, 0, numSamp);
    
    // Process reverb
    juce::dsp::Reverb::Parameters params;
    params.roomSize = size_;
    params.damping = damping_;
    params.width = 1.0f;
    // The reverb alone, at its nominal level; the dry signal is mixed in below
    const auto levels = AudioForge::DSP::reverbLevelsForMix(1.0f);
    params.wetLevel = levels.wet;
    params.dryLevel = levels.dry;
    reverb_.setParameters(params);
    
    juce::dsp::AudioBlock<float> block(buf);
    juce::dsp::ProcessContextReplacing<float> context(block);
    reverb_.process(context);
    
    // Add pitch-shifted shimmer
    if (shimmer_ > 0.01f) {
        for (int s = 0; s < numSamp; ++s) {
            for (int ch = 0; ch < numCh; ++ch) {
                float wet = buf.getSample(ch, s);
                pitchBuffer_[ch][pitchWritePos_] = wet;
                
                // Pitch shift (simple granular approach)
                float pitchShift = 0.0f;
                if (pitchMode_ == 0 || pitchMode_ == 2) { // octave up
                    int readPos = (pitchWritePos_ - static_cast<int>(bufSize * 0.5f) + bufSize) % bufSize;
                    pitchShift += pitchBuffer_[ch][readPos] * 0.5f;
                }
                if (pitchMode_ == 1 || pitchMode_ == 2) { // octave down
                    int readPos = (pitchWritePos_ - static_cast<int>(bufSize * 0.25f) + bufSize) % bufSize;
                    pitchShift += pitchBuffer_[ch][readPos] * 0.5f;
                }
                
                float shimmerSample = wet + pitchShift * shimmer_;
                buf.setSample(ch, s, shimmerSample * feedback_ + wet * (1.0f - feedback_ * 0.5f));
            }
            pitchWritePos_ = (pitchWritePos_ + 1) % bufSize;
        }
    }
    
    // Mix dry/wet
    for (int ch = 0; ch < numCh; ++ch) {
        for (int s = 0; s < numSamp; ++s) {
            float dry = dryBuffer_.getSample(ch, s);
            float wet = buf.getSample(ch, s);
            buf.setSample(ch, s, dry * (1.0f - mix_) + wet * mix_);
        }
    }
}
