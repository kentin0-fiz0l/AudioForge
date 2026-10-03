// Plugin stress host.
//
// Loads each VST3 given on the command line and drives it with its
// parameters at their defaults, minimums, maximums and a set of random
// settings, at several sample rates, while playing full-velocity notes and
// feeding noise. Reports any non-finite output and the loudest peak seen.
//
// Usage: StressHost <plugin.vst3> [more plugins...]

#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>
#include <cstdlib>
#include <iostream>

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

    // Plays a three-note chord at full velocity for a second, with noise on
    // any audio inputs, then releases and lets the plugin ring out.
    RunResult run(juce::AudioProcessor& plugin, double sampleRate)
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
                for (int note : { 36, 60, 96 })
                    midi.addEvent(juce::MidiMessage::noteOn(1, note, (juce::uint8) 127), 0);

            if (block == noteOffBlock)
                for (int note : { 36, 60, 96 })
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

    int numFailed = 0;

    for (int arg = 1; arg < argc; ++arg)
    {
        const juce::String path(argv[arg]);
        const auto name = juce::File(path).getFileNameWithoutExtension();

        juce::OwnedArray<juce::PluginDescription> descriptions;
        format->findAllTypesForFile(descriptions, path);

        if (descriptions.isEmpty())
        {
            std::cout << name << "\tLOAD-FAILED\tno plugin found in bundle" << std::endl;
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

        const bool failed = ! nonFiniteWhere.isEmpty();
        if (failed)
            ++numFailed;

        std::cout << name << "\t" << (failed ? "NON-FINITE" : "ok")
                  << "\tpeak " << juce::String(worstPeak, 2) << " (" << worstPeakWhere << ")";

        if (failed)
            std::cout << "\t" << nonFiniteWhere.size() << " runs, first: " << nonFiniteWhere[0]
                      << "\t" << firstNonFiniteParams;
        else if (worstPeak > 8.0f)
            std::cout << "\tloudest with: " << worstPeakParams;

        std::cout << std::endl;
    }

    return numFailed == 0 ? 0 : 1;
}
