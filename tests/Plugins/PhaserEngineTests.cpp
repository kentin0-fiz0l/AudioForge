/**
 * PhaserFlanger ModulationEngine Tests (phaser mode)
 *
 * - The wet path is a chain of all-pass stages: flat in level at every frequency
 * - A 50/50 mix of dry and wet carves a deep notch
 * - The LFO sweeps the notch, so a steady tone rises and falls in level
 * - Feedback feeds the output back into the stages: the wet path stops being flat
 * - Maximum feedback, positive or negative, stays stable
 * - Feedback's resonance is compensated: turning it up does not make the effect much louder
 */

#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>

#include "../../plugins/PhaserFlanger/Source/ModulationEngine.h"

class PhaserEngineTests : public juce::UnitTest
{
public:
    PhaserEngineTests()
        : juce::UnitTest("PhaserFlanger Phaser", "Plugins")
    {
    }

    void runTest() override
    {
        testWetPathIsAllPass();
        testHalfMixCarvesNotch();
        testLfoSweepsNotch();
        testFeedbackShapesWetPath();
        testMaximumFeedbackIsStable();
        testFeedbackDoesNotAddLevel();
    }

private:
    static constexpr double sampleRate = 48000.0;
    static constexpr int blockSize = 512;

    static void setUp(ModulationEngine& engine, float depth, float feedback, float mix)
    {
        engine.prepareToPlay(sampleRate, blockSize);
        engine.setMode(ModulationEngine::Mode::Phaser);
        engine.setRate(0.5f);
        engine.setDepth(depth);
        engine.setFeedback(feedback);
        engine.setCentreFrequency(1000.0f);
        engine.setMix(mix);
    }

    // Fills both channels with a 0.25-amplitude sine, carrying the phase across calls
    static void fillSine(juce::AudioBuffer<float>& buf, double& phase, double inc)
    {
        for (int i = 0; i < buf.getNumSamples(); ++i)
        {
            const float s = 0.25f * (float) std::sin(phase);
            phase += inc;
            buf.setSample(0, i, s);
            buf.setSample(1, i, s);
        }
    }

    // Fills each channel with its own uniform noise in [-amplitude, amplitude]
    static void fillNoise(juce::AudioBuffer<float>& buf, juce::Random& rng, float amplitude)
    {
        for (int ch = 0; ch < 2; ++ch)
            for (int i = 0; i < buf.getNumSamples(); ++i)
                buf.setSample(ch, i, amplitude * (rng.nextFloat() * 2.0f - 1.0f));
    }

    // Gain in dB of a steady sine through a fresh engine, measured as RMS over
    // the second half of a one-second run so the filters have settled
    static float sineGainDb(float freq, float depth, float feedback, float mix)
    {
        ModulationEngine engine;
        setUp(engine, depth, feedback, mix);

        const int totalBlocks = (int) sampleRate / blockSize;
        juce::AudioBuffer<float> buf(2, blockSize);
        double phase = 0.0;
        const double inc = juce::MathConstants<double>::twoPi * freq / sampleRate;
        double sumIn = 0.0, sumOut = 0.0;

        for (int b = 0; b < totalBlocks; ++b)
        {
            fillSine(buf, phase, inc);
            juce::AudioBuffer<float> in;
            in.makeCopyOf(buf);
            engine.processBlock(buf);

            if (b >= totalBlocks / 2)
                for (int i = 0; i < blockSize; ++i)
                {
                    sumIn += in.getSample(0, i) * in.getSample(0, i);
                    sumOut += buf.getSample(0, i) * buf.getSample(0, i);
                }
        }
        return (float) (10.0 * std::log10((sumOut + 1e-20) / sumIn));
    }

    static std::vector<float> sweepFrequencies()
    {
        std::vector<float> freqs;
        for (int i = 0; i < 60; ++i)
            freqs.push_back(40.0f * std::pow(2.0f, (float) i / 7.0f)); // 40 Hz to ~14 kHz
        return freqs;
    }

    struct GainRange
    {
        float lowest = 100.0f, highest = -100.0f;
    };

    // Lowest and highest sine gain across sweepFrequencies(), with the LFO still
    static GainRange sweptGainRangeDb(float feedback, float mix)
    {
        GainRange range;
        for (float f : sweepFrequencies())
        {
            const float g = sineGainDb(f, 0.0f, feedback, mix);
            range.lowest = juce::jmin(range.lowest, g);
            range.highest = juce::jmax(range.highest, g);
        }
        return range;
    }

    void testWetPathIsAllPass()
    {
        beginTest("Wet path is flat in level at every frequency");

        for (float f : { 100.0f, 1000.0f, 5000.0f, 15000.0f })
        {
            const float g = sineGainDb(f, 0.0f, 0.0f, 1.0f);
            expect(std::abs(g) < 0.5f,
                   "Fully wet with no feedback should pass " + juce::String(f) + " Hz at unity, got "
                       + juce::String(g, 2) + " dB");
        }
    }

    void testHalfMixCarvesNotch()
    {
        beginTest("A 50/50 mix carves a deep notch");

        const auto [lowest, highest] = sweptGainRangeDb(0.0f, 0.5f);
        expect(lowest < -20.0f, "Deepest point should be below -20 dB, got " + juce::String(lowest, 1));
        expect(highest > -1.0f && highest < 0.5f,
               "Between notches the level should be about unity, got " + juce::String(highest, 1));
    }

    void testLfoSweepsNotch()
    {
        beginTest("The LFO sweeps the notch past a steady tone");

        ModulationEngine engine;
        setUp(engine, 1.0f, 0.0f, 0.5f);
        engine.setRate(2.0f);

        // Two seconds of a 1 kHz tone, RMS measured in 10 ms windows
        const int window = (int) (sampleRate / 100.0);
        juce::AudioBuffer<float> buf(2, window);
        double phase = 0.0;
        const double inc = juce::MathConstants<double>::twoPi * 1000.0 / sampleRate;
        float quietest = 1.0f, loudest = 0.0f;

        for (int w = 0; w < 200; ++w)
        {
            fillSine(buf, phase, inc);
            engine.processBlock(buf);
            if (w < 20)
                continue;
            const float rms = buf.getRMSLevel(0, 0, window);
            quietest = juce::jmin(quietest, rms);
            loudest = juce::jmax(loudest, rms);
        }
        const float swingDb = juce::Decibels::gainToDecibels(loudest / juce::jmax(quietest, 1e-9f));
        expect(swingDb > 12.0f, "A swept notch should move a 1 kHz tone by more than 12 dB, got "
                                    + juce::String(swingDb, 1) + " dB");
    }

    void testFeedbackShapesWetPath()
    {
        beginTest("Feedback is fed back through the stages");

        const auto [lowest, highest] = sweptGainRangeDb(0.7f, 1.0f);
        // An all-pass loop with feedback rings at some frequencies and dips at
        // others; adding dry signal to the output would leave it flat-ish
        expect(highest - lowest > 10.0f,
               "With 0.7 feedback the wet path should vary by more than 10 dB across the band, got "
                   + juce::String(highest - lowest, 1) + " dB");
    }

    void testMaximumFeedbackIsStable()
    {
        beginTest("Maximum feedback stays stable");

        for (float fb : { 0.9f, -0.9f })
        {
            ModulationEngine engine;
            setUp(engine, 1.0f, fb, 1.0f);
            engine.setRate(10.0f);

            juce::Random rng(42);
            juce::AudioBuffer<float> buf(2, blockSize);
            float peak = 0.0f;
            bool finite = true;
            for (int b = 0; b < (int) (2.0 * sampleRate) / blockSize; ++b)
            {
                fillNoise(buf, rng, 0.25f);
                engine.processBlock(buf);
                for (int ch = 0; ch < 2; ++ch)
                    for (int i = 0; i < blockSize; ++i)
                    {
                        const float s = buf.getSample(ch, i);
                        finite = finite && std::isfinite(s);
                        peak = juce::jmax(peak, std::abs(s));
                    }
            }
            expect(finite, "Output should stay finite at feedback " + juce::String(fb));
            expect(peak < 2.0f, "Peak should stay below 2.0 at feedback " + juce::String(fb) + ", got "
                                    + juce::String(peak, 2));
        }
    }

    // RMS change in dB of a second and a half of noise through the
    // engine at the given feedback, everything else at the plugin's defaults
    static float noiseLevelChangeDb(float feedback)
    {
        ModulationEngine engine;
        setUp(engine, 0.5f, feedback, 0.5f);

        juce::Random rng(7);
        juce::AudioBuffer<float> buf(2, blockSize);
        double sumIn = 0.0, sumOut = 0.0;
        const int totalBlocks = (int) (1.5 * sampleRate) / blockSize;
        for (int b = 0; b < totalBlocks; ++b)
        {
            fillNoise(buf, rng, 0.355f);
            juce::AudioBuffer<float> in;
            in.makeCopyOf(buf);
            engine.processBlock(buf);
            if (b < totalBlocks / 3)
                continue;
            for (int i = 0; i < blockSize; ++i)
            {
                sumIn += in.getSample(0, i) * in.getSample(0, i);
                sumOut += buf.getSample(0, i) * buf.getSample(0, i);
            }
        }
        return (float) (10.0 * std::log10(sumOut / sumIn));
    }

    void testFeedbackDoesNotAddLevel()
    {
        beginTest("Feedback does not make the effect much louder");

        // RMS is the measure here: the all-pass stages turn uniform noise into
        // something closer to Gaussian, which raises its peaks even with no
        // feedback, while the notches take some level away
        const float noFeedbackDb = noiseLevelChangeDb(0.0f);
        expect(noFeedbackDb > -2.0f && noFeedbackDb < 0.0f,
               "With no feedback the notches should cost under 2 dB, got " + juce::String(noFeedbackDb, 2));

        for (float fb : { 0.5f, 0.9f, -0.9f })
        {
            const float rmsDb = noiseLevelChangeDb(fb);
            expect(rmsDb <= noFeedbackDb + 0.5f,
                   "Feedback " + juce::String(fb) + " should not add level, got " + juce::String(rmsDb, 2)
                       + " dB against " + juce::String(noFeedbackDb, 2) + " dB with none");
            expect(rmsDb > -4.0f,
                   "Feedback " + juce::String(fb) + " should cost under 4 dB, got " + juce::String(rmsDb, 2));
        }
    }
};

static PhaserEngineTests phaserEngineTests;
