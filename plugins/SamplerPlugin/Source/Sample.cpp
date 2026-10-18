#include "Sample.h"

Sample::Sample(juce::AudioFormatReader& reader, double maxLengthSecs)
{
    sampleRate = reader.sampleRate;
    numChannels = static_cast<int>(reader.numChannels);

    // Limit sample length to maxLengthSecs
    auto maxSamples = static_cast<int>(maxLengthSecs * sampleRate);
    lengthInSamples = static_cast<int>(juce::jmin(reader.lengthInSamples,
                                                    static_cast<juce::int64>(maxSamples)));

    // Allocate buffer (always use stereo for simplicity; the reader copies a
    // mono file into both channels)
    audioData.setSize(2, lengthInSamples);
    reader.read(&audioData, 0, lengthInSamples, 0, true, true);

    // Set default loop points to full sample
    loopStartSamples = 0;
    loopEndSamples = lengthInSamples - 1;

    // A loop stored in the file
    if (reader.metadataValues.getValue("NumSampleLoops", "0").getIntValue() > 0)
    {
        fileLoopStart = juce::jlimit(0, juce::jmax(0, lengthInSamples - 1),
                                     reader.metadataValues.getValue("Loop0Start", "0").getIntValue());
        fileLoopEnd = juce::jlimit(0, juce::jmax(0, lengthInSamples - 1),
                                   reader.metadataValues.getValue("Loop0End", "0").getIntValue());
    }
}

size_t Sample::getSizeInBytes() const
{
    // Each sample is 4 bytes (float32), multiply by channels and length
    return static_cast<size_t>(audioData.getNumChannels() * lengthInSamples * sizeof(float));
}

void Sample::setLoopPoints(double startSec, double endSec)
{
    setLoopPointsInSamples(static_cast<int>(startSec * sampleRate), static_cast<int>(endSec * sampleRate));
}

void Sample::setLoopPointsInSamples(int startSample, int endSample)
{
    loopStartSamples = startSample;
    loopEndSamples = endSample;

    // Clamp to valid range
    loopStartSamples = juce::jlimit(0, lengthInSamples - 1, loopStartSamples);
    loopEndSamples = juce::jlimit(loopStartSamples + 1, lengthInSamples - 1, loopEndSamples);
}
