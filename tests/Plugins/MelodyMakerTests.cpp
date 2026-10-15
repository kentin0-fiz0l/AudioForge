/**
 * MelodyMaker Plugin Tests
 *
 * MelodyMaker plays a generated melody in time with the host's transport.
 * These tests cover what a user does with it:
 * - Picking a style changes the kind of melody it generates
 * - Its settings and its melody come back when a session is reopened, and
 *   opening the window does not replace the melody with a new one
 * - Turning the filter cutoff while it plays changes the sound, and moving
 *   the delay time glides rather than clicking
 * - Replacing the melody while a note sounds releases that note
 * - Exporting a melody over an existing MIDI file replaces that file
 */

#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>
#include <thread>
#include <atomic>
#include <limits>

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

    // Plays the processor from beat up to endBeat, as a host would with its transport
    // running, appends the left channel of what it played to output, and leaves beat
    // where it stopped so a later call carries on from there
    void playRange(MelodyMakerProcessor& processor, double& beat, double endBeat, std::vector<float>& output)
    {
        FakePlayHead playHead;
        processor.setPlayHead(&playHead);

        const double beatsPerBlock = blockSize * playHead.bpm / (60.0 * sampleRate);
        juce::AudioBuffer<float> audio(2, blockSize);
        juce::MidiBuffer midi;

        for (; beat < endBeat; beat += beatsPerBlock)
        {
            playHead.ppq = beat;
            processor.processBlock(audio, midi);
            output.insert(output.end(), audio.getReadPointer(0), audio.getReadPointer(0) + blockSize);
        }

        processor.setPlayHead(nullptr);
    }

    // Plays the processor from the top of the melody for the given number of beats
    std::vector<float> play(MelodyMakerProcessor& processor, double beats)
    {
        std::vector<float> output;
        double beat = 0.0;
        playRange(processor, beat, beats, output);
        return output;
    }

    int sampleAtBeat(double beat) { return static_cast<int>(beat * 60.0 / 120.0 * sampleRate); }

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

    void setParameter(juce::AudioProcessor& processor, const juce::String& id, float value)
    {
        if (auto* param = findParameter(processor, id))
            param->setValueNotifyingHost(param->convertTo0to1(value));
    }

    // Replaces the processor's melody with the given notes, the way reopening a session does
    void loadMelody(MelodyMakerProcessor& processor, const std::vector<MelodyEngine::Note>& notes)
    {
        juce::MemoryBlock saved;
        processor.getStateInformation(saved);
        auto state = juce::ValueTree::fromXml(*juce::AudioProcessor::getXmlFromBinary(saved.getData(), static_cast<int>(saved.getSize())));
        auto melody = state.getChildWithName("Melody");
        melody.removeAllChildren(nullptr);
        for (const auto& n : notes)
        {
            juce::ValueTree note("Note");
            note.setProperty("pitch", n.pitch, nullptr);
            note.setProperty("start", n.startTime, nullptr);
            note.setProperty("duration", n.duration, nullptr);
            note.setProperty("velocity", n.velocity, nullptr);
            melody.appendChild(note, nullptr);
        }
        juce::MemoryBlock replaced;
        juce::AudioProcessor::copyXmlToBinary(*state.createXml(), replaced);
        processor.setStateInformation(replaced.getData(), static_cast<int>(replaced.getSize()));
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
        testDamagedSessionIsMadeSafe();
        testSavingWhileTheMelodyChanges();
        testOpeningEditorKeepsMelody();
        testEditorShowsKeyBelowMiddleC();
        testReplacingMelodyReleasesSoundingNotes();
        testDelayTimeChangeDoesNotClick();
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

    // A project file is outside the plugin's control: a damaged or hand-edited one must not
    // put out-of-range notes or timings into the audio thread, or make it allocate without bound
    void testDamagedSessionIsMadeSafe()
    {
        beginTest("A damaged session is clamped to safe values");

        MelodyMakerProcessor original;
        original.prepareToPlay(sampleRate, blockSize);
        original.generateNewMelody("Pop");
        juce::MemoryBlock good;
        original.getStateInformation(good);

        auto xml = juce::AudioProcessor::getXmlFromBinary(good.getData(), static_cast<int>(good.getSize()));
        expect(xml != nullptr);
        auto state = juce::ValueTree::fromXml(*xml);
        state.setProperty("keyRoot", 5000, nullptr);
        state.setProperty("style", "<not a style>", nullptr);

        auto melody = state.getChildWithName("Melody");
        melody.removeAllChildren(nullptr);
        // pitch, start (beats), duration (beats), velocity: the first two have usable timings
        // but out-of-range values, the last two have timings no note can have
        const juce::var bad[][4] = { { -40, 1.0, 0.5, 999 },
                                     { 300, 2.0, 1.0e30, -7 },
                                     { 64, std::numeric_limits<double>::quiet_NaN(), 1.0, 64 },
                                     { 64, -3.0, -1.0, 64 } };
        for (int i = 0; i < 100000; ++i)
        {
            const auto& v = bad[i % 4];
            juce::ValueTree note("Note");
            note.setProperty("pitch", v[0], nullptr);
            note.setProperty("start", v[1], nullptr);
            note.setProperty("duration", v[2], nullptr);
            note.setProperty("velocity", v[3], nullptr);
            melody.appendChild(note, nullptr);
        }
        juce::MemoryBlock damaged;
        juce::AudioProcessor::copyXmlToBinary(*state.createXml(), damaged);

        MelodyMakerProcessor restored;
        restored.prepareToPlay(sampleRate, blockSize);
        restored.setStateInformation(damaged.getData(), static_cast<int>(damaged.getSize()));

        expect(restored.getKeyRoot() >= 0 && restored.getKeyRoot() + 24 <= 127,
               "The key root should be kept to a playable range, not " + juce::String(restored.getKeyRoot()));
        expectEquals(juce::String(restored.getStyle()), juce::String("Pop"), "An unknown style should be ignored");
        expect(! restored.getCurrentMelody().empty(), "Notes with usable timings should be kept, clamped");
        expect(restored.getCurrentMelody().size() <= 4096, "The melody should be capped, not "
               + juce::String((int) restored.getCurrentMelody().size()) + " notes");
        for (const auto& note : restored.getCurrentMelody())
        {
            expect(note.pitch >= 0 && note.pitch <= 127, "Pitch out of range: " + juce::String(note.pitch));
            expect(note.velocity >= 1 && note.velocity <= 127, "Velocity out of range: " + juce::String(note.velocity));
            expect(std::isfinite(note.startTime) && note.startTime >= 0.0f, "Bad start time");
            expect(std::isfinite(note.duration) && note.duration > 0.0f && note.duration <= 64.0f, "Bad duration");
        }

        for (auto s : play(restored, 8.0))
            if (! std::isfinite(s))
            {
                expect(false, "The output should stay finite");
                break;
            }
    }

    // Hosts may save from a background thread while the melody is being replaced
    void testSavingWhileTheMelodyChanges()
    {
        beginTest("Saving on another thread while the melody is replaced is safe");

        MelodyMakerProcessor processor;
        processor.prepareToPlay(sampleRate, blockSize);
        std::atomic<bool> done { false };
        std::atomic<int> saves { 0 };

        std::thread saver([&] {
            while (! done.load())
            {
                juce::MemoryBlock state;
                processor.getStateInformation(state);
                saves.fetch_add(1);
            }
        });

        const char* styles[] = { "Pop", "EDM", "Jazz", "Ballad" };
        for (int i = 0; i < 2000; ++i)
            processor.generateNewMelody(styles[i % 4]);

        done.store(true);
        saver.join();
        expect(saves.load() > 0, "The saving thread should have run");
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

    void testEditorShowsKeyBelowMiddleC()
    {
        beginTest("The window shows a key whose root is below middle C");

        MelodyMakerProcessor processor;
        processor.setKey(59, true); // B3
        processor.generateNewMelody("Pop");
        std::unique_ptr<juce::AudioProcessorEditor> editor(processor.createEditor());

        auto* key = dynamic_cast<juce::ComboBox*>(editor->findChildWithID("key"));
        expect(key != nullptr, "No key control");
        if (key != nullptr)
            expectEquals(key->getText(), juce::String("B"));
    }

    // A note that is sounding when the melody is replaced (Generate, or a session being
    // loaded) gets no note-off from the new melody, so it must not be left to ring forever
    void testReplacingMelodyReleasesSoundingNotes()
    {
        beginTest("Replacing the melody while a note sounds releases that note");

        MelodyMakerProcessor processor;
        processor.prepareToPlay(sampleRate, blockSize);
        setParameter(processor, "sustain", 1.0f);
        setParameter(processor, "release", 0.01f);
        loadMelody(processor, { { 60, 0.0f, 2.0f, 100, true } });

        std::vector<float> output;
        double beat = 0.0;
        playRange(processor, beat, 1.0, output);
        expect(rms(output) > 0.01f, "The first note should be audible");

        loadMelody(processor, { { 72, 12.0f, 0.5f, 100, true } });
        output.clear();
        playRange(processor, beat, 8.0, output);

        const std::vector<float> tail(output.begin() + sampleAtBeat(2.0), output.end());
        expect(rms(tail) < 0.0001f, "The old note should have been released, but it still plays at "
               + juce::String(rms(tail)));
    }

    // Moving the Delay Time knob while echoes play should glide, not jump the read head
    void testDelayTimeChangeDoesNotClick()
    {
        beginTest("Changing the delay time while echoes play does not click");

        float worstJump = 0.0f, worstSlope = 0.0f;
        // Several change points, so the result does not hang on where in its cycle the echo is
        for (int trial = 0; trial < 8; ++trial)
        {
            MelodyMakerProcessor processor;
            processor.prepareToPlay(sampleRate, blockSize);
            setParameter(processor, "attack", 0.005f);
            setParameter(processor, "sustain", 1.0f);
            setParameter(processor, "release", 0.005f);
            setParameter(processor, "delayTime", 0.25f);
            setParameter(processor, "delayFeedback", 0.0f);
            setParameter(processor, "delayMix", 1.0f);
            // One note for the first beat, so after it only its echo plays
            loadMelody(processor, { { 60, 0.0f, 1.0f, 100, true } });

            std::vector<float> output;
            double beat = 0.0;
            playRange(processor, beat, 1.1 + 0.02 * trial, output);
            const auto changeAt = output.size();

            // From here a 10 ms delay reads only silence
            setParameter(processor, "delayTime", 0.01f);
            playRange(processor, beat, beat + 0.5, output);

            for (size_t i = 1; i < changeAt; ++i)
                worstSlope = juce::jmax(worstSlope, std::abs(output[i] - output[i - 1]));
            for (size_t i = changeAt; i < output.size(); ++i)
                worstJump = juce::jmax(worstJump, std::abs(output[i] - output[i - 1]));
        }

        expect(worstSlope > 0.0f, "The melody and its echo should be audible");
        expect(worstJump < 8.0f * worstSlope, "The delay time change jumped by " + juce::String(worstJump)
               + " in one sample; the steepest step while playing normally was " + juce::String(worstSlope));
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
