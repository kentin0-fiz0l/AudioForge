#pragma once

#include <juce_audio_formats/juce_audio_formats.h>

namespace AudioForge {
namespace FileSafety {

/**
 * Checks for files named by something the user didn't write themselves: a
 * saved project, an SFZ instrument, a preset from elsewhere.
 *
 * A network path (\\host\share or //host/share) is refused: on Windows,
 * opening one sends the user's credentials to that host. A file whose
 * header claims an impossible sample rate or channel count is refused
 * before anything is allocated from those numbers.
 */

constexpr double minSampleRate = 8000.0;
constexpr double maxSampleRate = 384000.0;
constexpr int maxChannels = 8;

/** Whether a path names a file on another machine. */
inline bool isNetworkPath(const juce::String& path)
{
    const auto p = path.trimStart();
    return p.startsWith("\\\\") || p.startsWith("//") || p.startsWith("\\/") || p.startsWith("/\\")
        || p.contains("://");
}

/** Why a reader's header can't be trusted, or an empty string if it can. */
inline juce::String checkReader(const juce::AudioFormatReader& reader)
{
    if (! (reader.sampleRate >= minSampleRate && reader.sampleRate <= maxSampleRate))
        return "unsupported sample rate " + juce::String(reader.sampleRate) + " Hz";
    if (reader.numChannels < 1 || reader.numChannels > (unsigned int) maxChannels)
        return "unsupported channel count " + juce::String(reader.numChannels);
    if (reader.lengthInSamples <= 0)
        return "no audio";
    return {};
}

} // namespace FileSafety
} // namespace AudioForge
