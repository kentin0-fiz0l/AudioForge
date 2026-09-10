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
            apFilters_[ch][i].z1 = 0.0f;
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
    const int numChannels = buf.getNumChannels();

    for (int sample = 0; sample < numSamples; ++sample) {
        float lfo = getLFOValue();
        // Map LFO to all-pass coefficient (affects phase shift frequency)
        float normalizedFreq = (centreFreq_ / static_cast<float>(sampleRate_)) * (1.0f + depth_ * lfo);
        float coeff = (1.0f - normalizedFreq) / (1.0f + normalizedFreq);
        coeff = juce::jlimit(-0.95f, 0.95f, coeff);

        for (int ch = 0; ch < numChannels; ++ch) {
            float input = buf.getSample(ch, sample);
            float output = input;

            // Cascade all-pass filters
            for (int stage = 0; stage < NUM_STAGES; ++stage)
                output = apFilters_[ch][stage].processSample(output, coeff);

            // Feedback
            output = output + (input * feedback_);

            // Mix
            buf.setSample(ch, sample, input * (1.0f - mix_) + output * mix_);
        }
    }
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
