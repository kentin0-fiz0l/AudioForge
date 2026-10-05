/**
 * Polysynth Tests
 *
 * The Filter Cutoff knob covers three decades, 20 Hz to 20 kHz. Its travel
 * has to be spread over them the way hearing is, with 1 kHz in the middle,
 * or the whole useful sweep is crammed into the bottom of the knob.
 */

#include <juce_audio_processors/juce_audio_processors.h>

#include "../../plugins/Polysynth/Source/PluginProcessor.h"

class PolysynthTests : public juce::UnitTest
{
public:
    PolysynthTests()
        : juce::UnitTest("Polysynth", "Plugins")
    {
    }

    void runTest() override
    {
        beginTest("Filter Cutoff has 1 kHz at the middle of its travel");

        PolysynthProcessor processor;
        juce::RangedAudioParameter* cutoff = nullptr;

        for (auto* param : processor.getParameters())
            if (param->getName(64) == "Filter Cutoff")
                cutoff = dynamic_cast<juce::RangedAudioParameter*>(param);

        expect(cutoff != nullptr, "Polysynth has no Filter Cutoff parameter");
        if (cutoff == nullptr)
            return;

        expectWithinAbsoluteError(cutoff->convertFrom0to1(0.0f), 20.0f, 0.5f);
        expectWithinAbsoluteError(cutoff->convertFrom0to1(0.5f), 1000.0f, 5.0f);
        expectWithinAbsoluteError(cutoff->convertFrom0to1(1.0f), 20000.0f, 0.5f);

        // The default stays where it was, at 2 kHz, now past the middle
        expectWithinAbsoluteError(cutoff->convertFrom0to1(cutoff->getDefaultValue()), 2000.0f, 1.0f);
        expect(cutoff->getDefaultValue() > 0.5f && cutoff->getDefaultValue() < 0.7f,
               "2 kHz sits at " + juce::String(cutoff->getDefaultValue(), 2) + " of the travel");
    }
};

static PolysynthTests polysynthTests;
