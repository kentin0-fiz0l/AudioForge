#include "ModulationEngine.h"

ModulationEngine::ModulationEngine() {}

void ModulationEngine::prepareToPlay(double sr, int sb) {
    sampleRate_ = sr;
    delayBuffer_.setSize(2, MAX_DELAY_SAMPLES);
    delayBuffer_.clear();
    delayWritePos_ = 0;
    lfoPhase_ = 0.0f;
    for (int ch = 0; ch < 2; ++ch)
        for (int i = 0; i < NUM_STAGES; ++i)
            apFilters_[ch][i].state = 0.0f;
    phaserFeedbackState_[0] = phaserFeedbackState_[1] = 0.0f;
}

void ModulationEngine::processBlock(juce::AudioBuffer<float>& buf) {
    if (mode_ == Mode::Phaser)
        processPhaser(buf);
    else
        processFlanger(buf);
}

float ModulationEngine::getLFOValue() {
    float lfo = std::sin(lfoPhase_);
    lfoPhase_ += (2.0f * juce::MathConstants<float>::pi * rate_) / static_cast<float>(sampleRate_);
    if (lfoPhase_ >= 2.0f * juce::MathConstants<float>::pi)
        lfoPhase_ -= 2.0f * juce::MathConstants<float>::pi;
    return lfo;
}

void ModulationEngine::processPhaser(juce::AudioBuffer<float>& buf) {
    const int numSamples = buf.getNumSamples();
    const int numChannels = juce::jmin(buf.getNumChannels(), 2);
    const float sr = static_cast<float>(sampleRate_);
    const float wetGain = feedbackWetGain();

    for (int sample = 0; sample < numSamples; ++sample) {
        // Sweep the break frequency up to two octaves either side of the centre
        float lfo = getLFOValue();
        float breakFreq = centreFreq_ * std::exp2(2.0f * depth_ * lfo);
        breakFreq = juce::jlimit(20.0f, 0.45f * sr, breakFreq);
        float t = std::tan(juce::MathConstants<float>::pi * breakFreq / sr);
        float a = (t - 1.0f) / (t + 1.0f);

        for (int ch = 0; ch < numChannels; ++ch) {
            float input = buf.getSample(ch, sample);

            // Feedback: the last stage's previous output goes back into the first
            float output = input + feedback_ * phaserFeedbackState_[ch];
            for (int stage = 0; stage < NUM_STAGES; ++stage)
                output = apFilters_[ch][stage].processSample(output, a);
            phaserFeedbackState_[ch] = output;

            buf.setSample(ch, sample, input * (1.0f - mix_) + output * wetGain * mix_);
        }
    }
}

// Gain applied to the wet signal to keep feedback's resonance in check.
// Around the loop the all-pass stages have unity gain, so at the frequencies
// where the feedback comes back in phase the wet path peaks at 1 / (1 - |fb|):
// +6 dB at 0.5, +20 dB at 0.9. Without compensation, turning Feedback up makes
// the effect much louder. The square root meets it halfway: the peaks rise by
// half as many decibels (+10 dB at 0.9) and broadband level holds up better
// than a full 1 - |fb| would leave it.
float ModulationEngine::feedbackWetGain() const {
    return std::sqrt(1.0f - std::abs(feedback_));
}

void ModulationEngine::processFlanger(juce::AudioBuffer<float>& buf) {
    const int numSamples = buf.getNumSamples();
    const int numChannels = buf.getNumChannels();

    for (int sample = 0; sample < numSamples; ++sample) {
        float lfo = getLFOValue();
        // Map LFO to delay time (1-15ms typical flanger range)
        float delayMs = 5.0f + (depth_ * 10.0f * (0.5f + 0.5f * lfo));
        float delaySamples = (delayMs / 1000.0f) * static_cast<float>(sampleRate_);

        for (int ch = 0; ch < numChannels; ++ch) {
            float input = buf.getSample(ch, sample);

            // Write to delay buffer
            delayBuffer_.setSample(ch, delayWritePos_, input);

            // Read from delay buffer with interpolation
            float readPos = delayWritePos_ - delaySamples;
            while (readPos < 0.0f) readPos += MAX_DELAY_SAMPLES;

            int readIdx = static_cast<int>(readPos);
            float frac = readPos - readIdx;
            int readIdx2 = (readIdx + 1) % MAX_DELAY_SAMPLES;

            float delayed = delayBuffer_.getSample(ch, readIdx % MAX_DELAY_SAMPLES) * (1.0f - frac) +
                          delayBuffer_.getSample(ch, readIdx2) * frac;

            // Feedback
            delayBuffer_.setSample(ch, delayWritePos_,
                delayBuffer_.getSample(ch, delayWritePos_) + delayed * feedback_);

            // Mix
            buf.setSample(ch, sample, input * (1.0f - mix_) + delayed * mix_);
        }

        delayWritePos_ = (delayWritePos_ + 1) % MAX_DELAY_SAMPLES;
    }
}
