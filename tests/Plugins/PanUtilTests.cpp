/**
 * PanUtil Plugin Tests
 *
 * Integration tests for the PanUtil stereo panning plugin:
 * - Plugin initialization
 * - Pan, width, and mode parameters
 * - Panning algorithm correctness
 * - Balance mode
 * - Stereo width processing
 * - State save/restore
 */

#include <juce_audio_processors/juce_audio_processors.h>

// Include PanUtil plugin headers
#include "../../plugins/PanUtil/Source/PluginProcessor.h"
#include "../../plugins/PanUtil/Source/PluginEditor.h"

class PanUtilPluginTests : public juce::UnitTest
{
public:
    PanUtilPluginTests()
        : juce::UnitTest("PanUtil Plugin", "Plugins")
    {
    }

    void runTest() override
    {
        testPluginInitialization();
        testParameterRanges();
        testPanModeProcessing();
        testBalanceModeProcessing();
        testWidthControl();
        testStateManagement();
        testLevelMetering();
        testGainAndMonoExist();
        testDefaultsLeaveTheSignalAlone();
        testGain();
        testMono();
        testOldStateStillLoads();
        testEmptyStateChangesNothing();
        testStateKeepsGainAndMono();
        testChangesGlide();
        testBadNumbersInAStateAreSkipped();
        testNeedsStereo();
        testStereoFieldIsClearOfTheControls();
    }

private:
    void testPluginInitialization()
    {
        beginTest("Plugin initialization");

        PanUtilProcessor processor;

        // Check basic plugin properties
        expect(processor.getName().isNotEmpty(), "Plugin should have a name");
        expect(!processor.acceptsMidi(), "Should not accept MIDI");
        expect(!processor.producesMidi(), "Should not produce MIDI");
        expect(processor.getTailLengthSeconds() == 0.0, "Should have no tail");

        // Check bus configuration (stereo)
        expect(processor.getBusCount(true) == 1, "Should have one input bus");
        expect(processor.getBusCount(false) == 1, "Should have one output bus");
    }

    void testParameterRanges()
    {
        beginTest("Parameter range validation");

        PanUtilProcessor processor;
        const auto& params = processor.getParameters();

        expect(params.size() == 5, "Should have 5 parameters");

        // Check pan parameter
        auto* panParam = dynamic_cast<juce::AudioParameterFloat*>(params[0]);
        expect(panParam != nullptr, "Pan should be float parameter");

        auto panRange = panParam->getNormalisableRange();
        expectEquals(panRange.start, -1.0f, "Pan min should be -1.0");
        expectEquals(panRange.end, 1.0f, "Pan max should be +1.0");
        expectEquals(panParam->get(), 0.0f, "Default pan should be center");

        // Check width parameter
        auto* widthParam = dynamic_cast<juce::AudioParameterFloat*>(params[1]);
        expect(widthParam != nullptr, "Width should be float parameter");

        auto widthRange = widthParam->getNormalisableRange();
        expectEquals(widthRange.start, 0.0f, "Width min should be 0.0");
        expectEquals(widthRange.end, 2.0f, "Width max should be 2.0");
        expectEquals(widthParam->get(), 1.0f, "Default width should be 1.0");

        // Check mode parameter
        auto* modeParam = dynamic_cast<juce::AudioParameterChoice*>(params[2]);
        expect(modeParam != nullptr, "Mode should be choice parameter");
        expectEquals(modeParam->getIndex(), 1, "Default mode should be Balance, which leaves a centred signal alone");
    }

    void testPanModeProcessing()
    {
        beginTest("Pan mode processing");

        PanUtilProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        const auto& params = processor.getParameters();
        auto* panParam = dynamic_cast<juce::AudioParameterFloat*>(params[0]);
        auto* modeParam = dynamic_cast<juce::AudioParameterChoice*>(params[2]);

        // Set to pan mode
        modeParam->setValueNotifyingHost(0.0f);

        // Create mono test signal
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        for (int i = 0; i < blockSize; ++i)
        {
            float value = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * i / blockSize);
            buffer.setSample(0, i, value);
            buffer.setSample(1, i, value);
        }

        // Test hard left pan
        panParam->setValueNotifyingHost(panParam->convertTo0to1(-1.0f));

        // Process multiple blocks to let smoothing settle
        for (int i = 0; i < 10; ++i)
        {
            buffer.clear();
            for (int j = 0; j < blockSize; ++j)
            {
                float value = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * j / blockSize);
                buffer.setSample(0, j, value);
                buffer.setSample(1, j, value);
            }
            processor.processBlock(buffer, midiBuffer);
        }

        float leftPeak = buffer.getMagnitude(0, 0, blockSize);
        float rightPeak = buffer.getMagnitude(1, 0, blockSize);

        expect(leftPeak > 0.4f, "Left channel should be loud when panned left");
        expect(rightPeak < 0.1f, "Right channel should be quiet when panned left");

        // Test center pan
        panParam->setValueNotifyingHost(panParam->convertTo0to1(0.0f));

        for (int i = 0; i < 10; ++i)
        {
            buffer.clear();
            for (int j = 0; j < blockSize; ++j)
            {
                float value = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * j / blockSize);
                buffer.setSample(0, j, value);
                buffer.setSample(1, j, value);
            }
            processor.processBlock(buffer, midiBuffer);
        }

        leftPeak = buffer.getMagnitude(0, 0, blockSize);
        rightPeak = buffer.getMagnitude(1, 0, blockSize);

        // At center, both channels should be similar (~-3 dB each)
        // Using slightly larger tolerance to account for parameter smoothing iterations
        expectWithinAbsoluteError(leftPeak, rightPeak, 0.1f,
                                "Center pan should have balanced L/R");
    }

    void testBalanceModeProcessing()
    {
        beginTest("Balance mode processing");

        PanUtilProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        const auto& params = processor.getParameters();
        auto* panParam = dynamic_cast<juce::AudioParameterFloat*>(params[0]);
        auto* modeParam = dynamic_cast<juce::AudioParameterChoice*>(params[2]);

        // Set to balance mode
        modeParam->setValueNotifyingHost(1.0f);

        // Create stereo test signal
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        buffer.clear();
        for (int i = 0; i < blockSize; ++i)
        {
            buffer.setSample(0, i, 0.8f); // Left
            buffer.setSample(1, i, 0.6f); // Right
        }

        // Test balance left (should attenuate right)
        panParam->setValueNotifyingHost(panParam->convertTo0to1(-0.5f));

        // Process multiple blocks to let smoothing settle
        for (int i = 0; i < 10; ++i)
            processor.processBlock(buffer, midiBuffer);

        float leftValue = std::abs(buffer.getSample(0, blockSize - 1));
        float rightValue = std::abs(buffer.getSample(1, blockSize - 1));

        expect(leftValue > 0.7f, "Left should be mostly unchanged in balance left");
        expect(rightValue < 0.4f, "Right should be attenuated in balance left");
    }

    void testWidthControl()
    {
        beginTest("Stereo width control");

        PanUtilProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        const auto& params = processor.getParameters();
        auto* widthParam = dynamic_cast<juce::AudioParameterFloat*>(params[1]);

        // Create stereo signal with phase difference
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        for (int i = 0; i < blockSize; ++i)
        {
            buffer.setSample(0, i, 1.0f);  // Left positive
            buffer.setSample(1, i, -1.0f); // Right negative (max stereo)
        }

        // Test width = 0 (mono)
        widthParam->setValueNotifyingHost(widthParam->convertTo0to1(0.0f));

        // Half a second of blocks, to let the 50 ms smoothing settle
        for (int i = 0; i < 47; ++i)
        {
            buffer.clear();
            for (int j = 0; j < blockSize; ++j)
            {
                buffer.setSample(0, j, 1.0f);  // Left positive
                buffer.setSample(1, j, -1.0f); // Right negative (max stereo)
            }
            processor.processBlock(buffer, midiBuffer);
        }

        float leftFinal = buffer.getSample(0, blockSize - 1);
        float rightFinal = buffer.getSample(1, blockSize - 1);

        expectWithinAbsoluteError(leftFinal, rightFinal, 0.01f,
                                "Width 0 should produce mono (L ≈ R)");

        // Test width = 2.0 (wide)
        widthParam->setValueNotifyingHost(widthParam->convertTo0to1(2.0f));

        for (int i = 0; i < 47; ++i)
        {
            buffer.clear();
            for (int j = 0; j < blockSize; ++j)
            {
                buffer.setSample(0, j, 1.0f);
                buffer.setSample(1, j, -1.0f);
            }
            processor.processBlock(buffer, midiBuffer);
        }

        leftFinal = buffer.getSample(0, blockSize - 1);
        rightFinal = buffer.getSample(1, blockSize - 1);

        float stereoSeparation = std::abs(leftFinal - rightFinal);
        expect(stereoSeparation > 2.0f,
             "Width 2.0 should enhance stereo separation");
    }

    void testStateManagement()
    {
        beginTest("State save and restore");

        PanUtilProcessor processor1;

        // Set specific parameter values
        const auto& params = processor1.getParameters();
        auto* panParam = dynamic_cast<juce::AudioParameterFloat*>(params[0]);
        auto* widthParam = dynamic_cast<juce::AudioParameterFloat*>(params[1]);
        auto* modeParam = dynamic_cast<juce::AudioParameterChoice*>(params[2]);

        panParam->setValueNotifyingHost(panParam->convertTo0to1(0.75f));
        widthParam->setValueNotifyingHost(widthParam->convertTo0to1(1.5f));
        modeParam->setValueNotifyingHost(1.0f); // Balance mode

        // Save state
        juce::MemoryBlock stateData;
        processor1.getStateInformation(stateData);

        expect(stateData.getSize() > 0, "State data should not be empty");

        // Create new processor and restore state
        PanUtilProcessor processor2;
        processor2.setStateInformation(stateData.getData(),
                                      static_cast<int>(stateData.getSize()));

        // Check that parameters were restored
        const auto& params2 = processor2.getParameters();
        auto* panParam2 = dynamic_cast<juce::AudioParameterFloat*>(params2[0]);
        auto* widthParam2 = dynamic_cast<juce::AudioParameterFloat*>(params2[1]);
        auto* modeParam2 = dynamic_cast<juce::AudioParameterChoice*>(params2[2]);

        expectWithinAbsoluteError(panParam2->get(), 0.75f, 0.01f, "Pan should be restored");
        expectWithinAbsoluteError(widthParam2->get(), 1.5f, 0.01f, "Width should be restored");
        expectEquals(modeParam2->getIndex(), 1, "Mode should be restored");
    }

    void testLevelMetering()
    {
        beginTest("Level metering (L/R independent)");

        PanUtilProcessor processor;

        // Prepare processor
        const double sampleRate = 48000.0;
        const int blockSize = 512;
        processor.prepareToPlay(sampleRate, blockSize);

        // Create buffer with different L/R levels
        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midiBuffer;

        buffer.clear();
        buffer.setSample(0, 0, 0.9f);  // Left peak
        buffer.setSample(1, 100, 0.5f); // Right peak

        // Process
        processor.processBlock(buffer, midiBuffer);

        // Check level meters
        float leftLevel = processor.getLeftLevel();
        float rightLevel = processor.getRightLevel();

        expect(leftLevel > 0.0f, "Left level meter should register signal");
        expect(rightLevel > 0.0f, "Right level meter should register signal");
        expect(leftLevel != rightLevel, "L/R meters should be independent");
    }

    //==========================================================================
    // Gain and mono: with pan and width, what a track utility needs

    static juce::RangedAudioParameter& parameter(PanUtilProcessor& processor, const juce::String& name)
    {
        for (auto* param : processor.getParameters())
            if (param->getName(64) == name)
                return *dynamic_cast<juce::RangedAudioParameter*>(param);

        jassertfalse;
        return *dynamic_cast<juce::RangedAudioParameter*>(processor.getParameters()[0]);
    }

    static float shown(PanUtilProcessor& processor, const juce::String& name)
    {
        auto& param = parameter(processor, name);
        return param.convertFrom0to1(param.getValue());
    }

    static void set(PanUtilProcessor& processor, const juce::String& name, float value)
    {
        auto& param = parameter(processor, name);
        param.setValueNotifyingHost(param.convertTo0to1(value));
    }

    struct Levels { float left, right; };

    // A steady signal through the plugin for half a second; what it settles at
    static Levels settle(PanUtilProcessor& processor, float left, float right)
    {
        processor.setRateAndBufferSizeDetails(48000.0, 512);
        processor.prepareToPlay(48000.0, 512);

        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midi;

        for (int block = 0; block < 47; ++block)
        {
            for (int i = 0; i < 512; ++i)
            {
                buffer.setSample(0, i, left);
                buffer.setSample(1, i, right);
            }

            processor.processBlock(buffer, midi);
        }

        return { buffer.getSample(0, 511), buffer.getSample(1, 511) };
    }

    void testGainAndMonoExist()
    {
        beginTest("Gain and Mono come after the parameters hosts already know");

        PanUtilProcessor processor;
        const auto& params = processor.getParameters();

        expectEquals(params.size(), 5);
        expectEquals(params[0]->getName(64), juce::String("Pan"));
        expectEquals(params[1]->getName(64), juce::String("Width"));
        expectEquals(params[2]->getName(64), juce::String("Mode"));

        if (params.size() == 5)
        {
            expectEquals(params[3]->getName(64), juce::String("Gain"));
            expectEquals(params[4]->getName(64), juce::String("Mono"));
            expectWithinAbsoluteError(shown(processor, "Gain"), 0.0f, 0.0001f, "Gain starts at 0 dB");
            expectEquals(shown(processor, "Mono"), 0.0f, "Mono starts off");
        }
    }

    void testDefaultsLeaveTheSignalAlone()
    {
        beginTest("At its default settings the signal passes unchanged");

        PanUtilProcessor processor;
        const auto out = settle(processor, 0.5f, -0.25f);

        expectWithinAbsoluteError(out.left, 0.5f, 0.0001f);
        expectWithinAbsoluteError(out.right, -0.25f, 0.0001f);
    }

    void testGain()
    {
        beginTest("Gain changes the level by the decibels it shows");

        for (float gainDb : { -36.0f, -12.0f, -6.0f, 6.0f, 24.0f })
        {
            PanUtilProcessor processor;
            set(processor, "Mode", 1.0f); // Balance: no pan law in the way
            set(processor, "Gain", gainDb);

            const auto out = settle(processor, 0.01f, 0.01f);
            const float expected = 0.01f * juce::Decibels::decibelsToGain(gainDb);

            expectWithinAbsoluteError(out.left, expected, expected * 0.001f, juce::String(gainDb, 0) + " dB, left");
            expectWithinAbsoluteError(out.right, expected, expected * 0.001f, juce::String(gainDb, 0) + " dB, right");
        }
    }

    void testMono()
    {
        beginTest("Mono puts the same signal on both channels");

        PanUtilProcessor processor;
        set(processor, "Mode", 1.0f);
        set(processor, "Mono", 1.0f);
        set(processor, "Width", 2.0f); // Mono wins over width

        const auto out = settle(processor, 0.8f, 0.2f);

        expectWithinAbsoluteError(out.left, 0.5f, 0.0001f, "Left should be the average of the two");
        expectWithinAbsoluteError(out.right, 0.5f, 0.0001f, "Right should be the average of the two");
    }

    void testOldStateStillLoads()
    {
        beginTest("A state saved before Gain and Mono existed still loads");

        juce::MemoryBlock old;
        {
            juce::MemoryOutputStream stream(old, false);
            stream.writeFloat(-0.5f); // Pan
            stream.writeFloat(1.5f);  // Width
            stream.writeInt(1);       // Mode: Balance
        }

        PanUtilProcessor processor;
        set(processor, "Gain", -6.0f);
        processor.setStateInformation(old.getData(), static_cast<int>(old.getSize()));

        expectWithinAbsoluteError(shown(processor, "Pan"), -0.5f, 0.001f);
        expectWithinAbsoluteError(shown(processor, "Width"), 1.5f, 0.001f);
        expectEquals(shown(processor, "Mode"), 1.0f);
        expectWithinAbsoluteError(shown(processor, "Gain"), -6.0f, 0.001f, "What the old state did not hold is left alone");
    }

    void testEmptyStateChangesNothing()
    {
        beginTest("A state too short to read changes nothing");

        PanUtilProcessor processor;
        const char junk[] = { 1, 2, 3 };
        processor.setStateInformation(junk, sizeof(junk));
        processor.setStateInformation(nullptr, 0);

        // It used to read zeros, which set Width to nothing: mono
        expectWithinAbsoluteError(shown(processor, "Width"), 1.0f, 0.001f);
        expectWithinAbsoluteError(shown(processor, "Pan"), 0.0f, 0.001f);
    }

    void testStateKeepsGainAndMono()
    {
        beginTest("A saved state keeps Gain and Mono");

        PanUtilProcessor original;
        set(original, "Gain", 4.5f);
        set(original, "Mono", 1.0f);
        set(original, "Pan", 0.25f);

        juce::MemoryBlock state;
        original.getStateInformation(state);

        PanUtilProcessor restored;
        restored.setStateInformation(state.getData(), static_cast<int>(state.getSize()));

        expectWithinAbsoluteError(shown(restored, "Gain"), 4.5f, 0.001f);
        expectEquals(shown(restored, "Mono"), 1.0f);
        expectWithinAbsoluteError(shown(restored, "Pan"), 0.25f, 0.001f);
    }

    void testChangesGlide()
    {
        beginTest("Gain and Mono glide to their new settings while audio plays");

        // Left and right opposite, so Mono takes the signal away entirely
        for (const char* name : { "Gain", "Mono" })
        {
            PanUtilProcessor processor;
            processor.setRateAndBufferSizeDetails(48000.0, 512);
            processor.prepareToPlay(48000.0, 512);

            set(processor, name, juce::String(name) == "Gain" ? -36.0f : 1.0f);

            juce::AudioBuffer<float> buffer(2, 512);
            juce::MidiBuffer midi;
            for (int i = 0; i < 512; ++i)
            {
                buffer.setSample(0, i, 0.5f);
                buffer.setSample(1, i, -0.5f);
            }

            processor.processBlock(buffer, midi);

            float largestStep = std::abs(buffer.getSample(0, 0) - 0.5f);
            for (int i = 1; i < 512; ++i)
                largestStep = juce::jmax(largestStep, std::abs(buffer.getSample(0, i) - buffer.getSample(0, i - 1)));

            expect(buffer.getSample(0, 0) > 0.45f, juce::String(name) + ": the first sample should still be near where it was");
            expect(buffer.getSample(0, 511) < 0.45f, juce::String(name) + ": it should be on its way by the end of the block");
            expect(largestStep < 0.01f, juce::String(name) + " moved in a step of " + juce::String(largestStep, 4));
        }
    }

    void testBadNumbersInAStateAreSkipped()
    {
        beginTest("A state holding something that is not a number leaves that setting alone");

        juce::MemoryBlock bad;
        {
            juce::MemoryOutputStream stream(bad, false);
            stream.writeFloat(std::numeric_limits<float>::quiet_NaN());  // Pan
            stream.writeFloat(0.5f);                                     // Width
            stream.writeInt(1);                                          // Mode
            stream.writeFloat(std::numeric_limits<float>::infinity());   // Gain
            stream.writeInt(0);                                          // Mono
        }

        PanUtilProcessor processor;
        set(processor, "Pan", 0.3f);
        set(processor, "Gain", -3.0f);
        processor.setStateInformation(bad.getData(), static_cast<int>(bad.getSize()));

        expectWithinAbsoluteError(shown(processor, "Pan"), 0.3f, 0.001f);
        expectWithinAbsoluteError(shown(processor, "Gain"), -3.0f, 0.001f);
        expectWithinAbsoluteError(shown(processor, "Width"), 0.5f, 0.001f, "The numbers that are good are still read");
    }

    void testNeedsStereo()
    {
        beginTest("The plugin asks for stereo in and out");

        PanUtilProcessor processor;
        juce::AudioProcessor::BusesLayout stereo, mono;
        stereo.inputBuses.add(juce::AudioChannelSet::stereo());
        stereo.outputBuses.add(juce::AudioChannelSet::stereo());
        mono.inputBuses.add(juce::AudioChannelSet::mono());
        mono.outputBuses.add(juce::AudioChannelSet::mono());

        expect(processor.checkBusesLayoutSupported(stereo));
        expect(! processor.checkBusesLayoutSupported(mono), "On one channel it could do nothing, and used to do so silently");
    }

    void testStereoFieldIsClearOfTheControls()
    {
        beginTest("The stereo field is drawn clear of every control");

        PanUtilProcessor processor;
        PanUtilEditor editor(processor);
        const auto field = editor.getStereoFieldBounds();

        expect(editor.getLocalBounds().contains(field), "The stereo field is drawn off the edge of the window");

        // It used to sit on top of the Pan and Width sliders
        for (auto* child : editor.getChildren())
            if (child->isVisible())
                expect(! child->getBounds().intersects(field),
                       "The stereo field is drawn under " + child->getName() + " " + child->getBounds().toString());
    }
};

static PanUtilPluginTests panUtilPluginTests;
