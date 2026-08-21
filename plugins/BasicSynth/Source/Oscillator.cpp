#include "Oscillator.h"

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
    }

    // Advance phase using shared utility
    AudioForge::DSP::WaveformGenerators::advancePhase(phase, phaseIncrement);

    return sample;
}

void Oscillator::reset()
{
    phase = 0.0f;
}
