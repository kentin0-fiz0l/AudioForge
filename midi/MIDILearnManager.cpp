#include "MIDILearnManager.h"

namespace AudioForge {

MIDILearnManager::MIDILearnManager(juce::AudioProcessorValueTreeState& apvts)
    : apvts_(apvts) {
}

MIDILearnManager::~MIDILearnManager() {
}

void MIDILearnManager::startLearning(const juce::String& parameterID) {
    learningActive_ = true;
    learningParameterID_ = parameterID;
}

void MIDILearnManager::stopLearning() {
    learningActive_ = false;
    learningParameterID_ = "";
}

void MIDILearnManager::addMapping(const MIDIMapping& mapping) {
    // Remove existing mapping for this parameter
    removeMapping(mapping.parameterID);
    
    // Add new mapping
    mappings_.push_back(mapping);
    
    if (onMappingCreated)
        onMappingCreated(mapping.parameterID, mapping.midiCC);
}

void MIDILearnManager::removeMapping(const juce::String& parameterID) {
    auto it = std::remove_if(mappings_.begin(), mappings_.end(),
        [&parameterID](const MIDIMapping& m) { return m.parameterID == parameterID; });
    
    if (it != mappings_.end()) {
        mappings_.erase(it, mappings_.end());
        
        if (onMappingRemoved)
            onMappingRemoved(parameterID);
    }
}

void MIDILearnManager::clearAllMappings() {
    mappings_.clear();
}

bool MIDILearnManager::hasMapping(const juce::String& parameterID) const {
    return std::any_of(mappings_.begin(), mappings_.end(),
        [&parameterID](const MIDIMapping& m) { return m.parameterID == parameterID; });
}

MIDIMapping* MIDILearnManager::getMapping(const juce::String& parameterID) {
    auto it = std::find_if(mappings_.begin(), mappings_.end(),
        [&parameterID](const MIDIMapping& m) { return m.parameterID == parameterID; });
    
    return (it != mappings_.end()) ? &(*it) : nullptr;
}

void MIDILearnManager::processMidiMessage(const juce::MidiMessage& message) {
    if (!message.isController())
        return;
    
    int cc = message.getControllerNumber();
    int ccValue = message.getControllerValue();
    int channel = message.getChannel();
    
    // MIDI Learn Mode: Create mapping on first CC received
    if (learningActive_) {
        MIDIMapping newMapping(learningParameterID_, cc, channel);
        addMapping(newMapping);
        stopLearning();
        return;
    }
    
    // Normal Mode: Apply existing mappings
    for (const auto& mapping : mappings_) {
        if (mapping.matches(cc, channel)) {
            float paramValue = mapping.ccToParameter(ccValue);
            setParameterValue(mapping.parameterID, paramValue);
            
            if (onParameterChanged)
                onParameterChanged(mapping.parameterID, paramValue);
        }
    }
}

void MIDILearnManager::setParameterValue(const juce::String& parameterID, float value) {
    if (auto* param = apvts_.getParameter(parameterID)) {
        // Get parameter's range
        auto range = param->getNormalisableRange();
        
        // Normalize the value to 0-1 range
        float normalizedValue = range.convertTo0to1(value);
        
        // Set parameter value (APVTS handles thread safety)
        param->setValueNotifyingHost(normalizedValue);
    }
}

std::unique_ptr<juce::XmlElement> MIDILearnManager::saveToXml() const {
    auto xml = std::make_unique<juce::XmlElement>("MIDILearnMappings");
    
    for (const auto& mapping : mappings_) {
        xml->addChildElement(mapping.toXml().release());
    }
    
    return xml;
}

void MIDILearnManager::loadFromXml(const juce::XmlElement& xml) {
    if (xml.getTagName() != "MIDILearnMappings")
        return;
    
    clearAllMappings();
    
    for (auto* mappingXml : xml.getChildIterator()) {
        if (mappingXml->getTagName() == "Mapping") {
            auto mapping = MIDIMapping::fromXml(*mappingXml);
            if (mapping.isValid())
                mappings_.push_back(mapping);
        }
    }
}

} // namespace AudioForge
