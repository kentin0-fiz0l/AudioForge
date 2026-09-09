#include "SitarEngine.h"

SitarEngine::SitarEngine()
{
    updateSympatheticTuning();
}

void SitarEngine::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    sampleRate_ = sampleRate;
    reset();
}

void SitarEngine::reset()
{
    mainString_.reset();
    for (auto& string : sympatheticStrings_)
        string.reset();
    droneString1_.reset();
    droneString2_.reset();

    vibratoPhase_ = 0.0f;
    dronePhase1_ = 0.0f;
    dronePhase2_ = 0.0f;
}

float SitarEngine::processSample(float baseFrequency, float velocity, bool noteOn, int samplesSinceNoteOn)
{
    // Apply vibrato
    vibratoPhase_ += vibratoRate_ * juce::MathConstants<float>::twoPi / static_cast<float>(sampleRate_);
    if (vibratoPhase_ > juce::MathConstants<float>::twoPi)
        vibratoPhase_ -= juce::MathConstants<float>::twoPi;

    float vibrato = 1.0f + vibratoDepth_ * std::sin(vibratoPhase_);
    float frequency = baseFrequency * vibrato * std::pow(2.0f, pitchBendAmount_ / 12.0f);

    // Update main string delay length
    float delayLength = static_cast<float>(sampleRate_) / frequency;
    mainString_.delayLength = delayLength;

    // Pluck excitation (noise burst at note start)
    float excitation = 0.0f;
    if (noteOn && samplesSinceNoteOn < 20)
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

    float decayEnv = noteOn ? 1.0f : std::pow(0.9999f, static_cast<float>(samplesSinceNoteOn));
    mainOutput *= attackEnv * decayEnv * velocity;

    // Process sympathetic strings (excited by main string output)
    float sympatheticOutput = 0.0f;
    for (int i = 0; i < NUM_SYMPATHETIC_STRINGS; ++i)
    {
        float sympatheticFreq = baseFrequency * sympatheticTuning_[i];
        sympatheticStrings_[i].delayLength = static_cast<float>(sampleRate_) / sympatheticFreq;
        sympatheticStrings_[i].feedback = 0.998f; // High feedback for long sustain

        // Feed main string output into sympathetic strings
        sympatheticOutput += sympatheticStrings_[i].process(mainOutput * 0.1f);
    }
    sympatheticOutput *= sympatheticResonance_ / static_cast<float>(NUM_SYMPATHETIC_STRINGS);

    // Drone strings (continuous excitation)
    float droneOutput = 0.0f;
    if (droneLevel_ > 0.0f)
    {
        // Tonic drone
        float droneFreq1 = 130.81f; // C3
        droneString1_.delayLength = static_cast<float>(sampleRate_) / droneFreq1;
        droneString1_.feedback = 0.9995f;

        // Fifth drone
        float droneFreq2 = droneFreq1 * 1.5f; // G3
        droneString2_.delayLength = static_cast<float>(sampleRate_) / droneFreq2;
        droneString2_.feedback = 0.9995f;

        // Continuous pluck-like excitation for drones
        dronePhase1_ += 1.0f;
        dronePhase2_ += 1.0f;

        float droneExcite1 = 0.0f;
        float droneExcite2 = 0.0f;

        if (dronePhase1_ > 4.0f * sampleRate_) // Pluck every 4 seconds
        {
            droneExcite1 = (random_.nextFloat() * 2.0f - 1.0f) * 0.3f;
            dronePhase1_ = 0.0f;
        }

        if (dronePhase2_ > 5.0f * sampleRate_) // Slightly different timing
        {
            droneExcite2 = (random_.nextFloat() * 2.0f - 1.0f) * 0.3f;
            dronePhase2_ = 0.0f;
        }

        droneOutput = droneString1_.process(droneExcite1) + droneString2_.process(droneExcite2);
        droneOutput *= droneLevel_ * 0.3f;
    }

    return (mainOutput + sympatheticOutput + droneOutput) * 0.5f;
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
