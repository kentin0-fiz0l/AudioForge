#include "FMOperator.h"
#include <algorithm>

FMOperator::FMOperator()
{
}

void FMOperator::setFrequency(float frequencyHz)
{
    baseFrequency = frequencyHz;
}

void FMOperator::setRatio(float ratio_)
{
    ratio = std::clamp(ratio_, 0.1f, 16.0f);
}

void FMOperator::setLevel(float level_)
{
    level = std::clamp(level_, 0.0f, 1.0f);
}

void FMOperator::setEnvelope(float attack, float decay, float sustain, float release)
{
    envelope.setAttack(attack);
    envelope.setDecay(decay);
    envelope.setSustain(sustain);
    envelope.setRelease(release);
}

void FMOperator::trigger()
{
    envelope.trigger();
}

void FMOperator::release()
{
    envelope.release();
}

void FMOperator::reset()
{
    phase = 0.0f;
    envelope.reset();
}

float FMOperator::processSample(double sampleRate, float phaseModulation)
{
    float deltaTime = static_cast<float>(1.0 / sampleRate);
    float envValue = envelope.getNextValue(deltaTime);

    // Generate sine wave with phase modulation
    float output = std::sin(phase + phaseModulation);
    output *= envValue * level;

    advancePhase(sampleRate);
    return output;
}

void FMOperator::advancePhase(double sampleRate)
{
    float actualFrequency = baseFrequency * ratio;
    float phaseIncrement = (2.0f * static_cast<float>(M_PI) * actualFrequency) / static_cast<float>(sampleRate);

    phase += phaseIncrement;

    // Wrap phase
    while (phase >= 2.0f * static_cast<float>(M_PI))
        phase -= 2.0f * static_cast<float>(M_PI);
}
