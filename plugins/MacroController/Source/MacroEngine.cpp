#include "MacroEngine.h"
#include <cmath>

MacroEngine::MacroEngine() {
    // Initialize with sensible defaults
    for (int i = 0; i < NUM_TARGETS; ++i) {
        targets_[i].enabled = (i < 4);  // Enable first 4 by default
        targets_[i].cc = i + 1;         // CC 1-8
        targets_[i].minValue = 0.0f;
        targets_[i].maxValue = 1.0f;
        targets_[i].curve = 1.0f;       // Linear
    }
}

std::array<float, MacroEngine::NUM_TARGETS> MacroEngine::getTargetValues(float macroPosition) const {
    std::array<float, NUM_TARGETS> values;

    for (int i = 0; i < NUM_TARGETS; ++i) {
        if (!targets_[i].enabled) {
            values[i] = 0.0f;
            continue;
        }

        // Apply curve to macro position
        float curved = applyCurve(macroPosition, targets_[i].curve);

        // Map to target range
        float range = targets_[i].maxValue - targets_[i].minValue;
        values[i] = targets_[i].minValue + (curved * range);

        // Clamp to 0.0-1.0
        values[i] = juce::jlimit(0.0f, 1.0f, values[i]);
    }

    return values;
}

void MacroEngine::setTargetEnabled(int index, bool enabled) {
    if (index >= 0 && index < NUM_TARGETS)
        targets_[index].enabled = enabled;
}

void MacroEngine::setTargetCC(int index, int cc) {
    if (index >= 0 && index < NUM_TARGETS)
        targets_[index].cc = juce::jlimit(1, 127, cc);
}

void MacroEngine::setTargetRange(int index, float min, float max) {
    if (index >= 0 && index < NUM_TARGETS) {
        targets_[index].minValue = juce::jlimit(0.0f, 1.0f, min);
        targets_[index].maxValue = juce::jlimit(0.0f, 1.0f, max);
    }
}

void MacroEngine::setTargetCurve(int index, float curve) {
    if (index >= 0 && index < NUM_TARGETS)
        targets_[index].curve = juce::jlimit(0.1f, 10.0f, curve);
}

const MacroEngine::Target& MacroEngine::getTarget(int index) const {
    static Target dummy;
    if (index >= 0 && index < NUM_TARGETS)
        return targets_[index];
    return dummy;
}

float MacroEngine::applyCurve(float position, float curve) const {
    if (curve < 0.99f || curve > 1.01f) {
        // Power curve: y = x^curve
        // curve < 1.0: logarithmic (slow start, fast end)
        // curve > 1.0: exponential (fast start, slow end)
        return std::pow(position, curve);
    }
    return position;  // Linear
}
