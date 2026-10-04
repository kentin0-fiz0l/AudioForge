/**
 * HiHat Engine Tests
 *
 * A hit should be audible, bright, and bounded at every preset and sample
 * rate, and an open hat should ring longer than a closed one.
 */

#include <juce_core/juce_core.h>
#include <cmath>
#include <vector>

#include "../../plugins/HiHat/Source/HiHatEngine.h"

class HiHatEngineTests : public juce::UnitTest
{
public:
    HiHatEngineTests()
        : juce::UnitTest("HiHatEngine", "Plugins")
    {
    }

    void runTest() override
    {
        testHitsAreAudibleAndBright();
        testOpenRingsLongerThanClosed();
    }

private:
    struct Settings
    {
        const char* name;
        float tone, decay, metallic, openness;
    };

    // The factory presets, plus the plugin's default settings
    static constexpr Settings settings[] = {
        { "Default",      0.5f,  0.08f, 0.7f,  0.3f },
        { "808 Closed",   0.5f,  0.06f, 0.7f,  0.1f },
        { "808 Open",     0.5f,  0.25f, 0.7f,  0.85f },
        { "909 Closed",   0.65f, 0.05f, 0.85f, 0.05f },
        { "909 Open",     0.6f,  0.35f, 0.8f,  0.9f },
        { "Tight Sizzle", 0.8f,  0.04f, 0.95f, 0.0f },
        { "Washy",        0.4f,  0.45f, 0.6f,  1.0f },
    };

    // One full-velocity hit, rendered until the engine goes quiet
    static std::vector<float> renderHit(const Settings& s, double sampleRate)
    {
        HiHatEngine engine;
        engine.prepareToPlay(sampleRate, 512);
        engine.setTone(s.tone);
        engine.setDecay(s.decay);
        engine.setMetallic(s.metallic);
        engine.setOpenness(s.openness);
        engine.trigger(1.0f);

        std::vector<float> samples;
        const auto limit = static_cast<size_t>(sampleRate * 10.0);

        while (engine.isActive() && samples.size() < limit)
            samples.push_back(engine.processSample());

        return samples;
    }

    void testHitsAreAudibleAndBright()
    {
        beginTest("Every preset is audible, bounded and bright at every sample rate");

        bool allFinite = true, allAudible = true, allBounded = true, allBright = true;
        juce::String firstNonFinite, firstQuiet, firstLoud, firstDull;

        for (double sampleRate : { 44100.0, 48000.0, 96000.0 })
        {
            for (const auto& s : settings)
            {
                const auto samples = renderHit(s, sampleRate);

                bool finite = true;
                float peak = 0.0f;
                int crossings = 0;

                // Brightness is judged over the first 20 ms, where the hit is loudest
                const auto attack = juce::jmin(samples.size(), static_cast<size_t>(sampleRate * 0.02));

                for (size_t i = 0; i < samples.size(); ++i)
                {
                    finite = finite && std::isfinite(samples[i]);
                    peak = juce::jmax(peak, std::abs(samples[i]));

                    if (i > 0 && i < attack && (samples[i] < 0.0f) != (samples[i - 1] < 0.0f))
                        ++crossings;
                }

                // A tone at f Hz crosses zero 2f times a second, so this asks
                // for the sound to be centred above 3 kHz
                const double crossingsPerSecond = attack > 0 ? crossings * sampleRate / static_cast<double>(attack) : 0.0;

                const bool audible = finite && peak >= 0.1f;
                const bool bounded = finite && peak <= 1.0f;
                const bool bright = finite && crossingsPerSecond >= 6000.0;

                const auto where = juce::String(s.name) + " at " + juce::String(static_cast<int>(sampleRate));

                if (! finite && firstNonFinite.isEmpty())
                    firstNonFinite = where;
                if (! audible && firstQuiet.isEmpty())
                    firstQuiet = where + ": peak " + juce::String(peak, 5);
                if (! bounded && firstLoud.isEmpty())
                    firstLoud = where + ": peak " + juce::String(peak, 3);
                if (! bright && firstDull.isEmpty())
                    firstDull = where + ": " + juce::String(static_cast<int>(crossingsPerSecond)) + " zero crossings a second";

                allFinite = allFinite && finite;
                allAudible = allAudible && audible;
                allBounded = allBounded && bounded;
                allBright = allBright && bright;
            }
        }

        expect(allFinite, "Non-finite output for " + firstNonFinite);
        expect(allAudible, "Too quiet to hear: " + firstQuiet);
        expect(allBounded, "Peak above full scale: " + firstLoud);
        expect(allBright, "Not bright enough for a hi-hat: " + firstDull);
    }

    void testOpenRingsLongerThanClosed()
    {
        beginTest("An open hat rings longer than a closed one, by the same amount at any sample rate");

        const auto& closed = settings[1];
        const auto& open = settings[2];

        const double closedSeconds = renderHit(closed, 48000.0).size() / 48000.0;
        const double openSeconds = renderHit(open, 48000.0).size() / 48000.0;
        const double openSecondsAt96k = renderHit(open, 96000.0).size() / 96000.0;

        expect(closedSeconds > 0.05, "Closed hat lasts only " + juce::String(closedSeconds, 3) + " s");
        expect(openSeconds > 3.0 * closedSeconds,
               "Open hat lasts " + juce::String(openSeconds, 3) + " s against "
                   + juce::String(closedSeconds, 3) + " s closed");
        expectWithinAbsoluteError(openSecondsAt96k, openSeconds, openSeconds * 0.05);
    }
};

static HiHatEngineTests hiHatEngineTests;
