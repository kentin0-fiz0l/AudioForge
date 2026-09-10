#include "TomEngine.h"
TomEngine::TomEngine() {}
float TomEngine::processSample() {
    if (!active_) return 0.0f;
    float bodyDecay = 1.0f / (decay_ * static_cast<float>(sampleRate_));
    bodyEnv_ *= (1.0f - bodyDecay);
    float pitchDecay = 1.0f / (0.05f * static_cast<float>(sampleRate_));
    pitchEnv_state *= (1.0f - pitchDecay);
    float noiseDecay = 1.0f / (0.02f * static_cast<float>(sampleRate_));
    noiseEnv_ *= (1.0f - noiseDecay);
    if (bodyEnv_ < 0.001f) { active_ = false; return 0.0f; }
    
    float currentPitch = pitch_ * (1.0f + pitchEnv_ * pitchEnv_state * bend_);
    float inc = currentPitch * 6.28318f / static_cast<float>(sampleRate_);
    phase_ += inc;
    if (phase_ > 6.28318f) phase_ -= 6.28318f;
    
    float sine = std::sin(phase_);
    float body = sine * bodyEnv_;
    float noise = generateNoise() * noiseEnv_ * noiseMix_;
    float sample = (body * (1.0f - noiseMix_ * 0.5f)) + noise;
    
    lpFilter_ += 0.5f * (sample - lpFilter_);
    sample = lpFilter_;
    hpFilter_ += 0.01f * (sample - hpFilter_);
    sample -= hpFilter_;
    
    return sample * velocity_ * 0.7f;
}
