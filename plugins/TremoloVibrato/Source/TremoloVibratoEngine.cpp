#include "TremoloVibratoEngine.h"

TremoloVibratoEngine::TremoloVibratoEngine() {}

void TremoloVibratoEngine::prepareToPlay(double sr, int sb) {
    sampleRate_ = sr;
    delayBuffer_.setSize(2, MAX_DELAY_SAMPLES);
    delayBuffer_.clear();
    delayWritePos_ = 0;
    lfoPhase_ = 0.0f;
}

void TremoloVibratoEngine::processBlock(juce::AudioBuffer<float>& buf) {
    if (mode_ == Mode::Tremolo)
        processTremolo(buf);
    else
        processVibrato(buf);
}

float TremoloVibratoEngine::getLFOValue() {
    float lfo = 0.0f;
    if (waveform_ == 0) { // Sine
        lfo = std::sin(lfoPhase_);
    } else if (waveform_ == 1) { // Triangle
        lfo = (2.0f / juce::MathConstants<float>::pi) * std::asin(std::sin(lfoPhase_));
    } else { // Square
        lfo = std::sin(lfoPhase_) >= 0.0f ? 1.0f : -1.0f;
    }
    lfoPhase_ += (2.0f * juce::MathConstants<float>::pi * rate_) / static_cast<float>(sampleRate_);
    if (lfoPhase_ >= 2.0f * juce::MathConstants<float>::pi)
        lfoPhase_ -= 2.0f * juce::MathConstants<float>::pi;
    return lfo;
}

void TremoloVibratoEngine::processTremolo(juce::AudioBuffer<float>& buf) {
    const int numSamples = buf.getNumSamples();
    const int numChannels = buf.getNumChannels();

    for (int sample = 0; sample < numSamples; ++sample) {
        float lfo = getLFOValue();
        // Map LFO to amplitude (0.0 to 1.0 range for tremolo)
        float amplitude = 1.0f - (depth_ * 0.5f * (1.0f - lfo));

        for (int ch = 0; ch < numChannels; ++ch) {
            float input = buf.getSample(ch, sample);
            float output = input * amplitude;
            buf.setSample(ch, sample, input * (1.0f - mix_) + output * mix_);
        }
    }
}

void TremoloVibratoEngine::processVibrato(juce::AudioBuffer<float>& buf) {
    const int numSamples = buf.getNumSamples();
    const int numChannels = buf.getNumChannels();

    for (int sample = 0; sample < numSamples; ++sample) {
        float lfo = getLFOValue();
        // Map LFO to delay time for pitch modulation (vibrato)
        // Typical vibrato depth: ±0.5 semitones (about ±3% pitch deviation)
        float delayMs = 10.0f + (depth_ * 3.0f * lfo); // 7-13ms delay variation
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

            // Mix
            buf.setSample(ch, sample, input * (1.0f - mix_) + delayed * mix_);
        }

        delayWritePos_ = (delayWritePos_ + 1) % MAX_DELAY_SAMPLES;
    }
}
