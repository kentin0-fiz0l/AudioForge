#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <array>

class MacroEngine {
public:
    static constexpr int NUM_TARGETS = 8;

    struct Target {
        bool enabled = false;
        int cc = 1;
        float minValue = 0.0f;  // 0.0-1.0
        float maxValue = 1.0f;  // 0.0-1.0
        float curve = 1.0f;     // 0.5=log, 1.0=linear, 2.0=exp
    };

    MacroEngine();

    // Get target values based on macro position
    std::array<float, NUM_TARGETS> getTargetValues(float macroPosition) const;

    // Target configuration
    void setTargetEnabled(int index, bool enabled);
    void setTargetCC(int index, int cc);
    void setTargetRange(int index, float min, float max);
    void setTargetCurve(int index, float curve);

    const Target& getTarget(int index) const;

private:
    std::array<Target, NUM_TARGETS> targets_;

    float applyCurve(float position, float curve) const;
};
