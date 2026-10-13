// Audio demos for the website: plays an instrument, or feeds an effect, a short
// musical phrase and writes what comes out to a WAV file.
//
// Phrases (4 bars at 110 BPM, then a tail so reverbs and releases ring out):
//   keys    four seventh chords (Cmaj7 Am7 Fmaj7 G7) with a melody on top
//   bass    a root-note bass line under the same chords
//   lead    the melody alone, one note at a time
//   drums   a General MIDI groove: kick, snare, clap, closed and open hats, a tom fill
//   effect  no notes: the plugin is fed
// An effect is fed a built-in loop of drums and chords instead, synthesized here,
// so the demos need no recordings and carry no licence.

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <cmath>
#include <vector>

namespace demo
{
    constexpr double sampleRate = 48000.0;
    constexpr int blockSize = 512;
    constexpr double bpm = 110.0;
    constexpr double beatSeconds = 60.0 / bpm;
    constexpr int bars = 4;
    constexpr double phraseSeconds = bars * 4 * beatSeconds;
    constexpr double tailSeconds = 2.5;

    struct Note
    {
        double beat;     // start, in beats from the top
        double length;   // in beats
        int pitch;
        int velocity;
    };

    // Cmaj7, Am7, Fmaj7, G7: one bar each
    inline const int chords[bars][4] = { { 48, 52, 55, 59 }, { 45, 48, 52, 55 }, { 41, 45, 48, 52 }, { 43, 47, 50, 53 } };

    inline std::vector<Note> phrase(const juce::String& name)
    {
        std::vector<Note> notes;
        const int melody[] = { 76, 74, 72, 74, 76, 79, 76, 74, 72, 69, 72, 74, 76, 74, 72, 71 };

        if (name == "drums")
        {
            for (int bar = 0; bar < bars; ++bar)
            {
                const double top = bar * 4.0;
                for (double b : { 0.0, 1.5, 2.0 })
                    notes.push_back({ top + b, 0.25, 36, 120 });               // kick
                for (double b : { 1.0, 3.0 })
                    notes.push_back({ top + b, 0.25, bar == 3 ? 39 : 38, 110 }); // snare, a clap in the last bar
                for (int eighth = 0; eighth < 8; ++eighth)
                    if (! (bar == 3 && eighth >= 6))
                        notes.push_back({ top + eighth * 0.5, 0.25, eighth == 7 ? 46 : 42, eighth % 2 ? 70 : 95 });
            }
            for (int i = 0; i < 4; ++i) // tom fill to finish
                notes.push_back({ 15.0 + i * 0.25, 0.25, 50 - i * 3, 105 });
            notes.push_back({ 16.0, 0.5, 36, 127 });
            return notes;
        }

        if (name == "bass")
        {
            for (int bar = 0; bar < bars; ++bar)
                for (double b : { 0.0, 1.0, 1.5, 2.5, 3.0 })
                    notes.push_back({ bar * 4.0 + b, 0.45, chords[bar][0] - 12, b == 0.0 ? 115 : 95 });
            notes.push_back({ 16.0, 1.0, 36, 110 });
            return notes;
        }

        // keys and lead share the melody: eighths in the first half of each bar, a long note after
        for (int bar = 0; bar < bars; ++bar)
            for (int i = 0; i < 4; ++i)
                notes.push_back({ bar * 4.0 + (i < 3 ? i * 0.5 : 1.5), i < 3 ? 0.45 : 2.4,
                                  melody[bar * 4 + i], 100 });
        notes.push_back({ 16.0, 2.0, 72, 100 });

        if (name == "keys")
            for (int bar = 0; bar < bars; ++bar)
                for (int pitch : chords[bar])
                    notes.push_back({ bar * 4.0, 3.9, pitch, 80 });

        return notes;
    }

    // The loop fed to effects: drums and saw-wave chords at about -10 dBFS
    inline juce::AudioBuffer<float> sourceLoop()
    {
        const int length = static_cast<int>(phraseSeconds * sampleRate);
        juce::AudioBuffer<float> loop(2, length);
        loop.clear();
        juce::Random random(7);
        const auto beatSamples = beatSeconds * sampleRate;

        auto add = [&](int start, int n, auto&& sample)
        {
            for (int i = 0; i < n && start + i < length; ++i)
            {
                const float s = sample(i);
                loop.addSample(0, start + i, s);
                loop.addSample(1, start + i, s);
            }
        };

        for (int beat = 0; beat < bars * 4; ++beat)
        {
            const int at = static_cast<int>(beat * beatSamples);
            if (beat % 2 == 0) // kick: a falling sine
                add(at, static_cast<int>(0.35 * sampleRate), [&](int i) {
                    const double t = i / sampleRate;
                    return static_cast<float>(0.6 * std::sin(2.0 * juce::MathConstants<double>::pi * (45.0 * t + 6.0 * (1.0 - std::exp(-t * 30.0)))) * std::exp(-t * 7.0));
                });
            else // snare: noise and a tone
                add(at, static_cast<int>(0.2 * sampleRate), [&](int i) {
                    const double t = i / sampleRate;
                    return static_cast<float>((0.35 * (2.0 * random.nextDouble() - 1.0) + 0.2 * std::sin(2.0 * juce::MathConstants<double>::pi * 190.0 * t)) * std::exp(-t * 18.0));
                });
            for (int half = 0; half < 2; ++half) // hats on the eighths
                add(at + static_cast<int>(half * beatSamples / 2), static_cast<int>(0.05 * sampleRate), [&](int i) {
                    return static_cast<float>(0.12 * (2.0 * random.nextDouble() - 1.0) * std::exp(-i / sampleRate * 70.0));
                });
        }

        for (int bar = 0; bar < bars; ++bar) // chords: detuned saws, softened
        {
            const int at = static_cast<int>(bar * 4 * beatSamples);
            const int n = static_cast<int>(4 * beatSamples);
            for (int pitch : chords[bar])
            {
                const double freq = 440.0 * std::pow(2.0, (pitch + 12 - 69) / 12.0);
                double lowpass = 0.0;
                add(at, n, [&](int i) {
                    const double t = i / sampleRate;
                    const double saw = std::fmod(t * freq, 1.0) * 2.0 - 1.0 + std::fmod(t * freq * 1.004, 1.0) * 2.0 - 1.0;
                    lowpass += 0.08 * (saw - lowpass);
                    const double envelope = std::min(1.0, t * 20.0) * std::min(1.0, (n - i) / (0.05 * sampleRate));
                    return static_cast<float>(0.045 * lowpass * envelope);
                });
            }
        }
        return loop;
    }

    // Renders a demo. Returns an error message, or an empty string on success.
    inline juce::String render(juce::AudioProcessor& plugin, const juce::String& phraseName, const juce::File& out)
    {
        const bool isEffect = phraseName == "effect";
        if (plugin.getTotalNumOutputChannels() == 0)
            return "it has no audio output (a MIDI tool)";

        plugin.setRateAndBufferSizeDetails(sampleRate, blockSize);
        plugin.prepareToPlay(sampleRate, blockSize);

        const int total = static_cast<int>((phraseSeconds + tailSeconds) * sampleRate);
        const int channels = juce::jmax(2, plugin.getTotalNumInputChannels(), plugin.getTotalNumOutputChannels());
        juce::AudioBuffer<float> block(channels, blockSize), result(2, total);
        result.clear();

        const auto loop = isEffect ? sourceLoop() : juce::AudioBuffer<float>();
        const auto notes = isEffect ? std::vector<Note>() : phrase(phraseName);
        const auto beatSamples = beatSeconds * sampleRate;

        for (int start = 0; start < total; start += blockSize)
        {
            const int n = juce::jmin(blockSize, total - start);
            block.clear();
            juce::MidiBuffer midi;

            for (const auto& note : notes) // sample-accurate note on and off
            {
                const int on = static_cast<int>(note.beat * beatSamples);
                const int off = static_cast<int>((note.beat + note.length) * beatSamples);
                if (on >= start && on < start + n)
                    midi.addEvent(juce::MidiMessage::noteOn(1, note.pitch, (juce::uint8) note.velocity), on - start);
                if (off >= start && off < start + n)
                    midi.addEvent(juce::MidiMessage::noteOff(1, note.pitch), off - start);
            }

            if (isEffect)
                for (int ch = 0; ch < juce::jmin(2, plugin.getTotalNumInputChannels()); ++ch)
                    if (start < loop.getNumSamples())
                        block.copyFrom(ch, 0, loop, ch, start, juce::jmin(n, loop.getNumSamples() - start));

            juce::AudioBuffer<float> view(block.getArrayOfWritePointers(), channels, n);
            plugin.processBlock(view, midi);

            for (int ch = 0; ch < 2; ++ch)
            {
                const int from = juce::jmin(ch, plugin.getTotalNumOutputChannels() - 1);
                result.copyFrom(ch, start, view, from, 0, n);
            }
        }
        plugin.releaseResources();

        float peak = 0.0f;
        for (int ch = 0; ch < 2; ++ch)
            for (int i = 0; i < total; ++i)
            {
                const float s = result.getSample(ch, i);
                if (! std::isfinite(s))
                    return "the output is not finite";
                peak = juce::jmax(peak, std::abs(s));
            }
        if (peak < 1.0e-4f)
            return "the output is silent";
        if (peak > 1.0f) // keep the shape, avoid clipping in the file
            result.applyGain(0.98f / peak);

        out.deleteFile();
        juce::WavAudioFormat wav;
        std::unique_ptr<juce::AudioFormatWriter> writer(
            wav.createWriterFor(new juce::FileOutputStream(out), sampleRate, 2, 24, {}, 0));
        if (writer == nullptr)
            return "could not write " + out.getFullPathName();
        writer->writeFromAudioSampleBuffer(result, 0, total);
        return {};
    }
}
