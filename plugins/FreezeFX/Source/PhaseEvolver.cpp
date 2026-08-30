#include "PhaseEvolver.h"
#include <cmath>

PhaseEvolver::PhaseEvolver()
{
    random.setSeed(juce::Time::currentTimeMillis());
}

void PhaseEvolver::evolvePhase(std::vector<float>& phase, float deltaTime)
{
    // Ensure buffers match spectrum size
    if (phaseIncrement.size() != phase.size())
    {
        phaseIncrement.resize(phase.size());
        phaseAccumulator.resize(phase.size());

        // Initialize with frequency-dependent random increments
        for (size_t i = 0; i < phaseIncrement.size(); ++i)
        {
            // Lower frequencies evolve slower (more natural)
            float freqFactor = 1.0f / (1.0f + (float)i * 0.01f);
            phaseIncrement[i] = (random.nextFloat() * 2.0f - 1.0f) * evolutionSpeed * freqFactor;
        }
    }

    // Evolution modes
    float timeStep = deltaTime * evolutionSpeed;

    if (randomAmount < 0.01f)
    {
        // No evolution - static frozen phase
        return;
    }

    if (randomAmount < 0.5f)
    {
        // Mode 1: Smooth random walk (low randomization)
        evolvePhaseSmooth(phase, timeStep);
    }
    else
    {
        // Mode 2: Aggressive randomization (high randomization)
        evolvePhaseRandom(phase, timeStep);
    }
}

void PhaseEvolver::evolvePhaseSmooth(std::vector<float>& phase, float timeStep)
{
    // Smooth evolution with correlated adjacent bins
    for (size_t i = 0; i < phase.size(); ++i)
    {
        // Slowly drift phase increment (random walk)
        float drift = (random.nextFloat() * 2.0f - 1.0f) * 0.01f * randomAmount;
        phaseIncrement[i] += drift;

        // Clamp increment to prevent runaway
        phaseIncrement[i] = juce::jlimit(-evolutionSpeed, evolutionSpeed, phaseIncrement[i]);

        // Accumulate phase
        phaseAccumulator[i] += phaseIncrement[i] * timeStep;
        phase[i] += phaseIncrement[i] * timeStep;

        // Smooth correlation with neighbors (spectral coherence)
        if (i > 0 && i < phase.size() - 1)
        {
            float neighborAvg = (phase[i - 1] + phase[i + 1]) * 0.5f;
            float correlation = 0.1f * randomAmount;  // 10% influence
            phase[i] = phase[i] * (1.0f - correlation) + neighborAvg * correlation;
        }

        // Wrap phase to [-pi, pi]
        phase[i] = wrapPhase(phase[i]);
    }
}

void PhaseEvolver::evolvePhaseRandom(std::vector<float>& phase, float timeStep)
{
    // Aggressive randomization for chaotic textures
    for (size_t i = 0; i < phase.size(); ++i)
    {
        // Strong random offset
        float randomOffset = (random.nextFloat() * 2.0f - 1.0f) * randomAmount * 0.3f;
        phase[i] += randomOffset;

        // Add frequency-dependent modulation
        float freqFactor = 1.0f / (1.0f + (float)i * 0.02f);
        float modulation = std::sin(phaseAccumulator[i]) * randomAmount * freqFactor;
        phase[i] += modulation * timeStep;

        // Accumulate for LFO-like modulation
        phaseAccumulator[i] += phaseIncrement[i] * timeStep;

        // Wrap phase
        phase[i] = wrapPhase(phase[i]);
    }
}

float PhaseEvolver::wrapPhase(float phase)
{
    // Efficient phase wrapping to [-pi, pi]
    while (phase > juce::MathConstants<float>::pi)
        phase -= 2.0f * juce::MathConstants<float>::pi;

    while (phase < -juce::MathConstants<float>::pi)
        phase += 2.0f * juce::MathConstants<float>::pi;

    return phase;
}

void PhaseEvolver::setRandomizationAmount(float amount)
{
    randomAmount = juce::jlimit(0.0f, 1.0f, amount);
}

void PhaseEvolver::setEvolutionSpeed(float speed)
{
    evolutionSpeed = juce::jmax(0.0f, speed);

    // Update phase increments with new speed
    for (size_t i = 0; i < phaseIncrement.size(); ++i)
    {
        float freqFactor = 1.0f / (1.0f + (float)i * 0.01f);
        phaseIncrement[i] = (random.nextFloat() * 2.0f - 1.0f) * evolutionSpeed * freqFactor;
    }
}
