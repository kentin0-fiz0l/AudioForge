#include "DoublerEngine.h"

DoublerEngine::DoublerEngine()
    : randomGen_(std::random_device{}()),
      randomDist_(-1.0f, 1.0f) {
    voices_.resize(4); // Max 4 voices
}

void DoublerEngine::prepare(double sampleRate, int samplesPerBlock) {
    sampleRate_ = sampleRate;
    maxDelaySamples_ = static_cast<int>(sampleRate * kMaxDelayMs / 1000.0);

    // Initialize delay buffers for each voice
    for (auto& voice : voices_) {
        voice.delayBuffer.setSize(2, maxDelaySamples_ + samplesPerBlock);
        voice.delayBuffer.clear();
        voice.delayWritePos = 0;
        voice.lfoPhase = randomDist_(randomGen_) * juce::MathConstants<float>::pi;
        voice.lfoSpeed = 0.1f + randomDist_(randomGen_) * 0.05f;
    }

    // Initialize filters (4 voices max, stereo)
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 2;

    highPassFilters_.clear();
    lowPassFilters_.clear();

    for (int i = 0; i < 4; ++i) {
        highPassFilters_.emplace_back();
        lowPassFilters_.emplace_back();
        highPassFilters_[i].prepare(spec);
        lowPassFilters_[i].prepare(spec);
    }

    updateVoiceParameters();
}

void DoublerEngine::reset() {
    for (auto& voice : voices_) {
        voice.delayBuffer.clear();
        voice.delayWritePos = 0;
        voice.pitchReadPos = 0.0f;
    }

    for (auto& filter : highPassFilters_) {
        filter.reset();
    }
    for (auto& filter : lowPassFilters_) {
        filter.reset();
    }
}

void DoublerEngine::updateVoiceParameters() {
    // Distribute voices across stereo field
    for (int i = 0; i < voiceCount_; ++i) {
        auto& voice = voices_[i];

        // Stereo positioning (spread voices evenly)
        if (voiceCount_ == 1) {
            voice.panPosition = 0.0f; // Center
        } else {
            float normalizedPos = static_cast<float>(i) / (voiceCount_ - 1);
            voice.panPosition = (normalizedPos * 2.0f - 1.0f) * width_;
        }

        // Pitch variation (alternating + and -)
        float pitchSign = (i % 2 == 0) ? 1.0f : -1.0f;
        float variation = pitchVariation_ * (i + 1) / voiceCount_;
        voice.pitchShift = pitchSign * variation + randomDist_(randomGen_) * humanize_ * 0.01f;

        // Convert pitch shift (cents) to playback speed
        voice.pitchReadSpeed = std::pow(2.0f, voice.pitchShift / 1200.0f);

        // Timing offset
        float timingMs = timingShift_ * (i + 1) / voiceCount_;
        voice.timingOffset = static_cast<float>(sampleRate_ * timingMs / 1000.0);

        // Randomize LFO speed for humanization
        voice.lfoSpeed = 0.1f + randomDist_(randomGen_) * humanize_ * 0.001f;
    }

    // Update filter coefficients
    auto highPassCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(
        sampleRate_, highPassFreq_);
    auto lowPassCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(
        sampleRate_, lowPassFreq_);

    for (int i = 0; i < 4; ++i) {
        *highPassFilters_[i].state = *highPassCoeffs;
        *lowPassFilters_[i].state = *lowPassCoeffs;
    }
}

float DoublerEngine::readDelayedSample(Voice& voice, int channel, float delaySamples) {
    const int bufferSize = voice.delayBuffer.getNumSamples();
    const float* bufferData = voice.delayBuffer.getReadPointer(channel);

    // Calculate read position with fractional delay
    float readPos = voice.delayWritePos - delaySamples;
    while (readPos < 0.0f) readPos += bufferSize;
    while (readPos >= bufferSize) readPos -= bufferSize;

    // Linear interpolation for fractional delay
    int readPos1 = static_cast<int>(readPos);
    int readPos2 = (readPos1 + 1) % bufferSize;
    float frac = readPos - readPos1;

    return bufferData[readPos1] * (1.0f - frac) + bufferData[readPos2] * frac;
}

void DoublerEngine::process(juce::AudioBuffer<float>& buffer) {
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    if (numChannels == 0 || numSamples == 0) return;

    // Create a copy of dry signal
    juce::AudioBuffer<float> dryBuffer(numChannels, numSamples);
    for (int ch = 0; ch < numChannels; ++ch) {
        dryBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);
    }

    // Clear buffer for wet signal accumulation
    buffer.clear();

    // Process each active voice
    for (int v = 0; v < voiceCount_; ++v) {
        auto& voice = voices_[v];

        juce::AudioBuffer<float> voiceBuffer(numChannels, numSamples);

        for (int sample = 0; sample < numSamples; ++sample) {
            // Write input to delay buffer
            for (int ch = 0; ch < numChannels; ++ch) {
                voice.delayBuffer.setSample(ch, voice.delayWritePos,
                                           dryBuffer.getSample(ch, sample));
            }

            // Update LFO for humanization
            voice.lfoPhase += voice.lfoSpeed * 2.0f * juce::MathConstants<float>::pi / sampleRate_;
            if (voice.lfoPhase > juce::MathConstants<float>::twoPi) {
                voice.lfoPhase -= juce::MathConstants<float>::twoPi;
            }

            float lfoValue = std::sin(voice.lfoPhase) * humanize_ * 0.2f;

            // Calculate delay with humanization
            float totalDelay = voice.timingOffset + lfoValue;

            // Read delayed samples with pitch shifting (variable speed)
            voice.pitchReadPos += voice.pitchReadSpeed;
            float pitchDelay = totalDelay + voice.pitchReadPos;

            for (int ch = 0; ch < numChannels; ++ch) {
                float delayedSample = readDelayedSample(voice, ch, pitchDelay);
                voiceBuffer.setSample(ch, sample, delayedSample);
            }

            // Advance write position
            voice.delayWritePos = (voice.delayWritePos + 1) % voice.delayBuffer.getNumSamples();
        }

        // Apply filters to voice
        juce::dsp::AudioBlock<float> block(voiceBuffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        highPassFilters_[v].process(context);
        lowPassFilters_[v].process(context);

        // Apply stereo panning and mix into output
        for (int sample = 0; sample < numSamples; ++sample) {
            // Calculate pan gains (constant power panning)
            float panAngle = (voice.panPosition + 1.0f) * 0.25f * juce::MathConstants<float>::pi;
            float leftGain = std::cos(panAngle);
            float rightGain = std::sin(panAngle);

            float monoSample = 0.0f;
            for (int ch = 0; ch < numChannels; ++ch) {
                monoSample += voiceBuffer.getSample(ch, sample);
            }
            monoSample /= numChannels;

            // Stereo output
            if (numChannels >= 2) {
                buffer.addSample(0, sample, monoSample * leftGain);
                buffer.addSample(1, sample, monoSample * rightGain);
            } else {
                buffer.addSample(0, sample, monoSample);
            }
        }
    }

    // Apply output gain
    buffer.applyGain(outputGain_);

    // Mix wet and dry
    for (int ch = 0; ch < numChannels; ++ch) {
        for (int sample = 0; sample < numSamples; ++sample) {
            float wet = buffer.getSample(ch, sample);
            float dry = dryBuffer.getSample(ch, sample);
            buffer.setSample(ch, sample, dry * (1.0f - mix_) + wet * mix_);
        }
    }
}

// Parameter setters
void DoublerEngine::setVoiceCount(int count) {
    voiceCount_ = juce::jlimit(1, 4, count);
    updateVoiceParameters();
}

void DoublerEngine::setWidth(float widthPercent) {
    width_ = juce::jlimit(0.0f, 100.0f, widthPercent) / 100.0f;
    updateVoiceParameters();
}

void DoublerEngine::setPitchVariation(float cents) {
    pitchVariation_ = juce::jlimit(0.0f, 20.0f, cents);
    updateVoiceParameters();
}

void DoublerEngine::setTimingShift(float milliseconds) {
    timingShift_ = juce::jlimit(0.0f, 30.0f, milliseconds);
    updateVoiceParameters();
}

void DoublerEngine::setHumanize(float amount) {
    humanize_ = juce::jlimit(0.0f, 100.0f, amount);
    updateVoiceParameters();
}

void DoublerEngine::setHighPass(float freqHz) {
    highPassFreq_ = juce::jlimit(20.0f, 500.0f, freqHz);
    updateVoiceParameters();
}

void DoublerEngine::setLowPass(float freqHz) {
    lowPassFreq_ = juce::jlimit(2000.0f, 20000.0f, freqHz);
    updateVoiceParameters();
}

void DoublerEngine::setMix(float mix) {
    mix_ = juce::jlimit(0.0f, 1.0f, mix);
}

void DoublerEngine::setOutputGain(float gainDb) {
    outputGain_ = juce::Decibels::decibelsToGain(juce::jlimit(-12.0f, 12.0f, gainDb));
}
