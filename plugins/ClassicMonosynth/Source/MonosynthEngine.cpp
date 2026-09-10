#include "MonosynthEngine.h"

MonosynthEngine::MonosynthEngine()
{
    filterEnv_.sampleRate = 44100.0;
    ampEnv_.sampleRate = 44100.0;
}

void MonosynthEngine::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    sampleRate_ = sampleRate;
    filterEnv_.sampleRate = sampleRate;
    ampEnv_.sampleRate = sampleRate;
    filter_.sampleRate = sampleRate;

    reset();
}

void MonosynthEngine::reset()
{
    osc1Phase_ = 0.0f;
    osc2Phase_ = 0.0f;
    osc3Phase_ = 0.0f;
    lfo1Phase_ = 0.0f;
    lfo2Phase_ = 0.0f;

    filter_.reset();
    filterEnv_.reset();
    ampEnv_.reset();
}

void MonosynthEngine::noteOn()
{
    filterEnv_.trigger();
    ampEnv_.trigger();
}

void MonosynthEngine::noteOff()
{
    filterEnv_.release_();
    ampEnv_.release_();
}

float MonosynthEngine::processSample(float targetFrequency)
{
    // Update glide
    updateGlide(targetFrequency);
    float frequency = currentFrequency_;

    // LFO 1 modulates pitch (vibrato)
    lfo1Phase_ += lfo1Rate_ * juce::MathConstants<float>::twoPi / static_cast<float>(sampleRate_);
    if (lfo1Phase_ > juce::MathConstants<float>::twoPi)
        lfo1Phase_ -= juce::MathConstants<float>::twoPi;
    float lfo1 = std::sin(lfo1Phase_);
    frequency *= std::pow(2.0f, (lfo1 * lfo1Amount_) / 12.0f);

    // LFO 2 modulates pulse width
    lfo2Phase_ += lfo2Rate_ * juce::MathConstants<float>::twoPi / static_cast<float>(sampleRate_);
    if (lfo2Phase_ > juce::MathConstants<float>::twoPi)
        lfo2Phase_ -= juce::MathConstants<float>::twoPi;
    float lfo2 = std::sin(lfo2Phase_);
    float modPW = pulseWidth_ + lfo2 * lfo2Amount_ * 0.3f;
    modPW = juce::jlimit(0.05f, 0.95f, modPW);

    // Generate oscillators
    // Osc1: Sawtooth
    // Osc2: Square (with PWM)
    // Osc3: Triangle
    float osc1 = generateOscillator(osc1Phase_, frequency, 0, 0.5f);
    float osc2Freq = frequency * std::pow(2.0f, osc2Detune_ / 12.0f);
    float osc2 = generateOscillator(osc2Phase_, osc2Freq, 1, modPW);
    float osc3Freq = frequency * std::pow(2.0f, osc3Detune_ / 12.0f);
    float osc3 = generateOscillator(osc3Phase_, osc3Freq, 2, 0.5f);

    // Oscillator sync (osc2 syncs to osc1)
    if (oscSync_)
    {
        float osc1Raw = osc1Phase_ / juce::MathConstants<float>::twoPi;
        if (osc1Raw < syncMaster_)
        {
            osc2Phase_ = 0.0f; // Hard sync reset
        }
        syncMaster_ = osc1Raw;
    }

    // Mix oscillators
    float mixed = osc1 * osc1Level_ + osc2 * osc2Level_ + osc3 * osc3Level_;

    // Process envelopes
    float filterEnvValue = filterEnv_.process();
    float ampEnvValue = ampEnv_.process();

    // Apply filter with envelope modulation
    float modCutoff = filterCutoff_ + filterEnvValue * filterEnvAmount_;
    modCutoff = juce::jlimit(20.0f, 20000.0f, modCutoff);
    filter_.cutoff = modCutoff;
    filter_.resonance = filterResonance_;

    float filtered = filter_.process(mixed);

    // Apply amplitude envelope
    return filtered * ampEnvValue;
}

float MonosynthEngine::generateOscillator(float& phase, float frequency, int waveform, float pw)
{
    float increment = frequency * juce::MathConstants<float>::twoPi / static_cast<float>(sampleRate_);
    phase += increment;

    if (phase > juce::MathConstants<float>::twoPi)
        phase -= juce::MathConstants<float>::twoPi;

    float output = 0.0f;
    float t = phase / juce::MathConstants<float>::twoPi;

    switch (waveform)
    {
        case 0: // Sawtooth
            output = 2.0f * t - 1.0f;
            break;

        case 1: // Square with PWM
            output = (t < pw) ? 1.0f : -1.0f;
            break;

        case 2: // Triangle
            if (t < 0.5f)
                output = 4.0f * t - 1.0f;
            else
                output = -4.0f * t + 3.0f;
            break;
    }

    return output;
}

void MonosynthEngine::updateGlide(float targetFrequency)
{
    if (glideTime_ < 0.001f)
    {
        currentFrequency_ = targetFrequency;
    }
    else
    {
        float glideRate = 1.0f / (glideTime_ * static_cast<float>(sampleRate_));
        float diff = targetFrequency - currentFrequency_;
        currentFrequency_ += diff * glideRate;
    }
}
