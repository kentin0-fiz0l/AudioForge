#pragma once

#include "Sample.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <vector>
#include <memory>

/**
 * Sample Library
 *
 * Manages loaded audio samples with memory budget enforcement.
 * Thread-safe sample loading with shared ownership via std::shared_ptr.
 */
class SampleLibrary
{
public:
    static constexpr size_t MAX_MEMORY_BYTES = 512 * 1024 * 1024;  // 512 MB

    SampleLibrary();

    /**
     * Load a sample from a file.
     * Returns nullptr if file cannot be loaded or exceeds memory budget.
     *
     * @param file Audio file to load
     * @return Shared pointer to loaded sample, or nullptr on failure
     */
    std::shared_ptr<Sample> loadSample(const juce::File& file);

    /**
     * Unload a sample by index.
     *
     * @param index Sample index to unload
     */
    void unloadSample(int index);

    /**
     * Clear all samples from library.
     */
    void clearAllSamples();

    /**
     * Get a sample by index.
     *
     * @param index Sample index
     * @return Shared pointer to sample, or nullptr if index is invalid
     */
    std::shared_ptr<Sample> getSample(int index) const;

    /**
     * Get number of loaded samples.
     */
    int getNumSamples() const { return static_cast<int>(samples.size()); }

    /**
     * Get total memory used by all samples.
     */
    size_t getTotalMemoryUsed() const { return totalMemoryUsed; }

    /**
     * Get last error message (empty if no error).
     */
    const juce::String& getLastError() const { return lastError; }

private:
    std::vector<std::shared_ptr<Sample>> samples;
    size_t totalMemoryUsed = 0;
    juce::AudioFormatManager formatManager;
    juce::String lastError;

    void updateMemoryUsage();
};
