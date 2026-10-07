/**
 * GranularEngine tests
 *
 * At its defaults GranularEngine came out about 21 dB under what went in.
 * Three things did it: the default grains, 50 ms ten times a second, left
 * half of every second silent; the grains were scaled down as if all 64 of
 * them were always playing; and a stereo input was written into the grain
 * buffer twice per block, left then right, so grains were cut from a stream
 * that jumped back half a block every 512 samples.
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>

#include "../../plugins/GranularEngine/Source/PluginProcessor.h"
#include "../../plugins/GranularEngine/Source/PluginEditor.h"

class GranularEngineTests : public juce::UnitTest
{
public:
    GranularEngineTests()
        : juce::UnitTest("GranularEngine", "Plugins")
    {
    }

    void runTest() override
    {
        testDefaultsKeepTheLevel();
        testStereoInputIsOneStream();
        testWidthSpreadsTheGrains();
        testReverseSetsTheShareOfBackwardGrains();
        testScanSpeedSetsHowFastGrainsMoveThroughTheInput();
        testEditorShowsTheSettingsItOpensWith();
        testEditorFollowsChangesMadeElsewhere();
        testControlsSetTheValuesTheyShow();
        testNoControlsOverlap();
    }

private:
    static constexpr double sampleRate = 48000.0;
    static constexpr int blockSize = 512;
    static constexpr int numBlocks = 188;            // Two seconds
    static constexpr int settledBlock = 94;          // Measure the second one, once the buffer has filled
    static constexpr float calibratedPeak = 0.355f;  // -9 dBFS, where every AudioForge instrument peaks

    // Runs two seconds of input, the same on both channels, and returns the
    // left channel's output over the second second
    template <typename Source>
    static std::vector<float> run(Source&& source)
    {
        GranularEngineProcessor processor;
        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midi;
        std::vector<float> output;

        for (int block = 0; block < numBlocks; ++block)
        {
            for (int i = 0; i < blockSize; ++i)
            {
                const float x = source(block * blockSize + i);
                buffer.setSample(0, i, x);
                buffer.setSample(1, i, x);
            }

            processor.processBlock(buffer, midi);

            if (block >= settledBlock)
                output.insert(output.end(), buffer.getReadPointer(0), buffer.getReadPointer(0) + blockSize);
        }

        return output;
    }

    static double meanSquare(const std::vector<float>& x)
    {
        double sum = 0.0;
        for (float v : x)
            sum += v * v;
        return sum / x.size();
    }

    void testDefaultsKeepTheLevel()
    {
        beginTest("At its defaults the grains come out near the level that went in");

        // Both sides together: the grains are panned at random, so one side
        // alone gets more or less of them from run to run
        const auto [left, right] = runStereo(100.0f);
        const double inputMeanSquare = calibratedPeak * calibratedPeak / 3.0;   // Of the uniform noise runStereo plays
        const float levelChangeDb = static_cast<float>(10.0 * std::log10(0.5 * (meanSquare(left) + meanSquare(right)) / inputMeanSquare));

        float outputPeak = 0.0f;
        for (size_t i = 0; i < left.size(); ++i)
            outputPeak = juce::jmax(outputPeak, std::abs(left[i]), std::abs(right[i]));
        const float peakChangeDb = juce::Decibels::gainToDecibels(outputPeak / calibratedPeak);

        logMessage("Level " + juce::String(levelChangeDb, 1) + " dB, peak " + juce::String(peakChangeDb, 1) + " dB");

        // Fifty-millisecond grains twenty times a second just touch. Each is
        // shaped by its window, so it is not the full level, but near it.
        expect(levelChangeDb > -6.0f && levelChangeDb < 1.0f,
               "Level changed by " + juce::String(levelChangeDb, 1) + " dB");

        // A grain panned hard to one side is 3 dB up on that side, and
        // nothing on the other, as equal-power panning keeps its power
        expect(peakChangeDb < 4.0f,
               "Peak rose by " + juce::String(peakChangeDb, 1) + " dB");
    }

    void testStereoInputIsOneStream()
    {
        beginTest("A stereo input reaches the grains as one unbroken stream");

        // A 1 kHz tone, the same on both sides. Grains of an unbroken tone are
        // tone too; grains of one that jumps back every block are smeared
        // across the spectrum.
        constexpr double frequency = 1000.0;
        const auto output = run([&] (int n)
        {
            return 0.3f * static_cast<float>(std::sin(juce::MathConstants<double>::twoPi * frequency * n / sampleRate));
        });

        constexpr int order = 15;
        constexpr int size = 1 << order;
        juce::dsp::FFT fft(order);
        std::vector<float> data(2 * size, 0.0f);
        std::copy(output.begin(), output.begin() + size, data.begin());
        fft.performFrequencyOnlyForwardTransform(data.data());

        // 100 Hz either side of the tone holds a 50 ms Hann grain's main lobe
        const double binWidth = sampleRate / size;
        double near = 0.0, total = 0.0;
        for (int bin = 1; bin < size / 2; ++bin)
        {
            const double energy = static_cast<double>(data[bin]) * data[bin];
            total += energy;
            if (std::abs(bin * binWidth - frequency) <= 100.0)
                near += energy;
        }

        expect(total > 0.0, "The grains are silent");
        expect(near / total > 0.95,
               "Only " + juce::String(100.0 * near / total, 1) + "% of the output is near the tone");
    }

    // Left and right output of a mono noise input, over the second second
    static std::pair<std::vector<float>, std::vector<float>> runStereo(float width)
    {
        GranularEngineProcessor processor;
        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);
        set(processor, "stereoWidth", width);

        juce::Random random(11);
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midi;
        std::vector<float> left, right;

        for (int block = 0; block < numBlocks; ++block)
        {
            for (int i = 0; i < blockSize; ++i)
            {
                const float x = calibratedPeak * (2.0f * random.nextFloat() - 1.0f);
                buffer.setSample(0, i, x);
                buffer.setSample(1, i, x);
            }

            processor.processBlock(buffer, midi);

            if (block >= settledBlock)
            {
                left.insert(left.end(), buffer.getReadPointer(0), buffer.getReadPointer(0) + blockSize);
                right.insert(right.end(), buffer.getReadPointer(1), buffer.getReadPointer(1) + blockSize);
            }
        }

        return { left, right };
    }

    // How much of the output differs between the sides: 0 for mono
    static double sideShare(const std::pair<std::vector<float>, std::vector<float>>& lr)
    {
        double mid = 0.0, side = 0.0;
        for (size_t i = 0; i < lr.first.size(); ++i)
        {
            const double m = 0.5 * (lr.first[i] + lr.second[i]), s = 0.5 * (lr.first[i] - lr.second[i]);
            mid += m * m;
            side += s * s;
        }
        return side / (mid + side);
    }

    void testWidthSpreadsTheGrains()
    {
        beginTest("Width spreads the grains across the stereo field, and at zero keeps them in the centre");

        // At its default of 100% every grain used to land in the centre
        const double atDefault = sideShare(runStereo(100.0f));
        const double atZero = sideShare(runStereo(0.0f));
        const double atFull = sideShare(runStereo(200.0f));

        expect(atDefault > 0.1, "At 100% only " + juce::String(100.0 * atDefault, 1) + "% of the output is stereo");
        expect(atZero < 1.0e-6, "At 0% " + juce::String(100.0 * atZero, 3) + "% of the output is stereo");
        expect(atFull > atDefault, "200% should be wider than 100%");
    }

    // The share of grains played backwards, read from a sawtooth input: it
    // rises slowly and drops sharply, so a grain played forwards keeps its
    // sharp drops and one played backwards turns them into sharp rises
    static double backwardShare(float reversePercent)
    {
        GranularEngineProcessor processor;
        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);
        set(processor, "reverse", reversePercent);
        set(processor, "stereoWidth", 0.0f);   // Every grain on both sides

        constexpr double period = 240.0;       // 200 Hz
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midi;
        int up = 0, down = 0;
        float previous = 0.0f;

        // Five seconds, about a hundred grains, so chance alone moves the
        // share at 50% by only about 0.05
        const int blocks = settledBlock + 5 * static_cast<int>(sampleRate / blockSize);
        for (int block = 0; block < blocks; ++block)
        {
            for (int i = 0; i < blockSize; ++i)
            {
                const double phase = std::fmod(block * blockSize + i, period) / period;
                const float x = 0.3f * static_cast<float>(2.0 * phase - 1.0);
                buffer.setSample(0, i, x);
                buffer.setSample(1, i, x);
            }

            processor.processBlock(buffer, midi);

            for (int i = 0; i < blockSize; ++i)
            {
                const float y = buffer.getSample(0, i);
                if (block >= settledBlock)
                {
                    // A jump many times the slow slope's 0.0025 per sample
                    if (y - previous > 0.05f) ++up;
                    if (previous - y > 0.05f) ++down;
                }
                previous = y;
            }
        }

        return up + down > 0 ? static_cast<double>(up) / (up + down) : std::nan("");
    }

    void testReverseSetsTheShareOfBackwardGrains()
    {
        beginTest("Reverse sets the share of grains played backwards");

        // It used to do nothing: every grain played forwards
        const double atNone = backwardShare(0.0f);
        const double atHalf = backwardShare(50.0f);
        const double atAll = backwardShare(100.0f);

        logMessage("Backward share at 0, 50, 100%: " + juce::String(atNone, 2) + ", "
                   + juce::String(atHalf, 2) + ", " + juce::String(atAll, 2));

        expect(atNone < 0.05, "At 0% " + juce::String(100.0 * atNone, 1) + "% of grains played backwards");
        expect(atHalf > 0.3 && atHalf < 0.7, "At 50% " + juce::String(100.0 * atHalf, 1) + "% of grains played backwards");
        expect(atAll > 0.95, "At 100% only " + juce::String(100.0 * atAll, 1) + "% of grains played backwards");
    }

    //==========================================================================
    // The editor

    static void set(juce::AudioProcessor& processor, const juce::String& id, float value)
    {
        for (auto* param : processor.getParameters())
            if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*>(param); ranged != nullptr && ranged->paramID == id)
                ranged->setValueNotifyingHost(ranged->convertTo0to1(value));
    }

    static float get(juce::AudioProcessor& processor, const juce::String& id)
    {
        for (auto* param : processor.getParameters())
            if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*>(param); ranged != nullptr && ranged->paramID == id)
                return ranged->convertFrom0to1(ranged->getValue());
        return std::nanf("");
    }

    template <typename Control>
    Control& find(juce::Component& editor, const juce::String& id)
    {
        auto* control = dynamic_cast<Control*>(editor.findChildWithID(id));
        expect(control != nullptr, "No control for " + id);
        static Control missing;
        return control != nullptr ? *control : missing;
    }

    // How fast the output rises, per second, when the input is a slow ramp:
    // each grain comes out at the level of the moment it was cut from, so
    // this is how fast the grains move through the input
    static double rampSlopeOut(float scanSpeed)
    {
        GranularEngineProcessor processor;
        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);
        set(processor, "timeStretch", scanSpeed);
        set(processor, "position", 1.0f);      // The newest audio
        set(processor, "stereoWidth", 0.0f);   // Every grain on both sides

        const int blocks = 3 * static_cast<int>(sampleRate / blockSize);
        const double rampPerSample = 0.3 / (blocks * blockSize);
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midi;

        // Mean output over a quarter second, from 1 s and from 2.5 s; a
        // quarter second holds five whole grains, so their windows average out
        auto windowMean = [](const std::vector<float>& x, double startSeconds)
        {
            const int start = static_cast<int>(startSeconds * sampleRate);
            const int length = static_cast<int>(0.25 * sampleRate);
            double sum = 0.0;
            for (int i = start; i < start + length; ++i)
                sum += x[(size_t) i];
            return sum / length;
        };

        std::vector<float> output;
        for (int block = 0; block < blocks; ++block)
        {
            for (int i = 0; i < blockSize; ++i)
            {
                const float x = static_cast<float>(0.1 + rampPerSample * (block * blockSize + i));
                buffer.setSample(0, i, x);
                buffer.setSample(1, i, x);
            }
            processor.processBlock(buffer, midi);
            output.insert(output.end(), buffer.getReadPointer(0), buffer.getReadPointer(0) + blockSize);
        }
        return (windowMean(output, 2.5) - windowMean(output, 1.0)) / 1.5;
    }

    void testScanSpeedSetsHowFastGrainsMoveThroughTheInput()
    {
        beginTest("Scan speed sets how fast the grains move through the input");

        // It was stored and never used, so every speed gave the same slope
        const double atOne = rampSlopeOut(1.0f);
        const double atHalf = rampSlopeOut(0.5f) / atOne;
        const double atQuarter = rampSlopeOut(0.25f) / atOne;
        logMessage("Against 1x: 0.5x moves at " + juce::String(atHalf, 3) + ", 0.25x at " + juce::String(atQuarter, 3));

        expect(atOne > 0.0, "At 1x the grains should follow the input");
        expect(std::abs(atHalf - 0.5) < 0.1,
               "At 0.5x the grains should move through the input at half speed, got " + juce::String(atHalf, 2));
        expect(std::abs(atQuarter - 0.25) < 0.07,
               "At 0.25x the grains should move through the input at quarter speed, got " + juce::String(atQuarter, 2));
    }

    void testEditorShowsTheSettingsItOpensWith()
    {
        beginTest("The editor opens showing the plugin's settings, not its defaults");

        // As after a saved session is loaded
        GranularEngineProcessor processor;
        set(processor, "grainSize", 120.0f);
        set(processor, "grainDensity", 55.0f);
        set(processor, "timeStretch", 2.0f);
        set(processor, "dryWet", 40.0f);
        set(processor, "windowType", 3.0f);

        GranularEngineEditor editor(processor);

        expectWithinAbsoluteError(find<juce::Slider>(editor, "grainSize").getValue(), 120.0, 0.01);
        expectWithinAbsoluteError(find<juce::Slider>(editor, "grainDensity").getValue(), 55.0, 0.01);
        expectWithinAbsoluteError(find<juce::Slider>(editor, "timeStretch").getValue(), 2.0, 0.01);
        expectWithinAbsoluteError(find<juce::Slider>(editor, "dryWet").getValue(), 40.0, 0.01);
        expectEquals(find<juce::ComboBox>(editor, "windowType").getText(), juce::String("Tukey"));
    }

    void testEditorFollowsChangesMadeElsewhere()
    {
        beginTest("The editor follows changes made by the host while it is open");

        GranularEngineProcessor processor;
        GranularEngineEditor editor(processor);

        set(processor, "pitchShift", -7.0f);
        set(processor, "spray", 30.0f);

        expectWithinAbsoluteError(find<juce::Slider>(editor, "pitchShift").getValue(), -7.0, 0.01);
        expectWithinAbsoluteError(find<juce::Slider>(editor, "spray").getValue(), 30.0, 0.01);
    }

    void testControlsSetTheValuesTheyShow()
    {
        beginTest("Turning a control sets the value it shows");

        GranularEngineProcessor processor;
        GranularEngineEditor editor(processor);

        // Time Stretch is skewed, so a straight line from 0.25 to 4 missed it
        find<juce::Slider>(editor, "timeStretch").setValue(2.0, juce::sendNotificationSync);
        find<juce::Slider>(editor, "stereoWidth").setValue(150.0, juce::sendNotificationSync);
        find<juce::ComboBox>(editor, "windowType").setSelectedId(5, juce::sendNotificationSync);

        expectWithinAbsoluteError(get(processor, "timeStretch"), 2.0f, 0.01f);
        expectWithinAbsoluteError(get(processor, "stereoWidth"), 150.0f, 0.01f);
        expectWithinAbsoluteError(get(processor, "windowType"), 4.0f, 0.01f, "Blackman");
    }

    void testNoControlsOverlap()
    {
        beginTest("Every control fits in the window without overlapping another");

        GranularEngineProcessor processor;
        GranularEngineEditor editor(processor);

        juce::Array<juce::Component*> shown;
        for (auto* child : editor.getChildren())
            if (child->isVisible())
                shown.add(child);

        for (int i = 0; i < shown.size(); ++i)
        {
            const auto bounds = shown[i]->getBounds();
            expect(editor.getLocalBounds().contains(bounds), describe(shown[i]) + " runs off the window");

            // The Position slider's label used to sit on the Pitch label
            for (int j = i + 1; j < shown.size(); ++j)
                expect(! bounds.intersects(shown[j]->getBounds()),
                       describe(shown[i]) + " overlaps " + describe(shown[j]));
        }
    }

    static juce::String describe(juce::Component* c)
    {
        if (auto* label = dynamic_cast<juce::Label*>(c))
            return "the label '" + label->getText() + "'";
        return c->getComponentID().isNotEmpty() ? c->getComponentID() : c->getBounds().toString();
    }
};

static GranularEngineTests granularEngineTests;
