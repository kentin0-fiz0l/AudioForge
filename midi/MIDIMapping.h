#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

namespace AudioForge {

/**
 * Represents a single MIDI CC to parameter mapping
 */
struct MIDIMapping {
    juce::String parameterID;       // Parameter identifier in APVTS
    int midiCC;                      // MIDI CC number (0-127)
    int midiChannel;                 // MIDI channel (1-16, 0 = omni)
    float minValue;                  // Minimum parameter value for mapping
    float maxValue;                  // Maximum parameter value for mapping
    bool inverted;                   // Invert CC direction
    
    MIDIMapping() 
        : midiCC(-1), midiChannel(0), minValue(0.0f), maxValue(1.0f), inverted(false) {}
    
    MIDIMapping(const juce::String& paramID, int cc, int channel = 0)
        : parameterID(paramID), midiCC(cc), midiChannel(channel),
          minValue(0.0f), maxValue(1.0f), inverted(false) {}
    
    bool isValid() const { return midiCC >= 0 && midiCC <= 127; }
    
    // Convert MIDI CC value (0-127) to parameter value
    float ccToParameter(int ccValue) const {
        float normalized = juce::jlimit(0.0f, 1.0f, ccValue / 127.0f);
        if (inverted) normalized = 1.0f - normalized;
        return minValue + (normalized * (maxValue - minValue));
    }
    
    // Check if MIDI message matches this mapping
    bool matches(int cc, int channel) const {
        if (midiCC != cc) return false;
        if (midiChannel == 0) return true; // Omni mode
        return midiChannel == channel;
    }
    
    // Serialize to XML
    std::unique_ptr<juce::XmlElement> toXml() const {
        auto xml = std::make_unique<juce::XmlElement>("Mapping");
        xml->setAttribute("parameterID", parameterID);
        xml->setAttribute("midiCC", midiCC);
        xml->setAttribute("midiChannel", midiChannel);
        xml->setAttribute("minValue", minValue);
        xml->setAttribute("maxValue", maxValue);
        xml->setAttribute("inverted", inverted);
        return xml;
    }
    
    // Deserialize from XML
    static MIDIMapping fromXml(const juce::XmlElement& xml) {
        MIDIMapping mapping;
        mapping.parameterID = xml.getStringAttribute("parameterID");
        mapping.midiCC = xml.getIntAttribute("midiCC", -1);
        mapping.midiChannel = xml.getIntAttribute("midiChannel", 0);
        mapping.minValue = (float)xml.getDoubleAttribute("minValue", 0.0);
        mapping.maxValue = (float)xml.getDoubleAttribute("maxValue", 1.0);
        mapping.inverted = xml.getBoolAttribute("inverted", false);
        return mapping;
    }
};

} // namespace AudioForge
