/**
 * DrumMachine Plugin Tests
 *
 * Plays the processor against a fake host transport and checks when hits
 * sound:
 * - Hits start on their own sample, not at the start of the audio block
 * - Swing delays the off-beat 16ths and leaves the on-beat ones alone
 * - Fills work in every style and their crash is kept
 * - Settings and the pattern survive a save and restore
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <algorithm>
#include <tuple>

#include "../../plugins/DrumMachine/Source/PluginProcessor.h"

class DrumMachinePluginTests : public juce::UnitTest
{
public:
    DrumMachinePluginTests()
        : juce::UnitTest("DrumMachine Plugin", "Plugins")
    {
    }

    void runTest() override
    {
        testHitsStartOnTheirOwnSample();
        testPatternLoopsEveryBar();
        testSwingDelaysOffBeats();
        testSwingLeavesOnBeatsAlone();
        testSwingDoesNotChangeThePattern();
        testFillsInEveryStyle();
        testStateRoundTrip();
        testIgnoresForeignState();
    }

private:
    static constexpr double sampleRate = 48000.0;
    static constexpr int blockSize = 512;
    static constexpr double bpm = 120.0;
    // At 120 BPM and 48 kHz a beat is 24000 samples and a 16th step 6000
    static constexpr int samplesPerStep = 6000;

    class FakePlayHead : public juce::AudioPlayHead
    {
    public:
        juce::Optional<PositionInfo> getPosition() const override
        {
            PositionInfo info;
            info.setIsPlaying(true);
            info.setPpqPosition(ppq);
            info.setBpm(bpm);
            return info;
        }

        double ppq = 0.0;
    };

    // A pattern holding a single kick on the given step
    static void setSingleKick(DrumMachineProcessor& processor, int step)
    {
        auto& pattern = processor.getCurrentPattern();
        for (auto& voiceHits : pattern.voices)
            voiceHits.clear();
        pattern.voices[DrumEngine::KICK].push_back({ DrumEngine::KICK, step, 1.0f });
    }

    // Plays from the top of the bar and returns the left channel
    static std::vector<float> play(DrumMachineProcessor& processor, int numSamples)
    {
        FakePlayHead playHead;
        processor.setPlayHead(&playHead);
        processor.prepareToPlay(sampleRate, blockSize);

        std::vector<float> output;
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midi;

        while (static_cast<int>(output.size()) < numSamples)
        {
            processor.processBlock(buffer, midi);
            output.insert(output.end(), buffer.getReadPointer(0), buffer.getReadPointer(0) + blockSize);
            playHead.ppq += blockSize * bpm / (60.0 * sampleRate);
        }

        processor.setPlayHead(nullptr);
        return output;
    }

    // The first sample at or after `from` that is not silent, or -1
    static int firstSound(const std::vector<float>& output, int from = 0)
    {
        for (int i = from; i < static_cast<int>(output.size()); ++i)
            if (std::abs(output[static_cast<size_t>(i)]) > 1.0e-6f)
                return i;
        return -1;
    }

    // A kick's first sample is sin(0) = 0, so the sound shows one sample after the hit
    void expectHitAt(int actualFirstSound, int expectedHitSample, const juce::String& what)
    {
        expect(actualFirstSound >= expectedHitSample && actualFirstSound <= expectedHitSample + 2,
               what + ": expected the hit at sample " + juce::String(expectedHitSample)
                   + ", it sounded at " + juce::String(actualFirstSound));
    }

    void testHitsStartOnTheirOwnSample()
    {
        beginTest("A hit starts on its own sample, not at the start of the block");

        DrumMachineProcessor processor;
        setSingleKick(processor, 1);

        // Step 1 starts at sample 6000, 368 samples into the block starting at 5632
        expectHitAt(firstSound(play(processor, 8 * samplesPerStep)), samplesPerStep, "Kick on step 1");
    }

    void testPatternLoopsEveryBar()
    {
        beginTest("The pattern loops after one bar of 16 steps");

        DrumMachineProcessor processor;
        setSingleKick(processor, 0);

        const int bar = 16 * samplesPerStep;
        const auto output = play(processor, bar + 4 * samplesPerStep);

        expectHitAt(firstSound(output), 0, "First bar");
        // The kick has died away well before the bar ends; 96000 is mid-block
        expectHitAt(firstSound(output, bar - samplesPerStep), bar, "Second bar");
    }

    void testSwingDelaysOffBeats()
    {
        beginTest("Swing delays the off-beat 16ths");

        DrumMachineProcessor processor;
        processor.setSwing(1.0f);
        setSingleKick(processor, 1);

        // Full swing moves the off-beat a third of a step later, onto the 16th triplet
        expectHitAt(firstSound(play(processor, 8 * samplesPerStep)),
                    samplesPerStep + samplesPerStep / 3, "Kick on step 1 at full swing");

        processor.setSwing(0.5f);
        expectHitAt(firstSound(play(processor, 8 * samplesPerStep)),
                    samplesPerStep + samplesPerStep / 6, "Kick on step 1 at half swing");
    }

    void testSwingLeavesOnBeatsAlone()
    {
        beginTest("Swing leaves the on-beat 16ths where they are");

        DrumMachineProcessor processor;
        processor.setSwing(1.0f);
        setSingleKick(processor, 2);

        expectHitAt(firstSound(play(processor, 8 * samplesPerStep)), 2 * samplesPerStep, "Kick on step 2");
    }

    void testSwingDoesNotChangeThePattern()
    {
        beginTest("Swing changes timing, not which hits a pattern has");

        auto hatSteps = [](float swing)
        {
            DrumMachineProcessor processor;
            processor.setSwing(swing);
            processor.generateNewPattern("Jazz");

            std::vector<int> steps;
            for (const auto& hit : processor.getCurrentPattern().voices[DrumEngine::HIHAT])
                steps.push_back(hit.step);
            std::sort(steps.begin(), steps.end());
            return steps;
        };

        expect(hatSteps(0.0f) == hatSteps(1.0f), "Jazz should have the same hihats at any swing");
    }

    void testFillsInEveryStyle()
    {
        for (const auto* style : { "Rock", "Hip-Hop", "Jazz", "EDM", "Funk", "Latin", "Blues" })
        {
            beginTest(juce::String("Fills in ") + style);

            DrumMachineProcessor processor;
            processor.setComplexity(10);  // EDM has its own crash on step 0 at this setting
            processor.setFillsEnabled(true);
            processor.generateNewPattern(style);

            const auto& pattern = processor.getCurrentPattern();

            std::vector<int> toms;
            for (const auto& hit : pattern.voices[DrumEngine::TOM])
                toms.push_back(hit.step);
            std::sort(toms.begin(), toms.end());
            expect(toms == std::vector<int> { 12, 13, 14, 15 }, "The fill should be toms on the last beat");

            const auto& crashes = pattern.voices[DrumEngine::CRASH];
            expectEquals(static_cast<int>(crashes.size()), 1, "There should be exactly one crash");
            if (! crashes.empty())
                expectEquals(crashes.front().step, 0, "The crash should land on the downbeat");

            processor.setFillsEnabled(false);
            processor.generateNewPattern(style);
            expect(processor.getCurrentPattern().voices[DrumEngine::TOM].empty(), "No fill when fills are off");
        }
    }

    static std::vector<std::tuple<int, int, float>> hits(DrumMachineProcessor& processor)
    {
        std::vector<std::tuple<int, int, float>> result;
        for (const auto& voiceHits : processor.getCurrentPattern().voices)
            for (const auto& hit : voiceHits)
                result.emplace_back(static_cast<int>(hit.voice), hit.step, hit.velocity);
        std::sort(result.begin(), result.end());
        return result;
    }

    void testStateRoundTrip()
    {
        beginTest("Settings and the pattern are saved and restored");

        DrumMachineProcessor original;
        original.setComplexity(8);
        original.setSwing(0.4f);
        original.setFillsEnabled(true);
        original.generateNewPattern("Funk");
        original.setPatternEnabled(false);
        // An edit made in the grid
        original.getCurrentPattern().voices[DrumEngine::TOM].push_back({ DrumEngine::TOM, 2, 0.6f });

        juce::MemoryBlock state;
        original.getStateInformation(state);

        DrumMachineProcessor restored;
        restored.setStateInformation(state.getData(), static_cast<int>(state.getSize()));

        expectEquals(restored.getComplexity(), 8);
        expectWithinAbsoluteError(restored.getSwing(), 0.4f, 0.001f);
        expect(restored.getFillsEnabled(), "Fills should be restored");
        expect(! restored.isPatternEnabled(), "Pattern on/off should be restored");
        expectEquals(juce::String(restored.getCurrentPattern().style), juce::String("Funk"));

        const auto expected = hits(original);
        const auto actual = hits(restored);
        expectEquals(static_cast<int>(actual.size()), static_cast<int>(expected.size()), "Same number of hits");
        for (size_t i = 0; i < std::min(expected.size(), actual.size()); ++i)
        {
            expectEquals(std::get<0>(actual[i]), std::get<0>(expected[i]), "Hit voice");
            expectEquals(std::get<1>(actual[i]), std::get<1>(expected[i]), "Hit step");
            expectWithinAbsoluteError(std::get<2>(actual[i]), std::get<2>(expected[i]), 0.001f, "Hit velocity");
        }
    }

    void testIgnoresForeignState()
    {
        beginTest("A state that is not DrumMachine's is ignored");

        DrumMachineProcessor processor;
        processor.setComplexity(7);
        const auto before = hits(processor);

        const char junk[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
        processor.setStateInformation(junk, sizeof(junk));
        processor.setStateInformation(nullptr, 0);

        juce::XmlElement other("SomeOtherPlugin");
        juce::MemoryBlock otherState;
        juce::AudioProcessor::copyXmlToBinary(other, otherState);
        processor.setStateInformation(otherState.getData(), static_cast<int>(otherState.getSize()));

        expectEquals(processor.getComplexity(), 7);
        expect(hits(processor) == before, "The pattern should be as it was");
    }
};

static DrumMachinePluginTests drumMachinePluginTests;
