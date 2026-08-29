#include "ADSREnvelope.h"
#include <juce_core/juce_core.h>
#include <algorithm>

ADSREnvelope::ADSREnvelope()
{
}

void ADSREnvelope::trigger()
{
    stage = Stage::Attack;
    stageTime = 0.0f;
    // Don't reset level - allows for retriggering during release
}

void ADSREnvelope::release()
{
    if (stage == Stage::Off)
        return;

    releaseStartLevel = level;
    stage = Stage::Release;
    stageTime = 0.0f;
}

float ADSREnvelope::getNextValue(float deltaTime)
{
    updateStage(deltaTime);
    return level;
}

void ADSREnvelope::setAttack(float seconds)
{
    attackTime = std::max(0.001f, seconds);
}

void ADSREnvelope::setDecay(float seconds)
{
    decayTime = std::max(0.001f, seconds);
}

void ADSREnvelope::setSustain(float levelValue)
{
    sustainLevel = std::max(0.0f, std::min(1.0f, levelValue));
}

void ADSREnvelope::setRelease(float seconds)
{
    releaseTime = std::max(0.001f, seconds);
}

void ADSREnvelope::reset()
{
    stage = Stage::Off;
    level = 0.0f;
    stageTime = 0.0f;
    releaseStartLevel = 0.0f;
}

void ADSREnvelope::updateStage(float deltaTime)
{
    stageTime += deltaTime;

    switch (stage)
    {
        case Stage::Off:
            level = 0.0f;
            break;

        case Stage::Attack:
            if (attackTime <= 0.001f)
            {
                // Instant attack
                level = 1.0f;
                stage = Stage::Decay;
                stageTime = 0.0f;
            }
            else
            {
                // Linear attack from current level to 1.0
                level = std::min(1.0f, stageTime / attackTime);

                if (level >= 1.0f)
                {
                    stage = Stage::Decay;
                    stageTime = 0.0f;
                }
            }
            break;

        case Stage::Decay:
            if (decayTime <= 0.001f)
            {
                // Instant decay
                level = sustainLevel;
                stage = Stage::Sustain;
                stageTime = 0.0f;
            }
            else
            {
                // Exponential decay from 1.0 to sustain level
                float decayAmount = (1.0f - sustainLevel) * (stageTime / decayTime);
                level = 1.0f - std::min(decayAmount, 1.0f - sustainLevel);

                if (level <= sustainLevel || stageTime >= decayTime)
                {
                    level = sustainLevel;
                    stage = Stage::Sustain;
                    stageTime = 0.0f;
                }
            }
            break;

        case Stage::Sustain:
            // Hold at sustain level
            level = sustainLevel;
            break;

        case Stage::Release:
            if (releaseTime <= 0.001f)
            {
                // Instant release
                level = 0.0f;
                stage = Stage::Off;
            }
            else
            {
                // Linear release from releaseStartLevel to 0
                float releaseProgress = stageTime / releaseTime;
                level = std::max(0.0f, releaseStartLevel * (1.0f - releaseProgress));

                if (level <= 0.001f || stageTime >= releaseTime)
                {
                    // Envelope finished
                    level = 0.0f;
                    stage = Stage::Off;
                }
            }
            break;
    }
}
