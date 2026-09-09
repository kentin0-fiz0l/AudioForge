#include "BrassEngine.h"

BrassEngine::BrassEngine()
{
    generateTrumpetWavetable();
}

void BrassEngine::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    sampleRate_ = sampleRate;
    filterLP_ = 0.0f;
    filterBP_ = 0.0f;
    filterHP_ = 0.0f;
    noiseLP_ = 0.0f;
}

void BrassEngine::setInstrumentType(InstrumentType type)
{
    if (currentInstrument_ != type)
    {
        currentInstrument_ = type;

        switch (type)
        {
            case InstrumentType::Trumpet:
                generateTrumpetWavetable();
                break;
            case InstrumentType::Trombone:
                generateTromboneWavetable();
                break;
            case InstrumentType::Saxophone:
                generateSaxophoneWavetable();
                break;
        }
    }
}

void BrassEngine::setArticulation(Articulation articulation)
{
    currentArticulation_ = articulation;
}

void BrassEngine::setBrightness(float brightness)
{
    brightness_ = juce::jlimit(0.0f, 1.0f, brightness);
}

void BrassEngine::setBreathNoise(float amount)
{
    breathNoise_ = juce::jlimit(0.0f, 1.0f, amount);
}

void BrassEngine::setVibratoDepth(float depth)
{
    vibratoDepth_ = juce::jlimit(0.0f, 1.0f, depth);
}

void BrassEngine::setVibratoRate(float rate)
{
    vibratoRate_ = juce::jlimit(0.0f, 1.0f, rate);
}

void BrassEngine::setExpression(float expression)
{
    expression_ = juce::jlimit(0.0f, 1.0f, expression);
}

void BrassEngine::setSectionSize(int size)
{
    sectionSize_ = juce::jlimit(1, 4, size);
}

float BrassEngine::processSample(float frequency, float velocity, bool isNoteOn, int samplesSinceNoteOn)
{
    // Apply vibrato to frequency
    float modulatedFrequency = processVibrato(frequency);

    // Update phase increment
    phaseIncrement_ = modulatedFrequency / static_cast<float>(sampleRate_);

    // Articulation envelope shaping
    if (isNoteOn)
    {
        targetAmplitude_ = velocity * expression_;

        // Different attack/release based on articulation
        switch (currentArticulation_)
        {
            case Articulation::Sustain:
                attackRate_ = 0.01f;   // Smooth attack
                releaseRate_ = 0.998f; // Slow release
                break;

            case Articulation::Staccato:
                attackRate_ = 0.05f;   // Fast attack
                releaseRate_ = 0.99f;  // Fast release
                // Auto-release after short time
                if (samplesSinceNoteOn > static_cast<int>(sampleRate_ * 0.15f))
                {
                    targetAmplitude_ = 0.0f;
                }
                break;

            case Articulation::Marcato:
                attackRate_ = 0.1f;    // Very fast attack
                releaseRate_ = 0.995f; // Medium release
                // Accent at the beginning
                if (samplesSinceNoteOn < static_cast<int>(sampleRate_ * 0.05f))
                {
                    targetAmplitude_ *= 1.3f;
                }
                break;

            case Articulation::FallOff:
                attackRate_ = 0.02f;   // Normal attack
                releaseRate_ = 0.997f; // Slow release
                // Pitch bend down at end
                if (samplesSinceNoteOn > static_cast<int>(sampleRate_ * 0.5f))
                {
                    float fallAmount = static_cast<float>(samplesSinceNoteOn - sampleRate_ * 0.5f) / (sampleRate_ * 0.3f);
                    fallAmount = juce::jlimit(0.0f, 1.0f, fallAmount);
                    phaseIncrement_ *= (1.0f - fallAmount * 0.1f); // Bend down by up to 10%
                }
                break;
        }
    }
    else
    {
        targetAmplitude_ = 0.0f;
    }

    // Envelope smoothing
    if (currentAmplitude_ < targetAmplitude_)
    {
        // Attack
        currentAmplitude_ += (targetAmplitude_ - currentAmplitude_) * attackRate_;
    }
    else
    {
        // Release
        currentAmplitude_ *= releaseRate_;
    }

    // Generate sample with section voices
    float sample = 0.0f;
    for (int i = 0; i < sectionSize_; ++i)
    {
        float voiceSample = interpolateWavetable(sectionPhases_[i]);

        // Add slight detuning for section effect
        float detunedIncrement = phaseIncrement_ * (1.0f + sectionDetune_[i]);

        sectionPhases_[i] += detunedIncrement;
        while (sectionPhases_[i] >= 1.0f)
            sectionPhases_[i] -= 1.0f;

        sample += voiceSample;
    }

    // Normalize by section size
    sample /= static_cast<float>(sectionSize_);

    // Apply amplitude
    sample *= currentAmplitude_;

    // Add breath noise
    if (breathNoise_ > 0.0f && currentAmplitude_ > 0.01f)
    {
        float noise = generateBreathNoise();
        sample += noise * breathNoise_ * currentAmplitude_ * 0.3f;
    }

    // Dynamic filtering (brightness increases with amplitude)
    float cutoffMod = currentAmplitude_ * expression_;
    sample = processFiltering(sample, cutoffMod);

    return sample;
}

void BrassEngine::reset()
{
    phase_ = 0.0f;
    for (auto& p : sectionPhases_)
        p = 0.0f;

    currentAmplitude_ = 0.0f;
    targetAmplitude_ = 0.0f;
    vibratoPhase_ = 0.0f;
}

void BrassEngine::generateTrumpetWavetable()
{
    // Trumpet: bright, penetrating sound with strong odd harmonics
    // Emphasis on 3rd, 5th, 7th harmonics
    for (int i = 0; i < WAVETABLE_SIZE; ++i)
    {
        float phase = static_cast<float>(i) / static_cast<float>(WAVETABLE_SIZE);
        float sample = 0.0f;

        // Fundamental
        sample += std::sin(phase * juce::MathConstants<float>::twoPi) * 1.0f;

        // Strong odd harmonics (trumpet characteristic)
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 3.0f) * 0.6f;
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 5.0f) * 0.4f;
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 7.0f) * 0.3f;

        // Some even harmonics for richness
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 2.0f) * 0.3f;
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 4.0f) * 0.2f;

        wavetable_[i] = sample / 2.8f; // Normalize
    }
}

void BrassEngine::generateTromboneWavetable()
{
    // Trombone: warm, rich sound with strong fundamentals
    // More balanced harmonic content than trumpet
    for (int i = 0; i < WAVETABLE_SIZE; ++i)
    {
        float phase = static_cast<float>(i) / static_cast<float>(WAVETABLE_SIZE);
        float sample = 0.0f;

        // Strong fundamental
        sample += std::sin(phase * juce::MathConstants<float>::twoPi) * 1.0f;

        // Balanced harmonics
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 2.0f) * 0.5f;
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 3.0f) * 0.4f;
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 4.0f) * 0.3f;
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 5.0f) * 0.25f;
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 6.0f) * 0.2f;

        wavetable_[i] = sample / 2.65f; // Normalize
    }
}

void BrassEngine::generateSaxophoneWavetable()
{
    // Saxophone: reedy, complex harmonics, vocal quality
    // Complex mix of odd and even harmonics
    for (int i = 0; i < WAVETABLE_SIZE; ++i)
    {
        float phase = static_cast<float>(i) / static_cast<float>(WAVETABLE_SIZE);
        float sample = 0.0f;

        // Fundamental
        sample += std::sin(phase * juce::MathConstants<float>::twoPi) * 1.0f;

        // Complex harmonic structure (reedy sound)
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 2.0f) * 0.4f;
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 3.0f) * 0.7f;
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 4.0f) * 0.3f;
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 5.0f) * 0.5f;
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 7.0f) * 0.4f;

        // Add slight inharmonic content for "reediness"
        sample += std::sin(phase * juce::MathConstants<float>::twoPi * 2.3f) * 0.2f;

        wavetable_[i] = sample / 3.5f; // Normalize
    }
}

float BrassEngine::interpolateWavetable(float phase)
{
    float scaledPhase = phase * static_cast<float>(WAVETABLE_SIZE - 1);
    int index0 = static_cast<int>(scaledPhase);
    int index1 = (index0 + 1) % WAVETABLE_SIZE;
    float frac = scaledPhase - static_cast<float>(index0);

    return wavetable_[index0] * (1.0f - frac) + wavetable_[index1] * frac;
}

float BrassEngine::generateBreathNoise()
{
    // Generate breath noise (filtered white noise)
    float noise = random_.nextFloat() * 2.0f - 1.0f;

    // Low-pass filter the noise for more realistic breath sound
    noiseLP_ = noiseLP_ * 0.95f + noise * 0.05f;

    return noiseLP_;
}

float BrassEngine::processFiltering(float sample, float cutoffMod)
{
    // State variable filter (SVF)
    // Cutoff frequency increases with amplitude (louder = brighter)
    float baseCutoff = 500.0f + (brightness_ * 4000.0f);
    float dynamicCutoff = baseCutoff + (cutoffMod * 3000.0f);

    // Clamp cutoff
    dynamicCutoff = juce::jlimit(200.0f, 10000.0f, dynamicCutoff);

    // Calculate filter coefficients
    float f = 2.0f * std::sin(juce::MathConstants<float>::pi * dynamicCutoff / static_cast<float>(sampleRate_));
    float q = 1.0f / filterResonance_;

    // Process SVF
    filterLP_ += f * filterBP_;
    filterHP_ = sample - filterLP_ - q * filterBP_;
    filterBP_ += f * filterHP_;

    // Return low-pass filtered output (warm brass sound)
    return filterLP_;
}

float BrassEngine::processVibrato(float frequency)
{
    if (vibratoDepth_ <= 0.0f)
        return frequency;

    // Vibrato rate: 4-8 Hz (typical brass vibrato)
    float vibratoHz = 4.0f + (vibratoRate_ * 4.0f);

    // Generate vibrato LFO
    float lfo = std::sin(vibratoPhase_ * juce::MathConstants<float>::twoPi);

    // Advance vibrato phase
    vibratoPhase_ += vibratoHz / static_cast<float>(sampleRate_);
    while (vibratoPhase_ >= 1.0f)
        vibratoPhase_ -= 1.0f;

    // Apply vibrato (pitch modulation ±3%)
    float pitchMod = 1.0f + (lfo * vibratoDepth_ * 0.03f);

    return frequency * pitchMod;
}
