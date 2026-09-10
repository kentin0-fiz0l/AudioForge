#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "MIDIMapping.h"

namespace AudioForge {

/**
 * Manages MIDI learn functionality for a plugin
 * Handles CC-to-parameter mapping, learning mode, and persistence
 */
class MIDILearnManager {
public:
    MIDILearnManager(juce::AudioProcessorValueTreeState& apvts);
    ~MIDILearnManager();
    
    // MIDI Learn Mode
    void startLearning(const juce::String& parameterID);
    void stopLearning();
    bool isLearning() const { return learningActive_; }
    juce::String getLearningParameterID() const { return learningParameterID_; }
    
    // Mapping Management
    void addMapping(const MIDIMapping& mapping);
    void removeMapping(const juce::String& parameterID);
    void clearAllMappings();
    bool hasMapping(const juce::String& parameterID) const;
    MIDIMapping* getMapping(const juce::String& parameterID);
    const std::vector<MIDIMapping>& getAllMappings() const { return mappings_; }
    
    // MIDI Processing
    void processMidiMessage(const juce::MidiMessage& message);
    
    // Persistence
    std::unique_ptr<juce::XmlElement> saveToXml() const;
    void loadFromXml(const juce::XmlElement& xml);
    
    // Callbacks
    std::function<void(const juce::String& parameterID, int midiCC)> onMappingCreated;
    std::function<void(const juce::String& parameterID)> onMappingRemoved;
    std::function<void(const juce::String& parameterID, float value)> onParameterChanged;
    
private:
    juce::AudioProcessorValueTreeState& apvts_;
    std::vector<MIDIMapping> mappings_;
    
    bool learningActive_ = false;
    juce::String learningParameterID_;
    
    void setParameterValue(const juce::String& parameterID, float value);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDILearnManager)
};

} // namespace AudioForge
