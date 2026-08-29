#include "Voice.h"
#include <juce_core/juce_core.h>
#include <dsp/AudioForgeDSP.h>
#include <cmath>
#include <algorithm>

SynthVoice::SynthVoice()
{
}

void SynthVoice::noteOn(int midiNoteNumber, float noteVelocity, double sampleRate)
{
    active = true;
    currentNote = midiNoteNumber;
    velocity = noteVelocity;

    // Convert MIDI note to frequency using shared DSP library
    float frequency = AudioForge::DSP::WaveformGenerators::midiNoteToFrequency(midiNoteNumber);

    // Set frequency for all oscillators in the bank
    oscillatorBank.setFrequency(frequency, sampleRate);

    // Trigger both envelopes
    ampEnvelope.trigger();
    filterEnvelope.trigger();
}

void SynthVoice::noteOff()
{
    if (!active)
        return;

    // Release both envelopes
    ampEnvelope.release();
    filterEnvelope.release();
}

float SynthVoice::processSample(double sampleRate,
                                int waveform,
                                float attack,
                                float decay,
                                float sustain,
                                float release,
                                float filterCutoff,
                                float filterResonance)
{
    if (!active)
        return 0.0f;

    // Update envelopes
    float deltaTime = 1.0f / static_cast<float>(sampleRate);

    // Set ADSR parameters
    ampEnvelope.setAttack(attack);
    ampEnvelope.setDecay(decay);
    ampEnvelope.setSustain(sustain);
    ampEnvelope.setRelease(release);

    filterEnvelope.setAttack(attack * 0.5f);  // Faster filter attack
    filterEnvelope.setDecay(decay * 0.8f);
    filterEnvelope.setSustain(sustain);
    filterEnvelope.setRelease(release * 0.7f);

    // Get envelope values
    float ampEnvValue = ampEnvelope.getNextValue(deltaTime);
    float filterEnvValue = filterEnvelope.getNextValue(deltaTime);

    // Deactivate voice when amp envelope completes release
    if (!ampEnvelope.isActive())
    {
        active = false;
        return 0.0f;
    }

    // Update LFOs
    float lfo1Value = lfo1.getNextSample(sampleRate);
    float lfo2Value = lfo2.getNextSample(sampleRate);

    // Collect modulation source values
    ModulationMatrix::SourceValues sources;
    sources.lfo1 = lfo1Value;
    sources.lfo2 = lfo2Value;
    sources.ampEnvelope = ampEnvValue;
    sources.filterEnvelope = filterEnvValue;
    sources.velocity = velocity;
    sources.modWheel = 0.0f; // TODO: Get from MIDI CC

    // Get modulation for filter cutoff
    float cutoffMod = modulationMatrix.getModulation(
        ModulationMatrix::Destination::FilterCutoff, sources);

    // Apply modulation to filter cutoff (±2 octaves)
    float modulatedCutoff = filterCutoff * std::pow(2.0f, cutoffMod * 2.0f);
    modulatedCutoff = std::max(20.0f, std::min(20000.0f, modulatedCutoff));

    // Generate sample from oscillator bank (automatically mixes all enabled oscillators)
    float sample = oscillatorBank.getNextSample();

    // Apply amplitude envelope and velocity
    sample *= ampEnvValue * velocity;

    // Apply low-pass filter
    // Only recalculate coefficients when parameters change (performance optimization)
    if (modulatedCutoff != lastFilterCutoff || filterResonance != lastFilterResonance)
    {
        auto filterCoeffs = AudioForge::DSP::FilterDesign::makeLowPass(
            modulatedCutoff,
            static_cast<float>(sampleRate),
            filterResonance);
        filter.setCoefficients(filterCoeffs);
        lastFilterCutoff = modulatedCutoff;
        lastFilterResonance = filterResonance;
    }

    sample = filter.processSample(sample);

    return sample;
}

void SynthVoice::reset()
{
    active = false;
    currentNote = -1;
    velocity = 0.0f;
    ampEnvelope.reset();
    filterEnvelope.reset();
    oscillatorBank.reset();
    filter.reset();
    lastFilterCutoff = -1.0f;
    lastFilterResonance = -1.0f;
}

