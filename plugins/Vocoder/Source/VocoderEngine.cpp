#include "VocoderEngine.h"

VocoderEngine::VocoderEngine() {
    for (auto& voice : voices_)
        voice.active = false;
}

void VocoderEngine::prepareToPlay(double sr, int sb) {
    sampleRate_ = sr;
    dryBuffer_.setSize(2, sb);
    modulatorBuffer_.setSize(1, sb);
    carrierBuffer_.setSize(1, sb);

    // Coefficients first: prepare() sizes each filter's state for its order
    updateBandFilters();

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate_;
    spec.maximumBlockSize = static_cast<juce::uint32>(sb);
    spec.numChannels = 1;

    for (auto& band : bands_) {
        band.modulatorFilter.prepare(spec);
        band.carrierFilter.prepare(spec);
        band.envelopeLevel = 0.0f;
    }
}

void VocoderEngine::setNumBands(int bands) {
    bands = juce::jlimit(8, 32, bands);
    if (bands != numBands_) {
        numBands_ = bands;
        bandsChanged_ = true;
    }
}

void VocoderEngine::setFormantShift(float shift) {
    shift = juce::jlimit(0.5f, 2.0f, shift);
    if (shift != formantShift_) {
        formantShift_ = shift;
        bandsChanged_ = true;
    }
}

void VocoderEngine::updateBandFilters() {
    // Distribute bands logarithmically from 100Hz to 8kHz. Every band gets
    // band-pass coefficients, including those above the current count, so all
    // filters stay second order and a later change never resizes their state.
    const float minFreq = 100.0f;
    const float maxFreq = 8000.0f;
    const float ratio = std::pow(maxFreq / minFreq, 1.0f / numBands_);
    const float maxCenter = 0.45f * static_cast<float>(sampleRate_);

    for (int i = 0; i < MAX_BANDS; ++i) {
        auto& band = bands_[i];
        band.centerFreq = juce::jmin(minFreq * std::pow(ratio, static_cast<float>(i)), maxCenter);

        // Formant Shift moves the carrier band relative to the modulator band
        const float carrierFreq = juce::jmin(band.centerFreq * formantShift_, maxCenter);

        // Bandpass filter: Q = 5 for narrow bands. Assigning ArrayCoefficients
        // reuses the existing coefficient storage, so this is safe on the audio thread.
        using Coeffs = juce::dsp::IIR::ArrayCoefficients<float>;
        *band.modulatorFilter.coefficients = Coeffs::makeBandPass(sampleRate_, band.centerFreq, 5.0f);
        *band.carrierFilter.coefficients = Coeffs::makeBandPass(sampleRate_, carrierFreq, 5.0f);
    }

    bandsChanged_ = false;
}

void VocoderEngine::noteOn(int note, float vel) {
    for (auto& voice : voices_) {
        if (!voice.active) {
            voice.midiNote = note;
            voice.frequency = 440.0f * std::pow(2.0f, (note - 69) / 12.0f);
            voice.velocity = vel;
            voice.phase = 0.0f;
            voice.active = true;
            return;
        }
    }
}

void VocoderEngine::noteOff(int note) {
    for (auto& voice : voices_) {
        if (voice.active && voice.midiNote == note) {
            voice.active = false;
        }
    }
}

float VocoderEngine::generateCarrier() {
    float output = 0.0f;
    int activeCount = 0;

    for (auto& voice : voices_) {
        if (voice.active) {
            // Sawtooth wave
            float saw = (voice.phase / juce::MathConstants<float>::pi) - 1.0f;
            output += saw * voice.velocity;
            
            voice.phase += (2.0f * juce::MathConstants<float>::pi * voice.frequency) / static_cast<float>(sampleRate_);
            if (voice.phase >= 2.0f * juce::MathConstants<float>::pi)
                voice.phase -= 2.0f * juce::MathConstants<float>::pi;
            
            ++activeCount;
        }
    }

    return activeCount > 0 ? (output / activeCount) * carrierLevel_ : 0.0f;
}

void VocoderEngine::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {
    // Process MIDI events
    for (const auto metadata : midi) {
        auto message = metadata.getMessage();
        if (message.isNoteOn())
            noteOn(message.getNoteNumber(), message.getVelocity() / 127.0f);
        else if (message.isNoteOff())
            noteOff(message.getNoteNumber());
    }

    if (bandsChanged_)
        updateBandFilters();

    processVocoder(buffer);
}

void VocoderEngine::processVocoder(juce::AudioBuffer<float>& buffer) {
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    if (numChannels == 0)
        return;

    // Keep the input for the dry mix, and use the mono sum of all its channels as the modulator
    dryBuffer_.makeCopyOf(buffer, true);
    modulatorBuffer_.copyFrom(0, 0, buffer, 0, 0, numSamples);
    for (int ch = 1; ch < numChannels; ++ch)
        modulatorBuffer_.addFrom(0, 0, buffer, ch, 0, numSamples);
    modulatorBuffer_.applyGain(0, 0, numSamples, 1.0f / static_cast<float>(numChannels));

    // Generate carrier
    for (int sample = 0; sample < numSamples; ++sample)
        carrierBuffer_.setSample(0, sample, generateCarrier());

    // Clear output
    buffer.clear();

    // Process each band
    float attackCoeff = 1.0f - std::exp(-1.0f / (attack_ * static_cast<float>(sampleRate_)));
    float releaseCoeff = 1.0f - std::exp(-1.0f / (release_ * static_cast<float>(sampleRate_)));

    for (int band = 0; band < numBands_; ++band) {
        for (int sample = 0; sample < numSamples; ++sample) {
            // Get samples
            float modSample = modulatorBuffer_.getSample(0, sample);
            float carSample = carrierBuffer_.getSample(0, sample);

            // Filter using processSample method
            modSample = bands_[band].modulatorFilter.processSample(modSample);
            carSample = bands_[band].carrierFilter.processSample(carSample);

            // Envelope follow the modulator
            float modEnv = std::abs(modSample);
            if (modEnv > bands_[band].envelopeLevel)
                bands_[band].envelopeLevel += (modEnv - bands_[band].envelopeLevel) * attackCoeff;
            else
                bands_[band].envelopeLevel += (modEnv - bands_[band].envelopeLevel) * releaseCoeff;

            // Apply envelope to carrier
            float vocodedSample = carSample * bands_[band].envelopeLevel * modulatorLevel_;

            // Sum to output
            for (int ch = 0; ch < numChannels; ++ch) {
                float current = buffer.getSample(ch, sample);
                buffer.setSample(ch, sample, current + vocodedSample);
            }
        }
    }

    // Apply mix
    for (int ch = 0; ch < numChannels; ++ch) {
        for (int sample = 0; sample < numSamples; ++sample) {
            float dry = dryBuffer_.getSample(ch, sample);
            float wet = buffer.getSample(ch, sample);
            buffer.setSample(ch, sample, dry * (1.0f - mix_) + wet * mix_);
        }
    }
}

void VocoderEngine::renderNextBlock(juce::AudioBuffer<float>& buffer, int start, int num) {
    // Unused - processBlock handles everything
}

float VocoderEngine::getBandLevel(int band) const {
    if (band >= 0 && band < numBands_)
        return bands_[band].envelopeLevel;
    return 0.0f;
}
