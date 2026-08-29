#include "NoiseGenerator.h"

NoiseGenerator::NoiseGenerator()
{
    reset();
}

float NoiseGenerator::getNextSample(NoiseType type)
{
    switch (type)
    {
        case NoiseType::White:
            return generateWhite();

        case NoiseType::Pink:
            return generatePink();

        default:
            return 0.0f;
    }
}

void NoiseGenerator::reset()
{
    for (int i = 0; i < 7; ++i)
        pinkState[i] = 0.0f;

    pinkCounter = 0;
}

float NoiseGenerator::generateWhite()
{
    // Generate uniform random number in range [-1.0, 1.0]
    return random.nextFloat() * 2.0f - 1.0f;
}

float NoiseGenerator::generatePink()
{
    // Paul Kellett's refined pink noise algorithm
    // Approximates -3dB/octave pink noise using 7 generators

    float white = generateWhite();

    // Update pink noise state
    pinkState[0] = 0.99886f * pinkState[0] + white * 0.0555179f;
    pinkState[1] = 0.99332f * pinkState[1] + white * 0.0750759f;
    pinkState[2] = 0.96900f * pinkState[2] + white * 0.1538520f;
    pinkState[3] = 0.86650f * pinkState[3] + white * 0.3104856f;
    pinkState[4] = 0.55000f * pinkState[4] + white * 0.5329522f;
    pinkState[5] = -0.7616f * pinkState[5] - white * 0.0168980f;

    float pink = pinkState[0] + pinkState[1] + pinkState[2] +
                 pinkState[3] + pinkState[4] + pinkState[5] +
                 pinkState[6] + white * 0.5362f;

    pinkState[6] = white * 0.115926f;

    // Scale to [-1.0, 1.0] range
    pink *= 0.11f;

    // Soft clip
    if (pink > 1.0f)
        pink = 1.0f;
    else if (pink < -1.0f)
        pink = -1.0f;

    return pink;
}
