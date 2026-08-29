#include "OscillatorBank.h"
#include <cmath>

OscillatorBank::OscillatorBank()
{
    // Initialize default configurations
    for (int i = 0; i < NUM_OSCILLATORS; ++i)
    {
        configs[i] = OscillatorConfig();

        // Default settings per oscillator
        if (i == OSC_1)
        {
            configs[i].level = 0.7f;
            configs[i].waveform = static_cast<int>(Oscillator::Waveform::Sawtooth);
            configs[i].enabled = true;
        }
        else if (i == OSC_2)
        {
            configs[i].level = 0.5f;
            configs[i].detune = 7.0f; // Perfect 5th above
            configs[i].waveform = static_cast<int>(Oscillator::Waveform::Sawtooth);
            configs[i].enabled = false; // Disabled by default
        }
        else if (i == OSC_3)
        {
            configs[i].level = 0.4f;
            configs[i].detune = -12.0f; // Octave below
            configs[i].waveform = static_cast<int>(Oscillator::Waveform::Square);
            configs[i].enabled = false; // Disabled by default
        }
        else if (i == SUB_OSC)
        {
            configs[i].level = 0.6f;
            configs[i].detune = -12.0f; // Always 1 octave below
            configs[i].waveform = static_cast<int>(Oscillator::Waveform::Sine);
            configs[i].enabled = false; // Disabled by default
        }
        else if (i == NOISE)
        {
            configs[i].level = 0.3f;
            configs[i].enabled = false; // Disabled by default
        }
    }
}

void OscillatorBank::setFrequency(float frequency, double sampleRate)
{
    baseFrequency = frequency;
    currentSampleRate = sampleRate;

    // Update all oscillators with detuned frequencies
    for (int i = 0; i < NUM_OSCILLATORS; ++i)
    {
        if (i == NOISE)
            continue; // Noise doesn't have frequency

        float detuneRatio = semitonesToRatio(configs[i].detune);
        float oscFrequency = frequency * detuneRatio;

        oscillators[i].setFrequency(oscFrequency, sampleRate);
    }
}

void OscillatorBank::configure(int oscIndex, const OscillatorConfig& config)
{
    if (oscIndex < 0 || oscIndex >= NUM_OSCILLATORS)
        return;

    configs[oscIndex] = config;

    // Update oscillator frequency if detune changed
    if (oscIndex != NOISE)
    {
        float detuneRatio = semitonesToRatio(config.detune);
        float oscFrequency = baseFrequency * detuneRatio;
        oscillators[oscIndex].setFrequency(oscFrequency, currentSampleRate);
    }
}

float OscillatorBank::getNextSample()
{
    float mixedOutput = 0.0f;
    int activeOscCount = 0;

    // Mix all enabled oscillators
    for (int i = 0; i < NUM_OSCILLATORS; ++i)
    {
        if (!configs[i].enabled)
            continue;

        float sample = 0.0f;

        if (i == NOISE)
        {
            // Noise generator
            sample = noiseGenerator.getNextSample(NoiseGenerator::NoiseType::White);
        }
        else
        {
            // Regular oscillator
            auto waveform = static_cast<Oscillator::Waveform>(configs[i].waveform);
            sample = oscillators[i].getNextSample(waveform);

            // Apply pulse width modulation for pulse waves
            if (waveform == Oscillator::Waveform::Pulse)
            {
                // Pulse wave implementation (square with adjustable duty cycle)
                // This is a placeholder - will be implemented in Oscillator class
                sample = oscillators[i].getNextSample(Oscillator::Waveform::Square);
            }
        }

        // Apply oscillator level
        mixedOutput += sample * configs[i].level;
        activeOscCount++;
    }

    // Normalize by number of active oscillators to prevent clipping
    if (activeOscCount > 0)
    {
        mixedOutput /= std::sqrt(static_cast<float>(activeOscCount));
    }

    // Soft clip to prevent harsh clipping
    if (mixedOutput > 1.0f)
        mixedOutput = 1.0f;
    else if (mixedOutput < -1.0f)
        mixedOutput = -1.0f;

    return mixedOutput;
}

void OscillatorBank::reset()
{
    for (int i = 0; i < NUM_OSCILLATORS; ++i)
    {
        if (i != NOISE)
            oscillators[i].reset();
    }
}

float OscillatorBank::semitonesToRatio(float semitones) const
{
    // 12 semitones = 1 octave = frequency × 2
    // Formula: ratio = 2^(semitones/12)
    return std::pow(2.0f, semitones / 12.0f);
}
