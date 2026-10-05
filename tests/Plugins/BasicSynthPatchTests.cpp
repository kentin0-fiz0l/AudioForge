/**
 * BasicSynth factory patches, and what they rely on: a Waveform control
 * that changes the sound, and a saved state that holds every parameter.
 */

#include <juce_audio_processors/juce_audio_processors.h>

#include "../../plugins/BasicSynth/Source/PluginProcessor.h"

class BasicSynthPatchTests : public juce::UnitTest
{
public:
    BasicSynthPatchTests()
        : juce::UnitTest("BasicSynth Patches", "Plugins")
    {
    }

    void runTest() override
    {
        testWaveformChangesTheSound();
        testPatchList();
        testFirstPatchIsTheDefault();
        testBassPatchIsDry();
        testPatchLevels();
        testStateKeepsEveryParameter();
        testStateKeepsThePatchWithoutReloadingIt();
        testOldStateStillLoads();
        testBrokenStateIsIgnored();
    }

private:
    static constexpr double sampleRate = 48000.0;
    static constexpr int blockSize = 512;

    static juce::RangedAudioParameter& parameter(BasicSynthProcessor& processor, const juce::String& name)
    {
        for (auto* param : processor.getParameters())
            if (param->getName(64) == name)
                return *dynamic_cast<juce::RangedAudioParameter*>(param);

        jassertfalse;
        return *dynamic_cast<juce::RangedAudioParameter*>(processor.getParameters()[0]);
    }

    // The value a control shows: seconds, Hz, or the index of a choice
    static float shown(BasicSynthProcessor& processor, const juce::String& name)
    {
        auto& param = parameter(processor, name);
        return param.convertFrom0to1(param.getValue());
    }

    static void set(BasicSynthProcessor& processor, const juce::String& name, float value)
    {
        auto& param = parameter(processor, name);
        param.setValueNotifyingHost(param.convertTo0to1(value));
    }

    static std::vector<float> allShown(BasicSynthProcessor& processor)
    {
        std::vector<float> values;
        for (auto* param : processor.getParameters())
            values.push_back(shown(processor, param->getName(64)));
        return values;
    }

    static int programNamed(BasicSynthProcessor& processor, const juce::String& name)
    {
        for (int i = 0; i < processor.getNumPrograms(); ++i)
            if (processor.getProgramName(i) == name)
                return i;
        return -1;
    }

    // One full-velocity note held for a second, then released: the left channel
    static std::vector<float> render(BasicSynthProcessor& processor, int note, double seconds = 1.5)
    {
        processor.prepareToPlay(sampleRate, blockSize);

        const int totalBlocks = static_cast<int>(seconds * sampleRate / blockSize);
        const int noteOffBlock = static_cast<int>(sampleRate / blockSize);

        juce::AudioBuffer<float> buffer(2, blockSize);
        std::vector<float> output;

        for (int block = 0; block < totalBlocks; ++block)
        {
            juce::MidiBuffer midi;
            if (block == 0)
                midi.addEvent(juce::MidiMessage::noteOn(1, note, (juce::uint8) 127), 0);
            if (block == noteOffBlock)
                midi.addEvent(juce::MidiMessage::noteOff(1, note), 0);

            buffer.clear();
            processor.processBlock(buffer, midi);

            for (int i = 0; i < blockSize; ++i)
                output.push_back(juce::jmax(std::abs(buffer.getSample(0, i)), std::abs(buffer.getSample(1, i))));
        }

        return output;
    }

    struct Shape
    {
        float jaggedness; // Sample-to-sample steps against overall size: low for a sine
        float rms;        // A square is louder than a sawtooth of the same height
    };

    static Shape shapeOf(int waveform)
    {
        BasicSynthProcessor processor;
        set(processor, "Waveform", static_cast<float>(waveform));
        set(processor, "Chorus Mix", 0.0f);
        set(processor, "Reverb Mix", 0.0f);
        processor.prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> buffer(2, blockSize);
        juce::MidiBuffer midi;
        midi.addEvent(juce::MidiMessage::noteOn(1, 48, (juce::uint8) 127), 0);

        double steps = 0.0, size = 0.0;
        for (int block = 0; block < 40; ++block)
        {
            buffer.clear();
            processor.processBlock(buffer, midi);
            midi.clear();

            if (block < 10) // Let the attack pass
                continue;

            for (int i = 1; i < blockSize; ++i)
            {
                const double step = buffer.getSample(0, i) - buffer.getSample(0, i - 1);
                steps += step * step;
                size += buffer.getSample(0, i) * buffer.getSample(0, i);
            }
        }

        const int count = 30 * (blockSize - 1);
        return { size > 0.0 ? static_cast<float>(std::sqrt(steps / size)) : 0.0f,
                 static_cast<float>(std::sqrt(size / count)) };
    }

    void testWaveformChangesTheSound()
    {
        beginTest("Waveform changes the sound");

        const auto sine = shapeOf(0);
        const auto sawtooth = shapeOf(1);
        const auto square = shapeOf(2);

        expect(sine.rms > 0.0f, "A sine note should sound");
        expect(sawtooth.jaggedness > 3.0f * sine.jaggedness, "A sawtooth should be far more jagged than a sine");
        expect(square.jaggedness > 3.0f * sine.jaggedness, "A square should be far more jagged than a sine");
        expect(square.rms > 1.4f * sawtooth.rms, "A square should be louder than a sawtooth of the same height");
    }

    void testPatchList()
    {
        beginTest("There are Init, Bass, Pad and Lead patches");

        BasicSynthProcessor processor;

        expectEquals(processor.getNumPrograms(), 4);
        expectEquals(processor.getCurrentProgram(), 0);
        expectEquals(processor.getProgramName(0), juce::String("Init"));
        expectEquals(processor.getProgramName(1), juce::String("Bass"));
        expectEquals(processor.getProgramName(2), juce::String("Pad"));
        expectEquals(processor.getProgramName(3), juce::String("Lead"));

        processor.setCurrentProgram(2);
        expectEquals(processor.getCurrentProgram(), 2);

        processor.setCurrentProgram(99);
        processor.setCurrentProgram(-1);
        expectEquals(processor.getCurrentProgram(), 2, "A patch that does not exist is ignored");
    }

    void testFirstPatchIsTheDefault()
    {
        beginTest("The first patch is what a new instance starts with");

        BasicSynthProcessor fresh;
        const auto defaults = allShown(fresh);

        expectEquals(shown(fresh, "Waveform"), 1.0f, "A new instance has always sounded as a sawtooth");

        BasicSynthProcessor processor;
        processor.setCurrentProgram(programNamed(processor, "Pad"));
        expect(allShown(processor) != defaults, "Choosing a patch should change the settings");

        processor.setCurrentProgram(0);
        expect(allShown(processor) == defaults, "Choosing the first patch should bring the defaults back");

        // Not only the values shown, but the sound. A value that has been
        // through its parameter's range comes back a rounding error away,
        // so allow for that and no more.
        const auto chosen = render(processor, 48);
        const auto started = render(fresh, 48);

        float difference = 0.0f;
        for (size_t i = 0; i < chosen.size(); ++i)
            difference = juce::jmax(difference, std::abs(chosen[i] - started[i]));

        expect(difference < 1.0e-5f, "The first patch should sound as a new instance does");
    }

    void testBassPatchIsDry()
    {
        beginTest("The bass patch is fast and has no chorus or reverb");

        BasicSynthProcessor processor;
        processor.setCurrentProgram(programNamed(processor, "Bass"));

        expectEquals(shown(processor, "Chorus Mix"), 0.0f);
        expectEquals(shown(processor, "Reverb Mix"), 0.0f);
        expect(shown(processor, "Attack") <= 0.01f, "A bass needs a fast attack");
        expect(shown(processor, "Filter Cutoff") <= 1000.0f, "A bass should be dark");
        expectEquals(processor.getTailLengthSeconds(), 0.0, "With no reverb there is no tail");
    }

    void testPatchLevels()
    {
        beginTest("Every patch's loudest note peaks between -12 and -6 dBFS");

        // The same standard and the same notes as the level check in
        // tools/stress-host, which can only measure the default patch
        BasicSynthProcessor names;

        for (int program = 0; program < names.getNumPrograms(); ++program)
        {
            float loudest = 0.0f;

            for (int note : { 36, 38, 42, 46, 48, 60, 72 })
            {
                BasicSynthProcessor processor;
                processor.setCurrentProgram(program);

                const auto output = render(processor, note);
                loudest = juce::jmax(loudest, *std::max_element(output.begin(), output.end()));
            }

            const float level = juce::Decibels::gainToDecibels(loudest);
            const auto name = names.getProgramName(program);
            logMessage(name + " peaks at " + juce::String(level, 1) + " dBFS");

            expect(level >= -12.0f && level <= -6.0f,
                   name + " peaks at " + juce::String(level, 1) + " dBFS");
        }
    }

    void testStateKeepsEveryParameter()
    {
        beginTest("A saved state keeps every parameter");

        // A value for each parameter that is not its default
        BasicSynthProcessor original;
        for (auto* param : original.getParameters())
            param->setValueNotifyingHost(param->getDefaultValue() < 0.5f ? 0.75f : 0.25f);

        juce::MemoryBlock state;
        original.getStateInformation(state);

        BasicSynthProcessor restored;
        restored.setStateInformation(state.getData(), static_cast<int>(state.getSize()));

        for (auto* param : original.getParameters())
        {
            const auto name = param->getName(64);
            expectWithinAbsoluteError(shown(restored, name), shown(original, name), 0.001f,
                                      name + " should be restored");
        }
    }

    void testStateKeepsThePatchWithoutReloadingIt()
    {
        beginTest("A saved state keeps the chosen patch and the changes made to it");

        BasicSynthProcessor original;
        const int bass = programNamed(original, "Bass");
        original.setCurrentProgram(bass);
        set(original, "Filter Cutoff", 1234.0f);

        juce::MemoryBlock state;
        original.getStateInformation(state);

        BasicSynthProcessor restored;
        restored.setStateInformation(state.getData(), static_cast<int>(state.getSize()));

        expectEquals(restored.getCurrentProgram(), bass);
        expectWithinAbsoluteError(shown(restored, "Filter Cutoff"), 1234.0f, 1.0f,
                                  "A change made after choosing the patch should survive");
    }

    void testOldStateStillLoads()
    {
        beginTest("A state saved before patches existed still loads");

        // The old format: the waveform index and seven values, nothing else.
        // The waveform was saved but never used, so every old set sounded
        // as a sawtooth whatever it says.
        juce::MemoryBlock old;
        {
            juce::MemoryOutputStream stream(old, false);
            stream.writeInt(2);        // Waveform: "Square"
            stream.writeFloat(0.5f);   // Volume
            stream.writeFloat(0.001f); // Attack
            stream.writeFloat(0.1f);   // Decay
            stream.writeFloat(0.7f);   // Sustain
            stream.writeFloat(0.01f);  // Release
            stream.writeFloat(962.0f); // Filter cutoff
            stream.writeFloat(0.7f);   // Filter resonance
        }
        expectEquals(static_cast<int>(old.getSize()), 32);

        BasicSynthProcessor processor;
        processor.setStateInformation(old.getData(), static_cast<int>(old.getSize()));

        expectWithinAbsoluteError(shown(processor, "Volume"), 0.5f, 0.001f);
        expectWithinAbsoluteError(shown(processor, "Attack"), 0.001f, 0.0005f);
        expectWithinAbsoluteError(shown(processor, "Release"), 0.01f, 0.0005f);
        expectWithinAbsoluteError(shown(processor, "Filter Cutoff"), 962.0f, 1.0f);
        expectEquals(shown(processor, "Waveform"), 1.0f, "An old set should keep sounding as a sawtooth");
        expectWithinAbsoluteError(shown(processor, "Chorus Mix"), 0.3f, 0.001f,
                                  "What the old format did not hold stays at its default");
    }

    void testBrokenStateIsIgnored()
    {
        beginTest("A state that cannot be read changes nothing");

        BasicSynthProcessor processor;
        const auto before = allShown(processor);

        const char junk[] = { 1, 2, 3, 4, 5 };
        processor.setStateInformation(junk, sizeof(junk));
        processor.setStateInformation(nullptr, 0);

        // One byte short of, and one byte over, the old format
        const std::vector<char> zeros(33, 0);
        processor.setStateInformation(zeros.data(), 31);
        processor.setStateInformation(zeros.data(), 33);

        // Another plugin's state
        juce::XmlElement other("SomeOtherPlugin");
        other.setAttribute("volume", 0.1);
        juce::MemoryBlock otherState;
        juce::AudioProcessor::copyXmlToBinary(other, otherState);
        processor.setStateInformation(otherState.getData(), static_cast<int>(otherState.getSize()));

        expect(allShown(processor) == before);

        // A patch number that does not exist, with one value that does
        juce::XmlElement odd("BasicSynthState");
        odd.setAttribute("program", 99);
        odd.setAttribute("volume", 0.25);
        juce::MemoryBlock oddState;
        juce::AudioProcessor::copyXmlToBinary(odd, oddState);
        processor.setStateInformation(oddState.getData(), static_cast<int>(oddState.getSize()));

        expectEquals(processor.getCurrentProgram(), 0, "A patch number that does not exist is ignored");
        expectWithinAbsoluteError(shown(processor, "Volume"), 0.25f, 0.001f, "The values that can be read still are");
        expectWithinAbsoluteError(shown(processor, "Attack"), before[2], 0.0001f, "A value the state lacks is left alone");
    }
};

static BasicSynthPatchTests basicSynthPatchTests;
