// Plugin stress host.
//
// Loads each VST3 given on the command line and drives it with its
// parameters at their defaults, minimums, maximums and a set of random
// settings, at several sample rates, while playing full-velocity notes and
// feeding noise. Reports any non-finite output and the loudest peak seen.
//
// Usage: StressHost [--max-peak <level>] <plugin.vst3> [more plugins...]
//        StressHost --levels [--level-range <low dBFS> <high dBFS>] <plugin.vst3> [more plugins...]
//
// With --levels it does not stress anything. It reports how loud each
// instrument is at its default settings, and with --level-range fails any
// whose loudest single note is outside the range.
//
// Options apply to the plugins named after them, so put them first.

#include <juce_audio_processors/juce_audio_processors.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

namespace
{
    constexpr int blockSize = 512;
    constexpr int numRandomConfigs = 24;
    constexpr double secondsPerRun = 1.5;

    struct RunResult
    {
        bool nonFinite = false;
        float peak = 0.0f;
    };

    // What the caller is prepared to accept from a plugin
    struct Limits
    {
        float maxPeak = 0.0f; // Set by --max-peak; 0 = no ceiling given
    };

    enum class Verdict { ok, tooLoud, nonFinite };

    const char* toString(Verdict verdict)
    {
        switch (verdict)
        {
            case Verdict::tooLoud:   return "TOO-LOUD";
            case Verdict::nonFinite: return "NON-FINITE";
            case Verdict::ok:        break;
        }

        return "ok";
    }

    // Decides whether a plugin's results count as a failure. worstPeak is the
    // loudest finite sample across every setting and sample rate; the inputs
    // were noise at 0.5 and full-velocity notes.
    //
    // Silence is not a failure: pattern generators and a sampler with nothing
    // loaded are legitimately silent here.
    Verdict judge(bool nonFinite, float worstPeak, const Limits& limits)
    {
        if (nonFinite)
            return Verdict::nonFinite;

        if (limits.maxPeak > 0.0f && worstPeak > limits.maxPeak)
            return Verdict::tooLoud;

        return Verdict::ok;
    }

    struct Config
    {
        juce::String name;
        juce::Array<float> values; // Normalised value per parameter; empty = leave defaults
    };

    juce::String describeParameters(juce::AudioProcessor& plugin)
    {
        juce::StringArray parts;
        for (auto* param : plugin.getParameters())
            parts.add(param->getName(24) + "=" + juce::String(param->getValue(), 3));
        return parts.joinIntoString(", ");
    }

    // Plays the notes together at full velocity for a second, with noise on
    // any audio inputs, then releases and lets the plugin ring out.
    RunResult run(juce::AudioProcessor& plugin, double sampleRate, const std::vector<int>& notes = { 36, 60, 96 })
    {
        RunResult result;

        const int channels = juce::jmax(plugin.getTotalNumInputChannels(),
                                        plugin.getTotalNumOutputChannels(), 2);
        const int numInputs = plugin.getTotalNumInputChannels();
        const int totalBlocks = static_cast<int>(secondsPerRun * sampleRate / blockSize);
        const int noteOffBlock = static_cast<int>(1.0 * sampleRate / blockSize);

        juce::AudioBuffer<float> buffer(channels, blockSize);
        juce::Random random(42);

        for (int block = 0; block < totalBlocks; ++block)
        {
            juce::MidiBuffer midi;

            if (block == 0)
                for (int note : notes)
                    midi.addEvent(juce::MidiMessage::noteOn(1, note, (juce::uint8) 127), 0);

            if (block == noteOffBlock)
                for (int note : notes)
                    midi.addEvent(juce::MidiMessage::noteOff(1, note), 0);

            buffer.clear();

            if (block < noteOffBlock)
                for (int channel = 0; channel < numInputs; ++channel)
                    for (int i = 0; i < blockSize; ++i)
                        buffer.setSample(channel, i, 0.5f * (2.0f * random.nextFloat() - 1.0f));

            plugin.processBlock(buffer, midi);

            for (int channel = 0; channel < plugin.getTotalNumOutputChannels(); ++channel)
            {
                const float* data = buffer.getReadPointer(channel);
                for (int i = 0; i < blockSize; ++i)
                {
                    if (! std::isfinite(data[i]))
                        result.nonFinite = true;
                    else
                        result.peak = juce::jmax(result.peak, std::abs(data[i]));
                }
            }
        }

        return result;
    }

    juce::String decibels(float peak)
    {
        return peak > 0.0f ? juce::String(juce::Decibels::gainToDecibels(peak), 1) + " dBFS" : juce::String("silent");
    }

    struct LevelRange
    {
        float low = 0.0f, high = 0.0f; // dBFS
        bool given = false;
    };

    // How loud an instrument is at its default settings: the loudest single
    // full-velocity note, and the loudest of a chord and a drum kit's worth
    // of notes played together. Returns false if the single note is outside
    // the range. Effects are reported as such and skipped, and so are
    // instruments that stay silent: a pattern generator waiting for the
    // transport, or a sampler with nothing loaded.
    bool reportLevels(const juce::String& name, juce::AudioPluginFormatManager& formatManager,
                      const juce::PluginDescription& description, double sampleRate, const LevelRange& range)
    {
        if (! description.isInstrument)
        {
            std::cout << name << "\tEFFECT" << std::endl;
            return true;
        }

        // A new instance for every measurement, so the tail of one note
        // cannot add to the next
        const auto measure = [&] (const std::vector<int>& notes)
        {
            juce::String error;
            auto plugin = formatManager.createPluginInstance(description, sampleRate, blockSize, error);
            if (plugin == nullptr)
                return 0.0f;

            plugin->setRateAndBufferSizeDetails(sampleRate, blockSize);
            plugin->prepareToPlay(sampleRate, blockSize);
            const float peak = run(*plugin, sampleRate, notes).peak;
            plugin->releaseResources();
            return peak;
        };

        // Some instruments are excited by random noise, and one note can
        // differ from the next by several dB. The median of a few is steady.
        const auto typical = [&] (const std::vector<int>& notes)
        {
            std::array<float, 5> peaks;
            for (auto& peak : peaks)
                peak = measure(notes);

            std::sort(peaks.begin(), peaks.end());
            return peaks[peaks.size() / 2];
        };

        float loudestNotePeak = 0.0f;
        int loudestNote = -1;

        // Drum instruments answer particular notes; pitched ones answer all
        for (int note : { 36, 38, 42, 46, 48, 60, 72 })
        {
            const float peak = typical({ note });
            if (peak > loudestNotePeak)
            {
                loudestNotePeak = peak;
                loudestNote = note;
            }
        }

        if (loudestNote < 0)
        {
            std::cout << name << "\tSILENT" << std::endl;
            return true;
        }

        const float together = juce::jmax(measure({ 48, 60, 64, 67 }), measure({ 36, 38, 42 }));
        const float level = juce::Decibels::gainToDecibels(loudestNotePeak);
        const bool inRange = ! range.given || (level >= range.low && level <= range.high);

        std::cout << name << "\t" << (inRange ? "LEVEL" : "LEVEL-OUT-OF-RANGE")
                  << "\tnote " << loudestNote << "\t" << decibels(loudestNotePeak)
                  << "\ttogether\t" << decibels(together) << std::endl;
        return inRange;
    }

    juce::Array<Config> makeConfigs(int numParams)
    {
        juce::Array<Config> configs;
        configs.add({ "defaults", {} });

        Config minimum { "all-min", {} }, maximum { "all-max", {} };
        for (int i = 0; i < numParams; ++i)
        {
            minimum.values.add(0.0f);
            maximum.values.add(1.0f);
        }
        configs.add(minimum);
        configs.add(maximum);

        // Random settings, biased towards the extremes where trouble lives
        juce::Random random(20261003);
        for (int c = 0; c < numRandomConfigs; ++c)
        {
            Config config { "random-" + juce::String(c + 1), {} };
            for (int i = 0; i < numParams; ++i)
            {
                const float r = random.nextFloat();
                config.values.add(r < 0.25f ? 0.0f : (r > 0.75f ? 1.0f : random.nextFloat()));
            }
            configs.add(config);
        }

        return configs;
    }
}

int main(int argc, char* argv[])
{
    juce::ScopedJuceInitialiser_GUI juceInit;

    juce::AudioPluginFormatManager formatManager;
    formatManager.addFormat(new juce::VST3PluginFormat());
    auto* format = formatManager.getFormat(0);

    Limits limits;
    bool levelsOnly = false;
    LevelRange levelRange;
    int numFailed = 0;

    for (int arg = 1; arg < argc; ++arg)
    {
        const juce::String path(argv[arg]);

        if (path == "--max-peak" && arg + 1 < argc)
        {
            limits.maxPeak = juce::String(argv[++arg]).getFloatValue();
            continue;
        }

        if (path == "--levels")
        {
            levelsOnly = true;
            continue;
        }

        if (path == "--level-range" && arg + 2 < argc)
        {
            levelRange.low = juce::String(argv[++arg]).getFloatValue();
            levelRange.high = juce::String(argv[++arg]).getFloatValue();
            levelRange.given = true;
            continue;
        }

        const auto name = juce::File(path).getFileNameWithoutExtension();

        juce::OwnedArray<juce::PluginDescription> descriptions;
        format->findAllTypesForFile(descriptions, path);

        if (descriptions.isEmpty())
        {
            std::cout << name << "\tLOAD-FAILED\tno plugin found in bundle" << std::endl;
            ++numFailed;
            continue;
        }

        if (levelsOnly)
        {
            if (! reportLevels(name, formatManager, *descriptions[0], 48000.0, levelRange))
                ++numFailed;

            continue;
        }

        float worstPeak = 0.0f;
        juce::String worstPeakWhere;
        juce::String worstPeakParams;
        juce::StringArray nonFiniteWhere;
        juce::String firstNonFiniteParams;
        bool loadFailed = false;

        for (double sampleRate : { 44100.0, 48000.0, 96000.0 })
        {
            juce::String error;
            auto plugin = formatManager.createPluginInstance(*descriptions[0], sampleRate, blockSize, error);

            if (plugin == nullptr)
            {
                std::cout << name << "\tLOAD-FAILED\t" << error << std::endl;
                loadFailed = true;
                break;
            }

            const auto& params = plugin->getParameters();
            juce::Array<float> defaults;
            for (auto* param : params)
                defaults.add(param->getValue());

            for (const auto& config : makeConfigs(params.size()))
            {
                for (int i = 0; i < params.size(); ++i)
                    params[i]->setValue(config.values.isEmpty() ? defaults[i] : config.values[i]);

                // A fresh prepare for every run, so one bad run cannot poison the next
                plugin->releaseResources();
                plugin->setRateAndBufferSizeDetails(sampleRate, blockSize);
                plugin->prepareToPlay(sampleRate, blockSize);

                const auto result = run(*plugin, sampleRate);
                const auto where = config.name + "@" + juce::String(static_cast<int>(sampleRate));

                if (result.nonFinite)
                {
                    if (nonFiniteWhere.isEmpty())
                        firstNonFiniteParams = describeParameters(*plugin);
                    nonFiniteWhere.add(where);
                }

                if (std::getenv("STRESS_VERBOSE") != nullptr)
                    std::cout << "  " << where << "\tpeak " << juce::String(result.peak, 3)
                              << (result.nonFinite ? "\tNON-FINITE" : "") << std::endl;

                if (result.peak > worstPeak)
                {
                    worstPeak = result.peak;
                    worstPeakWhere = where;
                    worstPeakParams = describeParameters(*plugin);
                }
            }

            plugin->releaseResources();
        }

        if (loadFailed)
        {
            ++numFailed;
            continue;
        }

        const auto verdict = judge(! nonFiniteWhere.isEmpty(), worstPeak, limits);
        if (verdict != Verdict::ok)
            ++numFailed;

        std::cout << name << "\t" << toString(verdict)
                  << "\tpeak " << juce::String(worstPeak, 2) << " (" << worstPeakWhere << ")";

        if (verdict == Verdict::nonFinite)
            std::cout << "\t" << nonFiniteWhere.size() << " runs, first: " << nonFiniteWhere[0]
                      << "\t" << firstNonFiniteParams;
        else if (verdict == Verdict::tooLoud || worstPeak > 8.0f)
            std::cout << "\tloudest with: " << worstPeakParams;

        std::cout << std::endl;
    }

    return numFailed == 0 ? 0 : 1;
}
