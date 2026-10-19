/**
 * ConvolutionReverb Tests
 *
 * The reverb is an impulse response named in the plugin's state. Loaded
 * from there, a click should come out as the response: a tail that lasts
 * as long as it does, after the pre-delay. With Mix at zero, or bypassed,
 * the signal passes through untouched, and with no response loaded there
 * is no wet signal at all.
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <cmath>
#include <cstring>

#include "../../plugins/ConvolutionReverb/Source/PluginProcessor.h"

class ConvolutionReverbTests : public juce::UnitTest
{
public:
    ConvolutionReverbTests()
        : juce::UnitTest("ConvolutionReverb", "Plugins")
    {
    }

    void runTest() override
    {
        impulse = juce::File::getSpecialLocation(juce::File::tempDirectory)
                      .getNonexistentChildFile("ConvolutionReverbTests", ".wav");
        writeImpulse(impulse);

        testLoadsFromStateAndMakesATail();
        testPreDelay();
        testMixAtZeroAndBypassPassThrough();
        testNoImpulseNoWet();
        testStateKeepsTheImpulse();
        testParametersReadInUnits();
        testUntrustedFiles();

        impulse.deleteFile();
    }

private:
    static constexpr double sampleRate = 48000.0;
    static constexpr int blockSize = 512;
    static constexpr double impulseSeconds = 0.5;
    juce::File impulse;

    // Half a second of decaying noise, a different noise on each side
    static void writeImpulse(const juce::File& file)
    {
        const int length = static_cast<int>(impulseSeconds * sampleRate);
        juce::AudioBuffer<float> buffer(2, length);
        juce::Random random(11);
        for (int c = 0; c < 2; ++c)
            for (int i = 0; i < length; ++i)
                buffer.setSample(c, i, (2.0f * random.nextFloat() - 1.0f) * std::exp(-8.0f * i / static_cast<float>(sampleRate)));

        std::unique_ptr<juce::OutputStream> stream = std::make_unique<juce::FileOutputStream>(file);
        juce::WavAudioFormat wav;
        auto writer = wav.createWriterFor(stream, juce::AudioFormatWriterOptions()
                                                      .withSampleRate(sampleRate)
                                                      .withNumChannels(2)
                                                      .withBitsPerSample(24));
        writer->writeFromAudioSampleBuffer(buffer, 0, length);
    }

    void loadFromState(ConvolutionReverbProcessor& processor)
    {
        juce::XmlElement state("PARAMETERS");
        state.setAttribute("ir", impulse.getFullPathName());
        juce::MemoryBlock data;
        juce::AudioProcessor::copyXmlToBinary(state, data);
        processor.setStateInformation(data.getData(), static_cast<int>(data.getSize()));
    }

    static void set(ConvolutionReverbProcessor& processor, const char* id, float value)
    {
        auto* param = processor.getValueTreeState().getParameter(id);
        param->setValueNotifyingHost(param->convertTo0to1(value));
    }

    // A click at the start, then silence, through the plugin
    static juce::AudioBuffer<float> clickThrough(ConvolutionReverbProcessor& processor, double seconds)
    {
        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);

        const int length = static_cast<int>(seconds * sampleRate);
        juce::AudioBuffer<float> output(2, length);
        output.clear();
        output.setSample(0, 0, 1.0f);
        output.setSample(1, 0, 1.0f);

        juce::MidiBuffer midi;
        for (int start = 0; start < length; start += blockSize)
        {
            const int count = juce::jmin(blockSize, length - start);
            juce::AudioBuffer<float> block(output.getArrayOfWritePointers(), 2, start, count);
            processor.processBlock(block, midi);
        }
        return output;
    }

    static float rmsOf(const juce::AudioBuffer<float>& buffer, int channel, double from, double to)
    {
        const int start = static_cast<int>(from * sampleRate);
        return buffer.getRMSLevel(channel, start, static_cast<int>(to * sampleRate) - start);
    }

    void testLoadsFromStateAndMakesATail()
    {
        beginTest("An impulse response named in the state turns a click into its tail");

        ConvolutionReverbProcessor processor;
        loadFromState(processor);
        expect(processor.getImpulseResponseFile() == impulse);
        expectWithinAbsoluteError(processor.getImpulseResponseSeconds(), impulseSeconds, 0.001);
        expectGreaterThan(processor.getTailLengthSeconds(), impulseSeconds);

        set(processor, "mix", 100.0f);
        const auto output = clickThrough(processor, 1.0);

        expectGreaterThan(rmsOf(output, 0, 0.0, 0.1), 1.0e-3f, "the tail starts at once");
        expectGreaterThan(rmsOf(output, 0, 0.3, 0.4), 1.0e-5f, "and is still going");
        expectLessThan(rmsOf(output, 0, 0.3, 0.4), rmsOf(output, 0, 0.0, 0.1), "but dying away");
        expectLessThan(output.getMagnitude(0, static_cast<int>(0.55 * sampleRate), static_cast<int>(0.4 * sampleRate)),
                       1.0e-5f, "silence once the response is over");

        bool sidesDiffer = false;
        for (int i = 100; i < 1000; ++i)
            sidesDiffer |= std::abs(output.getSample(0, i) - output.getSample(1, i)) > 1.0e-4f;
        expect(sidesDiffer, "a stereo response gives each side its own tail");

        beginTest("A file that isn't audio isn't loaded");
        juce::String error;
        expect(! processor.loadImpulseResponse(juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile("no-such-ir.wav"), error));
        expect(error.isNotEmpty());
        expect(processor.getImpulseResponseFile() == impulse, "the old one stays");
    }

    void testPreDelay()
    {
        beginTest("Pre-delay holds the tail back");

        ConvolutionReverbProcessor processor;
        loadFromState(processor);
        set(processor, "mix", 100.0f);
        set(processor, "predelay", 100.0f);
        const auto output = clickThrough(processor, 0.5);

        expectLessThan(output.getMagnitude(0, 0, static_cast<int>(0.099 * sampleRate)), 1.0e-6f, "nothing for 100 ms");
        expectGreaterThan(output.getMagnitude(0, static_cast<int>(0.1 * sampleRate), 2400), 1.0e-3f, "then the room");
    }

    void testMixAtZeroAndBypassPassThrough()
    {
        beginTest("With Mix at zero, or bypassed, the click comes through untouched");

        for (const bool bypass : { false, true })
        {
            ConvolutionReverbProcessor processor;
            loadFromState(processor);
            if (bypass)
            {
                set(processor, "mix", 100.0f);
                processor.getValueTreeState().getParameter("bypass")->setValueNotifyingHost(1.0f);
            }
            else
            {
                set(processor, "mix", 0.0f);
            }

            const auto output = clickThrough(processor, 0.2);
            expectWithinAbsoluteError(output.getSample(0, 0), 1.0f, 1.0e-6f);
            expectLessThan(output.getMagnitude(0, 1, output.getNumSamples() - 1), 1.0e-6f);
        }

        ConvolutionReverbProcessor processor;
        expect(processor.getBypassParameter() == processor.getValueTreeState().getParameter("bypass"),
               "the host's bypass is the plugin's own");
    }

    void testNoImpulseNoWet()
    {
        beginTest("With no impulse response loaded there is no wet signal");

        ConvolutionReverbProcessor processor;
        set(processor, "mix", 100.0f);
        const auto output = clickThrough(processor, 0.2);
        expectEquals(output.getMagnitude(0, 0, output.getNumSamples()), 0.0f);
    }

    void testStateKeepsTheImpulse()
    {
        beginTest("The saved state names the impulse response, and loading it brings it back");

        ConvolutionReverbProcessor processor;
        loadFromState(processor);
        set(processor, "lowcut", 200.0f);

        juce::MemoryBlock saved;
        processor.getStateInformation(saved);
        auto xml = juce::AudioProcessor::getXmlFromBinary(saved.getData(), static_cast<int>(saved.getSize()));
        expect(xml != nullptr && xml->getStringAttribute("ir") == impulse.getFullPathName());

        ConvolutionReverbProcessor restored;
        restored.setStateInformation(saved.getData(), static_cast<int>(saved.getSize()));
        expect(restored.getImpulseResponseFile() == impulse);
        expectWithinAbsoluteError(restored.getValueTreeState().getRawParameterValue("lowcut")->load(), 200.0f, 1.0f);
    }

    void testParametersReadInUnits()
    {
        beginTest("Parameters read as the values they mean");

        ConvolutionReverbProcessor processor;
        const auto text = [&](const char* id, float value) {
            auto* param = processor.getValueTreeState().getParameter(id);
            return param->getText(param->convertTo0to1(value), 32) + " " + param->getLabel();
        };
        expectEquals(text("mix", 30.0f), juce::String("30 %"));
        expectEquals(text("predelay", 120.0f), juce::String("120 ms"));
        expectEquals(text("lowcut", 250.0f), juce::String("250 Hz"));
        expectEquals(text("highcut", 8000.0f), juce::String("8000 Hz"));
        expectEquals(text("output", -6.0f), juce::String("-6.0 dB"));
    }
    void testUntrustedFiles()
    {
        beginTest("An impulse response on a network path, or with an unlikely header, isn't loaded");

        ConvolutionReverbProcessor processor;
        loadFromState(processor);
        expect(processor.getImpulseResponseFile() == impulse);
        juce::String error;
        expect(! processor.loadImpulseResponse(juce::File("//host/share/hall.wav"), error));
        expect(error.contains("network"));

        juce::XmlElement state("PARAMETERS");
        state.setAttribute("ir", "\\\\host\\share\\hall.wav");
        juce::MemoryBlock data;
        juce::AudioProcessor::copyXmlToBinary(state, data);
        processor.setStateInformation(data.getData(), static_cast<int>(data.getSize()));
        expect(processor.getImpulseResponseFile() == juce::File(), "the older impulse response doesn't stay");
        expectEquals(processor.getImpulseResponseSeconds(), 0.0);
        set(processor, "mix", 100.0f);
        expectEquals(clickThrough(processor, 0.2).getMagnitude(0, 0, 9600), 0.0f, "and nothing of it plays");

        const auto bad = juce::File::getSpecialLocation(juce::File::tempDirectory)
                             .getNonexistentChildFile("ConvolutionReverbBadHeader", ".wav");
        for (const juce::uint32 rate : { 1000000000u, 4000u })
        {
            writeImpulse(bad);
            juce::MemoryBlock bytes;
            bad.loadFileAsData(bytes);
            auto* raw = static_cast<char*>(bytes.getData());
            for (size_t i = 12; i + 16 < bytes.getSize(); ++i)
                if (std::memcmp(raw + i, "fmt ", 4) == 0) { std::memcpy(raw + i + 12, &rate, 4); break; }
            bad.replaceWithData(bytes.getData(), bytes.getSize());

            expect(! processor.loadImpulseResponse(bad, error), juce::String(rate) + " Hz");
            expect(error.contains("sample rate"), error);
        }
        bad.deleteFile();
        expect(processor.getImpulseResponseFile() == juce::File(), "nothing was loaded");
    }
};

static ConvolutionReverbTests convolutionReverbTests;
