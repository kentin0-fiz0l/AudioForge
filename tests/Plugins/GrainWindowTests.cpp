/**
 * Grain Window Tests
 *
 * Tests for the window functions GranularEngine applies to each grain.
 * Every window, at every shape and grain size, should be finite, stay
 * between 0 and 1, and come close to 1 at its peak.
 */

#include <juce_core/juce_core.h>
#include <cmath>
#include <vector>

#include "../../plugins/GranularEngine/Source/GrainExtractor.h"

class GrainWindowTests : public juce::UnitTest
{
public:
    GrainWindowTests()
        : juce::UnitTest("Grain Windows", "Plugins")
    {
    }

    void runTest() override
    {
        testWindowsAreWellFormed();
    }

private:
    void testWindowsAreWellFormed()
    {
        beginTest("Every window is finite and within 0 to 1");

        const char* const names[] = { "Hann", "Gaussian", "Triangle", "Tukey", "Blackman", "Kaiser" };

        // A buffer full of ones: extracting a grain from it returns the window itself
        GrainBuffer source;
        source.prepare(48000.0, 32768);
        for (int i = 0; i < 32768; ++i)
            source.writeSample(1.0f);

        bool allFinite = true;
        bool allInRange = true;
        bool allPeakNearOne = true;
        juce::String firstNonFinite, firstOutOfRange, firstLowPeak;

        // Even and odd sizes matter: the Kaiser window used to compute its
        // centre with integer division and went out of range for even sizes
        for (int grainSize : { 64, 65, 441, 2206 })
        {
            for (int type = 0; type < 6; ++type)
            {
                for (float shape : { 0.0f, 0.5f, 1.0f })
                {
                    GrainExtractor extractor;
                    extractor.prepare(48000.0);
                    extractor.setGrainSize(grainSize);
                    extractor.setWindowType(type);
                    extractor.setWindowShape(shape);

                    std::vector<float> window(static_cast<size_t>(grainSize), 0.0f);
                    extractor.extractGrain(source, 0, window.data());

                    bool finite = true;
                    float lowest = 1.0f;
                    float highest = 0.0f;

                    for (float value : window)
                    {
                        finite = finite && std::isfinite(value);
                        lowest = std::min(lowest, value);
                        highest = std::max(highest, value);
                    }

                    const bool inRange = finite && lowest >= -0.001f && highest <= 1.001f;
                    // The narrowest Gaussian on an even-sized grid has its
                    // peak between two samples, so allow some shortfall
                    const bool peakNearOne = finite && highest >= 0.85f;

                    const auto where = juce::String(names[type]) + ", shape " + juce::String(shape)
                                       + ", size " + juce::String(grainSize);
                    const auto range = ": range " + juce::String(lowest) + " to " + juce::String(highest);

                    if (! finite && firstNonFinite.isEmpty())
                        firstNonFinite = where;
                    if (finite && ! inRange && firstOutOfRange.isEmpty())
                        firstOutOfRange = where + range;
                    if (finite && ! peakNearOne && firstLowPeak.isEmpty())
                        firstLowPeak = where + range;

                    allFinite = allFinite && finite;
                    allInRange = allInRange && inRange;
                    allPeakNearOne = allPeakNearOne && peakNearOne;
                }
            }
        }

        expect(allFinite, "Windows should be finite. First problem: " + firstNonFinite);
        expect(allInRange, "Windows should stay within 0 to 1. First problem: " + firstOutOfRange);
        expect(allPeakNearOne, "Windows should peak near 1. First problem: " + firstLowPeak);
    }
};

static GrainWindowTests grainWindowTests;
