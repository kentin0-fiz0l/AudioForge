#include "ShimmerEngine.h"

namespace {
constexpr float grainSeconds = 0.08f;       // Length of one pitch-shifter grain
constexpr double feedbackLowCutHz = 80.0;   // Stops octave-down feedback piling up below the audio band
}

void ShimmerEngine::prepareToPlay(double sr, int) {
    reverb_.setSampleRate(sr);
    reverb_.reset();
    windowLength_ = static_cast<float>(sr) * grainSeconds;
    // The longest delay is one grain; two extra samples for the interpolation
    for (auto& buffer : pitchBuffer_)
        buffer.assign(static_cast<size_t>(windowLength_) + 2, 0.0f);
    pitchWritePos_ = 0;
    pitchPhase_[0] = pitchPhase_[1] = 0.0f;
    auto highPass = juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(sr, feedbackLowCutHz);
    for (int ch = 0; ch < 2; ++ch) {
        feedbackSample_[ch] = 0.0f;
        feedbackHighPass_[ch].coefficients = highPass;
        feedbackHighPass_[ch].reset();
    }
}

// Reads the delay line `delay` samples behind the write position, with linear interpolation
float ShimmerEngine::readDelayed(int ch, float delay) const {
    const auto& buffer = pitchBuffer_[ch];
    const int size = static_cast<int>(buffer.size());
    float pos = static_cast<float>(pitchWritePos_) - delay;
    if (pos < 0.0f)
        pos += static_cast<float>(size);
    const int i0 = static_cast<int>(pos);
    const float frac = pos - static_cast<float>(i0);
    return buffer[i0 % size] + frac * (buffer[(i0 + 1) % size] - buffer[i0 % size]);
}

// Octave shifter: two taps half a grain apart sweep through the delay line,
// faster than real time for octave up and slower for octave down. Each tap
// fades in and out with a Hann window, and the two windows sum to one.
float ShimmerEngine::shift(int ch, float wet) {
    pitchBuffer_[ch][pitchWritePos_] = wet;

    float shifted = 0.0f;
    for (int dir = 0; dir < 2; ++dir) { // 0 = octave up, 1 = octave down
        if (pitchMode_ != 2 && pitchMode_ != dir)
            continue;
        const float phaseA = pitchPhase_[dir];
        const float phaseB = phaseA < 0.5f ? phaseA + 0.5f : phaseA - 0.5f;
        // Octave up reads ever closer to the write position, octave down ever further behind it
        auto delayAt = [&](float phase) { return (dir == 0 ? 1.0f - phase : phase) * windowLength_; };
        const float sine = std::sin(juce::MathConstants<float>::pi * phaseA);
        const float windowA = sine * sine; // Tap B's window is the same, half a grain later: 1 - windowA
        shifted += readDelayed(ch, delayAt(phaseA)) * windowA + readDelayed(ch, delayAt(phaseB)) * (1.0f - windowA);
    }
    return pitchMode_ == 2 ? shifted * 0.5f : shifted;
}

void ShimmerEngine::processBlock(juce::AudioBuffer<float>& buf) {
    int numCh = juce::jmin(buf.getNumChannels(), 2); // The reverb and the pitch buffers are stereo
    int numSamp = buf.getNumSamples();
    if (numCh == 0)
        return;

    juce::Reverb::Parameters params;
    params.roomSize = size_;
    params.damping = damping_;
    params.width = 1.0f;
    // The reverb alone, at its nominal level; the dry signal is mixed in below
    const auto levels = AudioForge::DSP::reverbLevelsForMix(1.0f);
    params.wetLevel = levels.wet;
    params.dryLevel = levels.dry;
    reverb_.setParameters(params);

    // A bigger room makes the reverb louder: its combs feed back by
    // 0.7 + 0.28 * size, and for noise the level goes as 1 / sqrt(1 - g^2),
    // about 0.64 / sqrt(1 - g^2) on a signal that is the same in both channels.
    // Scaling by the inverse keeps each pass round the loop at or below
    // `feedback_`, so the shimmer builds but always dies away.
    const float combFeedback = 0.7f + 0.28f * size_;
    const float loopGain = feedback_ * std::sqrt(1.0f - combFeedback * combFeedback) / 0.64f;

    const int bufSize = static_cast<int>(pitchBuffer_[0].size());
    const float upStep = 1.0f / windowLength_;   // Octave up: the delay shrinks by a sample per sample
    const float downStep = 0.5f / windowLength_; // Octave down: it grows by half a sample per sample

    for (int s = 0; s < numSamp; ++s) {
        float dry[2], wet[2];
        for (int ch = 0; ch < 2; ++ch) {
            dry[ch] = buf.getSample(ch < numCh ? ch : 0, s);
            // The shimmer goes back into the reverb, so each pass shifts it another octave
            wet[ch] = dry[ch] + feedbackSample_[ch];
        }
        reverb_.processStereo(&wet[0], &wet[1], 1);

        for (int ch = 0; ch < 2; ++ch) {
            const float shimmer = shift(ch, wet[ch]) * shimmer_;
            // Soft-clipped so the loop can never run away
            feedbackSample_[ch] = std::tanh(feedbackHighPass_[ch].processSample(shimmer * loopGain));
            wet[ch] += shimmer;
        }
        pitchWritePos_ = (pitchWritePos_ + 1) % bufSize;
        pitchPhase_[0] += upStep;
        if (pitchPhase_[0] >= 1.0f) pitchPhase_[0] -= 1.0f;
        pitchPhase_[1] += downStep;
        if (pitchPhase_[1] >= 1.0f) pitchPhase_[1] -= 1.0f;

        // Mix dry/wet
        for (int ch = 0; ch < numCh; ++ch)
            buf.setSample(ch, s, dry[ch] * (1.0f - mix_) + wet[ch] * mix_);
    }
}
