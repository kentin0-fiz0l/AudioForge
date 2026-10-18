/**
 * Sampler SFZ tests
 *
 * The sampler reads SFZ instruments (the subset that VSCO 2 uses) and plays
 * them: each note finds its regions by key and velocity, takes its turn in
 * a round robin or a keyswitched articulation, and is pitched from the
 * region's key centre. Saved sample paths come back with the state.
 *
 * The fixtures are tiny sine WAVs written to a temporary folder.
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <cmath>
#include <cstring>

#include "../../plugins/SamplerPlugin/Source/PluginProcessor.h"
#include "../../plugins/SamplerPlugin/Source/SfzParser.h"
#include "../../plugins/SamplerPlugin/Source/SfzSampleMap.h"
#include "../../shared/utils/AudioFileSafety.h"

class SamplerSfzTests : public juce::UnitTest
{
public:
    SamplerSfzTests()
        : juce::UnitTest("Sampler SFZ", "Plugins")
    {
    }

    void runTest() override
    {
        folder = juce::File::getSpecialLocation(juce::File::tempDirectory)
                     .getNonexistentChildFile("SamplerSfzTests", "");
        folder.createDirectory();

        testNoteNames();
        testOpcodesInherit();
        testSampleNamesAndPaths();
        testRegionsAreChosen();
        testRoundRobinAndKeyswitches();
        testPlaybackSettings();
        testProcessorPlaysAnSfz();
        testControllers();
        testSamplePathsRestore();
        testUntrustedFiles();

        folder.deleteRecursively();
    }

private:
    static constexpr double sampleRate = 48000.0;
    static constexpr int blockSize = 512;
    juce::File folder;

    // A sine at the given frequency, a second long unless told otherwise
    juce::File writeSine(const juce::String& name, double frequency, double rate = sampleRate,
                         int channels = 1, double seconds = 1.0, std::unordered_map<juce::String, juce::String> metadata = {})
    {
        auto file = folder.getChildFile(name);
        file.getParentDirectory().createDirectory();
        file.deleteFile();

        const int length = static_cast<int>(seconds * rate);
        juce::AudioBuffer<float> buffer(channels, length);
        for (int i = 0; i < length; ++i)
            for (int c = 0; c < channels; ++c)
                buffer.setSample(c, i, 0.5f * static_cast<float>(std::sin(juce::MathConstants<double>::twoPi * frequency * i / rate)));

        std::unique_ptr<juce::OutputStream> stream = std::make_unique<juce::FileOutputStream>(file);
        juce::WavAudioFormat wav;
        auto writer = wav.createWriterFor(stream, juce::AudioFormatWriterOptions()
                                                      .withSampleRate(rate)
                                                      .withNumChannels(channels)
                                                      .withBitsPerSample(16)
                                                      .withMetadataValues(metadata));
        writer->writeFromAudioSampleBuffer(buffer, 0, length);
        return file;
    }

    // Rewrites a WAV's header: its sample rate, or its channel count
    static void patchWavHeader(const juce::File& file, juce::uint32 sampleRateValue, juce::uint16 channels = 0)
    {
        juce::MemoryBlock data;
        file.loadFileAsData(data);
        auto* bytes = static_cast<char*>(data.getData());
        for (size_t i = 12; i + 16 < data.getSize(); ++i)
        {
            if (std::memcmp(bytes + i, "fmt ", 4) == 0)
            {
                if (channels != 0)
                    std::memcpy(bytes + i + 10, &channels, 2);
                std::memcpy(bytes + i + 12, &sampleRateValue, 4);
                break;
            }
        }
        file.replaceWithData(data.getData(), data.getSize());
    }

    juce::File writeSfz(const juce::String& name, const juce::String& text)
    {
        auto file = folder.getChildFile(name);
        file.replaceWithText(text);
        return file;
    }

    // Frequency from zero crossings, over a stretch of one channel
    static double frequencyOf(const juce::AudioBuffer<float>& buffer, int channel, int start, int length)
    {
        int crossings = 0;
        for (int i = start + 1; i < start + length; ++i)
            if (buffer.getSample(channel, i - 1) < 0.0f && buffer.getSample(channel, i) >= 0.0f)
                ++crossings;
        return crossings * sampleRate / length;
    }

    // Renders a MIDI sequence (times in samples) through the processor
    static juce::AudioBuffer<float> render(SamplerPluginProcessor& processor,
                                           const std::vector<std::pair<int, juce::MidiMessage>>& events,
                                           int length)
    {
        processor.setRateAndBufferSizeDetails(sampleRate, blockSize);
        processor.prepareToPlay(sampleRate, blockSize);

        juce::AudioBuffer<float> output(2, length);
        juce::AudioBuffer<float> block(2, blockSize);
        for (int start = 0; start < length; start += blockSize)
        {
            const int count = juce::jmin(blockSize, length - start);
            juce::MidiBuffer midi;
            for (const auto& [time, message] : events)
                if (time >= start && time < start + count)
                    midi.addEvent(message, time - start);

            block.setSize(2, count, false, false, true);
            processor.processBlock(block, midi);
            for (int c = 0; c < 2; ++c)
                output.copyFrom(c, start, block, c, 0, count);
        }
        return output;
    }

    static void setState(SamplerPluginProcessor& processor, const juce::XmlElement& xml)
    {
        juce::MemoryBlock state;
        juce::AudioProcessor::copyXmlToBinary(xml, state);
        processor.setStateInformation(state.getData(), static_cast<int>(state.getSize()));
    }

    //==============================================================================
    void testNoteNames()
    {
        beginTest("Notes are read as numbers or names, with c4 as middle C");

        expectEquals(Sfz::parseNote("60"), 60);
        expectEquals(Sfz::parseNote("c4"), 60);
        expectEquals(Sfz::parseNote("C4"), 60);
        expectEquals(Sfz::parseNote("f#3"), 54);
        expectEquals(Sfz::parseNote("eb2"), 39);
        expectEquals(Sfz::parseNote("a4"), 69);
        expectEquals(Sfz::parseNote("c-1"), 0);
        expectEquals(Sfz::parseNote("d#6"), 87);
        expectEquals(Sfz::parseNote("-1"), -1);
        expect(Sfz::parseNote("h2") < -1, "not a note");
        expect(Sfz::parseNote("") < -1, "nothing");
    }

    void testOpcodesInherit()
    {
        beginTest("Regions take their headers' opcodes and can override them");

        const auto instrument = Sfz::parse(R"(
            // A comment, and /* a block
               comment */ that's skipped
            <global> ampeg_release=0.8 volume=-3
            <group> lovel=1 hivel=64 ampeg_attack=0.01
            <region> sample=soft.wav key=60
            <region> sample=soft2.wav lokey=61 hikey=62 pitch_keycenter=61 volume=2
            <group> lovel=65 hivel=127
            <region> sample=loud.wav key=c4 trigger=release
            <region> sample=other.wav key=c4 xfin_lovel=10
        )");

        expectEquals((int) instrument.regions.size(), 4);
        const auto& soft = instrument.regions[0];
        expectEquals(soft.sample, juce::String("soft.wav"));
        expectEquals(soft.lokey, 60);
        expectEquals(soft.hikey, 60);
        expectEquals(soft.keycenter, 60);
        expectEquals(soft.hivel, 64);
        expectWithinAbsoluteError(soft.ampegRelease, 0.8f, 1.0e-6f);
        expectWithinAbsoluteError(soft.ampegAttack, 0.01f, 1.0e-6f);
        expectWithinAbsoluteError(soft.volume, -3.0f, 1.0e-6f);

        expectWithinAbsoluteError(instrument.regions[1].volume, 2.0f, 1.0e-6f, "the region's own volume wins");
        expectEquals(instrument.regions[1].hikey, 62);

        const auto& loud = instrument.regions[2];
        expectEquals(loud.lovel, 65, "a new group starts afresh");
        expectWithinAbsoluteError(loud.ampegAttack, -1.0f, 1.0e-6f, "not the last group's attack");
        expectWithinAbsoluteError(loud.ampegRelease, 0.8f, 1.0e-6f, "but still the global release");
        expect(loud.releaseTrigger);

        expect(instrument.warnings.joinIntoString(" ").contains("xfin_lovel"), "an unknown opcode is noted");
    }

    void testSampleNamesAndPaths()
    {
        beginTest("Sample names keep their spaces, default_path applies, and backslashes work");

        const auto instrument = Sfz::parse(
            "<control>\ndefault_path=Strings\\Violin Section\\susVib\\\n"
            "<region>\nsample=VlnEns_susVib A2_v1.wav\nlokey=56 hikey=57\n"
            "<region> sample=..\\Other\\b 2.wav lovel=10\n"
            "<control>\ndefault_path=Brass/\n"
            "<region> sample=horn.flac\n");

        expectEquals((int) instrument.regions.size(), 3);
        expectEquals(instrument.regions[0].sample, juce::String("Strings/Violin Section/susVib/VlnEns_susVib A2_v1.wav"));
        expectEquals(instrument.regions[0].lokey, 56);
        expectEquals(instrument.regions[1].sample, juce::String("Strings/Violin Section/susVib/../Other/b 2.wav"));
        expectEquals(instrument.regions[1].lovel, 10);
        expectEquals(instrument.regions[2].sample, juce::String("Brass/horn.flac"), "a second <control> changes the path");

        const auto sfz = juce::File("/libs/vsco/Violin.sfz");
        expectEquals(Sfz::resolveSample(sfz, "Strings/a b.wav").getFullPathName(), juce::String("/libs/vsco/Strings/a b.wav"));
        expectEquals(Sfz::resolveSample(sfz, "..\\x.wav").getFullPathName(), juce::String("/libs/x.wav"));
        expectEquals(Sfz::resolveSample(sfz, "/abs/x.wav").getFullPathName(), juce::String("/abs/x.wav"));
    }

    void testRegionsAreChosen()
    {
        beginTest("A note plays the regions that cover its key and velocity");

        writeSine("pick/low.wav", 220.0);
        writeSine("pick/high soft.wav", 440.0);
        writeSine("pick/high loud.wav", 440.0);
        const auto sfz = writeSfz("pick.sfz",
            "<control> default_path=pick/\n"
            "<region> sample=low.wav lokey=40 hikey=59 pitch_keycenter=a3\n"
            "<region> sample=high soft.wav lokey=60 hikey=80 pitch_keycenter=a4 hivel=64\n"
            "<region> sample=high loud.wav lokey=60 hikey=80 pitch_keycenter=a4 lovel=65\n"
            "<region> sample=missing.wav key=90\n"
            "<region> sample=low.wav key=91 trigger=release\n");

        SfzSampleMap map;
        juce::String error;
        expect(map.load(sfz, error), error);
        expectEquals((int) map.getRegions().size(), 3, "the missing sample and the release region are left out");
        expect(map.getWarnings().joinIntoString(" ").contains("missing.wav"), "the missing sample is noted");

        expectEquals((int) map.noteOn(50, 100, 0.0f, 0.0f).size(), 1);
        expectEquals((int) map.noteOn(30, 100, 0.0f, 0.0f).size(), 0, "nothing covers this key");

        const auto soft = map.noteOn(69, 30, 0.0f, 0.0f);
        const auto loud = map.noteOn(69, 100, 0.0f, 0.0f);
        expectEquals((int) soft.size(), 1);
        expectEquals((int) loud.size(), 1);
        expect(soft[0].sample->getSourceFile().getFileName() == "high soft.wav");
        expect(loud[0].sample->getSourceFile().getFileName() == "high loud.wav");

        beginTest("Spaces in folder and sample names load, as VSCO 2 writes them");
        writeSine("Strings/Cello Section/spic/Cello Section spic C2 v1.wav", 65.41);
        const auto vsco = writeSfz("Cello Spic.sfz",
            "<control>\n\n\ndefault_path=Strings\\Cello Section\\spic\\\n\n\n"
            "<global>\n\nampeg_attack=0.001\n\nampeg_release=0.3\n\nampeg_dynamic=1\n\n\n"
            "<group> //Begin Group for entire instrument\n\n\n"
            "<region>\nsample=Cello Section spic C2 v1.wav\nlokey=36\nhikey=37\npitch_keycenter=36\nlovel=0\nhivel=127\nvolume=20\n\n");
        SfzSampleMap cello;
        expect(cello.load(vsco, error), error);
        expectEquals((int) cello.getRegions().size(), 1);
        expect(cello.getWarnings().isEmpty(), cello.getWarnings().joinIntoString("; "));
        expectEquals((int) cello.noteOn(37, 100, 0.0f, 0.0f).size(), 1);

        beginTest("A missing file is an error");
        SfzSampleMap none;
        expect(! none.load(folder.getChildFile("nope.sfz"), error));
    }

    void testRoundRobinAndKeyswitches()
    {
        beginTest("Round robin regions take turns");

        writeSine("rr/a.wav", 440.0);
        writeSine("rr/b.wav", 440.0);
        const auto rr = writeSfz("rr.sfz",
            "<control> default_path=rr/\n"
            "<group> seq_length=2 seq_position=1\n<region> sample=a.wav key=60\n"
            "<group> seq_length=2 seq_position=2\n<region> sample=b.wav key=60\n");

        SfzSampleMap map;
        juce::String error;
        expect(map.load(rr, error), error);
        juce::StringArray played;
        for (int i = 0; i < 4; ++i)
        {
            const auto playbacks = map.noteOn(60, 100, 0.0f, 0.0f);
            expectEquals((int) playbacks.size(), 1);
            played.add(playbacks[0].sample->getSourceFile().getFileName());
        }
        expectEquals(played.joinIntoString(","), juce::String("a.wav,b.wav,a.wav,b.wav"));

        beginTest("Random round robin follows lorand and hirand");
        const auto random = writeSfz("rand.sfz",
            "<control> default_path=rr/\n"
            "<group> lorand=0.0 hirand=0.5\n<region> sample=a.wav key=60\n"
            "<group> lorand=0.5 hirand=1.0\n<region> sample=b.wav key=60\n");
        expect(map.load(random, error), error);
        expect(map.noteOn(60, 100, 0.2f, 0.0f)[0].sample->getSourceFile().getFileName() == "a.wav");
        expect(map.noteOn(60, 100, 0.7f, 0.0f)[0].sample->getSourceFile().getFileName() == "b.wav");

        beginTest("Keyswitches pick the articulation and play nothing themselves");
        const auto ks = writeSfz("ks.sfz",
            "<control> default_path=rr/\n"
            "<group> sw_default=c6 sw_lokey=c6 sw_hikey=d#6 sw_last=c6\n<region> sample=a.wav lokey=40 hikey=80\n"
            "<group> sw_default=c6 sw_lokey=c6 sw_hikey=d#6 sw_last=c#6\n<region> sample=b.wav lokey=40 hikey=80\n");
        expect(map.load(ks, error), error);
        expectEquals(map.getCurrentKeyswitch(), 84);
        expect(map.noteOn(60, 100, 0.0f, 0.0f)[0].sample->getSourceFile().getFileName() == "a.wav", "the default articulation");
        expectEquals((int) map.noteOn(85, 100, 0.0f, 0.0f).size(), 0, "the keyswitch itself is silent");
        const auto switched = map.noteOn(60, 100, 0.0f, 0.0f);
        expectEquals((int) switched.size(), 1);
        expect(switched[0].sample->getSourceFile().getFileName() == "b.wav", "the switched articulation");
    }

    void testPlaybackSettings()
    {
        beginTest("A region's pitch, level, pan, envelope and loop reach the voice");

        writeSine("set/tone.wav", 440.0);
        writeSine("set/looped.wav", 440.0, sampleRate, 1, 1.0,
                  { { "NumSampleLoops", "1" }, { "Loop0Start", "1000" }, { "Loop0End", "20000" },
                    { "Loop0Type", "0" }, { "Loop0Identifier", "0" }, { "Loop0Fraction", "0" }, { "Loop0PlayCount", "0" } });
        const auto sfz = writeSfz("set.sfz",
            "<control> default_path=set/\n"
            "<region> sample=tone.wav key=60 tune=50 transpose=-12 volume=-6 pan=100 amp_veltrack=0 ampeg_release=1.5 offset=100 end=4000\n"
            "<region> sample=looped.wav key=61 pitch_keycenter=59 ampeg_sustain=50\n"
            "<region> sample=tone.wav key=62 loop_mode=loop_continuous loop_start=10 loop_end=500\n"
            "<region> sample=tone.wav key=63 loop_mode=one_shot\n");

        SfzSampleMap map;
        juce::String error;
        expect(map.load(sfz, error), error);

        const auto a = map.noteOn(60, 64, 0.0f, 0.0f)[0];
        expectWithinAbsoluteError(a.pitchRatio, std::pow(2.0, -11.5 / 12.0), 1.0e-9, "transpose and tune in cents");
        expectWithinAbsoluteError(a.gainLeft, 0.0f, 1.0e-6f, "panned hard right");
        expectWithinAbsoluteError(a.gainRight, juce::Decibels::decibelsToGain(-6.0f), 1.0e-4f, "volume, and no velocity tracking");
        expectWithinAbsoluteError(a.release, 1.5f, 1.0e-6f);
        expectWithinAbsoluteError(a.attack, -1.0f, 1.0e-6f, "attack is left to the plugin");
        expectWithinAbsoluteError(a.sustain, 1.0f, 1.0e-6f, "SFZ sustains at full level");
        expectEquals(a.startFrame, 100);
        expectEquals(a.endFrame, 4000);
        expect(a.loopMode == Sample::LoopMode::None);

        const auto b = map.noteOn(61, 127, 0.0f, 0.0f)[0];
        expectWithinAbsoluteError(b.pitchRatio, std::pow(2.0, 2.0 / 12.0), 1.0e-9, "two keys above the key centre");
        expectWithinAbsoluteError(b.gainLeft, 1.0f, 1.0e-6f, "full velocity, full level");
        expectWithinAbsoluteError(b.sustain, 0.5f, 1.0e-6f);
        expect(b.loopMode == Sample::LoopMode::Forward, "the file's loop is used");
        expectEquals(b.loopStart, 1000);
        expectEquals(b.loopEnd, 20000);

        const auto softer = map.noteOn(61, 64, 0.0f, 0.0f)[0];
        expectWithinAbsoluteError(softer.gainLeft, (64.0f / 127.0f) * (64.0f / 127.0f), 1.0e-5f, "velocity squared");

        const auto c = map.noteOn(62, 100, 0.0f, 0.0f)[0];
        expect(c.loopMode == Sample::LoopMode::Forward);
        expectEquals(c.loopStart, 10);
        expectEquals(c.loopEnd, 500);

        expect(map.noteOn(63, 100, 0.0f, 0.0f)[0].oneShot);
        expectWithinAbsoluteError(map.noteOn(60, 100, 0.0f, 2.0f)[0].pitchRatio, std::pow(2.0, -9.5 / 12.0), 1.0e-9,
                                  "Master Tune adds on");
    }

    void testProcessorPlaysAnSfz()
    {
        beginTest("A state with sfz= loads the instrument and notes play it, pitched and in stereo");

        // Recorded at 44.1 kHz, played at 48: the voice allows for the rate
        writeSine("play/a4.wav", 440.0, 44100.0, 2, 2.0);
        const auto sfz = writeSfz("play.sfz", "<region> sample=play/a4.wav lokey=c4 hikey=c6 pitch_keycenter=a4 ampeg_release=0.05\n");

        SamplerPluginProcessor processor;
        juce::XmlElement state("Parameters");
        state.setAttribute("sfz", sfz.getFullPathName());
        setState(processor, state);
        expect(processor.getSfz() != nullptr, "the instrument loaded");

        const int second = static_cast<int>(sampleRate);
        auto output = render(processor, { { 0, juce::MidiMessage::noteOn(1, 69, (juce::uint8) 100) },
                                          { second / 2, juce::MidiMessage::noteOff(1, 69) },
                                          { second, juce::MidiMessage::noteOn(1, 81, (juce::uint8) 100) },
                                          { second * 3 / 2, juce::MidiMessage::noteOff(1, 81) } },
                             2 * second);

        expectWithinAbsoluteError(frequencyOf(output, 0, 2400, second / 3), 440.0, 6.0, "the key centre plays as recorded");
        expectWithinAbsoluteError(frequencyOf(output, 1, second + 2400, second / 3), 880.0, 9.0, "an octave up");
        expectGreaterThan(output.getMagnitude(1, 0, second / 2), 0.1f, "the right channel has it too");
        expectLessThan(output.getMagnitude(0, second / 2 + 4800, 2400), 1.0e-4f, "the release has finished");

        beginTest("A note lands where it falls in the block");
        auto timed = render(processor, { { 300, juce::MidiMessage::noteOn(1, 69, (juce::uint8) 100) } }, blockSize * 2);
        expectEquals(timed.getMagnitude(0, 0, 300), 0.0f, "silence before the note");
        expectGreaterThan(timed.getMagnitude(0, 300, 200), 0.0f);

        beginTest("The state keeps the instrument");
        juce::MemoryBlock saved;
        processor.getStateInformation(saved);
        auto xml = juce::AudioProcessor::getXmlFromBinary(saved.getData(), static_cast<int>(saved.getSize()));
        expect(xml != nullptr && xml->getStringAttribute("sfz") == sfz.getFullPathName());

        SamplerPluginProcessor restored;
        restored.setStateInformation(saved.getData(), static_cast<int>(saved.getSize()));
        expect(restored.getSfz() != nullptr && restored.getSfz()->getFile() == sfz);

        beginTest("A state without sfz= leaves no instrument");
        setState(restored, juce::XmlElement("Parameters"));
        expect(restored.getSfz() == nullptr);
    }

    void testControllers()
    {
        beginTest("Expression (CC11) scales the level, and the sustain pedal holds notes");

        writeSine("cc/a4.wav", 440.0, sampleRate, 1, 2.0);
        const auto sfz = writeSfz("cc.sfz", "<region> sample=cc/a4.wav key=69 ampeg_release=0.01\n");

        SamplerPluginProcessor processor;
        juce::String error;
        expect(processor.loadSfz(sfz, error), error);

        const int quarter = static_cast<int>(sampleRate / 4);
        auto output = render(processor, { { 0, juce::MidiMessage::controllerEvent(1, 11, 0) },
                                          { 0, juce::MidiMessage::noteOn(1, 69, (juce::uint8) 127) },
                                          { quarter, juce::MidiMessage::controllerEvent(1, 11, 127) },
                                          { 2 * quarter, juce::MidiMessage::controllerEvent(1, 64, 127) },
                                          { 2 * quarter + 100, juce::MidiMessage::noteOff(1, 69) },
                                          { 4 * quarter, juce::MidiMessage::controllerEvent(1, 64, 0) } },
                             5 * quarter);

        expectLessThan(output.getMagnitude(0, 2400, quarter - 2400), 1.0e-4f, "expression at zero is silent");
        expectGreaterThan(output.getMagnitude(0, quarter + 2400, quarter - 2400), 0.2f, "expression up, full level");
        expectGreaterThan(output.getMagnitude(0, 3 * quarter, quarter - 100), 0.2f, "the pedal holds the note past its note-off");
        expectLessThan(output.getMagnitude(0, 4 * quarter + 2400, quarter - 2400), 1.0e-4f, "pedal up lets it go");
    }

    void testSamplePathsRestore()
    {
        beginTest("Samples loaded by hand come back with the state, and their zones still play them");

        const auto file = writeSine("zones/c4.wav", 261.63);

        SamplerPluginProcessor processor;
        expect(processor.loadSample(file));
        KeyZone zone(48, 72, 60);
        zone.velocityLayers.emplace_back(0, 0.0f, 1.0f);
        processor.getKeyZoneMap().addZone(zone);

        juce::MemoryBlock saved;
        processor.getStateInformation(saved);

        SamplerPluginProcessor restored;
        restored.setStateInformation(saved.getData(), static_cast<int>(saved.getSize()));
        expectEquals(restored.getSampleLibrary().getNumSamples(), 1);
        expect(restored.getSampleLibrary().getSample(0)->getSourceFile() == file);
        expectEquals(restored.getKeyZoneMap().getNumZones(), 1);

        const int third = static_cast<int>(sampleRate / 3);
        auto output = render(restored, { { 0, juce::MidiMessage::noteOn(1, 72, (juce::uint8) 127) } }, third);
        expectWithinAbsoluteError(frequencyOf(output, 0, 2400, third - 2400), 523.25, 8.0, "pitched from the zone's root note");

        beginTest("A sample file that's gone is left out, and its zones go quiet");
        file.deleteFile();
        SamplerPluginProcessor missing;
        missing.setStateInformation(saved.getData(), static_cast<int>(saved.getSize()));
        expectEquals(missing.getSampleLibrary().getNumSamples(), 0);
        expectEquals(missing.getKeyZoneMap().getZones()[0].velocityLayers[0].sampleIndex, -1);
    }

    void testUntrustedFiles()
    {
        beginTest("Network paths are refused: in sfz=, in sample= and default_path, and in restored samples");

        expect(AudioForge::FileSafety::isNetworkPath("\\\\host\\share\\x.sfz"));
        expect(AudioForge::FileSafety::isNetworkPath("//host/share/x.sfz"));
        expect(AudioForge::FileSafety::isNetworkPath("smb://host/share/x.sfz"));
        expect(! AudioForge::FileSafety::isNetworkPath("/Users/me/vsco/x.sfz"));
        expect(! AudioForge::FileSafety::isNetworkPath("../Samples/x.wav"));
        expect(! AudioForge::FileSafety::isNetworkPath("C:\\Samples\\x.wav"));

        SamplerPluginProcessor processor;
        for (const auto* path : { "//host/share/x.sfz", "\\\\host\\share\\x.sfz" })
        {
            juce::XmlElement state("Parameters");
            state.setAttribute("sfz", path);
            setState(processor, state);
            expect(processor.getSfz() == nullptr, path);
        }

        writeSine("net/local.wav", 440.0);
        const auto sfz = writeSfz("net.sfz",
            "<region> sample=\\\\host\\share\\a.wav key=60\n"
            "<region> sample=//host/share/b.wav key=61\n"
            "<control> default_path=\\\\host\\share\\\n<region> sample=c.wav key=62\n"
            "<control> default_path=net/\n<region> sample=local.wav key=63\n");
        SfzSampleMap map;
        juce::String error;
        expect(map.load(sfz, error), error);
        expectEquals((int) map.getRegions().size(), 1, "only the local sample loads");
        expect(map.getWarnings().joinIntoString(" ").contains("network"), "and the others are noted");

        SampleLibrary library;
        expect(library.loadSample(juce::File("//host/share/a.wav")) == nullptr);
        expect(library.getLastError().contains("Network"));

        juce::XmlElement saved("Parameters");
        auto* samples = saved.createNewChildElement("SamplerState")->createNewChildElement("Samples");
        for (const auto* path : { "//host/share/a.wav", "\\\\host\\share\\b.wav", "relative.wav" })
        {
            auto* sample = samples->createNewChildElement("Sample");
            sample->setAttribute("index", samples->getNumChildElements() - 1);
            sample->setAttribute("path", path);
        }
        SamplerPluginProcessor restored;
        setState(restored, saved);
        expectEquals(restored.getSampleLibrary().getNumSamples(), 0);

        beginTest("A sample whose header gives an unlikely rate or channel count isn't loaded");
        for (const juce::uint32 rate : { 1000000000u, 4000u, 0u })
        {
            const auto file = writeSine("bad/rate.wav", 440.0);
            patchWavHeader(file, rate);
            expect(library.loadSample(file) == nullptr, juce::String(rate) + " Hz");
        }
        const auto manyChannels = writeSine("bad/channels.wav", 440.0);
        patchWavHeader(manyChannels, 48000u, 12);
        expect(library.loadSample(manyChannels) == nullptr, "12 channels");
        const auto good = writeSine("bad/ok.wav", 440.0);
        expect(library.loadSample(good) != nullptr, "an ordinary file still loads");

        beginTest("A sample too big for what's left of the memory budget is refused before it's read");
        SampleLibrary small(100000);  // 100 kB: a second of 48 kHz stereo floats is 384 kB
        expect(small.loadSample(good) == nullptr);
        expect(small.getLastError().contains("budget"));
        expectEquals((int) small.getTotalMemoryUsed(), 0);
        const auto shortFile = writeSine("bad/short.wav", 440.0, sampleRate, 1, 0.1);  // 38 kB
        expect(small.loadSample(shortFile) != nullptr);
        expect(small.loadSample(shortFile) != nullptr);
        expect(small.loadSample(shortFile) == nullptr, "the third doesn't fit");

        beginTest("An SFZ is read only if it's a regular file under the size cap");
        juce::String text, why;
        const auto smallSfz = folder.getChildFile("bad/small.sfz");
        smallSfz.replaceWithText("<region> sample=short.wav\n");
        expect(AudioForge::FileSafety::readText(smallSfz, text, why) && text.contains("short.wav"));
        expect(! AudioForge::FileSafety::readText(smallSfz, text, why, 8), "longer than the cap is refused");
        expect(why.contains("larger"), why);
        expect(! AudioForge::FileSafety::readText(folder, text, why), "a folder is refused");
       #if JUCE_MAC || JUCE_LINUX
        expect(! AudioForge::FileSafety::readText(juce::File("/dev/zero"), text, why), "a device is refused");
        SfzSampleMap endless;
        expect(! endless.load(juce::File("/dev/zero"), error) && error.contains("regular"), error);
       #endif
    }
};

static SamplerSfzTests samplerSfzTests;
