#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../../../shared/utils/AudioFileSafety.h"

namespace {
    // A float parameter whose text is its value to a fixed number of
    // decimals, with the unit as its label
    std::unique_ptr<juce::AudioParameterFloat> makeFloat(const char* id, const char* name,
                                                         juce::NormalisableRange<float> range, float defaultValue,
                                                         const char* unit, int decimals) {
        return std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{id, 1}, name, range, defaultValue,
            juce::AudioParameterFloatAttributes()
                .withLabel(unit)
                .withStringFromValueFunction([decimals](float value, int) { return juce::String(value, decimals); }));
    }

    juce::NormalisableRange<float> frequencyRange(float low, float high) {
        juce::NormalisableRange<float> range(low, high, 1.0f);
        range.setSkewForCentre(std::sqrt(low * high));  // Even steps in pitch
        return range;
    }
}

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(makeFloat("mix", "Mix", {0.0f, 100.0f, 0.1f}, 30.0f, "%", 0));
    params.push_back(makeFloat("predelay", "Pre-Delay", {0.0f, ConvolutionReverbProcessor::maxPreDelayMs, 0.1f}, 0.0f, "ms", 0));
    params.push_back(makeFloat("lowcut", "Low Cut", frequencyRange(20.0f, 1000.0f), 20.0f, "Hz", 0));
    params.push_back(makeFloat("highcut", "High Cut", frequencyRange(1000.0f, 20000.0f), 20000.0f, "Hz", 0));
    params.push_back(makeFloat("output", "Output", {-24.0f, 6.0f, 0.1f}, 0.0f, "dB", 1));
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"bypass", 1}, "Bypass", false));
    return {params.begin(), params.end()};
}

ConvolutionReverbProcessor::ConvolutionReverbProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()),
      midiLearnManager_(apvts_),
      presetManager_(apvts_, "ConvolutionReverb") {
    mixParam_ = apvts_.getRawParameterValue("mix");
    preDelayParam_ = apvts_.getRawParameterValue("predelay");
    lowCutParam_ = apvts_.getRawParameterValue("lowcut");
    highCutParam_ = apvts_.getRawParameterValue("highcut");
    outputParam_ = apvts_.getRawParameterValue("output");
    bypassParam_ = apvts_.getRawParameterValue("bypass");

    lowCut_.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    highCut_.setType(juce::dsp::StateVariableTPTFilterType::lowpass);

    // Scan for presets on startup
    presetManager_.scanPresets();
}

ConvolutionReverbProcessor::~ConvolutionReverbProcessor() {}

juce::AudioProcessorParameter* ConvolutionReverbProcessor::getBypassParameter() const {
    return apvts_.getParameter("bypass");
}

double ConvolutionReverbProcessor::getTailLengthSeconds() const {
    return irSeconds_ + maxPreDelayMs / 1000.0;
}

void ConvolutionReverbProcessor::prepareToPlay(double sr, int sb) {
    sampleRate_ = sr;
    juce::dsp::ProcessSpec spec{sr, static_cast<juce::uint32>(sb), 2};

    // Also takes up an impulse response loaded since the last prepare
    convolution_.prepare(spec);

    preDelay_.setMaximumDelayInSamples(static_cast<int>(std::ceil(maxPreDelayMs / 1000.0 * sr)) + 1);
    preDelay_.prepare(spec);
    lowCut_.prepare(spec);
    highCut_.prepare(spec);
    dry_.setSize(2, sb);

    mix_.reset(sr, 0.05);
    mix_.setCurrentAndTargetValue(mixParam_->load() / 100.0f);
    outputGain_.reset(sr, 0.05);
    outputGain_.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(outputParam_->load()));
}

void ConvolutionReverbProcessor::processBlock(juce::AudioBuffer<float>& buf, juce::MidiBuffer& midi) {
    juce::ScopedNoDenormals noDenormals;
    for (const auto metadata : midi)
        midiLearnManager_.processMidiMessage(metadata.getMessage());

    if (bypassParam_->load() >= 0.5f)
        return;

    const int numSamples = buf.getNumSamples();
    const int channels = juce::jmin(2, buf.getNumChannels());
    if (channels == 0 || numSamples > dry_.getNumSamples())
        return;

    for (int c = 0; c < channels; ++c)
        dry_.copyFrom(c, 0, buf, c, 0, numSamples);

    mix_.setTargetValue(mixParam_->load() / 100.0f);
    outputGain_.setTargetValue(juce::Decibels::decibelsToGain(outputParam_->load()));

    if (irSeconds_ > 0.0) {
        // Wet: pre-delay, the room, then the low and high cut
        preDelay_.setDelay(preDelayParam_->load() / 1000.0f * static_cast<float>(sampleRate_));
        for (int c = 0; c < channels; ++c) {
            auto* data = buf.getWritePointer(c);
            for (int i = 0; i < numSamples; ++i) {
                preDelay_.pushSample(c, data[i]);
                data[i] = preDelay_.popSample(c);
            }
        }

        juce::dsp::AudioBlock<float> block(buf.getArrayOfWritePointers(), static_cast<size_t>(channels),
                                           static_cast<size_t>(numSamples));
        juce::dsp::ProcessContextReplacing<float> context(block);
        convolution_.process(context);

        const float nyquistLimit = static_cast<float>(sampleRate_) * 0.45f;
        lowCut_.setCutoffFrequency(juce::jmin(lowCutParam_->load(), nyquistLimit));
        highCut_.setCutoffFrequency(juce::jmin(highCutParam_->load(), nyquistLimit));
        lowCut_.process(context);
        highCut_.process(context);
    } else {
        // No impulse response: no room, only the dry signal
        buf.clear();
    }

    for (int i = 0; i < numSamples; ++i) {
        const float mix = mix_.getNextValue();
        const float gain = outputGain_.getNextValue();
        for (int c = 0; c < channels; ++c)
            buf.setSample(c, i, gain * ((1.0f - mix) * dry_.getSample(c, i) + mix * buf.getSample(c, i)));
    }
}

bool ConvolutionReverbProcessor::loadImpulseResponse(const juce::File& file, juce::String& error) {
    if (const auto why = AudioForge::FileSafety::refusal(file); why.isNotEmpty()) {
        error = why;
        return false;
    }

    juce::AudioFormatManager formats;
    formats.registerBasicFormats();
    std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(file));
    if (reader == nullptr) {
        error = "can't read an impulse response from " + file.getFullPathName();
        return false;
    }

    // The header's numbers decide how much is allocated, so check them first
    const auto problem = AudioForge::FileSafety::checkReader(*reader);
    if (problem.isNotEmpty()) {
        error = "the impulse response in " + file.getFullPathName() + ": " + problem;
        return false;
    }

    const int channels = juce::jmin(2, static_cast<int>(reader->numChannels));
    // Capped in 64 bits at the longest response at the highest rate allowed,
    // whatever the header says, before it becomes an int
    const auto maxFrames = static_cast<juce::int64>(maxImpulseSeconds * AudioForge::FileSafety::maxSampleRate);
    const auto length = static_cast<int>(juce::jmin(reader->lengthInSamples, maxFrames,
                                                    static_cast<juce::int64>(maxImpulseSeconds * reader->sampleRate)));

    juce::AudioBuffer<float> impulse(channels, length);
    reader->read(&impulse, 0, length, 0, true, channels > 1);

    // Scaled to unit energy (on its louder side), so that fully wet, noise
    // comes out at the level it went in. JUCE's own normalising leaves the
    // room 18 dB down.
    double energy = 0.0;
    for (int c = 0; c < channels; ++c) {
        double sum = 0.0;
        for (int i = 0; i < length; ++i)
            sum += impulse.getSample(c, i) * impulse.getSample(c, i);
        energy = juce::jmax(energy, sum);
    }
    if (energy < 1.0e-12) {
        error = "the impulse response in " + file.getFullPathName() + " is silent";
        return false;
    }
    impulse.applyGain(static_cast<float>(1.0 / std::sqrt(energy)));

    convolution_.loadImpulseResponse(std::move(impulse), reader->sampleRate,
                                     channels > 1 ? juce::dsp::Convolution::Stereo::yes : juce::dsp::Convolution::Stereo::no,
                                     juce::dsp::Convolution::Trim::no, juce::dsp::Convolution::Normalise::no);
    irFile_ = file;
    irSeconds_ = length / reader->sampleRate;
    return true;
}

juce::AudioProcessorEditor* ConvolutionReverbProcessor::createEditor() {
    return new ConvolutionReverbEditor(*this);
}

void ConvolutionReverbProcessor::getStateInformation(juce::MemoryBlock& d) {
    auto s = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> x(s.createXml());
    x->removeAttribute("ir");
    if (irFile_ != juce::File())
        x->setAttribute("ir", irFile_.getFullPathName());
    x->addChildElement(midiLearnManager_.saveToXml().release());
    x->addChildElement(presetManager_.saveToXml().release());
    copyXmlToBinary(*x, d);
}

void ConvolutionReverbProcessor::setStateInformation(const void* d, int sz) {
    std::unique_ptr<juce::XmlElement> x(getXmlFromBinary(d, sz));
    // A state that did not parse, or that belongs to something else, is ignored
    if (x == nullptr || ! x->hasTagName(apvts_.state.getType()))
        return;

    apvts_.replaceState(juce::ValueTree::fromXml(*x));

    // The impulse response: a relative path is taken from the working directory
    const auto irPath = x->getStringAttribute("ir");
    const auto ir = juce::File::getCurrentWorkingDirectory().getChildFile(irPath);
    if (const auto why = AudioForge::FileSafety::refusal(irPath, ir); irPath.isNotEmpty() && why.isNotEmpty()) {
        juce::Logger::writeToLog("ConvolutionReverb: " + why);
        irFile_ = juce::File();
        irSeconds_ = 0.0;
    } else if (irPath.isNotEmpty() && ir != irFile_) {
        juce::String error;
        if (! loadImpulseResponse(ir, error))
            juce::Logger::writeToLog("ConvolutionReverb: " + error);
    } else if (irPath.isEmpty()) {
        // A state without one has no room
        irFile_ = juce::File();
        irSeconds_ = 0.0;
    }

    if (auto* midiXml = x->getChildByName("MIDILearnMappings"))
        midiLearnManager_.loadFromXml(*midiXml);
    if (auto* presetXml = x->getChildByName("PresetManagerState"))
        presetManager_.loadFromXml(*presetXml);
}

#ifndef AUDIOFORGE_TESTS
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new ConvolutionReverbProcessor();
}
#endif
