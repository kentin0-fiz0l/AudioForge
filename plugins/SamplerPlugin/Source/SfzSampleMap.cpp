#include "SfzSampleMap.h"
#include "../../../shared/utils/AudioFileSafety.h"
#include <cmath>
#include <map>

bool SfzSampleMap::load(const juce::File& sfzFile, juce::String& error)
{
    file = sfzFile;
    regions.clear();
    warnings.clear();
    library.clearAllSamples();
    keyswitchLow = 128;
    keyswitchHigh = -1;
    currentKeyswitch = -1;

    if (const auto why = AudioForge::FileSafety::refusal(sfzFile); why.isNotEmpty())
    {
        error = why;
        return false;
    }

    if (! sfzFile.existsAsFile())
    {
        error = "no SFZ file at " + sfzFile.getFullPathName();
        return false;
    }

    juce::String sfzText;
    if (! AudioForge::FileSafety::readText(sfzFile, sfzText, error))
        return false;

    auto instrument = Sfz::parse(sfzText);
    warnings = instrument.warnings;

    // Each file is loaded once, however many regions use it
    std::map<juce::String, std::shared_ptr<Sample>> loaded;
    juce::StringArray failures;

    for (const auto& opcodes : instrument.regions)
    {
        if (opcodes.releaseTrigger)
            continue;  // Release samples aren't played

        // A library may reach outside its folder (../, absolute paths),
        // but not onto another machine, unless that's an approved library
        const auto sampleFile = Sfz::resolveSample(sfzFile, opcodes.sample);
        if (const auto why = AudioForge::FileSafety::refusal(opcodes.sample, sampleFile); why.isNotEmpty())
        {
            warnings.addIfNotAlreadyThere(why);
            continue;
        }
        const auto path = sampleFile.getFullPathName();

        auto found = loaded.find(path);
        if (found == loaded.end())
        {
            auto sample = library.loadSample(sampleFile);
            if (sample == nullptr)
                failures.add(library.getLastError());
            found = loaded.emplace(path, sample).first;
        }

        if (found->second == nullptr)
            continue;

        regions.push_back({ opcodes, found->second, 0 });

        if (opcodes.swLokey >= 0 && opcodes.swHikey >= opcodes.swLokey)
        {
            keyswitchLow = juce::jmin(keyswitchLow, opcodes.swLokey);
            keyswitchHigh = juce::jmax(keyswitchHigh, opcodes.swHikey);
        }
        if (currentKeyswitch < 0 && opcodes.swDefault >= 0)
            currentKeyswitch = opcodes.swDefault;
    }

    if (! failures.isEmpty())
        warnings.add(juce::String(failures.size()) + " of " + juce::String((int) loaded.size())
                     + " samples couldn't be loaded, and their regions are left out; the first: " + failures[0]);

    if (regions.empty())
    {
        error = "no playable regions in " + sfzFile.getFullPathName()
              + (warnings.isEmpty() ? juce::String() : " (" + warnings[0] + ")");
        return false;
    }

    return true;
}

std::vector<SamplerVoice::Playback> SfzSampleMap::noteOn(int note, int velocity, float random, float semitones)
{
    std::vector<SamplerVoice::Playback> result;

    if (isKeyswitch(note))
    {
        currentKeyswitch = note;
        return result;
    }

    for (auto& region : regions)
    {
        const auto& r = region.opcodes;
        if (! r.containsKey(note) || ! r.containsVelocity(velocity))
            continue;
        if (r.swLast >= 0 && r.swLast != currentKeyswitch)
            continue;

        // Round robin: every region in reach counts the note, and plays on
        // its own turn
        const int turn = region.seqCounter++ % r.seqLength + 1;
        if (turn != r.seqPosition)
            continue;

        if (random < r.lorand || random >= r.hirand)
            continue;

        result.push_back(playbackFor(region, note, velocity, semitones));
    }

    return result;
}

SamplerVoice::Playback SfzSampleMap::playbackFor(const Region& region, int note, int velocity, float semitones)
{
    const auto& r = region.opcodes;
    SamplerVoice::Playback playback;
    playback.sample = region.sample;

    const double pitch = (note - r.keycenter) * r.keytrack / 100.0 + r.transpose + r.tune / 100.0 + semitones;
    playback.pitchRatio = std::pow(2.0, pitch / 12.0);

    // Velocity follows SFZ's default curve, the square of the velocity, by
    // amp_veltrack percent
    const float track = juce::jlimit(-1.0f, 1.0f, r.ampVeltrack / 100.0f);
    const float v = juce::jlimit(0.0f, 1.0f, velocity / 127.0f);
    const float velocityGain = track >= 0.0f ? 1.0f - track + track * v * v
                                             : 1.0f + track - track * (1.0f - v) * (1.0f - v);
    const float gain = velocityGain * juce::Decibels::decibelsToGain(r.volume) * r.amplitude / 100.0f;

    // Pan as a balance: the far side comes down
    const float pan = juce::jlimit(-1.0f, 1.0f, r.pan / 100.0f);
    playback.gainLeft = gain * (pan > 0.0f ? 1.0f - pan : 1.0f);
    playback.gainRight = gain * (pan < 0.0f ? 1.0f + pan : 1.0f);

    const auto& sample = *region.sample;
    const int lastFrame = sample.getLengthInSamples() - 1;
    playback.startFrame = (int) juce::jlimit<juce::int64>(0, juce::jmax(0, lastFrame), r.offset);
    playback.endFrame = r.end >= 0 ? (int) juce::jmin<juce::int64>(r.end, lastFrame) : -1;

    using Mode = SfzRegion::LoopMode;
    const bool loops = r.loopMode == Mode::Continuous || r.loopMode == Mode::Sustain
                    || (r.loopMode == Mode::Unset && sample.hasFileLoop());
    if (loops)
    {
        playback.loopStart = (int) (r.loopStart >= 0 ? r.loopStart : sample.getFileLoopStart());
        playback.loopEnd = (int) (r.loopEnd >= 0 ? r.loopEnd : (sample.hasFileLoop() ? sample.getFileLoopEnd() : lastFrame));
        playback.loopEnd = juce::jmin(playback.loopEnd, lastFrame);
        playback.loopStart = juce::jlimit(0, juce::jmax(0, playback.loopEnd - 1), playback.loopStart);
        playback.loopMode = playback.loopEnd > playback.loopStart ? Sample::LoopMode::Forward : Sample::LoopMode::None;
    }
    playback.oneShot = r.loopMode == Mode::OneShot;

    // The region's envelope where it has one. SFZ sustains at full level
    // unless told otherwise; attack and release fall back to the plugin's
    playback.attack = r.ampegAttack;
    playback.release = r.ampegRelease;
    playback.sustain = r.ampegSustain >= 0.0f ? r.ampegSustain / 100.0f : 1.0f;
    playback.decay = r.ampegDecay >= 0.0f ? r.ampegDecay : 0.0f;

    return playback;
}
