#include "Voice.h"
#include <dsp/WaveformGenerators.h>

Voice::Voice()
{
    // Set default envelopes
    ampEnvelope.setAttack(0.01f);
    ampEnvelope.setDecay(0.1f);
    ampEnvelope.setSustain(0.8f);
    ampEnvelope.setRelease(0.3f);

    filterEnvelope.setAttack(0.05f);
    filterEnvelope.setDecay(0.2f);
    filterEnvelope.setSustain(0.5f);
    filterEnvelope.setRelease(0.3f);
}

void Voice::noteOn(int midiNote, float velocity_, double sampleRate)
{
    currentNote = midiNote;
    velocity = velocity_;
    active = true;

    // Convert MIDI note to frequency
    float frequency = AudioForge::DSP::WaveformGenerators::midiNoteToFrequency(midiNote);

    oscillator.setFrequency(frequency);
    oscillator.setActive(true);

    // Trigger envelopes
    ampEnvelope.trigger();
    filterEnvelope.trigger();

    // Reset filter
    filter.reset();
}

void Voice::noteOff()
{
    ampEnvelope.release();
    filterEnvelope.release();
}

bool Voice::isActive() const
{
    return ampEnvelope.isActive() || filterEnvelope.isActive();
}

float Voice::getLevel() const
{
    return ampEnvelope.getLevel();
}

void Voice::reset()
{
    active = false;
    currentNote = -1;
    velocity = 0.0f;
    oscillator.reset();
    ampEnvelope.reset();
    filterEnvelope.reset();
    filter.reset();
}

float Voice::processSample(double sampleRate,
                          float wavetablePosition,
                          float filterCutoff,
                          float filterResonance,
                          int filterType,
                          float filterEnvAmount)
{
    if (!isActive())
        return 0.0f;

    // Get envelope values
    float deltaTime = static_cast<float>(1.0 / sampleRate);
    float ampEnv = ampEnvelope.getNextValue(deltaTime);
    float filtEnv = filterEnvelope.getNextValue(deltaTime);

    // Set wavetable position
    oscillator.setPosition(wavetablePosition);

    // Generate oscillator output
    float sample = oscillator.processSample(sampleRate);

    // Apply filter with envelope modulation
    float modulatedCutoff = filterCutoff + (filtEnv * filterEnvAmount * 10000.0f);
    modulatedCutoff = std::max(20.0f, std::min(20000.0f, modulatedCutoff));

    filter.setParameters(modulatedCutoff, filterResonance, sampleRate);
    filter.setFilterType(static_cast<MultiModeFilter::FilterType>(filterType));

    sample = filter.processSample(sample);

    // Apply amplitude envelope
    sample *= ampEnv * velocity;

    return sample;
}

void Voice::updateEnvelopes(float ampAttack, float ampDecay, float ampSustain, float ampRelease,
                           float filtAttack, float filtDecay, float filtSustain, float filtRelease)
{
    ampEnvelope.setAttack(ampAttack);
    ampEnvelope.setDecay(ampDecay);
    ampEnvelope.setSustain(ampSustain);
    ampEnvelope.setRelease(ampRelease);

    filterEnvelope.setAttack(filtAttack);
    filterEnvelope.setDecay(filtDecay);
    filterEnvelope.setSustain(filtSustain);
    filterEnvelope.setRelease(filtRelease);
}
