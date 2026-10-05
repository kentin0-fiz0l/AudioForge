/**
 * Output Level Tests
 *
 * ElectricPiano and Polysynth each have an Output Level control. At its
 * default it changes nothing, and moving it changes the output by exactly
 * the number of decibels it shows.
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>

#include "../../plugins/ElectricPiano/Source/PluginProcessor.h"
#include "../../plugins/Polysynth/Source/PluginProcessor.h"

class OutputLevelTests : public juce::UnitTest
{
public:
    OutputLevelTests()
        : juce::UnitTest("Output Level", "Plugins")
    {
    }

    void runTest() override
    {
        testInstrument<ElectricPianoProcessor>("ElectricPiano");
        testInstrument<PolysynthProcessor>("Polysynth");
    }

private:
    static constexpr double sampleRate = 48000.0;
    static constexpr int blockSize = 512;

    // Holds middle C for half a second and returns the RMS level of what
    // came out, with Output Level set to the given value first. A negative
    // infinity argument means leave the control alone.
    template <typename Processor>
    float levelOfANote(float outputLevelDb, bool setLevel)
    {
        Processor processor;

        if (setLevel)
        {
            auto* param = processor.getValueTreeState().getParameter("outputLevel");
            param->setValueNotifyingHost(param->convertTo0to1(outputLevelDb));
        }

        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(2, blockSize);
        double sumOfSquares = 0.0;
        int count = 0;

        for (int block = 0; block < static_cast<int>(0.5 * sampleRate / blockSize); ++block)
        {
            juce::MidiBuffer midi;
            if (block == 0)
                midi.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8) 100), 0);

            buffer.clear();
            processor.processBlock(buffer, midi);

            for (int i = 0; i < blockSize; ++i)
            {
                sumOfSquares += buffer.getSample(0, i) * buffer.getSample(0, i);
                ++count;
            }
        }

        return static_cast<float>(std::sqrt(sumOfSquares / count));
    }

    template <typename Processor>
    void testInstrument(const juce::String& name)
    {
        beginTest(name + " has an Output Level control that defaults to no change");

        Processor processor;
        auto* param = processor.getValueTreeState().getParameter("outputLevel");
        expect(param != nullptr, name + " has no Output Level parameter");
        if (param == nullptr)
            return;

        expectWithinAbsoluteError(param->convertFrom0to1(param->getDefaultValue()), 0.0f, 0.001f);
        expect(processor.getParameters().getLast() == param,
               "Output Level should be the last parameter, so the existing ones keep their positions");

        const float untouched = levelOfANote<Processor>(0.0f, false);
        const float atZero = levelOfANote<Processor>(0.0f, true);
        expect(untouched > 0.001f, name + " made no sound");
        expectWithinAbsoluteError(juce::Decibels::gainToDecibels(atZero / untouched), 0.0f, 0.3f);

        beginTest(name + "'s Output Level changes the output by the decibels it shows");

        for (float db : { -24.0f, -12.0f, -6.0f, 6.0f, 12.0f })
        {
            const float level = levelOfANote<Processor>(db, true);
            expectWithinAbsoluteError(juce::Decibels::gainToDecibels(level / atZero), db, 0.3f);
        }
    }
};

static OutputLevelTests outputLevelTests;
