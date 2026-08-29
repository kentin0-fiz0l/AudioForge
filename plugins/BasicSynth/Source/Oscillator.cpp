#include "Oscillator.h"
#include <algorithm>

Oscillator::Oscillator()
{
}

void Oscillator::setFrequency(float frequency, double sampleRate)
{
    // Use shared DSP library for phase increment calculation
    phaseIncrement = AudioForge::DSP::WaveformGenerators::calculatePhaseIncrement(frequency, sampleRate);
}

float Oscillator::getNextSample(Waveform waveform)
{
    float sample = 0.0f;

    // Use shared DSP waveform generators
    switch (waveform)
    {
        case Waveform::Sine:
            sample = AudioForge::DSP::WaveformGenerators::sine(phase);
            break;

        case Waveform::Sawtooth:
            sample = AudioForge::DSP::WaveformGenerators::sawtooth(phase);
            break;

        case Waveform::Square:
            sample = AudioForge::DSP::WaveformGenerators::square(phase);
            break;

        case Waveform::Pulse:
            // Pulse wave: square wave with adjustable duty cycle
            sample = (phase < pulseWidth) ? 1.0f : -1.0f;
            break;
    }

    // Advance phase using shared utility
    AudioForge::DSP::WaveformGenerators::advancePhase(phase, phaseIncrement);

    return sample;
}

void Oscillator::setPulseWidth(float width)
{
    // Clamp to safe range (0.05-0.95) to avoid DC offset issues
    pulseWidth = std::max(0.05f, std::min(0.95f, width));
}

void Oscillator::reset()
{
    phase = 0.0f;
}
