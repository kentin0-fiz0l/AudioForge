#include "ElectricPianoEngine.h"

ElectricPianoEngine::ElectricPianoEngine()
{
    generateRhodesWavetable();
}

void ElectricPianoEngine::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    sampleRate_ = sampleRate;
    chorusDelayBuffer_.fill(0.0f);
    chorusWritePos_ = 0;
    reverbBuffer_ = 0.0f;
}

void ElectricPianoEngine::setModel(EPModel model)
{
    if (currentModel_ != model)
    {
        currentModel_ = model;

        switch (model)
        {
            case EPModel::Rhodes:
                generateRhodesWavetable();
                break;
            case EPModel::Wurlitzer:
                generateWurlitzerWavetable();
                break;
            case EPModel::Hybrid:
                generateHybridWavetable();
                break;
        }
    }
}

void ElectricPianoEngine::setTone(float tone)
{
    tone_ = juce::jlimit(0.0f, 1.0f, tone);
}

void ElectricPianoEngine::setDecay(float decay)
{
    decay_ = juce::jlimit(0.0f, 1.0f, decay);
}

void ElectricPianoEngine::setMechanicalNoise(float amount)
{
    mechanicalNoise_ = juce::jlimit(0.0f, 1.0f, amount);
}

void ElectricPianoEngine::setVelocityCurve(float curve)
{
    velocityCurve_ = juce::jlimit(0.0f, 1.0f, curve);
}

void ElectricPianoEngine::setTremoloDepth(float depth)
{
    tremoloDepth_ = juce::jlimit(0.0f, 1.0f, depth);
}

void ElectricPianoEngine::setTremoloRate(float rate)
{
    tremoloRate_ = juce::jlimit(0.1f, 20.0f, rate);
}

void ElectricPianoEngine::setChorusDepth(float depth)
{
    chorusDepth_ = juce::jlimit(0.0f, 1.0f, depth);
}

void ElectricPianoEngine::setReverbMix(float mix)
{
    reverbMix_ = juce::jlimit(0.0f, 1.0f, mix);
}

float ElectricPianoEngine::processSample(float frequency, float velocity, bool isNoteOn)
{
    // Update phase increment
    phaseIncrement_ = frequency / static_cast<float>(sampleRate_);

    // Apply velocity curve
    float mappedVelocity = velocity;
    if (velocityCurve_ < 0.5f)
    {
        // Softer response
        float curve = velocityCurve_ * 2.0f;
        mappedVelocity = std::pow(velocity, 2.0f - curve);
    }
    else
    {
        // Harder response
        float curve = (velocityCurve_ - 0.5f) * 2.0f;
        mappedVelocity = std::pow(velocity, 0.5f + curve * 0.5f);
    }

    // Target amplitude from velocity
    if (isNoteOn)
    {
        targetAmplitude_ = mappedVelocity;
        releaseRate_ = 0.999f + (decay_ * 0.0009f); // Slower release = longer decay
    }
    else
    {
        targetAmplitude_ = 0.0f;
    }

    // Envelope smoothing
    if (currentAmplitude_ < targetAmplitude_)
    {
        // Attack
        currentAmplitude_ += (targetAmplitude_ - currentAmplitude_) * 0.05f;
    }
    else
    {
        // Release
        currentAmplitude_ *= releaseRate_;
    }

    // Generate wavetable sample
    float sample = interpolateWavetable(phase_);

    // Advance phase
    phase_ += phaseIncrement_;
    while (phase_ >= 1.0f)
        phase_ -= 1.0f;

    // Apply amplitude
    sample *= currentAmplitude_;

    // Apply tone control (simple low-pass)
    static float prevSample = 0.0f;
    float toneMix = tone_ * 0.5f + 0.5f; // 0.5 - 1.0 range
    sample = sample * toneMix + prevSample * (1.0f - toneMix);
    prevSample = sample;

    // Add mechanical noise on key-off
    if (!isNoteOn && currentAmplitude_ < 0.01f && mechanicalNoise_ > 0.0f)
    {
        float noise = random_.nextFloat() * 2.0f - 1.0f;
        sample += noise * mechanicalNoise_ * 0.05f * currentAmplitude_;
    }

    // Apply effects
    sample = processEffects(sample);

    return sample;
}

void ElectricPianoEngine::reset()
{
    phase_ = 0.0f;
    currentAmplitude_ = 0.0f;
    targetAmplitude_ = 0.0f;
}

void ElectricPianoEngine::generateRhodesWavetable()
{
    // Rhodes tine sound: combination of sine waves with inharmonic partials
    // Classic "bell-like" sound with characteristic overtones
    for (int i = 0; i < WAVETABLE_SIZE; ++i)
    {
        float phase = static_cast<float>(i) / static_cast<float>(WAVETABLE_SIZE);
        float sample = 0.0f;

        // Fundamental
        sample += std::sin(phase * juce::MathConstants<float>::twoPi) * 1.0f;

        // Inharmonic partials (characteristic of struck metal tines)
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 2.76f) * 0.4f;
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 5.40f) * 0.25f;
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 8.93f) * 0.15f;

        // Add slight detuning for warmth
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 1.003f) * 0.1f;

        wavetable_[i] = sample / 2.0f; // Normalize
    }
}

void ElectricPianoEngine::generateWurlitzerWavetable()
{
    // Wurlitzer reed sound: brighter, more harmonic than Rhodes
    // Characteristic "reedy" tone with strong even harmonics
    for (int i = 0; i < WAVETABLE_SIZE; ++i)
    {
        float phase = static_cast<float>(i) / static_cast<float>(WAVETABLE_SIZE);
        float sample = 0.0f;

        // Fundamental
        sample += std::sin(phase * juce::MathConstants<float>::twoPi) * 1.0f;

        // Strong even harmonics (reed characteristic)
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 2.0f) * 0.5f;
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 4.0f) * 0.3f;
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 6.0f) * 0.2f;

        // Odd harmonics for brightness
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 3.0f) * 0.4f;
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 5.0f) * 0.25f;

        wavetable_[i] = sample / 2.5f; // Normalize
    }
}

void ElectricPianoEngine::generateHybridWavetable()
{
    // Hybrid: blend Rhodes and Wurlitzer characteristics
    std::array<float, WAVETABLE_SIZE> rhodesTable;
    std::array<float, WAVETABLE_SIZE> wurlyTable;

    // Generate both tables temporarily
    auto oldTable = wavetable_;

    generateRhodesWavetable();
    rhodesTable = wavetable_;

    generateWurlitzerWavetable();
    wurlyTable = wavetable_;

    // Blend 50/50
    for (int i = 0; i < WAVETABLE_SIZE; ++i)
    {
        wavetable_[i] = (rhodesTable[i] + wurlyTable[i]) * 0.5f;
    }
}

float ElectricPianoEngine::interpolateWavetable(float phase)
{
    // Linear interpolation for smooth playback
    float scaledPhase = phase * static_cast<float>(WAVETABLE_SIZE - 1);
    int index0 = static_cast<int>(scaledPhase);
    int index1 = (index0 + 1) % WAVETABLE_SIZE;
    float frac = scaledPhase - static_cast<float>(index0);

    return wavetable_[index0] * (1.0f - frac) + wavetable_[index1] * frac;
}

float ElectricPianoEngine::processEffects(float sample)
{
    float output = sample;

    // Tremolo (amplitude modulation)
    if (tremoloDepth_ > 0.0f)
    {
        float lfo = std::sin(tremoloPhase_ * juce::MathConstants<float>::twoPi);
        float tremolo = 1.0f + (lfo * tremoloDepth_ * 0.5f);
        output *= tremolo;

        tremoloPhase_ += tremoloRate_ / static_cast<float>(sampleRate_);
        while (tremoloPhase_ >= 1.0f)
            tremoloPhase_ -= 1.0f;
    }

    // Simple chorus (short delay with modulation)
    if (chorusDepth_ > 0.0f)
    {
        // Write to delay buffer
        chorusDelayBuffer_[chorusWritePos_] = output;
        chorusWritePos_ = (chorusWritePos_ + 1) % chorusDelayBuffer_.size();

        // Read with slight delay (creates chorusing effect)
        int readPos = (chorusWritePos_ + 2) % chorusDelayBuffer_.size();
        float delayed = chorusDelayBuffer_[readPos];

        output = output * (1.0f - chorusDepth_ * 0.5f) + delayed * chorusDepth_ * 0.5f;
    }

    // Simple reverb (very basic feedback delay)
    if (reverbMix_ > 0.0f)
    {
        reverbBuffer_ = reverbBuffer_ * 0.8f + output * 0.2f;
        output = output * (1.0f - reverbMix_) + reverbBuffer_ * reverbMix_;
    }

    return output;
}
