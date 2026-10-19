/**
 * DrumMachine Plugin Tests
 *
 * Plays the processor against a fake host transport and checks when hits
 * sound:
 * - Hits start on their own sample, not at the start of the audio block
 * - Swing delays the off-beat 16ths and leaves the on-beat ones alone
 * - Fills work in every style and their crash is kept
 * - Settings and the pattern survive a save and restore; a damaged state is checked
 * - Every step is on time at 44.1 kHz, with 1- and 4096-sample blocks, after a
 *   count-in, with a drifting host position, and around a host loop
 * - Saving and playing on other threads while the pattern is replaced
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <algorithm>
#include <cmath>
#include <thread>
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
        testDamagedStateIsChecked();
        testTimingAtOtherRatesAndBlockSizes();
        testPreRollFiresTheDownbeatOnce();
        testHostPositionDriftDropsNoHits();
        testHostLoopRestartsThePattern();
        testGridEdits();
        testSavingWhileThePatternIsReplaced();
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
        DrumEngine::DrumPattern pattern;
        pattern.numSteps = 16;
        pattern.voices[DrumEngine::KICK].push_back({ DrumEngine::KICK, step, 1.0f });
        processor.setPattern(std::move(pattern));
    }

    // How the fake host runs its transport
    struct Transport
    {
        double sampleRate = DrumMachinePluginTests::sampleRate;
        int blockSize = DrumMachinePluginTests::blockSize;
        double startPpq = 0.0;
        double hostBpm = bpm;     // How fast the position really moves; the host always reports `bpm`
        double loopEndBeat = 0.0; // When set, the host jumps back to beat 0 here

        double samplesPerBeat() const { return 60.0 * sampleRate / hostBpm; }
    };

    // Plays from `startPpq` and returns the left channel
    static std::vector<float> play(DrumMachineProcessor& processor, int numSamples)
    {
        return play(processor, numSamples, Transport{});
    }

    static std::vector<float> play(DrumMachineProcessor& processor, int numSamples, const Transport& transport)
    {
        FakePlayHead playHead;
        processor.setPlayHead(&playHead);
        processor.prepareToPlay(transport.sampleRate, transport.blockSize);

        std::vector<float> output;
        juce::AudioBuffer<float> buffer(2, transport.blockSize);
        juce::MidiBuffer midi;

        while (static_cast<int>(output.size()) < numSamples)
        {
            playHead.ppq = transport.startPpq + static_cast<double>(output.size()) / transport.samplesPerBeat();
            if (transport.loopEndBeat > 0.0)
                playHead.ppq = std::fmod(playHead.ppq, transport.loopEndBeat);

            processor.processBlock(buffer, midi);
            output.insert(output.end(), buffer.getReadPointer(0), buffer.getReadPointer(0) + transport.blockSize);
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

    // Each step, on its own, sounds where the host's position reaches it, in two bars running;
    // `late` allows for a host whose position runs ahead of the tempo it reports
    void expectEveryStepOnTime(const Transport& transport, const juce::String& what, int late = 0)
    {
        const double samplesPerStep = transport.samplesPerBeat() / 4.0;
        const int bar = juce::roundToInt(16 * samplesPerStep);
        int missed = 0;

        for (int step = 0; step < 16; ++step)
        {
            DrumMachineProcessor processor;
            setSingleKick(processor, step);
            const int countIn = juce::roundToInt(-transport.startPpq * transport.samplesPerBeat());
            const auto output = play(processor, countIn + 2 * bar + transport.blockSize, transport);

            for (int barIndex = 0; barIndex < 2; ++barIndex)
            {
                const int expected = static_cast<int>((barIndex * 16 + step - transport.startPpq * 4.0) * samplesPerStep);
                // The kick from the bar before has died away 15 steps on
                const int heard = firstSound(output, std::max(0, expected - juce::roundToInt(samplesPerStep)));
                if (heard < expected || heard > expected + 2 + late)
                {
                    ++missed;
                    logMessage(what + ": step " + juce::String(step) + " in bar " + juce::String(barIndex + 1)
                               + " expected at " + juce::String(expected) + ", heard at " + juce::String(heard));
                }
            }
        }

        expectEquals(missed, 0, what + ": every step should sound once, on time");
    }

    void testTimingAtOtherRatesAndBlockSizes()
    {
        beginTest("Every step is on time at 44.1 kHz (a step is 5512.5 samples)");
        expectEveryStepOnTime({ 44100.0, 512 }, "44.1 kHz, 512");

        beginTest("Every step is on time with one-sample blocks");
        expectEveryStepOnTime({ 44100.0, 1 }, "44.1 kHz, 1");

        beginTest("Every step is on time with blocks holding several steps");
        expectEveryStepOnTime({ 48000.0, 4096 }, "48 kHz, 4096");
        expectEveryStepOnTime({ 44100.0, 4096 }, "44.1 kHz, 4096");
    }

    void testPreRollFiresTheDownbeatOnce()
    {
        beginTest("During a count-in (negative position) step 0 fires once, where the position crosses 0");

        DrumMachineProcessor processor;
        setSingleKick(processor, 0);

        // One beat of count-in: the bar starts 24000 samples in, mid-block
        Transport countIn;
        countIn.startPpq = -1.0;
        const auto output = play(processor, 4 * samplesPerStep * 2, countIn);
        expectHitAt(firstSound(output), 4 * samplesPerStep, "Kick on step 0 after a beat of count-in");

        beginTest("Every step is on time after a count-in");
        countIn.startPpq = -0.37;
        expectEveryStepOnTime(countIn, "Count-in of 0.37 beats");
    }

    void testHostPositionDriftDropsNoHits()
    {
        beginTest("A host position running ahead of its reported tempo drops no hits");

        // The position moves 10% faster than the tempo says, as in a tempo ramp: each 512-sample
        // block starts about 50 samples past where the last one was expected to end
        Transport drifting;
        drifting.hostBpm = bpm * 1.1;
        expectEveryStepOnTime(drifting, "Drift", 64);
    }

    void testHostLoopRestartsThePattern()
    {
        beginTest("A host loop back to the start plays the downbeat again");

        DrumMachineProcessor processor;
        setSingleKick(processor, 0);

        // A one-bar loop: 96000 samples is exactly 200 blocks of 480
        Transport loop;
        loop.blockSize = 480;
        loop.loopEndBeat = 4.0;
        const int bar = 16 * samplesPerStep;
        const auto output = play(processor, 2 * bar + 4 * samplesPerStep, loop);

        expectHitAt(firstSound(output), 0, "First time round");
        expectHitAt(firstSound(output, bar - samplesPerStep), bar, "Second time round");
        expectHitAt(firstSound(output, 2 * bar - samplesPerStep), 2 * bar, "Third time round");
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
            const auto pattern = processor.getCurrentPattern();
            for (const auto& hit : pattern.voices[DrumEngine::HIHAT])
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

            const auto pattern = processor.getCurrentPattern();

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
        original.setHit(DrumEngine::TOM, 2, true);

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

    void testGridEdits()
    {
        beginTest("Grid edits add, remove and clear hits");

        DrumMachineProcessor processor;
        processor.clearPattern();
        expect(hits(processor).empty(), "Clear empties the grid");

        expect(processor.setHit(DrumEngine::SNARE, 4, true), "Adding a hit changes the grid");
        expect(! processor.setHit(DrumEngine::SNARE, 4, true), "A cell holds one hit");
        expect(! processor.setHit(DrumEngine::NUM_VOICES, 4, true), "No such voice");
        expect(! processor.setHit(DrumEngine::SNARE, 16, true), "No such step");
        expectEquals(static_cast<int>(hits(processor).size()), 1);

        processor.toggleHit(DrumEngine::SNARE, 4);
        expect(hits(processor).empty(), "Toggling a hit removes it");
        processor.toggleHit(DrumEngine::KICK, 0);
        expectEquals(static_cast<int>(hits(processor).size()), 1, "Toggling an empty cell adds a hit");
    }

    void testSavingWhileThePatternIsReplaced()
    {
        beginTest("Saving and playing on other threads while the pattern is replaced");

        DrumMachineProcessor processor;
        FakePlayHead playHead;
        processor.setPlayHead(&playHead);
        processor.prepareToPlay(sampleRate, blockSize);

        std::atomic<bool> done { false };
        std::atomic<int> badSaves { 0 };

        // The host saving from a background thread
        std::thread saver([&]
        {
            while (! done)
            {
                juce::MemoryBlock state;
                processor.getStateInformation(state);
                const auto xml = juce::AudioProcessor::getXmlFromBinary(state.getData(), static_cast<int>(state.getSize()));
                if (xml == nullptr || ! xml->hasTagName("DrumMachine"))
                    ++badSaves;
            }
        });

        // The audio thread
        std::thread audio([&]
        {
            juce::AudioBuffer<float> buffer(2, blockSize);
            juce::MidiBuffer midi;
            while (! done)
            {
                processor.processBlock(buffer, midi);
                playHead.ppq += blockSize * bpm / (60.0 * sampleRate);
            }
        });

        // The message thread: new patterns, edits, and loading saved sessions
        juce::MemoryBlock saved;
        processor.getStateInformation(saved);
        for (int i = 0; i < 2000; ++i)
        {
            switch (i % 4)
            {
                case 0: processor.generateNewPattern(i % 8 == 0 ? "Funk" : "Latin"); break;
                case 1: processor.setStateInformation(saved.getData(), static_cast<int>(saved.getSize())); break;
                case 2: processor.toggleHit(i % DrumEngine::NUM_VOICES, i % 16); break;
                default: processor.clearPattern(); break;
            }
        }

        done = true;
        saver.join();
        audio.join();
        processor.setPlayHead(nullptr);

        expectEquals(badSaves.load(), 0, "Every save should be a whole DrumMachine state");
    }

    void testDamagedStateIsChecked()
    {
        beginTest("A damaged or hand-edited state is kept to safe values");

        DrumMachineProcessor processor;
        processor.setComplexity(7);
        processor.setSwing(0.3f);
        processor.generateNewPattern("Jazz");

        juce::XmlElement state("DrumMachine");
        state.setAttribute("style", "<not a style>");
        state.setAttribute("complexity", 999);
        state.setAttribute("swing", "nan");
        auto addHit = [&state](const juce::String& voice, const juce::String& step, const juce::String& velocity)
        {
            auto* hit = state.createNewChildElement("Hit");
            hit->setAttribute("voice", voice);
            hit->setAttribute("step", step);
            hit->setAttribute("velocity", velocity);
        };
        addHit("0", "0", "nan");       // Kept, at a usable velocity
        addHit("1", "4", "inf");       // Kept, clamped
        addHit("2", "8", "-3");        // Kept, clamped
        addHit("9", "0", "0.5");       // No such voice
        addHit("0", "16", "0.5");      // No such step
        addHit("0", "-1", "0.5");      // No such step
        for (int i = 0; i < 10000; ++i)
            addHit("3", "12", "0.5");  // One hit, however often it is repeated

        juce::MemoryBlock data;
        juce::AudioProcessor::copyXmlToBinary(state, data);
        processor.setStateInformation(data.getData(), static_cast<int>(data.getSize()));

        expectEquals(processor.getComplexity(), 10, "Complexity is kept to its range");
        expectWithinAbsoluteError(processor.getSwing(), 0.3f, 0.001f, "A non-finite swing is ignored");
        const auto pattern = processor.getCurrentPattern();
        expectEquals(juce::String(pattern.style), juce::String("Jazz"), "An unknown style is ignored");

        const auto loaded = hits(processor);
        expectEquals(static_cast<int>(loaded.size()), 4, "Only the four real hits are loaded");
        for (const auto& [voice, step, velocity] : loaded)
        {
            expect(std::isfinite(velocity) && velocity >= 0.0f && velocity <= 1.0f,
                   "Velocity " + juce::String(velocity) + " should be in 0-1");
            expect(voice >= 0 && voice < DrumEngine::NUM_VOICES && step >= 0 && step < 16, "Hit in the grid");
        }
    }
};

static DrumMachinePluginTests drumMachinePluginTests;
