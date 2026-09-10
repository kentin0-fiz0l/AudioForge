#include "HiHatEngine.h"

HiHatEngine::HiHatEngine() {}
void HiHatEngine::prepareToPlay(double sr, int) { sampleRate_ = sr; reset(); }
void HiHatEngine::reset() { envelope_ = 0.0f; active_ = false; for(int i=0; i<6; i++) bpFilter_[i]=0.0f; hpFilter_=0.0f; }
void HiHatEngine::trigger(float vel) { velocity_ = vel; active_ = true; envelope_ = 1.0f; }
void HiHatEngine::release() { /* one-shot */ }

float HiHatEngine::processSample()
{
    if (!active_) return 0.0f;
    float decayRate = 1.0f / (decay_ * (1.0f + openness_) * static_cast<float>(sampleRate_));
    envelope_ *= (1.0f - decayRate);
    if (envelope_ < 0.001f) { active_ = false; return 0.0f; }
    
    float noise = generateNoise();
    // 6-stage bandpass (metallic frequencies 4kHz-12kHz)
    float bp = noise;
    for (int i = 0; i < 6; i++) {
        float coeff = 0.1f + (tone_ * 0.15f);
        bpFilter_[i] += coeff * (bp - bpFilter_[i]);
        bp = (i < 5) ? (bpFilter_[i] - bpFilter_[i+1]) : bpFilter_[i];
    }
    float metallic = bp * (1.0f + metallic_ * 2.0f);
    hpFilter_ += 0.995f * (metallic - hpFilter_);
    float sample = (metallic - hpFilter_) * envelope_ * velocity_;
    return std::tanh(sample * 1.5f) * 0.6f;
}

float HiHatEngine::generateNoise() {
    noiseState_ = noiseState_ * 1103515245 + 12345;
    return ((int)(noiseState_ / 65536) % 32768) / 16384.0f - 1.0f;
}
