// Render host.
//
// Renders audio offline through one VST3: a MIDI file through an
// instrument, or a WAV file through an effect, into a WAV file. For making
// music with AudioForge's own plugins without a DAW in the loop (a game's
// score, test material), and for hearing what a plugin does to a part.
//
// Usage:
//   RenderHost <plugin.vst3> --list
//   RenderHost <plugin.vst3> [options] --out <out.wav>
//
// Options:
//   --midi <file.mid>       Notes to play (all tracks, tempo map honoured)
//   --in <file.wav>         Audio to feed the plugin's inputs (an effect)
//   --program <n>           Factory program to start from
//   --param <name>=<value>  Set a parameter by name (case-insensitive), as
//                           the normalised value 0..1; repeat as needed
//   --rate <hz>             Sample rate (default 48000)
//   --tail <seconds>        How long to keep rendering after the last note
//                           or the end of the input (default 3)
//   --gain <dB>             Gain on the output (default 0)
//
// --list prints the plugin's programs and parameters (name, current value,
// and its text at that value) so you can find what to set; with --param
// before it, it shows what those values mean.

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <iostream>
#include <memory>

namespace
{
    constexpr int blockSize = 512;

    void fail(const juce::String& message)
    {
        std::cerr << "RenderHost: " << message << std::endl;
        std::exit(1);
    }

    void list(juce::AudioPluginInstance& plugin)
    {
        std::cout << plugin.getName() << (plugin.acceptsMidi() ? " (instrument)" : " (effect)")
                  << ", " << plugin.getTotalNumInputChannels() << " in, "
                  << plugin.getTotalNumOutputChannels() << " out" << std::endl;

        std::cout << "Programs:" << std::endl;
        for (int i = 0; i < plugin.getNumPrograms(); ++i)
            std::cout << "  " << i << "\t" << plugin.getProgramName(i) << std::endl;

        std::cout << "Parameters:" << std::endl;
        for (auto* param : plugin.getParameters())
        {
            if (param->getName(64).startsWith("MIDI CC")) // the VST3 wrapper's stand-ins for MIDI controllers
                continue;
            std::cout << "  " << param->getName(64) << "\t" << juce::String(param->getValue(), 3)
                      << "\t" << param->getText(param->getValue(), 32) << " " << param->getLabel() << std::endl;
        }
    }

    juce::AudioProcessorParameter* findParameter(juce::AudioPluginInstance& plugin, const juce::String& name)
    {
        for (auto* param : plugin.getParameters())
            if (param->getName(64).equalsIgnoreCase(name))
                return param;
        return nullptr;
    }

    // Every note event in the file, on one track, with timestamps in seconds
    juce::MidiMessageSequence loadMidi(const juce::File& file)
    {
        juce::FileInputStream stream(file);
        if (! stream.openedOk())
            fail("can't read " + file.getFullPathName());

        juce::MidiFile midi;
        if (! midi.readFrom(stream))
            fail("not a MIDI file: " + file.getFullPathName());

        midi.convertTimestampTicksToSeconds();

        juce::MidiMessageSequence all;
        for (int track = 0; track < midi.getNumTracks(); ++track)
            all.addSequence(*midi.getTrack(track), 0.0);
        all.updateMatchedPairs();
        return all;
    }
}

int main(int argc, char* argv[])
{
    juce::ScopedJuceInitialiser_GUI juceInit;

    if (argc < 2)
        fail("usage: RenderHost <plugin.vst3> [--list | options --out <out.wav>]");

    const juce::String pluginPath(argv[1]);
    juce::File midiFile, inFile, outFile;
    juce::StringArray paramSettings;
    int program = -1;
    double sampleRate = 48000.0;
    double tail = 3.0;
    float gainDb = 0.0f;
    bool listOnly = false;

    for (int arg = 2; arg < argc; ++arg)
    {
        const juce::String option(argv[arg]);
        const auto next = [&] {
            if (arg + 1 >= argc)
                fail(option + " needs a value");
            return juce::String(argv[++arg]);
        };

        if (option == "--list")          listOnly = true;
        else if (option == "--midi")     midiFile = juce::File::getCurrentWorkingDirectory().getChildFile(next());
        else if (option == "--in")       inFile = juce::File::getCurrentWorkingDirectory().getChildFile(next());
        else if (option == "--out")      outFile = juce::File::getCurrentWorkingDirectory().getChildFile(next());
        else if (option == "--program")  program = next().getIntValue();
        else if (option == "--param")    paramSettings.add(next());
        else if (option == "--rate")     sampleRate = next().getDoubleValue();
        else if (option == "--tail")     tail = next().getDoubleValue();
        else if (option == "--gain")     gainDb = next().getFloatValue();
        else fail("unknown option " + option);
    }

    juce::AudioPluginFormatManager formatManager;
    formatManager.addFormat(new juce::VST3PluginFormat());

    juce::OwnedArray<juce::PluginDescription> descriptions;
    formatManager.getFormat(0)->findAllTypesForFile(descriptions, pluginPath);
    if (descriptions.isEmpty())
        fail("no plugin found in " + pluginPath);

    juce::String error;
    auto plugin = formatManager.createPluginInstance(*descriptions[0], sampleRate, blockSize, error);
    if (plugin == nullptr)
        fail("can't load " + pluginPath + ": " + error);

    if (program >= 0)
        plugin->setCurrentProgram(program);

    for (const auto& setting : paramSettings)
    {
        const auto name = setting.upToLastOccurrenceOf("=", false, false).trim();
        const auto value = setting.fromLastOccurrenceOf("=", false, false).getFloatValue();
        auto* param = findParameter(*plugin, name);
        if (param == nullptr)
            fail("no parameter called \"" + name + "\" (try --list)");
        param->setValueNotifyingHost(juce::jlimit(0.0f, 1.0f, value));
    }

    if (listOnly) // after the settings, so --param with --list shows what a value means
    {
        list(*plugin);
        return 0;
    }

    if (outFile == juce::File())
        fail("--out is required");

    juce::MidiMessageSequence notes;
    double length = 0.0;
    if (midiFile != juce::File())
    {
        notes = loadMidi(midiFile);
        length = notes.getEndTime();
    }

    juce::AudioFormatManager audioFormats;
    audioFormats.registerBasicFormats();
    std::unique_ptr<juce::AudioFormatReader> input;
    if (inFile != juce::File())
    {
        input.reset(audioFormats.createReaderFor(inFile));
        if (input == nullptr)
            fail("can't read audio from " + inFile.getFullPathName());
        if (std::abs(input->sampleRate - sampleRate) > 1.0)
            fail("the input is at " + juce::String(input->sampleRate) + " Hz; render at that rate with --rate");
        length = juce::jmax(length, (double) input->lengthInSamples / sampleRate);
    }

    if (length <= 0.0)
        fail("nothing to render: give --midi or --in");

    plugin->setPlayConfigDetails(plugin->getTotalNumInputChannels(), plugin->getTotalNumOutputChannels(), sampleRate, blockSize);
    plugin->setNonRealtime(true);
    plugin->prepareToPlay(sampleRate, blockSize);

    const int outChannels = juce::jmax(1, juce::jmin(2, plugin->getTotalNumOutputChannels()));
    const int channels = juce::jmax(plugin->getTotalNumInputChannels(), plugin->getTotalNumOutputChannels(), 2);
    const auto totalSamples = (juce::int64) std::ceil((length + tail) * sampleRate);

    outFile.deleteFile();
    outFile.getParentDirectory().createDirectory();
    juce::WavAudioFormat wav;
    std::unique_ptr<juce::AudioFormatWriter> writer(wav.createWriterFor(
        new juce::FileOutputStream(outFile), sampleRate, (unsigned int) outChannels, 24, {}, 0));
    if (writer == nullptr)
        fail("can't write " + outFile.getFullPathName());

    juce::AudioBuffer<float> buffer(channels, blockSize);
    const float gain = juce::Decibels::decibelsToGain(gainDb);
    int nextNote = 0;
    float peak = 0.0f;

    for (juce::int64 start = 0; start < totalSamples; start += blockSize)
    {
        const int count = (int) juce::jmin((juce::int64) blockSize, totalSamples - start);
        buffer.clear();

        if (input != nullptr)
        {
            juce::AudioBuffer<float> in(2, count);
            input->read(&in, 0, count, start, true, true);
            for (int c = 0; c < juce::jmin(2, plugin->getTotalNumInputChannels()); ++c)
                buffer.copyFrom(c, 0, in, c, 0, count);
        }

        juce::MidiBuffer midi;
        const double blockEnd = (double) (start + count) / sampleRate;
        while (nextNote < notes.getNumEvents())
        {
            const auto& message = notes.getEventPointer(nextNote)->message;
            if (message.getTimeStamp() >= blockEnd)
                break;
            const int offset = juce::jlimit(0, count - 1, (int) std::round(message.getTimeStamp() * sampleRate) - (int) start);
            if (! message.isMetaEvent())
                midi.addEvent(message, offset);
            ++nextNote;
        }

        juce::AudioBuffer<float> block(buffer.getArrayOfWritePointers(), channels, count);
        plugin->processBlock(block, midi);
        block.applyGain(gain);

        for (int c = 0; c < outChannels; ++c)
            peak = juce::jmax(peak, block.getMagnitude(c, 0, count));

        writer->writeFromAudioSampleBuffer(block, 0, count);
    }

    plugin->releaseResources();
    std::cout << outFile.getFileName() << "\t" << juce::String(totalSamples / sampleRate, 2) << " s\tpeak "
              << (peak > 0.0f ? juce::String(juce::Decibels::gainToDecibels(peak), 1) + " dBFS" : juce::String("silent"))
              << std::endl;
    return 0;
}
