#include "StringEngine.h"

StringEngine::StringEngine()
{
    // Initialize delay line buffers
    for (auto& delayLine : delayLines_)
    {
        delayLine.buffer.fill(0.0f);
        delayLine.writePos = 0;
        delayLine.readPos = 0;
    }
}

void StringEngine::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    sampleRate_ = sampleRate;

    // Reset delay lines
    for (auto& delayLine : delayLines_)
    {
        delayLine.buffer.fill(0.0f);
        delayLine.prevOutput = 0.0f;
    }
}

void StringEngine::setStringType(StringType type)
{
    currentString_ = type;
}

void StringEngine::setBrightness(float brightness)
{
    brightness_ = juce::jlimit(0.0f, 1.0f, brightness);
}

void StringEngine::setAttack(float attack)
{
    attack_ = juce::jlimit(0.0f, 1.0f, attack);
}

void StringEngine::setDecay(float decay)
{
    decay_ = juce::jlimit(0.0f, 1.0f, decay);
}

void StringEngine::setVibratoDepth(float depth)
{
    vibratoDepth_ = juce::jlimit(0.0f, 1.0f, depth);
}

void StringEngine::setVibratoRate(float rate)
{
    vibratoRate_ = juce::jlimit(0.0f, 1.0f, rate);
}

void StringEngine::setEnsembleSize(int size)
{
    ensembleSize_ = juce::jlimit(1, 4, size);
}

float StringEngine::processSample(float frequency, float velocity, bool isNoteOn, int samplesSinceNoteOn)
{
    // Apply vibrato to frequency
    float modulatedFrequency = processVibrato(frequency);

    // Envelope
    if (isNoteOn)
    {
        // Note just started - excite the strings
        if (samplesSinceNoteOn == 0)
        {
            for (int i = 0; i < ensembleSize_; ++i)
            {
                exciteString(i, velocity);
            }
        }

        targetAmplitude_ = velocity;

        // Attack rate based on attack parameter
        attackRate_ = 0.001f + (attack_ * 0.05f);  // Slower to faster attack
        releaseRate_ = 0.995f + (decay_ * 0.004f); // Slower to faster decay
    }
    else
    {
        targetAmplitude_ = 0.0f;
    }

    // Envelope smoothing
    if (currentAmplitude_ < targetAmplitude_)
    {
        // Attack
        currentAmplitude_ += (targetAmplitude_ - currentAmplitude_) * attackRate_;
    }
    else
    {
        // Release
        currentAmplitude_ *= releaseRate_;
    }

    // Generate ensemble voices
    float sample = 0.0f;
    for (int i = 0; i < ensembleSize_; ++i)
    {
        // Apply ensemble detuning
        float detuneCents = ensembleDetune_[i];
        float detuneRatio = std::pow(2.0f, detuneCents / 1200.0f);
        float detunedFreq = modulatedFrequency * detuneRatio;

        // Generate Karplus-Strong sample
        float voice = processKarplusStrong(detunedFreq, i);
        sample += voice;
    }

    // Normalize by ensemble size
    sample /= static_cast<float>(ensembleSize_);

    // Apply amplitude envelope
    sample *= currentAmplitude_;

    return sample;
}

void StringEngine::reset()
{
    currentAmplitude_ = 0.0f;
    targetAmplitude_ = 0.0f;
    vibratoPhase_ = 0.0f;

    for (auto& delayLine : delayLines_)
    {
        delayLine.buffer.fill(0.0f);
        delayLine.prevOutput = 0.0f;
    }
}

float StringEngine::processKarplusStrong(float frequency, int voiceIndex)
{
    auto& delayLine = delayLines_[voiceIndex];

    // Calculate delay length from frequency
    float delayLengthFloat = static_cast<float>(sampleRate_) / frequency;
    int delayLength = static_cast<int>(delayLengthFloat);
    delayLength = juce::jlimit(10, MAX_DELAY_SIZE - 1, delayLength);

    delayLine.delayLength = delayLength;

    // Calculate damping factor based on brightness and string type
    float baseDamping;
    switch (currentString_)
    {
        case StringType::Violin:  baseDamping = 0.996f; break; // Brightest
        case StringType::Viola:   baseDamping = 0.995f; break;
        case StringType::Cello:   baseDamping = 0.994f; break;
        case StringType::Bass:    baseDamping = 0.992f; break; // Darkest
        default: baseDamping = 0.995f;
    }

    // Brightness control affects damping
    delayLine.damping = baseDamping + (brightness_ * 0.003f);

    // Read from delay line
    float delayOutput = delayLine.buffer[delayLine.readPos];

    // Low-pass filter (damping)
    float filtered = (delayOutput + delayLine.prevOutput) * 0.5f * delayLine.damping;
    delayLine.prevOutput = filtered;

    // Write filtered output back to delay line (feedback)
    delayLine.buffer[delayLine.writePos] = filtered;

    // Advance read/write pointers
    delayLine.writePos = (delayLine.writePos + 1) % delayLength;
    delayLine.readPos = (delayLine.readPos + 1) % delayLength;

    return delayOutput;
}

void StringEngine::exciteString(int voiceIndex, float velocity)
{
    auto& delayLine = delayLines_[voiceIndex];

    // Fill delay line with noise burst (pluck/bow attack)
    float excitationLevel = velocity * (0.3f + attack_ * 0.7f);

    for (int i = 0; i < delayLine.delayLength; ++i)
    {
        // Generate noise
        float noise = (random_.nextFloat() * 2.0f - 1.0f) * excitationLevel;

        // Softer attack = smoother noise envelope
        if (attack_ < 0.5f)
        {
            float envelope = 1.0f - (static_cast<float>(i) / delayLine.delayLength);
            noise *= envelope;
        }

        delayLine.buffer[i] = noise;
    }

    // Reset pointers
    delayLine.writePos = 0;
    delayLine.readPos = 0;
    delayLine.prevOutput = 0.0f;
}

float StringEngine::processVibrato(float frequency)
{
    if (vibratoDepth_ <= 0.0f)
        return frequency;

    // Vibrato rate: 4-7 Hz (typical string vibrato)
    float vibratoHz = 4.0f + (vibratoRate_ * 3.0f);

    // Generate vibrato LFO
    float lfo = std::sin(vibratoPhase_ * juce::MathConstants<float>::twoPi);

    // Advance vibrato phase
    vibratoPhase_ += vibratoHz / static_cast<float>(sampleRate_);
    while (vibratoPhase_ >= 1.0f)
        vibratoPhase_ -= 1.0f;

    // Apply vibrato (pitch modulation ±2% for strings, more subtle than brass)
    float pitchMod = 1.0f + (lfo * vibratoDepth_ * 0.02f);

    return frequency * pitchMod;
}
