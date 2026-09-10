#include "VocoderEngine.h"

VocoderEngine::VocoderEngine() {
    for (auto& voice : voices_)
        voice.active = false;
}

void VocoderEngine::prepareToPlay(double sr, int sb) {
    sampleRate_ = sr;
    modulatorBuffer_.setSize(2, sb);
    carrierBuffer_.setSize(2, sb);
    initializeBands();
}

void VocoderEngine::initializeBands() {
    // Distribute bands logarithmically from 100Hz to 8kHz
    const float minFreq = 100.0f;
    const float maxFreq = 8000.0f;
    const float ratio = std::pow(maxFreq / minFreq, 1.0f / numBands_);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate_;
    spec.maximumBlockSize = 512;
    spec.numChannels = 1;

    for (int i = 0; i < numBands_; ++i) {
        float centerFreq = minFreq * std::pow(ratio, i);
        bands_[i].centerFreq = centerFreq;

        // Bandpass filter: Q = 5 for narrow bands
        auto coeffs = juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate_, centerFreq, 5.0f);
        bands_[i].modulatorFilter.prepare(spec);
        bands_[i].carrierFilter.prepare(spec);
        *bands_[i].modulatorFilter.coefficients = *coeffs;
        *bands_[i].carrierFilter.coefficients = *coeffs;
    }
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

    processVocoder(buffer);
}

void VocoderEngine::processVocoder(juce::AudioBuffer<float>& buffer) {
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Copy input as modulator
    modulatorBuffer_.makeCopyOf(buffer);

    // Generate carrier
    carrierBuffer_.clear();
    for (int sample = 0; sample < numSamples; ++sample) {
        float carrier = generateCarrier();
        for (int ch = 0; ch < numChannels; ++ch)
            carrierBuffer_.setSample(ch, sample, carrier);
    }

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
            float dry = modulatorBuffer_.getSample(ch, sample);
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
