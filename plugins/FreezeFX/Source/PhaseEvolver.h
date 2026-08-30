#pragma once

#include <vector>
#include <juce_core/juce_core.h>

/**
 * PhaseEvolver
 *
 * Evolves and randomizes phase spectrum over time to create ambient textures.
 * Implements controlled phase randomization for frozen spectrums.
 */
class PhaseEvolver
{
public:
    PhaseEvolver();
    ~PhaseEvolver() = default;

    //==============================================================================
    // Phase Evolution
    void evolvePhase(std::vector<float>& phase, float deltaTime);

    //==============================================================================
    // Parameters
    void setRandomizationAmount(float amount);  // 0-1: how much to randomize
    void setEvolutionSpeed(float speed);        // Hz: rate of phase evolution

    float getRandomizationAmount() const { return randomAmount; }
    float getEvolutionSpeed() const { return evolutionSpeed; }

private:
    //==============================================================================
    // Phase Increment (per bin)
    std::vector<float> phaseIncrement;
    std::vector<float> phaseAccumulator;

    //==============================================================================
    // Parameters
    float randomAmount = 0.5f;
    float evolutionSpeed = 1.0f;  // Hz

    //==============================================================================
    // Random Generator
    juce::Random random;
};
