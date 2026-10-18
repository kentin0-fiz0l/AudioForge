#include "SampleLibrary.h"
#include "../../../shared/utils/AudioFileSafety.h"

SampleLibrary::SampleLibrary(size_t memoryBudgetBytes)
    : memoryBudget(memoryBudgetBytes)
{
    // Register common audio formats
    formatManager.registerBasicFormats();
}

std::shared_ptr<Sample> SampleLibrary::loadSample(const juce::File& file)
{
    lastError.clear();

    if (AudioForge::FileSafety::isNetworkPath(file.getFullPathName()))
    {
        lastError = "Network paths aren't loaded: " + file.getFullPathName();
        return nullptr;
    }

    if (!file.existsAsFile())
    {
        lastError = "File does not exist: " + file.getFullPathName();
        return nullptr;
    }

    // Create reader for the file
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

    if (reader == nullptr)
    {
        lastError = "Unsupported audio format: " + file.getFileNameWithoutExtension();
        return nullptr;
    }

    // The header's numbers decide how much is allocated, so check them first
    const auto problem = AudioForge::FileSafety::checkReader(*reader);
    if (problem.isNotEmpty())
    {
        lastError = problem + ": " + file.getFullPathName();
        return nullptr;
    }

    // Check memory budget: up to maxLengthSecs, always two channels of floats
    const auto frames = juce::jmin(reader->lengthInSamples,
                                   static_cast<juce::int64>(maxLengthSecs * reader->sampleRate));
    const auto sampleSize = static_cast<size_t>(frames) * 2 * sizeof(float);

    if (sampleSize > memoryBudget || totalMemoryUsed > memoryBudget - sampleSize)
    {
        lastError = juce::String("Memory budget exceeded. Current: ") +
                    juce::String(totalMemoryUsed / (1024 * 1024)) + " MB, " +
                    "Sample: " + juce::String(sampleSize / (1024 * 1024)) + " MB, " +
                    "Limit: " + juce::String(memoryBudget / (1024 * 1024)) + " MB";
        return nullptr;
    }

    // Create sample from reader
    auto sample = std::make_shared<Sample>(*reader, maxLengthSecs);
    sample->setSourceFile(file);

    // Add sample to library
    samples.push_back(sample);
    totalMemoryUsed += sample->getSizeInBytes();

    return sample;
}

void SampleLibrary::unloadSample(int index)
{
    if (index >= 0 && index < static_cast<int>(samples.size()))
    {
        samples.erase(samples.begin() + index);
        updateMemoryUsage();
    }
}

void SampleLibrary::clearAllSamples()
{
    samples.clear();
    totalMemoryUsed = 0;
}

std::shared_ptr<Sample> SampleLibrary::getSample(int index) const
{
    if (index >= 0 && index < static_cast<int>(samples.size()))
    {
        return samples[static_cast<size_t>(index)];
    }
    return nullptr;
}

void SampleLibrary::updateMemoryUsage()
{
    totalMemoryUsed = 0;
    for (const auto& sample : samples)
    {
        if (sample)
        {
            totalMemoryUsed += sample->getSizeInBytes();
        }
    }
}
