#include "HarmonizerEngine.h"

HarmonizerEngine::HarmonizerEngine() {
    voices_.resize(4); // Max 4 voices
}

void HarmonizerEngine::prepare(double sampleRate, int samplesPerBlock) {
    sampleRate_ = sampleRate;
    maxDelaySamples_ = static_cast<int>(sampleRate * kMaxDelayMs / 1000.0);

    // Initialize delay buffers
    for (auto& voice : voices_) {
        voice.delayBuffer.setSize(2, maxDelaySamples_ + samplesPerBlock);
        voice.delayBuffer.clear();
        voice.formantBuffer.setSize(2, samplesPerBlock);
        voice.formantBuffer.clear();
        voice.writePos = 0;
        voice.readPos = 0.0f;
    }

    // Initialize filters
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

    // Update filter coefficients
    auto highPassCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(
        sampleRate, highPassFreq_);
    auto lowPassCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(
        sampleRate, lowPassFreq_);

    for (int i = 0; i < 4; ++i) {
        *highPassFilters_[i].state = *highPassCoeffs;
        *lowPassFilters_[i].state = *lowPassCoeffs;
    }
}

void HarmonizerEngine::reset() {
    for (auto& voice : voices_) {
        voice.delayBuffer.clear();
        voice.formantBuffer.clear();
        voice.writePos = 0;
        voice.readPos = 0.0f;
    }

    for (auto& filter : highPassFilters_) {
        filter.reset();
    }
    for (auto& filter : lowPassFilters_) {
        filter.reset();
    }
}

const std::vector<int>& HarmonizerEngine::getScaleIntervals(Scale scale) {
    static const std::vector<int> chromatic = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    static const std::vector<int> major = {0, 2, 4, 5, 7, 9, 11};
    static const std::vector<int> minor = {0, 2, 3, 5, 7, 8, 10};
    static const std::vector<int> dorian = {0, 2, 3, 5, 7, 9, 10};
    static const std::vector<int> phrygian = {0, 1, 3, 5, 7, 8, 10};
    static const std::vector<int> lydian = {0, 2, 4, 6, 7, 9, 11};
    static const std::vector<int> mixolydian = {0, 2, 4, 5, 7, 9, 10};
    static const std::vector<int> aeolian = {0, 2, 3, 5, 7, 8, 10};
    static const std::vector<int> locrian = {0, 1, 3, 5, 6, 8, 10};
    static const std::vector<int> harmonicMinor = {0, 2, 3, 5, 7, 8, 11};
    static const std::vector<int> melodicMinor = {0, 2, 3, 5, 7, 9, 11};
    static const std::vector<int> pentatonic = {0, 2, 4, 7, 9};

    switch (scale) {
        case Scale::Major: return major;
        case Scale::Minor: return minor;
        case Scale::Dorian: return dorian;
        case Scale::Phrygian: return phrygian;
        case Scale::Lydian: return lydian;
        case Scale::Mixolydian: return mixolydian;
        case Scale::Aeolian: return aeolian;
        case Scale::Locrian: return locrian;
        case Scale::HarmonicMinor: return harmonicMinor;
        case Scale::MelodicMinor: return melodicMinor;
        case Scale::Pentatonic: return pentatonic;
        case Scale::Chromatic:
        default: return chromatic;
    }
}

float HarmonizerEngine::quantizePitchToScale(float semitones) {
    if (currentScale_ == Scale::Chromatic) {
        return semitones; // No quantization
    }

    const auto& intervals = getScaleIntervals(currentScale_);

    // Find nearest scale interval
    int octave = static_cast<int>(std::floor(semitones / 12.0f));
    float semitoneInOctave = semitones - (octave * 12.0f);

    // Find closest scale degree
    float minDist = 12.0f;
    int closestInterval = 0;

    for (int interval : intervals) {
        float dist = std::abs(semitoneInOctave - interval);
        if (dist < minDist) {
            minDist = dist;
            closestInterval = interval;
        }
    }

    return octave * 12.0f + closestInterval;
}

float HarmonizerEngine::readInterpolated(const Voice& voice, int channel, float offset) {
    const int bufferSize = voice.delayBuffer.getNumSamples();
    const float* bufferData = voice.delayBuffer.getReadPointer(channel);

    float readPos = voice.writePos - offset;
    while (readPos < 0.0f) readPos += bufferSize;
    while (readPos >= bufferSize) readPos -= bufferSize;

    // Linear interpolation
    int pos1 = static_cast<int>(readPos);
    int pos2 = (pos1 + 1) % bufferSize;
    float frac = readPos - pos1;

    return bufferData[pos1] * (1.0f - frac) + bufferData[pos2] * frac;
}

void HarmonizerEngine::process(juce::AudioBuffer<float>& buffer) {
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    if (numChannels == 0 || numSamples == 0) return;

    // Save dry signal
    juce::AudioBuffer<float> dryBuffer(numChannels, numSamples);
    for (int ch = 0; ch < numChannels; ++ch) {
        dryBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);
    }

    // Clear buffer for wet accumulation
    buffer.clear();

    // Process each active voice
    for (int v = 0; v < voiceCount_; ++v) {
        auto& voice = voices_[v];

        // Apply scale quantization to pitch
        float quantizedPitch = quantizePitchToScale(voice.pitchShift);

        // Calculate read speed from pitch shift
        // Positive pitch = faster playback, negative = slower
        voice.readSpeed = std::pow(2.0f, quantizedPitch / 12.0f);

        juce::AudioBuffer<float> voiceBuffer(numChannels, numSamples);

        // Process each sample
        for (int sample = 0; sample < numSamples; ++sample) {
            // Write input to delay buffer
            for (int ch = 0; ch < numChannels; ++ch) {
                voice.delayBuffer.setSample(ch, voice.writePos,
                                           dryBuffer.getSample(ch, sample));
            }

            // Read with pitch shift
            voice.readPos += voice.readSpeed;

            // Keep readPos in valid range
            if (voice.readPos >= maxDelaySamples_) {
                voice.readPos -= maxDelaySamples_;
            }

            // Read interpolated samples
            for (int ch = 0; ch < numChannels; ++ch) {
                float shifted = readInterpolated(voice, ch, voice.readPos);
                voiceBuffer.setSample(ch, sample, shifted);
            }

            // Advance write position
            voice.writePos = (voice.writePos + 1) % voice.delayBuffer.getNumSamples();
        }

        // Apply filters
        juce::dsp::AudioBlock<float> block(voiceBuffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        highPassFilters_[v].process(context);
        lowPassFilters_[v].process(context);

        // Apply stereo panning and add to output
        for (int sample = 0; sample < numSamples; ++sample) {
            // Constant power panning
            float panAngle = (voice.panPosition + 1.0f) * 0.25f * juce::MathConstants<float>::pi;
            float leftGain = std::cos(panAngle);
            float rightGain = std::sin(panAngle);

            // Get mono sample
            float monoSample = 0.0f;
            for (int ch = 0; ch < numChannels; ++ch) {
                monoSample += voiceBuffer.getSample(ch, sample);
            }
            monoSample /= numChannels;

            // Apply panning
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
void HarmonizerEngine::setVoiceCount(int count) {
    voiceCount_ = juce::jlimit(1, 4, count);
}

void HarmonizerEngine::setVoicePitch(int voiceIndex, float semitones) {
    if (voiceIndex >= 0 && voiceIndex < 4) {
        voices_[voiceIndex].pitchShift = juce::jlimit(-24.0f, 24.0f, semitones);
    }
}

void HarmonizerEngine::setVoicePan(int voiceIndex, float pan) {
    if (voiceIndex >= 0 && voiceIndex < 4) {
        voices_[voiceIndex].panPosition = juce::jlimit(-1.0f, 1.0f, pan / 100.0f);
    }
}

void HarmonizerEngine::setScale(Scale scale) {
    currentScale_ = scale;
}

void HarmonizerEngine::setFormantShift(float semitones) {
    formantShift_ = juce::jlimit(-12.0f, 12.0f, semitones);
    // Note: Full formant preservation is complex and would require
    // spectral processing. This is a placeholder for future enhancement.
}

void HarmonizerEngine::setHighPass(float freqHz) {
    highPassFreq_ = juce::jlimit(20.0f, 500.0f, freqHz);

    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(
        sampleRate_, highPassFreq_);

    for (int i = 0; i < 4; ++i) {
        *highPassFilters_[i].state = *coeffs;
    }
}

void HarmonizerEngine::setLowPass(float freqHz) {
    lowPassFreq_ = juce::jlimit(2000.0f, 20000.0f, freqHz);

    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(
        sampleRate_, lowPassFreq_);

    for (int i = 0; i < 4; ++i) {
        *lowPassFilters_[i].state = *coeffs;
    }
}

void HarmonizerEngine::setMix(float mix) {
    mix_ = juce::jlimit(0.0f, 1.0f, mix);
}

void HarmonizerEngine::setOutputGain(float gainDb) {
    outputGain_ = juce::Decibels::decibelsToGain(juce::jlimit(-12.0f, 12.0f, gainDb));
}
