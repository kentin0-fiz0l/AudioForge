#include "BassEngine.h"

BassEngine::BassEngine()
{
    generateElectricWavetable();
}

void BassEngine::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    sampleRate_ = sampleRate;
    filterLP_ = 0.0f;
    filterPrev_ = 0.0f;
}

void BassEngine::setBassType(BassType type)
{
    if (currentType_ != type)
    {
        currentType_ = type;

        switch (type)
        {
            case BassType::Upright:
                generateUprightWavetable();
                break;
            case BassType::Electric:
                generateElectricWavetable();
                break;
        }
    }
}

void BassEngine::setPlayStyle(PlayStyle style)
{
    currentStyle_ = style;
}

void BassEngine::setTone(float tone)
{
    tone_ = juce::jlimit(0.0f, 1.0f, tone);
}

void BassEngine::setAttack(float attack)
{
    attack_ = juce::jlimit(0.0f, 1.0f, attack);
}

void BassEngine::setDecay(float decay)
{
    decay_ = juce::jlimit(0.0f, 1.0f, decay);
}

void BassEngine::setFretNoise(float amount)
{
    fretNoise_ = juce::jlimit(0.0f, 1.0f, amount);
}

void BassEngine::setSlideAmount(float amount)
{
    slideAmount_ = juce::jlimit(0.0f, 1.0f, amount);
}

float BassEngine::processSample(float frequency, float velocity, bool isNoteOn, int samplesSinceNoteOn)
{
    phaseIncrement_ = frequency / static_cast<float>(sampleRate_);

    // Note onset - set attack characteristics based on play style
    if (isNoteOn && samplesSinceNoteOn == 0)
    {
        targetAmp_ = velocity;

        switch (currentStyle_)
        {
            case PlayStyle::Fingered:
                // Smooth attack
                fundamentalAmp_ = velocity * 0.3f;
                harmonicAmp_ = velocity * 0.7f;
                break;

            case PlayStyle::Picked:
                // Bright attack
                fundamentalAmp_ = velocity * 0.4f;
                harmonicAmp_ = velocity * 1.0f;
                break;

            case PlayStyle::Slapped:
                // Percussive attack
                fundamentalAmp_ = velocity * 0.5f;
                harmonicAmp_ = velocity * 1.3f;
                break;
        }
    }

    // Envelope - fundamental sustains, harmonics decay faster
    float fundamentalDecay = 0.9999f;  // Very slow (bass sustain)
    float harmonicDecay = 0.996f + (decay_ * 0.003f);  // Faster decay

    if (isNoteOn)
    {
        fundamentalAmp_ += (targetAmp_ - fundamentalAmp_) * 0.001f;
    }
    else
    {
        fundamentalAmp_ *= fundamentalDecay;
    }

    harmonicAmp_ *= harmonicDecay;

    // Generate sample from wavetable
    float sample = interpolateWavetable(phase_);

    // Advance phase
    phase_ += phaseIncrement_;
    while (phase_ >= 1.0f)
        phase_ -= 1.0f;

    // Mix fundamental and harmonics
    float fundamental = sample * fundamentalAmp_;
    float harmonics = sample * harmonicAmp_ * 0.7f;
    sample = fundamental + harmonics;

    // Add fret noise at note onset
    if (fretNoise_ > 0.0f && samplesSinceNoteOn < 100)
    {
        float noise = generateFretNoise(samplesSinceNoteOn);
        sample += noise * fretNoise_ * 0.2f;
    }

    // Tone control (pickup position simulation)
    sample = processFiltering(sample);

    return sample;
}

void BassEngine::reset()
{
    phase_ = 0.0f;
    fundamentalAmp_ = 0.0f;
    harmonicAmp_ = 0.0f;
    targetAmp_ = 0.0f;
    filterLP_ = 0.0f;
    filterPrev_ = 0.0f;
}

void BassEngine::generateUprightWavetable()
{
    // Upright bass: strong fundamental with warm, woody harmonics
    for (int i = 0; i < WAVETABLE_SIZE; ++i)
    {
        float phase = static_cast<float>(i) / static_cast<float>(WAVETABLE_SIZE);
        float sample = 0.0f;

        // Very strong fundamental (upright characteristic)
        sample += std::sin(phase * juce::MathConstants<float>::twoPi) * 1.0f;

        // Warm even harmonics
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 2.0f) * 0.3f;
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 4.0f) * 0.15f;

        // Subtle odd harmonics for body
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 3.0f) * 0.2f;
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 5.0f) * 0.1f;

        wavetable_[i] = sample / 1.75f;
    }
}

void BassEngine::generateElectricWavetable()
{
    // Electric bass: strong fundamental with bright, punchy harmonics
    for (int i = 0; i < WAVETABLE_SIZE; ++i)
    {
        float phase = static_cast<float>(i) / static_cast<float>(WAVETABLE_SIZE);
        float sample = 0.0f;

        // Strong fundamental
        sample += std::sin(phase * juce::MathConstants<float>::twoPi) * 1.0f;

        // Bright harmonics (electric bass characteristic)
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 2.0f) * 0.4f;
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 3.0f) * 0.3f;
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 4.0f) * 0.25f;
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 5.0f) * 0.2f;
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 6.0f) * 0.15f;

        wavetable_[i] = sample / 2.3f;
    }
}

float BassEngine::interpolateWavetable(float phase)
{
    float scaledPhase = phase * static_cast<float>(WAVETABLE_SIZE - 1);
    int index0 = static_cast<int>(scaledPhase);
    int index1 = (index0 + 1) % WAVETABLE_SIZE;
    float frac = scaledPhase - static_cast<float>(index0);

    return wavetable_[index0] * (1.0f - frac) + wavetable_[index1] * frac;
}

float BassEngine::generateFretNoise(int samplesSinceNoteOn)
{
    // Fret noise - decaying noise burst at note onset
    float noise = random_.nextFloat() * 2.0f - 1.0f;

    // Envelope for noise (quick decay)
    float envelope = 1.0f - (static_cast<float>(samplesSinceNoteOn) / 100.0f);
    envelope = juce::jlimit(0.0f, 1.0f, envelope);

    return noise * envelope;
}

float BassEngine::processFiltering(float sample)
{
    // Tone control simulates pickup position
    // 0.0 = neck pickup (warm, bass-heavy)
    // 1.0 = bridge pickup (bright, treble-heavy)

    float cutoffMod = tone_ * 0.8f + 0.2f; // 0.2 - 1.0 range

    // Simple one-pole low-pass filter
    filterLP_ = filterLP_ * (1.0f - cutoffMod) + sample * cutoffMod;

    return filterLP_;
}
