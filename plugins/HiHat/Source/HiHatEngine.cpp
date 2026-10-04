#include "HiHatEngine.h"

HiHatEngine::HiHatEngine() { updateFilters(); }
void HiHatEngine::prepareToPlay(double sr, int) { sampleRate_ = sr; updateFilters(); reset(); }
void HiHatEngine::reset() { envelope_ = 0.0f; active_ = false; for(int i=0; i<6; i++) bpFilter_[i]=0.0f; }
void HiHatEngine::trigger(float vel) { velocity_ = vel; active_ = true; envelope_ = 1.0f; }
void HiHatEngine::release() { /* one-shot */ }

float HiHatEngine::processSample()
{
    if (!active_) return 0.0f;
    float decayRate = 1.0f / (decay_ * (1.0f + openness_) * static_cast<float>(sampleRate_));
    envelope_ *= (1.0f - decayRate);
    if (envelope_ < 0.001f) { active_ = false; return 0.0f; }
    
    float noise = generateNoise();
    // 6-stage bandpass (metallic frequencies 4kHz-12kHz): three high-pass
    // stages, each subtracting a low-passed copy, then three low-pass stages
    float bp = noise;
    for (int i = 0; i < 3; i++) {
        bpFilter_[i] += highPassCoeff_ * (bp - bpFilter_[i]);
        bp -= bpFilter_[i];
    }
    for (int i = 3; i < 6; i++) {
        bpFilter_[i] += lowPassCoeff_ * (bp - bpFilter_[i]);
        bp = bpFilter_[i];
    }
    // The band keeps only a fraction of the noise's energy, so make up the level
    float metallic = bp * 2.5f * (1.0f + metallic_ * 2.0f);
    float sample = metallic * envelope_ * velocity_;
    return std::tanh(sample * 1.5f) * 0.6f;
}

void HiHatEngine::updateFilters()
{
    // One-pole coefficient for a corner frequency, so the band stays put
    // when the sample rate changes
    auto coefficient = [this](float hz) {
        return 1.0f - std::exp(-juce::MathConstants<float>::twoPi * hz / static_cast<float>(sampleRate_));
    };

    // Tone raises the bottom of the band from 4 kHz to 8 kHz
    highPassCoeff_ = coefficient(4000.0f + tone_ * 4000.0f);
    lowPassCoeff_ = coefficient(12000.0f);
}

float HiHatEngine::generateNoise() {
    noiseState_ = noiseState_ * 1103515245 + 12345;
    return ((int)(noiseState_ / 65536) % 32768) / 16384.0f - 1.0f;
}
