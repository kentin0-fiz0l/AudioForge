#include "EffectsChain.h"

EffectsChain::EffectsChain()
{
}

void EffectsChain::prepare(double sampleRate, int maxBlockSize)
{
    // Prepare all effects
    chorus.prepare(sampleRate, maxBlockSize);
    reverb.prepare(sampleRate, maxBlockSize);
    // Saturation is stateless, doesn't need preparation
}

void EffectsChain::processSample(float input, float& leftOut, float& rightOut)
{
    // Signal flow: Input → Saturation → Chorus → Reverb → Output

    // 1. Saturation (mono in, mono out)
    float saturated = saturation.processSample(input);

    // 2. Chorus (mono in, stereo out)
    float chorusLeft, chorusRight;
    chorus.processSample(saturated, chorusLeft, chorusRight);

    // 3. Reverb (stereo in, stereo out)
    // Average stereo channels back to mono for reverb input
    float reverbInput = (chorusLeft + chorusRight) * 0.5f;
    float reverbLeft, reverbRight;
    reverb.processSample(reverbInput, reverbLeft, reverbRight);

    // Final output
    leftOut = reverbLeft;
    rightOut = reverbRight;
}

void EffectsChain::setSaturation(float drive, float mix, int type)
{
    saturation.setParameters(drive, mix, static_cast<Saturation::SaturationType>(type));
}

void EffectsChain::setChorus(float rate, float depth, float mix)
{
    chorus.setParameters(rate, depth, mix);
}

void EffectsChain::setReverb(float roomSize, float damping, float mix)
{
    reverb.setParameters(roomSize, damping, mix);
}

void EffectsChain::reset()
{
    saturation.reset();
    chorus.reset();
    reverb.reset();
}
