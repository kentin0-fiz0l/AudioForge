#include "SampleLibrary.h"

SampleLibrary::SampleLibrary()
{
    // Register common audio formats
    formatManager.registerBasicFormats();
}

std::shared_ptr<Sample> SampleLibrary::loadSample(const juce::File& file)
{
    lastError.clear();

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

    // Create sample from reader
    auto sample = std::make_shared<Sample>(*reader, 30.0);

    // Check memory budget
    size_t sampleSize = sample->getSizeInBytes();
    size_t newTotal = totalMemoryUsed + sampleSize;

    if (newTotal > MAX_MEMORY_BYTES)
    {
        lastError = juce::String("Memory budget exceeded. Current: ") +
                    juce::String(totalMemoryUsed / (1024 * 1024)) + " MB, " +
                    "Sample: " + juce::String(sampleSize / (1024 * 1024)) + " MB, " +
                    "Limit: " + juce::String(MAX_MEMORY_BYTES / (1024 * 1024)) + " MB";
        return nullptr;
    }

    // Add sample to library
    samples.push_back(sample);
    totalMemoryUsed = newTotal;

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
