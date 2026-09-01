#include "Sample.h"

Sample::Sample(juce::AudioFormatReader& reader, double maxLengthSecs)
{
    sampleRate = reader.sampleRate;
    numChannels = static_cast<int>(reader.numChannels);

    // Limit sample length to maxLengthSecs
    auto maxSamples = static_cast<int>(maxLengthSecs * sampleRate);
    lengthInSamples = static_cast<int>(juce::jmin(reader.lengthInSamples,
                                                    static_cast<juce::int64>(maxSamples)));

    // Allocate buffer (always use stereo for simplicity)
    audioData.setSize(2, lengthInSamples);

    if (numChannels == 1)
    {
        // Mono: read into both channels
        reader.read(&audioData, 0, lengthInSamples, 0, true, true);
    }
    else
    {
        // Stereo: read normally
        reader.read(&audioData, 0, lengthInSamples, 0, true, true);
    }

    // Set default loop points to full sample
    loopStartSamples = 0;
    loopEndSamples = lengthInSamples - 1;
}

size_t Sample::getSizeInBytes() const
{
    // Each sample is 4 bytes (float32), multiply by channels and length
    return static_cast<size_t>(audioData.getNumChannels() * lengthInSamples * sizeof(float));
}

void Sample::setLoopPoints(double startSec, double endSec)
{
    loopStartSamples = static_cast<int>(startSec * sampleRate);
    loopEndSamples = static_cast<int>(endSec * sampleRate);

    // Clamp to valid range
    loopStartSamples = juce::jlimit(0, lengthInSamples - 1, loopStartSamples);
    loopEndSamples = juce::jlimit(loopStartSamples + 1, lengthInSamples - 1, loopEndSamples);
}
