/*
  PluginProcessor.cpp - Implementation
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{

// Genre templates in ChordEngine, with the names shown to the user
struct Genre { const char* id; const char* name; };
constexpr Genre kGenres[] = {
    { "pop", "Pop" }, { "rock", "Rock" }, { "jazz", "Jazz" }, { "blues", "Blues" },
    { "edm", "EDM" }, { "emotional", "Emotional" }, { "happy", "Happy" }, { "sad", "Sad" }
};
static_assert (std::size (kGenres) == ChordGeniusProcessor::kNumGenres, "One entry per genre");

constexpr int kLowestKeyRoot = 60;   // Middle C: keys run from C4 to B4
constexpr juce::uint8 kVelocity = 100;
constexpr int kMidiChannel = 1;

// A host's reported position can differ from where the last block ended by
// rounding or tempo automation; anything beyond this is a loop or a seek.
constexpr double kJumpToleranceInQuarterNotes = 1.0e-3;

} // namespace

ChordGeniusProcessor::ChordGeniusProcessor()
    : AudioProcessor (BusesProperties()
                     .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                     .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters_ (*this, nullptr, "ChordGenius", createParameterLayout())
{
    keyParam_ = dynamic_cast<juce::AudioParameterChoice*> (parameters_.getParameter ("key"));
    modeParam_ = dynamic_cast<juce::AudioParameterChoice*> (parameters_.getParameter ("mode"));
    genreParam_ = dynamic_cast<juce::AudioParameterChoice*> (parameters_.getParameter ("genre"));

    for (int mode = 0; mode < 2; ++mode)
    {
        chordEngine_.setKey (kLowestKeyRoot, mode == 0);

        for (int genre = 0; genre < kNumGenres; ++genre)
        {
            const auto chords = chordEngine_.generateProgression (kGenres[genre].id, kChordsPerProgression);
            jassert (chords.size() == kChordsPerProgression);

            for (size_t i = 0; i < chords.size(); ++i)
                std::copy_n (chords[i].notes.begin(), 3, progressionsInC_[(size_t) mode][(size_t) genre][i].begin());
        }
    }
}

ChordGeniusProcessor::~ChordGeniusProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout ChordGeniusProcessor::createParameterLayout()
{
    juce::StringArray genreNames;
    for (const auto& genre : kGenres)
        genreNames.add (genre.name);

    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "key", 1 }, "Key",
        juce::StringArray { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" }, 0));
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "mode", 1 }, "Mode", juce::StringArray { "Major", "Minor" }, 0));
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "genre", 1 }, "Genre", genreNames, 0));
    return layout;
}

void ChordGeniusProcessor::prepareToPlay (double sampleRate, int)
{
    sampleRate_ = sampleRate;
    wasPlaying_ = false;
}

void ChordGeniusProcessor::releaseResources()
{
}

void ChordGeniusProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                         juce::MidiBuffer& midiMessages)
{
    buffer.clear();
    midiMessages.clear(); // This plugin only outputs the chords it plays

    const int numSamples = buffer.getNumSamples();

    // Follow the host transport: one chord per bar, with bar 1 at PPQ 0.
    // Without a musical position there is no bar grid, so treat that as stopped.
    bool isPlaying = false;
    double blockStartPpq = 0.0;
    double bpm = 120.0;
    double quarterNotesPerBar = 4.0;
    double loopEndPpq = std::numeric_limits<double>::max();

    if (auto* playHead = getPlayHead())
    {
        if (auto position = playHead->getPosition())
        {
            const auto ppq = position->getPpqPosition();
            isPlaying = position->getIsPlaying() && ppq.hasValue();
            blockStartPpq = ppq.orFallback (0.0);
            bpm = position->getBpm().orFallback (120.0);

            if (auto timeSignature = position->getTimeSignature())
                quarterNotesPerBar = timeSignature->numerator * 4.0 / timeSignature->denominator;

            // Some hosts (Live) don't split a block at the loop end. A bar line
            // at or past it belongs to the next pass, which starts at the loop start.
            if (auto loop = position->getLoopPoints())
                if (position->getIsLooping() && loop->ppqEnd > loop->ppqStart && blockStartPpq < loop->ppqEnd)
                    loopEndPpq = loop->ppqEnd;
        }
    }

    if (!isPlaying)
    {
        releaseChord (midiMessages, 0); // Transport stopped: silence what is sounding
        wasPlaying_ = false;
        return;
    }

    if (numSamples == 0)
        return;

    const double ppqPerSample = bpm / (60.0 * sampleRate_);
    const double blockEndPpq = blockStartPpq + numSamples * ppqPerSample;

    // Starting, looping or seeking: release what is sounding and come in with
    // this bar's chord. Before bar 1 (a count-in) we wait for bar 1.
    const bool isContinuous = wasPlaying_
                              && std::abs (blockStartPpq - nextBlockStartPpq_) < kJumpToleranceInQuarterNotes;
    if (!isContinuous)
    {
        releaseChord (midiMessages, 0);
        const auto bar = (juce::int64) std::floor (blockStartPpq / quarterNotesPerBar);
        nextBar_ = juce::jmax ((juce::int64) 0, bar);

        if (bar >= 0)
            startChord (midiMessages, nextBar_++, 0);
    }
    else
    {
        // A meter change moves the bar grid. Go to the new grid's next bar line
        // rather than playing every bar line it puts behind the playhead.
        const auto firstBarLine = (juce::int64) std::ceil ((blockStartPpq - kJumpToleranceInQuarterNotes) / quarterNotesPerBar);
        const auto lastBarLine = (juce::int64) std::floor (blockStartPpq / quarterNotesPerBar) + 1;
        nextBar_ = juce::jmax ((juce::int64) 0, juce::jlimit (firstBarLine, lastBarLine, nextBar_));
    }

    wasPlaying_ = true;
    nextBlockStartPpq_ = blockEndPpq;

    for (; nextBar_ * quarterNotesPerBar < juce::jmin (blockEndPpq, loopEndPpq); ++nextBar_)
    {
        const int sample = juce::jlimit (0, numSamples - 1,
            juce::roundToInt ((nextBar_ * quarterNotesPerBar - blockStartPpq) / ppqPerSample));
        releaseChord (midiMessages, sample);
        startChord (midiMessages, nextBar_, sample);
    }
}

void ChordGeniusProcessor::processBlockBypassed (juce::AudioBuffer<float>&, juce::MidiBuffer& midiMessages)
{
    // Bypassed: MIDI passes through, but a chord we started must not hang
    releaseChord (midiMessages, 0);
    wasPlaying_ = false;
}

void ChordGeniusProcessor::startChord (juce::MidiBuffer& midi, juce::int64 bar, int samplePosition)
{
    const auto mode = (size_t) modeParam_->getIndex();
    const auto genre = (size_t) genreParam_->getIndex();
    const auto chord = (size_t) (bar % kChordsPerProgression);
    const int transpose = keyParam_->getIndex();

    soundingNotes_ = progressionsInC_[mode][genre][chord];
    for (int& note : soundingNotes_)
    {
        note += transpose;
        midi.addEvent (juce::MidiMessage::noteOn (kMidiChannel, note, kVelocity), samplePosition);
    }
    chordSounding_ = true;
}

void ChordGeniusProcessor::releaseChord (juce::MidiBuffer& midi, int samplePosition)
{
    if (!chordSounding_)
        return;

    for (int note : soundingNotes_)
        midi.addEvent (juce::MidiMessage::noteOff (kMidiChannel, note), samplePosition);
    chordSounding_ = false;
}

juce::AudioProcessorEditor* ChordGeniusProcessor::createEditor()
{
    return new ChordGeniusEditor (*this);
}

void ChordGeniusProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = parameters_.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void ChordGeniusProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (parameters_.state.getType()))
            parameters_.replaceState (juce::ValueTree::fromXml (*xml));
}

std::vector<ChordEngine::Chord> ChordGeniusProcessor::getCurrentProgression()
{
    chordEngine_.setKey (kLowestKeyRoot + keyParam_->getIndex(), modeParam_->getIndex() == 0);
    return chordEngine_.generateProgression (kGenres[genreParam_->getIndex()].id, kChordsPerProgression);
}

void ChordGeniusProcessor::exportToMidiFile(const juce::File& file)
{
    juce::MidiFile midiFile;
    midiFile.setTicksPerQuarterNote(480);
    juce::MidiMessageSequence track;

    track.addEvent(juce::MidiMessage::tempoMetaEvent(500000), 0);
    track.addEvent(juce::MidiMessage::timeSignatureMetaEvent(4, 4), 0);
    track.addEvent(juce::MidiMessage::textMetaEvent(3, "Chords"), 0);

    int ticksPerBeat = 480;
    int currentBeat = 0;

    for (const auto& chord : getCurrentProgression()) {
        int timeStamp = currentBeat * ticksPerBeat;

        // Add all notes in the chord
        for (int note : chord.notes) {
            track.addEvent(juce::MidiMessage::noteOn(1, note, juce::uint8(100)), timeStamp);
            track.addEvent(juce::MidiMessage::noteOff(1, note), timeStamp + (ticksPerBeat * 4) - 1);
        }

        currentBeat += 4;  // Each chord lasts 4 beats
    }

    track.updateMatchedPairs();
    midiFile.addTrack(track);

    juce::FileOutputStream stream(file);
    if (stream.openedOk()) {
        midiFile.writeTo(stream);
    }
}

// This creates new instances of the plugin
#ifndef AUDIOFORGE_TESTS
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChordGeniusProcessor();
}
#endif
