#include "LeslieSimulator.h"
#include <cmath>
#include <cstring>
#include <algorithm>

LeslieSimulator::LeslieSimulator()
    : lfoPhase(0.0f)
    , lfoRate(SLOW_SPEED)
    , fastSpeed(false)
    , delayWritePos(0)
    , depth(0.5f)
    , mix(0.5f)
    , sampleRate(44100.0)
{
    std::memset(delayBuffer, 0, sizeof(delayBuffer));
}

void LeslieSimulator::prepare(double sr, int maxBlockSize)
{
    sampleRate = sr;
    reset();
}

void LeslieSimulator::setSpeed(bool fast)
{
    fastSpeed = fast;
    lfoRate = fast ? FAST_SPEED : SLOW_SPEED;
}

void LeslieSimulator::setDepth(float d)
{
    depth = std::max(0.0f, std::min(1.0f, d));
}

void LeslieSimulator::setMix(float m)
{
    mix = std::max(0.0f, std::min(1.0f, m));
}

float LeslieSimulator::processSample(float input)
{
    // Write input to delay line
    delayBuffer[delayWritePos] = input;

    // Generate LFO (sine wave)
    float lfo = std::sin(lfoPhase * 2.0f * M_PI);

    // Modulate delay time (0.5ms to 5ms range)
    float baseDelay = 2.0f; // ms
    float modulatedDelay = baseDelay + (lfo * depth * 3.0f); // 2-5ms range
    float delaySamples = (modulatedDelay * 0.001f) * static_cast<float>(sampleRate);

    // Read from delay with linear interpolation
    float readPos = static_cast<float>(delayWritePos) - delaySamples;
    while (readPos < 0.0f)
        readPos += MAX_DELAY_SAMPLES;

    int readPosInt = static_cast<int>(readPos);
    float readPosFrac = readPos - readPosInt;

    int readPos1 = readPosInt % MAX_DELAY_SAMPLES;
    int readPos2 = (readPosInt + 1) % MAX_DELAY_SAMPLES;

    float delayed = delayBuffer[readPos1] * (1.0f - readPosFrac) +
                    delayBuffer[readPos2] * readPosFrac;

    // Apply amplitude modulation (tremolo effect)
    float amountAM = 0.3f * depth; // Subtle amplitude modulation
    float am = 1.0f + (lfo * amountAM);
    delayed *= am;

    // Advance LFO phase
    lfoPhase += lfoRate / static_cast<float>(sampleRate);
    if (lfoPhase >= 1.0f)
        lfoPhase -= 1.0f;

    // Advance delay write position
    delayWritePos = (delayWritePos + 1) % MAX_DELAY_SAMPLES;

    // Mix dry and wet
    return input * (1.0f - mix) + delayed * mix;
}

void LeslieSimulator::reset()
{
    lfoPhase = 0.0f;
    delayWritePos = 0;
    std::memset(delayBuffer, 0, sizeof(delayBuffer));
}
