#include "PluginProcessor.h"
#include <dsp/WaveformGenerators.h>

Voice::Voice()
{
    // Set default envelopes
    carrier.setEnvelope(0.01f, 0.1f, 0.7f, 0.3f);
    modulator.setEnvelope(0.01f, 0.1f, 0.7f, 0.3f);
}

void Voice::noteOn(int midiNote, float velocity_, double sampleRate)
{
    currentNote = midiNote;
    velocity = velocity_;
    active = true;

    // Convert MIDI note to frequency
    float frequency = AudioForge::DSP::WaveformGenerators::midiNoteToFrequency(midiNote);

    // Set both operators to the note frequency
    carrier.setFrequency(frequency);
    modulator.setFrequency(frequency);

    // Trigger envelopes
    carrier.trigger();
    modulator.trigger();
}

void Voice::noteOff()
{
    carrier.release();
    modulator.release();
}

bool Voice::isActive() const
{
    return carrier.isActive() || modulator.isActive();
}

float Voice::getLevel() const
{
    return carrier.getEnvelopeLevel();
}

void Voice::reset()
{
    active = false;
    currentNote = -1;
    velocity = 0.0f;
    carrier.reset();
    modulator.reset();
}

float Voice::processSample(double sampleRate, float modulatorRatio, float modulatorDepth)
{
    if (!isActive())
        return 0.0f;

    // Update modulator ratio
    modulator.setRatio(modulatorRatio);

    // Generate modulation signal
    float modSignal = modulator.processSample(sampleRate, 0.0f);

    // Use modulator output to modulate carrier phase (FM synthesis)
    float output = carrier.processSample(sampleRate, modSignal * modulatorDepth);

    return output * velocity;
}

void Voice::updateEnvelopes(float carrierAttack, float carrierDecay, float carrierSustain, float carrierRelease,
                            float modAttack, float modDecay, float modSustain, float modRelease)
{
    carrier.setEnvelope(carrierAttack, carrierDecay, carrierSustain, carrierRelease);
    modulator.setEnvelope(modAttack, modDecay, modSustain, modRelease);
}
