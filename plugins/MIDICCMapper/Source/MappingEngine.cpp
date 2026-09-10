#include "MappingEngine.h"
#include <cmath>

MappingEngine::MappingEngine() {
    // Initialize with some defaults
    for (int i = 0; i < NUM_MAPPINGS; ++i) {
        mappings_[i].enabled = (i == 0);  // Enable first mapping only
        mappings_[i].inputCC = i + 1;
        mappings_[i].outputCC = (i + 1) * 10;  // Map CC1→CC10, CC2→CC20, etc.
        mappings_[i].minValue = 0.0f;
        mappings_[i].maxValue = 1.0f;
        mappings_[i].curve = 1.0f;
        mappings_[i].invertValue = false;
    }
}

std::vector<MappingEngine::OutputCC> MappingEngine::processCC(int inputCC, float inputValue) {
    std::vector<OutputCC> outputs;

    // Find all mappings that match this input CC
    for (const auto& mapping : mappings_) {
        if (!mapping.enabled || mapping.inputCC != inputCC)
            continue;

        float value = inputValue;

        // Apply inversion
        if (mapping.invertValue)
            value = 1.0f - value;

        // Apply curve
        value = applyCurve(value, mapping.curve);

        // Apply range scaling
        float range = mapping.maxValue - mapping.minValue;
        value = mapping.minValue + (value * range);

        // Clamp
        value = juce::jlimit(0.0f, 1.0f, value);

        outputs.push_back({mapping.outputCC, value});
    }

    return outputs;
}

void MappingEngine::setMappingEnabled(int index, bool enabled) {
    if (index >= 0 && index < NUM_MAPPINGS)
        mappings_[index].enabled = enabled;
}

void MappingEngine::setInputCC(int index, int cc) {
    if (index >= 0 && index < NUM_MAPPINGS)
        mappings_[index].inputCC = juce::jlimit(1, 127, cc);
}

void MappingEngine::setOutputCC(int index, int cc) {
    if (index >= 0 && index < NUM_MAPPINGS)
        mappings_[index].outputCC = juce::jlimit(1, 127, cc);
}

void MappingEngine::setRange(int index, float min, float max) {
    if (index >= 0 && index < NUM_MAPPINGS) {
        mappings_[index].minValue = juce::jlimit(0.0f, 1.0f, min);
        mappings_[index].maxValue = juce::jlimit(0.0f, 1.0f, max);
    }
}

void MappingEngine::setCurve(int index, float curve) {
    if (index >= 0 && index < NUM_MAPPINGS)
        mappings_[index].curve = juce::jlimit(0.1f, 10.0f, curve);
}

void MappingEngine::setInvert(int index, bool invert) {
    if (index >= 0 && index < NUM_MAPPINGS)
        mappings_[index].invertValue = invert;
}

const MappingEngine::Mapping& MappingEngine::getMapping(int index) const {
    static Mapping dummy;
    if (index >= 0 && index < NUM_MAPPINGS)
        return mappings_[index];
    return dummy;
}

float MappingEngine::applyCurve(float value, float curve) const {
    if (curve < 0.99f || curve > 1.01f) {
        return std::pow(value, curve);
    }
    return value;  // Linear
}
