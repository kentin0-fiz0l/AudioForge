/**
 * MelodyMaker Plugin Tests
 *
 * MelodyMaker plays a generated melody in time with the host's transport.
 * These tests cover what a user does with it:
 * - Picking a style changes the kind of melody it generates
 * - Its settings and its melody come back when a session is reopened, and
 *   opening the window does not replace the melody with a new one
 * - Turning the filter cutoff while it plays changes the sound
 * - Exporting a melody over an existing MIDI file replaces that file
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>

#include "../../plugins/MelodyMaker/Source/PluginProcessor.h"

namespace
{
    constexpr double sampleRate = 48000.0;
    constexpr int blockSize = 512;

    class FakePlayHead : public juce::AudioPlayHead
    {
    public:
        juce::Optional<PositionInfo> getPosition() const override
        {
            PositionInfo info;
            info.setIsPlaying(true);
            info.setPpqPosition(ppq);
            info.setBpm(bpm);
            return info;
        }

        double ppq = 0.0;
        double bpm = 120.0;
    };

    // Plays the processor from the top of the melody for the given number of
    // beats, as a host would with its transport running, and returns the left
    // channel of what it played
    std::vector<float> play(MelodyMakerProcessor& processor, double beats)
    {
        FakePlayHead playHead;
        processor.setPlayHead(&playHead);

        const double beatsPerBlock = blockSize * playHead.bpm / (60.0 * sampleRate);
        juce::AudioBuffer<float> audio(2, blockSize);
        juce::MidiBuffer midi;
        std::vector<float> output;

        for (; playHead.ppq < beats; playHead.ppq += beatsPerBlock)
        {
            processor.processBlock(audio, midi);
            output.insert(output.end(), audio.getReadPointer(0), audio.getReadPointer(0) + blockSize);
        }

        processor.setPlayHead(nullptr);
        return output;
    }

    float rms(const std::vector<float>& samples)
    {
        double sum = 0.0;
        for (auto s : samples)
            sum += static_cast<double>(s) * s;
        return samples.empty() ? 0.0f : static_cast<float>(std::sqrt(sum / static_cast<double>(samples.size())));
    }

    juce::RangedAudioParameter* findParameter(juce::AudioProcessor& processor, const juce::String& id)
    {
        for (auto* param : processor.getParameters())
            if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*>(param))
                if (ranged->paramID == id)
                    return ranged;
        return nullptr;
    }

    bool sameMelody(const std::vector<MelodyEngine::Note>& a, const std::vector<MelodyEngine::Note>& b)
    {
        return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](const auto& x, const auto& y) {
            return x.pitch == y.pitch && x.startTime == y.startTime
                && x.duration == y.duration && x.velocity == y.velocity;
        });
    }
}

class MelodyMakerPluginTests : public juce::UnitTest
{
public:
    MelodyMakerPluginTests()
        : juce::UnitTest("MelodyMaker Plugin", "Plugins")
    {
    }

    void runTest() override
    {
        testStyleChangesMelody();
        testSettingsAndMelodyAreRestored();
        testOpeningEditorKeepsMelody();
        testFilterCutoffFollowsParameter();
        testExportReplacesExistingFile();
    }

private:
    void testStyleChangesMelody()
    {
        beginTest("Each style generates its own kind of melody");

        MelodyMakerProcessor processor;

        processor.generateNewMelody("Pop");
        expectEquals((int) processor.getCurrentMelody().size(), 16, "Pop plays a note on every beat of 4 bars");

        processor.generateNewMelody("Ballad");
        const auto& ballad = processor.getCurrentMelody();
        expectEquals((int) ballad.size(), 8, "A ballad plays half as many notes");
        for (const auto& note : ballad)
            expectEquals(note.duration, 2.0f, "A ballad's notes last two beats");

        processor.generateNewMelody("EDM");
        expectEquals((int) processor.getCurrentMelody().size(), 32, "EDM plays a note every half beat");

        processor.generateNewMelody("Jazz");
        expectEquals((int) processor.getCurrentMelody().size(), 24, "Jazz plays in triplets");
    }

    void testSettingsAndMelodyAreRestored()
    {
        beginTest("Settings and melody come back when a session is reopened");

        MelodyMakerProcessor original;
        for (const auto* id : { "waveform", "attack", "decay", "sustain", "release", "reverbMix",
                                "delayTime", "delayFeedback", "delayMix", "filterCutoff", "filterResonance" })
            expect(findParameter(original, id) != nullptr, juce::String("No parameter ") + id);

        for (auto* param : original.getParameters())
            param->setValueNotifyingHost(param->getDefaultValue() < 0.5f ? 0.75f : 0.25f);
        original.setKey(62, false);
        original.generateNewMelody("Jazz");

        juce::MemoryBlock state;
        original.getStateInformation(state);

        MelodyMakerProcessor restored;
        restored.setStateInformation(state.getData(), static_cast<int>(state.getSize()));

        expectEquals(restored.getParameters().size(), original.getParameters().size());
        for (int i = 0; i < original.getParameters().size(); ++i)
            expectWithinAbsoluteError(restored.getParameters()[i]->getValue(), original.getParameters()[i]->getValue(), 0.001f,
                                      original.getParameters()[i]->getName(64) + " should be restored");

        expectEquals(restored.getKeyRoot(), 62, "The key should be restored");
        expect(! restored.getKeyIsMajor(), "The scale should be restored");
        expectEquals(juce::String(restored.getStyle()), juce::String("Jazz"), "The style should be restored");
        expect(sameMelody(restored.getCurrentMelody(), original.getCurrentMelody()), "The melody should be restored");

        original.prepareToPlay(sampleRate, blockSize);
        restored.prepareToPlay(sampleRate, blockSize);
        const auto before = play(original, 4.0);
        const auto after = play(restored, 4.0);
        expect(rms(before) > 0.001f, "The melody should be audible");
        expect(before == after, "The reopened session should sound the same as the one that was saved");
    }

    void testOpeningEditorKeepsMelody()
    {
        beginTest("Opening the window shows the restored melody's style and key, and keeps the melody");

        MelodyMakerProcessor original;
        original.setKey(65, false);
        original.generateNewMelody("Ballad");
        juce::MemoryBlock state;
        original.getStateInformation(state);

        MelodyMakerProcessor restored;
        restored.setStateInformation(state.getData(), static_cast<int>(state.getSize()));
        std::unique_ptr<juce::AudioProcessorEditor> editor(restored.createEditor());

        auto shown = [&editor](const juce::String& id) {
            auto* comboBox = dynamic_cast<juce::ComboBox*>(editor->findChildWithID(id));
            return comboBox != nullptr ? comboBox->getText() : juce::String("(no control)");
        };
        expectEquals(shown("style"), juce::String("Ballad"));
        expectEquals(shown("key"), juce::String("F"));
        expectEquals(shown("scale"), juce::String("Minor"));
        expect(sameMelody(restored.getCurrentMelody(), original.getCurrentMelody()), "The melody should be the one that was saved");
    }

    void testFilterCutoffFollowsParameter()
    {
        beginTest("Turning the filter cutoff while playing changes the sound");

        MelodyMakerProcessor processor;
        auto* cutoff = findParameter(processor, "filterCutoff");
        expect(cutoff != nullptr, "No filter cutoff parameter");
        if (cutoff == nullptr)
            return;

        processor.prepareToPlay(sampleRate, blockSize);
        const auto open = play(processor, 2.0);

        // The same two beats again, with the cutoff turned right down after
        // the processor was prepared, as a user turning the knob would
        processor.prepareToPlay(sampleRate, blockSize);
        cutoff->setValueNotifyingHost(0.0f);
        const auto closed = play(processor, 2.0);

        expect(rms(open) > 0.01f, "The melody should be audible with the filter open");
        expect(rms(closed) < 0.5f * rms(open),
               "A 200 Hz cutoff should take most of the level out of a melody above middle C: "
               + juce::String(rms(closed)) + " vs " + juce::String(rms(open)));
    }

    void testExportReplacesExistingFile()
    {
        beginTest("Exporting over an existing MIDI file replaces it");

        MelodyMakerProcessor processor;
        auto file = juce::File::createTempFile(".mid");

        processor.exportToMidiFile(file);
        const auto firstSize = file.getSize();
        processor.exportToMidiFile(file);

        expectEquals(file.getSize(), firstSize, "The second export should replace the first, not add to it");

        juce::MidiFile midiFile;
        juce::FileInputStream stream(file);
        expect(midiFile.readFrom(stream), "The exported file should be readable");

        int noteOns = 0;
        if (auto* track = midiFile.getTrack(0))
            for (auto* event : *track)
                noteOns += event->message.isNoteOn() ? 1 : 0;
        expectEquals(noteOns, (int) processor.getCurrentMelody().size(), "Every note of the melody should be in the file");

        file.deleteFile();
    }
};

static MelodyMakerPluginTests melodyMakerPluginTests;
