#include "Voice.h"
#include <dsp/WaveformGenerators.h>

Voice::Voice()
    : currentNote(-1)
    , velocity(0.0f)
    , active(false)
    , clickAmount(0.0f)
    , clickDecayTime(0.05f)
{
}

void Voice::prepare(double sampleRate, int maxBlockSize)
{
    leslie.prepare(sampleRate, maxBlockSize);

    // Fast percussion envelope
    percussionEnv.setAttack(0.001f);   // Instant attack
    percussionEnv.setDecay(0.05f);     // Fast decay
    percussionEnv.setSustain(0.0f);    // No sustain
    percussionEnv.setRelease(0.001f);  // Instant release
}

void Voice::noteOn(int midiNote, float vel, double sampleRate)
{
    currentNote = midiNote;
    velocity = vel;
    active = true;

    // Convert MIDI note to frequency
    float frequency = AudioForge::DSP::WaveformGenerators::midiNoteToFrequency(midiNote);
    drawbarEngine.setFrequency(frequency);
    drawbarEngine.reset();

    // Trigger percussion envelope
    percussionEnv.trigger();

    leslie.reset();
}

void Voice::noteOff()
{
    active = false;
    percussionEnv.release();
}

void Voice::reset()
{
    active = false;
    currentNote = -1;
    velocity = 0.0f;
    drawbarEngine.reset();
    leslie.reset();
    percussionEnv.release();
}

void Voice::processSample(double sampleRate,
                         const float drawbars[9],
                         bool leslieSpeed,
                         float leslieDepth,
                         float leslieMix,
                         float percussionClick,
                         float percussionDecay,
                         float& outLeft,
                         float& outRight)
{
    if (!active)
    {
        outLeft = outRight = 0.0f;
        return;
    }

    // Update percussion envelope decay
    percussionEnv.setDecay(percussionDecay);

    // Set drawbars
    drawbarEngine.setDrawbars(drawbars);

    // Generate organ sound
    float organSample = drawbarEngine.processSample(sampleRate);

    // Add percussion click (high-frequency transient)
    float deltaTime = 1.0f / static_cast<float>(sampleRate);
    float percEnvValue = percussionEnv.getNextValue(deltaTime);
    if (percussionClick > 0.0f && percEnvValue > 0.0f)
    {
        // Add bright harmonic click
        organSample += percEnvValue * percussionClick * 0.3f;
    }

    // Apply Leslie effect
    leslie.setSpeed(leslieSpeed);
    leslie.setDepth(leslieDepth);
    leslie.setMix(leslieMix);

    float output = leslie.processSample(organSample);

    // Apply velocity
    output *= velocity;

    // Stereo output (mono for now, Leslie adds stereo width)
    outLeft = outRight = output;
}
