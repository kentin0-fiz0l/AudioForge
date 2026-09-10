#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <array>

class MappingEngine {
public:
    static constexpr int NUM_MAPPINGS = 8;

    struct Mapping {
        bool enabled = false;
        int inputCC = 1;
        int outputCC = 1;
        float minValue = 0.0f;   // 0.0-1.0
        float maxValue = 1.0f;   // 0.0-1.0
        float curve = 1.0f;      // 0.5=log, 1.0=linear, 2.0=exp
        bool invertValue = false;
    };

    MappingEngine();

    // Process incoming CC message, returns output CCs
    struct OutputCC {
        int cc;
        float value;  // 0.0-1.0 (caller converts to 0-127)
    };
    std::vector<OutputCC> processCC(int inputCC, float inputValue);

    // Mapping configuration
    void setMappingEnabled(int index, bool enabled);
    void setInputCC(int index, int cc);
    void setOutputCC(int index, int cc);
    void setRange(int index, float min, float max);
    void setCurve(int index, float curve);
    void setInvert(int index, bool invert);

    const Mapping& getMapping(int index) const;

private:
    std::array<Mapping, NUM_MAPPINGS> mappings_;

    float applyCurve(float value, float curve) const;
};
