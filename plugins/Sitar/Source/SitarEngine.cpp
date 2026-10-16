#include "SitarEngine.h"

namespace
{
    // Damping of the string loops, in hertz: enough to darken the tone as it
    // rings, little enough to keep the bright attack. An A3's 8th harmonic
    // fades about 9 dB a second faster than its fundamental.
    constexpr float stringDampingHz = 17600.0f;

    // A released note fades 60 dB over this many seconds
    constexpr float releaseSeconds = 1.0f;

    // The drone strings fall 60 dB over this many seconds, about the tail the
    // processor reports
    constexpr float droneRingSeconds = 5.0f;

    // Length of the noise burst that plucks a string
    constexpr int pluckSamples = 20;
}

SitarEngine::SitarEngine()
{
    mainString_.dampingHz = stringDampingHz;
    updateSympatheticTuning();
}

void SitarEngine::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    sampleRate_ = sampleRate;
    releaseCoeff_ = std::pow(0.001f, 1.0f / (releaseSeconds * static_cast<float>(sampleRate)));
    reset();
}

void SitarEngine::reset()
{
    mainString_.reset();
    for (auto& string : sympatheticStrings_)
        string.reset();

    vibratoPhase_ = 0.0f;
    releaseLevel_ = 1.0f;
}

float SitarEngine::processSample(float baseFrequency, float velocity, bool noteOn, int samplesSinceNoteOn)
{
    // Apply vibrato
    vibratoPhase_ += vibratoRate_ * juce::MathConstants<float>::twoPi / static_cast<float>(sampleRate_);
    if (vibratoPhase_ > juce::MathConstants<float>::twoPi)
        vibratoPhase_ -= juce::MathConstants<float>::twoPi;

    float vibrato = 1.0f + vibratoDepth_ * std::sin(vibratoPhase_);
    float frequency = baseFrequency * vibrato * std::pow(2.0f, pitchBendAmount_ / 12.0f);

    // Tune the main string; Decay is the time it takes to fall 20 dB
    mainString_.tune(frequency, 0.1f, decayTime_, sampleRate_);

    // Pluck excitation (noise burst at note start)
    float excitation = 0.0f;
    if (noteOn && samplesSinceNoteOn < pluckSamples)
    {
        excitation = (random_.nextFloat() * 2.0f - 1.0f) * velocity;
    }

    // Process main string
    float mainOutput = mainString_.process(excitation);

    // Apply jawari (bridge buzz) - soft clipping with harmonic enrichment
    mainOutput = applyJawari(mainOutput);

    // Envelope
    float attackEnv = 1.0f;
    if (samplesSinceNoteOn < static_cast<int>(attackTime_ * sampleRate_))
    {
        attackEnv = static_cast<float>(samplesSinceNoteOn) / (attackTime_ * static_cast<float>(sampleRate_));
    }

    mainOutput *= attackEnv * velocity;

    // Release, counted from note-off
    if (!noteOn)
        releaseLevel_ *= releaseCoeff_;

    // Process sympathetic strings (excited by main string output)
    float sympatheticOutput = 0.0f;
    for (int i = 0; i < NUM_SYMPATHETIC_STRINGS; ++i)
    {
        float sympatheticFreq = baseFrequency * sympatheticTuning_[i];
        sympatheticStrings_[i].setFrequency(sympatheticFreq, sampleRate_);
        sympatheticStrings_[i].feedback = 0.998f; // High feedback for long sustain

        // Feed main string output into sympathetic strings
        sympatheticOutput += sympatheticStrings_[i].process(mainOutput * 0.1f);
    }
    sympatheticOutput *= sympatheticResonance_ / static_cast<float>(NUM_SYMPATHETIC_STRINGS);

    return (mainOutput + sympatheticOutput) * 0.5f * releaseLevel_;
}

float SitarEngine::applyJawari(float sample)
{
    // Jawari bridge buzz - soft clipping with adjustable drive
    float drive = 1.0f + jawariAmount_ * 8.0f;
    float shaped = std::tanh(sample * drive);

    // Mix dry/wet based on jawari amount
    return sample * (1.0f - jawariAmount_ * 0.5f) + shaped * jawariAmount_ * 0.5f;
}

void SitarEngine::updateSympatheticTuning()
{
    auto ratios = getScaleRatios();
    for (int i = 0; i < NUM_SYMPATHETIC_STRINGS; ++i)
    {
        sympatheticTuning_[i] = ratios[i];
    }
}

std::array<float, SitarEngine::NUM_SYMPATHETIC_STRINGS> SitarEngine::getScaleRatios()
{
    std::array<float, NUM_SYMPATHETIC_STRINGS> ratios{};

    switch (ragaScale_)
    {
        case RagaScale::Chromatic:
            // All 12 semitones + tonic
            for (int i = 0; i < NUM_SYMPATHETIC_STRINGS; ++i)
                ratios[i] = std::pow(2.0f, static_cast<float>(i) / 12.0f);
            break;

        case RagaScale::Bhairavi:
            // C Db Eb F G Ab Bb (Bhairavi scale degrees)
            {
                std::array<int, 13> semitones = {0, 1, 3, 5, 7, 8, 10, 12, 13, 15, 17, 19, 20};
                for (int i = 0; i < NUM_SYMPATHETIC_STRINGS; ++i)
                    ratios[i] = std::pow(2.0f, static_cast<float>(semitones[i]) / 12.0f);
            }
            break;

        case RagaScale::Yaman:
            // C D E F# G A B (Yaman/Kalyan)
            {
                std::array<int, 13> semitones = {0, 2, 4, 6, 7, 9, 11, 12, 14, 16, 18, 19, 21};
                for (int i = 0; i < NUM_SYMPATHETIC_STRINGS; ++i)
                    ratios[i] = std::pow(2.0f, static_cast<float>(semitones[i]) / 12.0f);
            }
            break;

        case RagaScale::Kafi:
            // C D Eb F G A Bb (Kafi)
            {
                std::array<int, 13> semitones = {0, 2, 3, 5, 7, 9, 10, 12, 14, 15, 17, 19, 21};
                for (int i = 0; i < NUM_SYMPATHETIC_STRINGS; ++i)
                    ratios[i] = std::pow(2.0f, static_cast<float>(semitones[i]) / 12.0f);
            }
            break;
    }

    return ratios;
}

void SitarDrone::prepareToPlay(double sampleRate)
{
    sampleRate_ = sampleRate;

    for (auto& string : strings_)
    {
        string.loop.dampingHz = stringDampingHz;
        string.loop.tune(string.frequency, 0.001f, droneRingSeconds, sampleRate);
    }

    reset();
}

void SitarDrone::reset()
{
    cycling_ = false;

    for (auto& string : strings_)
    {
        string.loop.reset();
        string.samplesToPluck = 0;
        string.excitationLeft = 0;
    }
}

void SitarDrone::render(juce::AudioBuffer<float>& buffer, int noteOnSample, bool notesSounding)
{
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        // A note starting while the drone is idle plucks both strings at once
        if (sample == noteOnSample)
            cycling_ = true;

        float droneOutput = 0.0f;
        for (auto& string : strings_)
        {
            if (cycling_ && --string.samplesToPluck <= 0)
            {
                // Noise for one whole period, the classic Karplus-Strong
                // pluck: a full, sustained tone rather than a click
                string.excitationLeft = static_cast<int>(sampleRate_ / string.frequency);
                string.samplesToPluck = static_cast<int>(string.pluckIntervalSeconds * sampleRate_);
            }

            float excitation = 0.0f;
            if (string.excitationLeft > 0)
            {
                --string.excitationLeft;
                excitation = random_.nextFloat() * 2.0f - 1.0f;
            }

            droneOutput += string.loop.process(excitation);
        }

        // Same mix as the drone had inside each voice
        droneOutput *= level_ * 0.3f * 0.5f;

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            buffer.addSample(channel, sample, droneOutput);
    }

    // Once every note has finished, the next note starts a fresh cycle
    if (!notesSounding)
    {
        cycling_ = false;
        for (auto& string : strings_)
            string.samplesToPluck = 0;
    }
}
