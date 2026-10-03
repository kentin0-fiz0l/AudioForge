/**
 * KotoEngine Tests
 *
 * Tests for the Koto plugin's plucked-string engine:
 * - A held note decays; it never grows, whatever the decay setting
 * - Output level stays sane in every play style
 */

#include <juce_core/juce_core.h>
#include <cmath>

#include "../../plugins/Koto/Source/KotoEngine.h"

class KotoEngineTests : public juce::UnitTest
{
public:
    KotoEngineTests()
        : juce::UnitTest("KotoEngine", "Plugins")
    {
    }

    void runTest() override
    {
        testHeldNoteDoesNotGrow();
        testLevelIsSaneInEveryPlayStyle();
    }

private:
    static constexpr double sampleRate = 48000.0;

    // Holds one full-velocity note and returns the peak between two times
    static float peakBetween(KotoEngine& engine, float frequency, double fromSeconds, double toSeconds,
                             bool* allFinite = nullptr)
    {
        const int from = static_cast<int>(fromSeconds * sampleRate);
        const int to = static_cast<int>(toSeconds * sampleRate);
        float peak = 0.0f;

        engine.reset();

        for (int i = 0; i < to; ++i)
        {
            const float sample = engine.processSample(frequency, 1.0f, true, i);

            if (allFinite != nullptr && ! std::isfinite(sample))
                *allFinite = false;

            if (i >= from)
                peak = std::max(peak, std::abs(sample));
        }

        return peak;
    }

    void testHeldNoteDoesNotGrow()
    {
        beginTest("Held note never grows louder");

        // Regression test: the string's loop gain was 0.994 + decay * 0.004,
        // and the Decay control goes up to 2.0. Above 1.5 the gain passed 1.0
        // and the note grew on every trip round the string.
        for (float decay : { 0.1f, 0.8f, 1.5f, 2.0f })
        {
            KotoEngine engine;
            engine.prepareToPlay(sampleRate, 512);
            engine.setPlayStyle(KotoEngine::PlayStyle::Pluck);
            engine.setDecay(decay);

            const float early = peakBetween(engine, 880.0f, 0.1, 0.5);
            const float late = peakBetween(engine, 880.0f, 4.5, 5.0);

            expect(early > 0.0f, "A plucked note should sound");
            expect(late <= early * 1.01f,
                   "Note should not be louder after 5 s than at the start (decay "
                       + juce::String(decay) + ": " + juce::String(early) + " -> " + juce::String(late) + ")");
        }
    }

    void testLevelIsSaneInEveryPlayStyle()
    {
        beginTest("Output level stays sane in every play style");

        bool allFinite = true;
        float worstPeak = 0.0f;
        juce::String worstSettings;

        for (auto style : { KotoEngine::PlayStyle::Pluck, KotoEngine::PlayStyle::Tremolo,
                            KotoEngine::PlayStyle::Scrape })
        {
            for (float decay : { 0.1f, 0.8f, 2.0f })
            {
                for (float body : { 0.0f, 1.0f })
                {
                    for (float tremoloRate : { 2.0f, 20.0f })
                    {
                        for (float frequency : { 65.4f, 440.0f, 2093.0f })
                        {
                            KotoEngine engine;
                            engine.prepareToPlay(sampleRate, 512);
                            engine.setPlayStyle(style);
                            engine.setDecay(decay);
                            engine.setBodyResonance(body);
                            engine.setTremoloRate(tremoloRate);
                            engine.setTone(1.0f);

                            const float peak = peakBetween(engine, frequency, 0.0, 3.0, &allFinite);

                            if (peak > worstPeak)
                            {
                                worstPeak = peak;
                                worstSettings = "style " + juce::String(static_cast<int>(style))
                                                + ", decay " + juce::String(decay)
                                                + ", body " + juce::String(body)
                                                + ", tremolo " + juce::String(tremoloRate)
                                                + " Hz, note " + juce::String(frequency) + " Hz";
                            }
                        }
                    }
                }
            }
        }

        expect(allFinite, "Output should always be finite");
        expect(worstPeak < 2.0f,
               "A single voice should stay near full scale. Worst: peak "
                   + juce::String(worstPeak) + " with " + worstSettings);
    }
};

static KotoEngineTests kotoEngineTests;
