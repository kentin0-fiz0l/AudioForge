#include "PhaseEvolver.h"
#include <cmath>

PhaseEvolver::PhaseEvolver()
{
    random.setSeed(juce::Time::currentTimeMillis());
}

void PhaseEvolver::evolvePhase(std::vector<float>& phase, float deltaTime)
{
    // Ensure phase increment buffers match spectrum size
    if (phaseIncrement.size() != phase.size())
    {
        phaseIncrement.resize(phase.size());
        phaseAccumulator.resize(phase.size());

        // Initialize with random increments
        for (size_t i = 0; i < phaseIncrement.size(); ++i)
        {
            phaseIncrement[i] = (random.nextFloat() * 2.0f - 1.0f) * evolutionSpeed;
        }
    }

    // Evolve phase for each bin
    float timeStep = deltaTime * evolutionSpeed;

    for (size_t i = 0; i < phase.size(); ++i)
    {
        // Accumulate phase
        phaseAccumulator[i] += phaseIncrement[i] * timeStep;

        // Apply randomization
        float randomOffset = (random.nextFloat() * 2.0f - 1.0f) * randomAmount * 0.1f;
        phase[i] += randomOffset;

        // Wrap phase to [-pi, pi]
        while (phase[i] > juce::MathConstants<float>::pi)
            phase[i] -= 2.0f * juce::MathConstants<float>::pi;

        while (phase[i] < -juce::MathConstants<float>::pi)
            phase[i] += 2.0f * juce::MathConstants<float>::pi;
    }
}

void PhaseEvolver::setRandomizationAmount(float amount)
{
    randomAmount = juce::jlimit(0.0f, 1.0f, amount);
}

void PhaseEvolver::setEvolutionSpeed(float speed)
{
    evolutionSpeed = juce::jmax(0.0f, speed);

    // Regenerate phase increments with new speed
    for (size_t i = 0; i < phaseIncrement.size(); ++i)
    {
        phaseIncrement[i] = (random.nextFloat() * 2.0f - 1.0f) * evolutionSpeed;
    }
}
